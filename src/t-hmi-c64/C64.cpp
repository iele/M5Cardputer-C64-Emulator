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
#include "C64.h"
#include "CPUC64.h"
#include "VIC.h"
#include "roms/charset.h"
#include <cstdint>
#include <esp_log.h>
#include "SDCard.h"
#include "loadactions.h"
#include "Keyboard.h"
#include <algorithm>

static const char *TAG = "C64";

static const uint16_t INTERRUPTSYSTEMRESOLUTION = 1000;

SDCard sdcard;

uint8_t *ram;
VIC vic;
CPUC64 cpu;
Keyboard keyboard;

uint16_t checkForKeyboardCnt = 0;

uint8_t throttlingCnt = 0;
uint32_t numofburnedcyclespersecond = 0;

static const uint16_t kBasicPrgStart = 0x0801;
static const uint16_t kBasicTxtTab = 0x002b;
static const uint16_t kBasicVarTab = 0x002d;
static const uint16_t kBasicAryTab = 0x002f;
static const uint16_t kBasicStrEnd = 0x0031;

hw_timer_t *interruptProfiling = NULL;
hw_timer_t *interruptTOD = NULL;
hw_timer_t *interruptSystem = NULL;
TaskHandle_t cpuTask;
TaskHandle_t vicTask;
TaskHandle_t loadTask;

void IRAM_ATTR interruptProfilingFunc()
{
  if (vic.cntRefreshs != 0)
  {
    ESP_LOGI(TAG, "fps: %d", vic.cntRefreshs);
  }
  vic.cntRefreshs = 0;
  ESP_LOGI(TAG, "noc: %d, nbc: %d", cpu.numofcyclespersecond,
           numofburnedcyclespersecond);
  cpu.numofcyclespersecond = 0;
  numofburnedcyclespersecond = 0;
}

bool updateTOD(CIA &cia)
{
  uint8_t dc08 = cia.latchrundc08.load(std::memory_order_acquire);
  dc08++;
  if (dc08 > 9)
  {
    dc08 = 0;
    uint8_t dc09 = cia.latchrundc09.load(std::memory_order_acquire);
    uint8_t dc09one = dc09 & 15;
    uint8_t dc09ten = dc09 >> 4;
    dc09one++;
    if (dc09one > 9)
    {
      dc09one = 0;
      dc09ten++;
      if (dc09ten > 5)
      {
        dc09ten = 0;
        uint8_t dc0a = cia.latchrundc0a.load(std::memory_order_acquire);
        uint8_t dc0aone = dc0a & 15;
        uint8_t dc0aten = dc0a >> 4;
        dc0aone++;
        if (dc0aone > 9)
        {
          dc0aone = 0;
          dc0aten++;
          if (dc0aten > 5)
          {
            dc0aten = 0;
            uint8_t dc0b = cia.latchrundc0b.load(std::memory_order_acquire);
            uint8_t dc0bone = dc0b & 15;
            uint8_t dc0bten = dc0b >> 4;
            bool pm = dc0b & 128;
            dc0bone++;
            if (((dc0bten == 0) && (dc0bone > 9)) ||
                (dc0bten && (dc0bone > 1)))
            {
              dc0bone = 0;
              dc0bten++;
              if (dc0bten > 1)
              {
                dc0bten = 0;
                pm = !pm;
              }
            }
            cia.latchrundc0b.store(dc0bone | (dc0bten << 4) | (pm ? 127 : 0),
                                   std::memory_order_release);
          }
        }
        cia.latchrundc0a.store(dc0aone | (dc0aten << 4),
                               std::memory_order_release);
      }
    }
    cia.latchrundc09.store(dc09one | (dc09ten << 4), std::memory_order_release);
  }
  cia.latchrundc08.store(dc08, std::memory_order_release);
  uint8_t alarmdc08 = cia.latchalarmdc08.load(std::memory_order_acquire);
  if (dc08 == alarmdc08)
  {
    uint8_t dc09 = cia.latchrundc09.load(std::memory_order_acquire);
    uint8_t alarmdc09 = cia.latchalarmdc09.load(std::memory_order_acquire);
    if (dc09 == alarmdc09)
    {
      uint8_t dc0a = cia.latchrundc0a.load(std::memory_order_acquire);
      uint8_t alarmdc0a = cia.latchalarmdc0a.load(std::memory_order_acquire);
      if (dc0a == alarmdc0a)
      {
        uint8_t dc0b = cia.latchrundc0b.load(std::memory_order_acquire);
        uint8_t alarmdc0b = cia.latchalarmdc0b.load(std::memory_order_acquire);
        if (dc0b == alarmdc0b)
        {
          return true;
        }
      }
    }
  }
  return false;
}

void IRAM_ATTR interruptTODFunc()
{
  if (cpu.cia1.isTODRunning.load(std::memory_order_acquire))
  {
    if (updateTOD(cpu.cia1))
    {
      cpu.cia1.isAlarm.store(true, std::memory_order_release);
    }
  }
  if (cpu.cia2.isTODRunning.load(std::memory_order_acquire))
  {
    if (updateTOD(cpu.cia2))
    {
      cpu.cia2.isAlarm.store(true, std::memory_order_release);
    }
  }
}

void IRAM_ATTR interruptSystemFunc()
{
  checkForKeyboardCnt++;
  if (checkForKeyboardCnt > 50)
  {
    keyboard.handleKeyboard();
    checkForKeyboardCnt = 0;
  }

  // throttle 6502 CPU
  throttlingCnt++;
  uint16_t measuredcyclestmp =
      cpu.measuredcycles.load(std::memory_order_acquire);
  if (measuredcyclestmp > throttlingCnt * INTERRUPTSYSTEMRESOLUTION)
  {
    uint16_t adjustcycles =
        measuredcyclestmp - throttlingCnt * INTERRUPTSYSTEMRESOLUTION;
    cpu.adjustcycles.store(adjustcycles, std::memory_order_release);
    numofburnedcyclespersecond += adjustcycles;
  }
  if (throttlingCnt == 50)
  {
    throttlingCnt = 0;
    cpu.measuredcycles.store(0, std::memory_order_release);
  }
}

void cpuCode(void *parameter)
{
  // interrupt each 100 ms to increment CIA real time clock (TOD)
  interruptTOD = timerBegin(2, 80, true);
  timerAttachInterrupt(interruptTOD, &interruptTODFunc, false);
  timerAlarmWrite(interruptTOD, 100000, true);
  timerAlarmEnable(interruptTOD);

  cpu.run();
}

void vicRefresh(void *parameter)
{
  while (true)
  {
    vic.refresh(cpu.refreshframecolor);
    vTaskDelay(1);
  }
}

void loadFile(void *parameter)
{
  vTaskDelay(3000);
  std::string path = (const char *)parameter;
  if (!path.empty())
  {
    ESP_LOGI(TAG, "load from sdcard... %s", path.c_str());
    cpu.cpuhalted = true;
    size_t pos = path.find_last_of('.');
    std::string ext = path.substr(pos);
    if (ext.compare(".bas") == 0)
    {
      cpu.cpuhalted = false;
      std::string bas = sdcard.loadBas(SD_MMC, path.c_str());
      for (char c : bas)
      {
        keyboard.typeCharacter(c);
      }
      vTaskDelete(NULL);
      return;
    }
    else if (ext.compare(".prg") == 0)
    {
      uint16_t addr = sdcard.loadFile(SD_MMC, path.c_str(), ram);
      if (sdcard.loadAddr == 0)
      {
        ESP_LOGI(TAG, "error loading file");
      }
      else if (sdcard.loadAddr == kBasicPrgStart)
      {
        ram[kBasicTxtTab] = kBasicPrgStart;
        ram[kBasicVarTab] = addr;
        ram[kBasicAryTab] = addr;
        ram[kBasicStrEnd] = addr;
        for (char &c : std::string("RUN\n"))
          keyboard.typeCharacter(c);
      }
      else
      {
        cpu.setPC(addr);
      }
      cpu.cpuhalted = false;
      vTaskDelete(NULL);
      return;
    }
    else
    {
      ESP_LOGI(TAG, "error init sdcard");
    }
    cpu.initMemAndRegs();
    cpu.cpuhalted = false;
  }
  vTaskDelete(NULL);
  return;
}

void C64::run(const std::string &path)
{
  // allocate ram
  ram = new uint8_t[1 << 16];

  // init VIC
  vic.init(ram, charset_rom);

  keyboard.init();

  // init CPU
  cpu.init(ram, charset_rom, &vic, &keyboard);

  // start cpu task
  xTaskCreatePinnedToCore(cpuCode,  // Function to implement the task
                          "CPU",    // Name of the task
                          10000,    // Stack size in words
                          NULL,     // Task input parameter
                          10,       // Priority of the task
                          &cpuTask, // Task handle
                          0);       // Core where the task should run

  // interrupt each 1000 us to get keyboard codes and throttle 6502 CPU
  interruptSystem = timerBegin(1, 80, true);
  timerAttachInterrupt(interruptSystem, &interruptSystemFunc, false);
  timerAlarmWrite(interruptSystem, INTERRUPTSYSTEMRESOLUTION, true);
  timerAlarmEnable(interruptSystem);

  // // profiling: interrupt each second
  interruptProfiling = timerBegin(3, 80, true);
  timerAttachInterrupt(interruptProfiling, &interruptProfilingFunc, false);
  timerAlarmWrite(interruptProfiling, 1000000, true);
  timerAlarmEnable(interruptProfiling);

  xTaskCreatePinnedToCore(vicRefresh,   // Function to implement the task
                          "vicRefresh", // Name of the task
                          10000,        // Stack size in words
                          NULL,         // Task input parameter
                          10,           // Priority of the task
                          &vicTask,     // Task handle
                          1);           // Core where the task should run

  xTaskCreatePinnedToCore(loadFile,             // Function to implement the task
                          "loadFile",           // Name of the task
                          10000,                // Stack size in words
                          (void *)path.c_str(), // Task input parameter
                          10,                   // Priority of the task
                          &loadTask,            // Task handle
                          1);                   // Core where the task should run

  while (!keyboard.reset())
  {
    vTaskDelay(1000);
  }

  vTaskDelete(cpuTask);
  vTaskDelete(vicTask);

  timerAlarmDisable(interruptProfiling);
  timerAlarmDisable(interruptTOD);
  timerAlarmDisable(interruptSystem);
  timerDetachInterrupt(interruptProfiling);
  timerDetachInterrupt(interruptTOD);
  timerDetachInterrupt(interruptSystem);
  timerEnd(interruptProfiling);
  timerEnd(interruptTOD);
  timerEnd(interruptSystem);

  delete ram;
}
