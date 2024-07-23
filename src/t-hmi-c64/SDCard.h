/*
 Copyright (C) 2024 iEle <melephas@gmail.com>
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
#pragma once

#include <FS.h>
#include <SD_MMC.h>
#include <cstdint>

class SDCard {
private:
  bool initalized;

public:
  SDCard();
  bool init();
  uint16_t load(fs::FS &fs, uint8_t *petsciipath, uint8_t *ram);

  uint16_t loadAddr;
  uint16_t loadFile(fs::FS &fs, const std::string& path, uint8_t *ram);
  std::string loadBas(fs::FS &fs, const std::string& path);
};
