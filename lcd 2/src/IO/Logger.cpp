#include "Logger.h"

Logger::Logger(void)
{
}

void Logger::init(Stream *serial)
{
  this->serial = serial;

  this->println("[INFO] [LOGGER] Initialized");
}

void Logger::_print(String message)
{
  this->serial->print(message);
}

void Logger::_print2(String message)
{
  this->serial->print(message);
}

void Logger::print(String message)
{
  this->_print(message);
}

void Logger::print(double message, int digits)
{
  this->_print(String(message, digits));
}

void Logger::println(void)
{
  this->_print(this->ln_suffix);
}

void Logger::println(String message)
{
  this->_print(message + this->ln_suffix);
}

void Logger::println(double message, int digits)
{
  this->_print(String(message, digits) + this->ln_suffix);
}

void Logger::hex(uint8_t data)
{
  char buffer[6];
  sprintf(buffer, "0x%02X", data);
  this->_print(buffer);
}

void Logger::hexln(uint8_t data)
{
  this->hex(data);
  this->println();
}

void Logger::hex(uint8_t *data, uint8_t len)
{
  for (int i = 0; i < len; i++)
  {
    if (i == len - 1)
    {
      this->hexln(data[i]);
    }
    else
    {
      this->hex(data[i]);
      this->_print(" ");
    }
  }
}

void Logger::printf(const char *format, ...)
{
  char buffer[256];
  va_list args;
  va_start(args, format);
  vsprintf(buffer, format, args);
  va_end(args);

  this->_print(buffer);
}

Logger logger;