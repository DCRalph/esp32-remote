#include "SerialControl.h"

#include <dcr_Logger.h>
#include <esp_heap_caps.h>
#include <esp_rom_crc.h>
#include <hal/usb_serial_jtag_ll.h>
#include <mbedtls/base64.h>

#include "ControllerPanel.h"
#include "Touch.h"
#include "config.h"
#include "screens/Screens.h"

#undef LOG_TAG
#define LOG_TAG "CLI"

namespace
{
  ControllerPanel *panel = nullptr;
  String line;
  bool lastWasCr = false;

  constexpr size_t kRleCapacity = static_cast<size_t>(LCD_WIDTH) * LCD_HEIGHT * 3; // every pixel its own run
  constexpr size_t kLineBytes = 57;                                                  // 76 base64 characters
  constexpr uint32_t kFifoWaitMs = 2000;                                             // host stopped reading

  struct NamedScreen
  {
    const char *name;
    const Screen2 *screen;
  };
  const NamedScreen kScreens[] = {
      {"home", &HomeScreen},
      {"ui", &UiTest::RootScreen},
      {"touch", &TouchTestScreen},
      {"device", &DeviceScreen},
      {"cal", &TouchCalScreen},
  };

  // HWCDC drops queued bytes whenever its connection heuristic blinks, which a
  // burst makes it do, so a shot goes into the USB-Serial-JTAG FIFO directly.
  void writeStream(const char *data, size_t len)
  {
#if defined(ARDUINO_USB_MODE) && ARDUINO_USB_MODE == 1
    while (len > 0)
    {
      for (uint32_t waited = 0; !usb_serial_jtag_ll_txfifo_writable(); waited++)
      {
        if (waited >= kFifoWaitMs)
          return;
        vTaskDelay(1);
      }
      const uint32_t wrote = usb_serial_jtag_ll_write_txfifo(reinterpret_cast<const uint8_t *>(data), len > 64 ? 64 : len);
      usb_serial_jtag_ll_txfifo_flush();
      data += wrote;
      len -= wrote;
    }
#else
    Serial.write(data, len);
#endif
  }

  // Runs of [count 1-255][low byte][high byte], rows top to bottom. The sprite holds
  // pixels byte-swapped, so its high byte is the colour's low byte.
  size_t captureRle565(const uint16_t *frame, uint8_t *out, size_t capacity)
  {
    size_t used = 0;
    uint16_t run = 0;
    uint8_t runLength = 0;
    const auto flush = [&]()
    {
      if (used + 3 > capacity)
        return false;
      out[used++] = runLength;
      out[used++] = run >> 8;
      out[used++] = run & 0xFF;
      return true;
    };
    const size_t pixels = static_cast<size_t>(LCD_WIDTH) * LCD_HEIGHT;
    for (size_t i = 0; i < pixels; ++i)
    {
      if (runLength > 0 && frame[i] == run && runLength < 255)
      {
        ++runLength;
        continue;
      }
      if (runLength > 0 && !flush())
        return 0;
      run = frame[i];
      runLength = 1;
    }
    return runLength > 0 && !flush() ? 0 : used;
  }

  void cmdShot()
  {
    const uint16_t *frame = panel != nullptr ? panel->frameBuffer() : nullptr;
    uint8_t *rle = static_cast<uint8_t *>(heap_caps_malloc(kRleCapacity, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    const size_t bytes = frame != nullptr && rle != nullptr ? captureRle565(frame, rle, kRleCapacity) : 0;
    if (bytes == 0)
    {
      heap_caps_free(rle);
      Serial.println(rle == nullptr ? "SHOT-ERROR alloc" : "SHOT-ERROR capture");
      return;
    }
    const uint32_t crc = esp_rom_crc32_le(0, rle, bytes);

    // Logging is muted while the payload streams, so no line lands inside it.
    const LogLevel previous = Logger::GetLevel();
    Logger::SetLevel(LogLevel::Fatal);
    Serial.flush();
    char text[96];
    int n = snprintf(text, sizeof(text), "SHOT-BEGIN w=%u h=%u fmt=rle565 bytes=%u crc=%08X\n",
                     LCD_WIDTH, LCD_HEIGHT, static_cast<unsigned>(bytes), static_cast<unsigned>(crc));
    writeStream(text, static_cast<size_t>(n));
    for (size_t offset = 0; offset < bytes; offset += kLineBytes)
    {
      const size_t chunk = bytes - offset < kLineBytes ? bytes - offset : kLineBytes;
      size_t written = 0;
      mbedtls_base64_encode(reinterpret_cast<unsigned char *>(text), sizeof(text) - 2, &written, rle + offset, chunk);
      text[written] = '\n';
      writeStream(text, written + 1);
    }
    writeStream("SHOT-END\n", 9);
    Logger::SetLevel(previous);
    heap_caps_free(rle);
  }

  bool parsePoint(const String &args, int16_t &x, int16_t &y)
  {
    const int space = args.indexOf(' ');
    if (space < 0)
      return false;
    const long px = args.substring(0, space).toInt();
    const long py = args.substring(space + 1).toInt();
    if (px < 0 || px >= LCD_WIDTH || py < 0 || py >= LCD_HEIGHT)
      return false;
    x = static_cast<int16_t>(px);
    y = static_cast<int16_t>(py);
    return true;
  }

  void cmdScreen(const String &name)
  {
    for (const NamedScreen &s : kScreens)
    {
      if (name == s.name)
      {
        screenManager.setScreen(s.screen);
        Serial.printf("Switched to %s\n", s.name);
        return;
      }
    }
    Serial.print("Usage: screen <");
    for (size_t i = 0; i < sizeof(kScreens) / sizeof(kScreens[0]); ++i)
      Serial.printf("%s%s", i ? "|" : "", kScreens[i].name);
    Serial.println(">");
  }

  void run(String text)
  {
    text.trim();
    const int space = text.indexOf(' ');
    const String cmd = space < 0 ? text : text.substring(0, space);
    String args = space < 0 ? String() : text.substring(space + 1);
    args.trim();
    int16_t x = 0, y = 0;

    if (cmd.length() == 0)
    {
    }
    else if (cmd == "tap" || cmd == "press")
    {
      if (!parsePoint(args, x, y))
        Serial.printf("Usage: %s <x 0-%u> <y 0-%u>\n", cmd.c_str(), LCD_WIDTH - 1, LCD_HEIGHT - 1);
      else if (cmd == "tap")
      {
        Touch::injectTap(x, y);
        Serial.printf("TAP %d,%d\n", x, y);
      }
      else
      {
        Touch::injectPress(x, y);
        Serial.printf("PRESS %d,%d\n", x, y);
      }
    }
    else if (cmd == "release")
    {
      Touch::injectRelease();
      Serial.println("RELEASE");
    }
    else if (cmd == "shot")
      cmdShot();
    else if (cmd == "screen")
      cmdScreen(args);
    else if (cmd == "version")
      Serial.println("Version: controller-board " __DATE__ " " __TIME__);
    else
    {
      Serial.println("Commands: tap <x> <y> | press <x> <y> | release | shot | screen <name> | version");
    }
    Serial.print("> ");
  }
}

void SerialControl::begin(ControllerPanel &p)
{
  panel = &p;
}

void SerialControl::loop()
{
  // A line ends on CR (what tools/agent.py sends) or LF; CRLF counts once.
  while (Serial.available() > 0)
  {
    const char c = static_cast<char>(Serial.read());
    if (c == '\n' && lastWasCr)
    {
      lastWasCr = false;
      continue;
    }
    lastWasCr = c == '\r';
    if (c == '\r' || c == '\n')
    {
      run(line);
      line = "";
    }
    else if (line.length() < 96)
      line += c;
  }
}
