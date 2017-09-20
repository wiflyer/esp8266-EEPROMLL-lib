/*
  __          ___ ______ _                 
  \ \        / (_)  ____| |                
   \ \  /\  / / _| |__  | |_   _  ___ _ __ 
    \ \/  \/ / | |  __| | | | | |/ _ \ '__|
     \  /\  /  | | |    | | |_| |  __/ |   
      \/  \/   |_|_|    |_|\__, |\___|_|   
                            __/ |          
                           |___/           

  http://github.com/wiflyer/esp8266-EEPROMLL-lib/

  EEPROMLL_shift.ino - esp8266 EEPROM long life (branch SDK 2.3.0 Arduino IDE 1.6.8)
  Example of usage EEPROM write operations without erase.

*/

#include "EEPROMLL.h"

void setup() {
  Serial.begin(115200); delay(10);
  Serial.printf("\nEEPROMLL\n");

  // some test counters
  int eepromCountErase    = 0;
  int eepromCount11111111 = 0;
  int eepromCount01111111 = 0;
  int eepromCount00111111 = 0;
  int eepromCount00011111 = 0;
  int eepromCount00001111 = 0;
  int eepromCount00000111 = 0;
  int eepromCount00000011 = 0;
  int eepromCount00000001 = 0;

  // read eeprom flash sector into eeprom data memory
  EEPROMLL.begin(4096);

  // read and modify eeprom data 
  for (int i=0; i<4096; i++) {
    uint8_t value = EEPROMLL.read(i);
    if      (value == 0b11111111) {EEPROMLL.write(i,0b01111111); eepromCount11111111++;}
    else if (value == 0b01111111) {EEPROMLL.write(i,0b00111111); eepromCount01111111++;}
    else if (value == 0b00111111) {EEPROMLL.write(i,0b00011111); eepromCount00111111++;}
    else if (value == 0b00011111) {EEPROMLL.write(i,0b00001111); eepromCount00011111++;}
    else if (value == 0b00001111) {EEPROMLL.write(i,0b00000111); eepromCount00001111++;}
    else if (value == 0b00000111) {EEPROMLL.write(i,0b00000011); eepromCount00000111++;}
    else if (value == 0b00000011) {EEPROMLL.write(i,0b00000001); eepromCount00000011++;}
    else if (value == 0b00000001) {EEPROMLL.write(i,0b00000000); eepromCount00000001++;}
    else                          {EEPROMLL.write(i,0b11111111); eepromCountErase++;}
  }

  // erase eeprom flash sector, if required
  // write eeprom data memory to eeprom flash sector
  EEPROMLL.end();

  // debug flash sector operations
  Serial.printf("EEPROMLL.flashEraseCounter %d\n",EEPROMLL.flashEraseCounter); 
  Serial.printf("EEPROMLL.flashWriteCounter %d\n",EEPROMLL.flashWriteCounter); 

  // debug flash data operations
  Serial.printf("eepromCountErase %d\n",eepromCountErase); 
  Serial.printf("eepromCount11111111 %d\n",eepromCount11111111); 
  Serial.printf("eepromCount01111111 %d\n",eepromCount01111111); 
  Serial.printf("eepromCount00111111 %d\n",eepromCount00111111); 
  Serial.printf("eepromCount00011111 %d\n",eepromCount00011111); 
  Serial.printf("eepromCount00001111 %d\n",eepromCount00001111); 
  Serial.printf("eepromCount00000111 %d\n",eepromCount00000111); 
  Serial.printf("eepromCount00000011 %d\n",eepromCount00000011); 
  Serial.printf("eepromCount00000001 %d\n",eepromCount00000001);   
}

void loop() {
  delay(1);
}
