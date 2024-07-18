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

#include "t-hmi-c64/Main.h"
#include <esp_log.h>
#include <M5Cardputer.h>

static const char *TAG = "T-HMI-C64";

void setup()
{
  M5Cardputer.begin();
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  ESP_LOGI(TAG, "start setup...");
  ESP_LOGI(TAG, "setup() running on core %d", xPortGetCoreID());
  Main::setup();
  ESP_LOGI(TAG, "setup done");
}

void loop() { Main::loop(); }
