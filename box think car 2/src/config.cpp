#include "config.h"

void canFrame::print(){
  Serial.print("ID: ");
  Serial.print(id, HEX);
  Serial.print("\t");
  Serial.print("EXT: ");
  Serial.print(extFlag, HEX);
  Serial.print("\t");
  Serial.print("LEN: ");
  Serial.print(len, HEX);
  Serial.print("\t");
  Serial.print("DATA: ");
  for (int i = 0; i < len; i++)
  {
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}