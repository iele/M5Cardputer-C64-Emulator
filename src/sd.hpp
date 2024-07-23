#pragma once

#include <FS.h>
#include <SD_MMC.h>

#define MAXFILES 256
#define LH 8
#define LW 6
#define FP 1
#define FM 1
#define FG

#define WIDTH M5Cardputer.Display.width()
#define HEIGHT M5Cardputer.Display.height()
#define FGCOLOR TFT_WHITE
#define BGCOLOR TFT_BLACK
#define ALCOLOR TFT_RED

void readFs(FS fs, String folder, String result[][3]);

void sortList(String fileList[][3], int fileListCount);

String loopSD(FS &fs);

String menu();