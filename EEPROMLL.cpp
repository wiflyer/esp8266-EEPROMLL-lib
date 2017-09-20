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

  EEPROMLL.cpp - esp8266 EEPROM long life (branch SDK 2.3.0 Arduino IDE 1.6.8)
  This file replace a part of the esp8266 core for Arduino environment.

  EEPROM.cpp - esp8266 EEPROM emulation

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

#include "Arduino.h"
#include "EEPROMLL.h"

extern "C" {
#include "c_types.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "spi_flash.h"
}

EEPROMLLClass::EEPROMLLClass(uint32_t sector)
: _sector(sector)
, _data(0)
, _size(0)
, _dirty(false)
, _erase(false)
{
}

void EEPROMLLClass::begin(size_t size) {
  if (size <= 0)
    return;
  if (size > SPI_FLASH_SEC_SIZE)
    size = SPI_FLASH_SEC_SIZE;

  size = (size + 3) & (~3);

  if (_data) {
    delete[] _data;
  }

  _data = new uint8_t[size];
  _size = size;

  noInterrupts();
  spi_flash_read(_sector * SPI_FLASH_SEC_SIZE, reinterpret_cast<uint32_t*>(_data), _size);
  interrupts();
}

void EEPROMLLClass::end() {
  if (!_size)
    return;

  commit();
  if(_data) {
    delete[] _data;
  }
  _data = 0;
  _size = 0;
}


uint8_t EEPROMLLClass::read(int address) {
  if (address < 0 || (size_t)address >= _size)
    return 0;
  if(!_data)
    return 0;

  return _data[address];
}

void EEPROMLLClass::write(int address, uint8_t value) {
  if (address < 0 || (size_t)address >= _size)
    return;
  if(!_data)
    return;

  // Optimise _dirty. Only flagged if data written is different.
  uint8_t* pData = &_data[address];
  if (*pData != value)
  {
    // Optimise _erase. Only flagged if toggle bits from 0 to 1.
    // ~1111.0000 : 0000.1111 & 1110.0000 = 0000.0000 == 0000.0000
    // ~1111.0000 : 0000.1111 & 1110.1110 = 0000.1110 != 0000.0000
    if (((~(*pData)) & value) != 0x00) { 
      _erase = true;
    }
    *pData = value;
    _dirty = true;
  }
}

bool EEPROMLLClass::commit() {
  bool ret = true;
  if (!_size)
    return false;
  if(!_dirty)
    return true;
  if(!_data)
    return false;

  noInterrupts();
  if (_erase) {
    flashEraseCounter++;
    if (spi_flash_erase_sector(_sector) != SPI_FLASH_RESULT_OK) {
      ret = false;
    }
  }
  if (ret) {
    flashWriteCounter++;
    if(spi_flash_write(_sector * SPI_FLASH_SEC_SIZE, reinterpret_cast<uint32_t*>(_data), _size) != SPI_FLASH_RESULT_OK) {
      ret = false;
    } else {
      _dirty = false;
      _erase = false;
    }
  }
  interrupts();

  return ret;
}

uint8_t * EEPROMLLClass::getDataPtr() {
  _dirty = true;
  return &_data[0];
}

extern "C" uint32_t _SPIFFS_end;
EEPROMLLClass EEPROMLL((((uint32_t)&_SPIFFS_end - 0x40200000) / SPI_FLASH_SEC_SIZE));
