#pragma once

#include <Arduino.h>


class Logger
{
private:
  String prefix = "[LOG] ";
  String ln_suffix = "\n";

  Stream *serial;

  void _print(String message);


public:
  Logger(void);

  void init(Stream *serial);

  void _print2(String message);
  void print(String message);
  // void print(int message);
  // void print(const Printable &);
  void print(double message, int digits = 2);

  void println(void);
  void println(String message);
  // void println(int message);
  // void println(const Printable &);
  void println(double message, int digits = 2);

  void hex(uint8_t data);
  void hexln(uint8_t data);
  void hex(uint8_t *data, uint8_t len);

  void printf(const char *format, ...);
};

extern Logger logger;
