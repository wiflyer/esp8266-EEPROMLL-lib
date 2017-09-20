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

  EEPROMLL.h - esp8266 EEPROM long life (branch SDK 2.3.0 Arduino IDE 1.6.8)
  This file replace a part of the esp8266 core for Arduino environment.

  Copyright (c) 2017 github.com/WiFlyer/esp8266-EEPROMLL. All rights reserved.
  This file is a replace a part of the esp8266 core for Arduino environment.

  EEPROM.h - esp8266 EEPROM emulation

  Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
  This file is part of the esp8266 core for Arduino environment.
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef EEPROMLL_h
#define EEPROMLL_h

#include <stddef.h>
#include <stdint.h>
#include <string.h>

class EEPROMLLClass {
public:
  EEPROMLLClass(uint32_t sector);

  void begin(size_t size);
  uint8_t read(int address);
  void write(int address, uint8_t val);
  bool commit();
  void end();

  uint8_t flashEraseCounter = 0; // increase on spi_flash_erase_sector
  uint8_t flashWriteCounter = 0; // increase on spi_flash_write
  uint8_t * getDataPtr();

  template<typename T> 
  T &get(int address, T &t) {
    if (address < 0 || address + sizeof(T) > _size)
      return t;

    memcpy((uint8_t*) &t, _data + address, sizeof(T));
    return t;
  }

  template<typename T> 
  const T &put(int address, const T &t) {
    if (address < 0 || address + sizeof(T) > _size)
      return t;

    memcpy(_data + address, (const uint8_t*) &t, sizeof(T));
    _dirty = true;
    _erase = true;
    return t;
  }

protected:
  uint32_t _sector;
  uint8_t* _data;
  size_t _size;
  bool _dirty; // true if required flash_write
  bool _erase; // true if required flash_erase
};

extern EEPROMLLClass EEPROMLL;

#endif
