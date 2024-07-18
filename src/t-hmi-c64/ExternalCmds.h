/*
 Copyright (C) 2024 retroelec <retroelec42@gmail.com>

 This program is free software; you can redistribute it and/or modify it
 under the terms of the GNU General Public License as published by the
 Free Software Foundation; either version 3 of the License, or (at your
 option) any later version.

 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 for more details.

 For the complete text of the GNU General Public License see
 http://www.gnu.org/licenses/.
*/
//#ifndef EXTERNALCMDS_H
//#define EXTERNALCMDS_H
//
//class CPUC64;
//
//#include "SDCard.h"
//#include <cstdint>
//
//// notifications may be not larger than 20 bytes
//
//struct BLENotificationStruct1 {
//  uint8_t type;
//  uint8_t joymode;
//  uint8_t refreshframecolor;
//  uint8_t sendrawkeycodes;
//  uint8_t switchdebug;
//};
//
//struct BLENotificationStruct2 {
//  uint8_t type;
//  uint8_t cpuRunning;
//  uint16_t pc;
//  uint8_t a;
//  uint8_t x;
//  uint8_t y;
//  uint8_t sr;
//  uint8_t d011;
//  uint8_t d016;
//  uint8_t d018;
//  uint8_t d019;
//  uint8_t d01a;
//  uint8_t register1;
//  uint8_t dc0d;
//  uint8_t dc0e;
//  uint8_t dc0f;
//  uint8_t dd0d;
//  uint8_t dd0e;
//  uint8_t dd0f;
//};
//
//static const uint8_t BLENOTIFICATIONTYPE3NUMOFBYTES =
//    16; // must be divisible by 8
//struct BLENotificationStruct3 {
//  uint8_t type;
//  uint8_t mem[BLENOTIFICATIONTYPE3NUMOFBYTES];
//};
//
//class ExternalCmds {
//private:
//  uint8_t *ram;
//  CPUC64 *cpu;
//  SDCard sdcard;
//
//  bool sendrawkeycodes;
//
//  void setVarTab(uint16_t addr);
//  void setType1Notification();
//  void setType2Notification();
//  void setType3Notification(uint16_t addr);
//
//public:
//  BLENotificationStruct1 type1notification;
//  BLENotificationStruct2 type2notification;
//  BLENotificationStruct3 type3notification;
//
//  void init(uint8_t *ram, CPUC64 *cpu);
//  uint8_t executeExternalCmd(uint8_t *buffer);
//};
//
//#endif // EXTERNALCMDS_H
