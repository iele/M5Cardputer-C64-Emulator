#include "M5Cardputer.h"
#include <SD_MMC.h>
#include <FFat.h>
#include <FS.h>

#include "sd.hpp"

String fileList[MAXFILES][3];

#define MAX_ITEMS 6
void listFiles(int index, String fileList[][3])
{
    M5Canvas canvas;
    canvas.createSprite(WIDTH, HEIGHT);
    canvas.fillScreen(BGCOLOR);
    canvas.fillRoundRect(0, 0, WIDTH, 20, 5, FGCOLOR);

    canvas.setCursor(10, 14);
    canvas.setTextColor(TFT_BLACK, FGCOLOR);
    canvas.setFont(&DejaVu12);
    canvas.setTextDatum(L_BASELINE);
    canvas.setTextSize(FM);
    canvas.println("C64 Emulator");
    canvas.setTextDatum(R_BASELINE);
    char batteryStr[4];
    sprintf(batteryStr, "%d\n", M5Cardputer.Power.getBatteryLevel());
    canvas.setCursor(WIDTH - 10 - canvas.textWidth(batteryStr), 14);
    canvas.println(batteryStr);

    canvas.setFont(&Font2);
    canvas.setTextDatum(L_BASELINE);
    canvas.setTextSize(FM);
    int arraySize = 0;
    while (fileList[arraySize][2] != "" && arraySize < MAXFILES)
        arraySize++;
    int i = 0;
    int start = 0;
    if (index >= MAX_ITEMS)
    {
        start = index - MAX_ITEMS + 1;
        if (start < 0)
            start = 0;
    }

    canvas.setCursor(10, 40);
    while (i < arraySize)
    {
        canvas.setCursor(10, canvas.getCursorY());
        if (i >= start && fileList[i][2] != "")
        {
            if (fileList[i][2] == "folder")
                canvas.setTextColor(FGCOLOR - 0x1111, TFT_BLACK);
            else if (fileList[i][2] == "operator")
                canvas.setTextColor(ALCOLOR, TFT_BLACK);
            else
                canvas.setTextColor(FGCOLOR, TFT_BLACK);

            if (index == i)
                canvas.print(">");
            else
                canvas.print(" ");
            canvas.println(fileList[i][0].substring(0, 30));
        }
        i++;
        if (i == (start + MAX_ITEMS) || fileList[i][2] == "")
            break;
    }
    canvas.pushSprite(&M5Cardputer.Display, 0, 0);
    canvas.deleteSprite();
}

void sortList(String fileList[][3], int fileListCount)
{
    bool swapped;
    String temp[3];
    String name1, name2;

    do
    {
        swapped = false;
        for (int i = 0; i < fileListCount - 1; i++)
        {
            name1 = fileList[i][0];
            name1.toUpperCase();
            name2 = fileList[i + 1][0];
            name2.toUpperCase();

            bool isFolder1 = fileList[i][2] == "folder";
            bool isFolder2 = fileList[i + 1][2] == "folder";

            if (isFolder1 && !isFolder2)
            {
                continue;
            }
            else if (!isFolder1 && isFolder2)
            {
                for (int j = 0; j < 3; j++)
                {
                    temp[j] = fileList[i][j];
                    fileList[i][j] = fileList[i + 1][j];
                    fileList[i + 1][j] = temp[j];
                }
                swapped = true;
            }
            else
            {
                if (name1.compareTo(name2) > 0)
                {
                    for (int j = 0; j < 2; j++)
                    {
                        temp[j] = fileList[i][j];
                        fileList[i][j] = fileList[i + 1][j];
                        fileList[i + 1][j] = temp[j];
                    }
                    swapped = true;
                }
            }
        }
    } while (swapped);
}

void readFs(FS fs, String folder, String result[][3])
{

    int allFilesCount = 0;
    while (allFilesCount < MAXFILES)
    {
        result[allFilesCount][0] = "";
        result[allFilesCount][1] = "";
        result[allFilesCount][2] = "";
        allFilesCount++;
    }
    allFilesCount = 0;
    File root = fs.open(folder);
    if (!root || !root.isDirectory())
    {
        result[allFilesCount][0] = "> Back";
        folder = folder.substring(0, folder.lastIndexOf('/'));
        if (folder == "")
            folder = "/";
        result[allFilesCount][1] = folder;
        result[allFilesCount][2] = "operator";
        return;
    }

    File file2 = root.openNextFile();
    while (file2 && allFilesCount < (MAXFILES - 1))
    {
        String fileName = file2.name();
        if (!file2.isDirectory())
        {
            String ext = fileName.substring(fileName.lastIndexOf(".") + 1);
            ext.toUpperCase();
            if ((ext.equals("BAS") || ext.equals("PRG")) && !fileName.startsWith("."))
            {
                result[allFilesCount][0] = fileName.substring(fileName.lastIndexOf("/") + 1);
                result[allFilesCount][1] = file2.path();
                result[allFilesCount][2] = "file";
                allFilesCount++;
            }
        }

        file2 = root.openNextFile();
    }
    file2.close();
    root.close();

    root = fs.open(folder);
    File file = root.openNextFile();
    while (file && allFilesCount < (MAXFILES - 1))
    {
        String fileName = file.name();
        if (file.isDirectory() && !fileName.startsWith("."))
        {
            result[allFilesCount][0] = fileName.substring(fileName.lastIndexOf("/") + 1);
            result[allFilesCount][1] = file.path();
            result[allFilesCount][2] = "folder";
            allFilesCount++;
        }
        file = root.openNextFile();
    }
    file.close();
    root.close();

    sortList(result, allFilesCount);
    result[allFilesCount][0] = "> Back";
    folder = folder.substring(0, folder.lastIndexOf('/'));
    if (folder == "")
        folder = "/";
    result[allFilesCount][1] = folder;
    result[allFilesCount][2] = "operator";
}

String loopSD(FS &fs)
{
    String result = "";
    bool reload = false;
    bool redraw = true;
    int index = 0;
    int maxFiles = 0;
    String Folder = "/";
    String PreFolder = "/";

    M5Cardputer.Display.fillScreen(BGCOLOR);

    readFs(fs, Folder, fileList);

    for (int i = 0; i < MAXFILES; i++)
        if (fileList[i][2] != "")
            maxFiles++;
        else
            break;

    while (1)
    {
        delay(150);
        M5Cardputer.update();
        if (redraw)
        {
            if (strcmp(PreFolder.c_str(), Folder.c_str()) != 0 || reload)
            {
                index = 0;
                readFs(fs, Folder, fileList);
                PreFolder = Folder;
                maxFiles = 0;
                for (int i = 0; i < MAXFILES; i++)
                    if (fileList[i][2] != "")
                        maxFiles++;
                    else
                        break;
                reload = false;
            }
            listFiles(index, fileList);
            ;
            redraw = false;
        }

        if (M5Cardputer.Keyboard.isKeyPressed(';'))
        {
            if (index == 0)
                index = maxFiles - 1;
            else if (index > 0)
                index--;
            redraw = true;
        }
        if (M5Cardputer.Keyboard.isKeyPressed('.'))
        {
            index++;
            if (index == maxFiles)
                index = 0;
            redraw = true;
        }

        if (M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER))
        {
            goto Files;
        }
        continue;

    Files:
        if (fileList[index][2] == "folder")
        {
            Folder = fileList[index][1];
            redraw = true;
        }
        else if (fileList[index][2] == "file")
        {
            result = fileList[index][1];
            break;
        }
        else
        {
            if (Folder == "/")
                break;
            Folder = fileList[index][1];
            index = 0;
            redraw = true;
        }
        redraw = true;
    }
    return result;
}

String menu()
{
start:
    bool reload = false;
    bool redraw = true;
    int index = 0;
    int maxFiles = 2;

    M5Cardputer.Display.fillScreen(BGCOLOR);

    while (1)
    {
        delay(150);
        M5Cardputer.update();
        if (redraw)
        {
            int allFilesCount = 0;
            while (allFilesCount < MAXFILES)
            {
                fileList[allFilesCount][0] = "";
                fileList[allFilesCount][1] = "";
                fileList[allFilesCount][2] = "";
                allFilesCount++;
            }
            fileList[0][0] = "> Direct to BASIC";
            fileList[0][1] = "";
            fileList[0][2] = "operator";
            fileList[1][0] = "> Load from SD";
            fileList[1][1] = "";
            fileList[1][2] = "operator";

            listFiles(index, fileList);
            redraw = false;
        }
        if (M5Cardputer.Keyboard.isKeyPressed(';'))
        {
            if (index == 0)
                index = maxFiles - 1;
            else if (index > 0)
                index--;
            redraw = true;
        }
        if (M5Cardputer.Keyboard.isKeyPressed('.'))
        {
            index++;
            if (index == maxFiles)
                index = 0;
            redraw = true;
        }

        if (M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER))
        {
            goto Files;
        }

        continue;

    Files:
        switch (index)
        {
        case 0:
        {
            return "";
        }
        case 1:
        {
            auto path = loopSD(SD_MMC);
            if (path.equals(""))
            {
                redraw = true;
                continue;
            }
            return path;
        }
        }
    }
}
