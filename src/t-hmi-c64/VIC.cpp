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
#include "VIC.h"
// #include "ST7789V.h"
#include <cstring>

#define DEBUG_VIC \
  {               \
  } // if (((uint8_t *)canvas.getBuffer())[idx] != 0 && ((uint8_t *)canvas.getBuffer())[idx] != 6) ESP_LOGI(TAG, "%d:%d", idx, ((uint8_t *)canvas.getBuffer())[idx]); }

static const char *TAG = "VIC";

static bool collArr[4] = {false, true, true, true};

void VIC::drawByteStdData(uint8_t data, uint16_t &idx, uint8_t &xp,
                          uint16_t col, uint16_t bgcol, uint8_t dx)
{
  uint8_t bitval = 128;
  for (uint8_t i = 0; i < 8 - dx; i++)
  {
    if (data & bitval)
    {
      ((uint8_t *)canvas.getBuffer())[idx++] = col;
      spritedatacoll[xp++] = true;
    }
    else
    {
      ((uint8_t *)canvas.getBuffer())[idx++] = bgcol;
      DEBUG_VIC;
      xp++;
    }
    bitval >>= 1;
  }
}

void VIC::drawByteMCData(uint8_t data, uint16_t &idx, uint8_t &xp,
                         uint16_t *tftColArr, bool *collArr, uint8_t dx)
{
  uint8_t bitshift = 6;
  for (uint8_t i = 0; i < (8 - dx) >> 1; i++)
  {
    uint8_t bitpair = (data >> bitshift) & 0x03;
    uint16_t tftcolor = tftColArr[bitpair];
    ((uint8_t *)canvas.getBuffer())[idx++] = tftcolor;
    DEBUG_VIC;
    ((uint8_t *)canvas.getBuffer())[idx++] = tftcolor;
    DEBUG_VIC;
    spritedatacoll[xp++] = collArr[bitpair];
    spritedatacoll[xp++] = collArr[bitpair];
    bitshift -= 2;
  }
}

bool VIC::shiftDyDx(uint8_t line, int8_t dy, uint8_t dx, uint16_t bgcol,
                    uint16_t &idx)
{
  only38rows = !(vicreg[0x16] & 8);
  if (line < dy)
  {
    uint16_t framecol = vicreg[0x20] & 15;
    for (uint8_t xp = 0; xp < 8; xp++)
    {
      if (only38rows)
      {
        ((uint8_t *)canvas.getBuffer())[idx++] = framecol;
        DEBUG_VIC;
      }
      else
      {
        ((uint8_t *)canvas.getBuffer())[idx++] = bgcol;
        DEBUG_VIC;
      }
    }
    for (uint16_t xp = 8; xp < 39 * 8; xp++)
    {
      ((uint8_t *)canvas.getBuffer())[idx++] = bgcol;
      DEBUG_VIC;
    }
    for (uint8_t xp = 0; xp < 8; xp++)
    {
      if (only38rows)
      {
        ((uint8_t *)canvas.getBuffer())[idx++] = framecol;
        DEBUG_VIC;
      }
      else
      {
        ((uint8_t *)canvas.getBuffer())[idx++] = bgcol;
        DEBUG_VIC;
      }
    }
    return true;
  }
  for (uint8_t i = 0; i < dx; i++)
  {
    ((uint8_t *)canvas.getBuffer())[idx++] = bgcol;
    DEBUG_VIC;
  }
  return false;
}

void VIC::drawOnly38ColsFrame(uint16_t idx)
{
  if (only38rows)
  {
    uint16_t framecol = vicreg[0x20] & 15;
    for (uint8_t xp = 0; xp < 8; xp++)
    {
      ((uint8_t *)canvas.getBuffer())[idx++] = framecol;
      DEBUG_VIC;
    }
  }
}

void VIC::drawStdCharModeInt(uint8_t *screenMap, uint16_t bgcol, uint8_t row,
                             uint8_t dx, uint8_t &xp, uint16_t idxmap,
                             uint16_t &idx)
{
  uint16_t col = colormap[idxmap] & 15;
  uint8_t ch = screenMap[idxmap];
  uint16_t idxch = ch << 3;
  uint8_t chardata = charset[idxch + row];
  drawByteStdData(chardata, idx, xp, col, bgcol, dx);
}

void VIC::drawStdCharMode(uint8_t *screenMap, uint8_t bgColor, uint8_t line,
                          int8_t dy, uint8_t dx, uint16_t idx)
{
  uint16_t bgcol = bgColor & 15;
  if (shiftDyDx(line, dy, dx, bgcol, idx))
  {
    return;
  }
  uint8_t y = line >> 3;
  uint8_t row = line & 7;
  uint16_t idxmap = y * 40;
  uint8_t xp = 0;
  drawStdCharModeInt(screenMap, bgcol, row, 0, xp, idxmap, idx);
  idxmap++;
  drawOnly38ColsFrame(idx - 8 - dx);
  for (uint8_t x = 1; x < 39; x++)
  {
    drawStdCharModeInt(screenMap, bgcol, row, 0, xp, idxmap, idx);
    idxmap++;
  }
  drawStdCharModeInt(screenMap, bgcol, row, dx, xp, idxmap, idx);
  drawOnly38ColsFrame(idx - 8);
}

void VIC::drawMCCharModeInt(uint8_t *screenMap, uint16_t bgcol,
                            uint16_t *tftColArr, uint8_t row, uint8_t dx,
                            uint8_t &xp, uint16_t idxmap, uint16_t &idx)
{
  uint8_t colc64 = colormap[idxmap] & 15;
  uint8_t ch = screenMap[idxmap];
  uint16_t idxch = ch << 3;
  uint8_t chardata = charset[idxch + row];
  if (colc64 & 8)
  {
    tftColArr[3] = colc64 & 7;
    drawByteMCData(chardata, idx, xp, tftColArr, collArr, dx);
  }
  else
  {
    drawByteStdData(chardata, idx, xp, colc64, bgcol,
                    dx);
  }
}

void VIC::drawMCCharMode(uint8_t *screenMap, uint8_t bgColor, uint8_t color1,
                         uint8_t color2, uint8_t line, int8_t dy, uint8_t dx,
                         uint16_t idx)
{
  uint16_t bgcol = bgColor & 15;
  if (shiftDyDx(line, dy, dx, bgcol, idx))
  {
    return;
  }
  uint16_t tftColArr[4];
  tftColArr[0] = bgcol;
  tftColArr[1] = color1 & 15;
  tftColArr[2] = color2 & 15;
  uint8_t y = line >> 3;
  uint8_t row = line & 7;
  uint16_t idxmap = y * 40;
  uint8_t xp = 0;
  drawMCCharModeInt(screenMap, bgcol, tftColArr, row, 0, xp, idxmap, idx);
  idxmap++;
  drawOnly38ColsFrame(idx - 8 - dx);
  for (uint8_t x = 1; x < 39; x++)
  {
    drawMCCharModeInt(screenMap, bgcol, tftColArr, row, 0, xp, idxmap, idx);
    idxmap++;
  }
  drawMCCharModeInt(screenMap, bgcol, tftColArr, row, dx, xp, idxmap, idx);
  drawOnly38ColsFrame(idx - 8);
}

void VIC::drawExtBGColCharModeInt(uint8_t *screenMap, uint8_t *bgColArr,
                                  uint8_t row, uint8_t dx, uint8_t &xp,
                                  uint16_t idxmap, uint16_t &idx)
{
  uint16_t col = colormap[idxmap] & 15;
  uint8_t ch = screenMap[idxmap];
  uint8_t ch6bits = ch & 0x3f;
  uint16_t bgcol = bgColArr[ch >> 6] & 15;
  uint16_t idxch = ch6bits << 3;
  uint8_t chardata = charset[idxch + row];
  drawByteStdData(chardata, idx, xp, col, bgcol, dx);
}

void VIC::drawExtBGColCharMode(uint8_t *screenMap, uint8_t *bgColArr,
                               uint8_t line, int8_t dy, uint8_t dx,
                               uint16_t idx)
{
  uint8_t bgcol0 = bgColArr[0];
  if (shiftDyDx(line, dy, dx, bgcol0, idx))
  {
    return;
  }
  uint8_t y = line >> 3;
  uint8_t row = line & 7;
  uint16_t idxmap = y * 40;
  uint8_t xp = 0;
  drawExtBGColCharModeInt(screenMap, bgColArr, row, 0, xp, idxmap, idx);
  idxmap++;
  drawOnly38ColsFrame(idx - 8 - dx);
  for (uint8_t x = 1; x < 39; x++)
  {
    drawExtBGColCharModeInt(screenMap, bgColArr, row, 0, xp, idxmap, idx);
    idxmap++;
  }
  drawExtBGColCharModeInt(screenMap, bgColArr, row, dx, xp, idxmap, idx);
  drawOnly38ColsFrame(idx - 8);
}

void VIC::drawMCBitmapModeInt(uint8_t *multicolorBitmap, uint8_t *colorMap1,
                              uint16_t *tftColArr, uint16_t cidx,
                              uint16_t mcidx, uint8_t row, uint8_t dx,
                              uint8_t &xp, uint16_t &idx)
{
  uint8_t color1 = colorMap1[cidx];
  uint8_t color2 = colormap[cidx];
  tftColArr[1] = (color1 >> 4) & 0x0f;
  tftColArr[2] = color1 & 0x0f;
  tftColArr[3] = color2 & 0x0f;
  uint8_t data = multicolorBitmap[mcidx + row];
  drawByteMCData(data, idx, xp, tftColArr, collArr, dx);
}

void VIC::drawMCBitmapMode(uint8_t *multicolorBitmap, uint8_t *colorMap1,
                           uint8_t backgroundColor, uint8_t line, int8_t dy,
                           uint8_t dx, uint16_t idx)
{
  uint16_t tftColArr[4];
  tftColArr[0] = backgroundColor & 0x0f;
  if (shiftDyDx(line, dy, dx, tftColArr[0], idx))
  {
    return;
  }
  uint8_t y = line >> 3;
  uint8_t row = line & 7;
  uint16_t cidx = y * 40;
  uint16_t mcidx = (y * 40) << 3;
  uint8_t xp = 0;
  drawMCBitmapModeInt(multicolorBitmap, colorMap1, tftColArr, cidx, mcidx, row,
                      0, xp, idx);
  cidx++;
  mcidx += 8;
  drawOnly38ColsFrame(idx - 8 - dx);
  for (uint8_t x = 1; x < 39; x++)
  {
    drawMCBitmapModeInt(multicolorBitmap, colorMap1, tftColArr, cidx, mcidx,
                        row, 0, xp, idx);
    cidx++;
    mcidx += 8;
  }
  drawMCBitmapModeInt(multicolorBitmap, colorMap1, tftColArr, cidx, mcidx, row,
                      dx, xp, idx);
  drawOnly38ColsFrame(idx - 8);
}

void VIC::drawStdBitmapModeInt(uint8_t *hiresBitmap, uint8_t *colorMap,
                               uint16_t hiidx, uint16_t &colidx, uint8_t row,
                               uint8_t dx, uint8_t &xp, uint16_t &idx)
{
  uint8_t color = colorMap[colidx++];
  uint8_t colorfg = (color & 0xf0) >> 4;
  uint8_t colorbg = color & 0x0f;
  uint16_t col = colorfg;
  uint16_t bgcol = colorbg;
  uint8_t data = hiresBitmap[hiidx + row];
  drawByteStdData(data, idx, xp, col, bgcol, dx);
}

void VIC::drawStdBitmapMode(uint8_t *hiresBitmap, uint8_t *colorMap,
                            uint8_t line, int8_t dy, uint8_t dx, uint16_t idx)
{
  // todo: background color is specific for each "tile"
  if (shiftDyDx(line, dy, dx, 0, idx))
  {
    return;
  }
  uint8_t y = line >> 3;
  uint8_t row = line & 7;
  uint16_t colidx = y * 40;
  uint16_t hiidx = (y * 40) << 3;
  uint8_t xp = 0;
  drawStdBitmapModeInt(hiresBitmap, colorMap, hiidx, colidx, row, 0, xp, idx);
  hiidx += 8;
  drawOnly38ColsFrame(idx - 8 - dx);
  for (uint8_t x = 1; x < 39; x++)
  {
    drawStdBitmapModeInt(hiresBitmap, colorMap, hiidx, colidx, row, 0, xp, idx);
    hiidx += 8;
  }
  drawStdBitmapModeInt(hiresBitmap, colorMap, hiidx, colidx, row, dx, xp, idx);
  drawOnly38ColsFrame(idx - 8);
}

void VIC::drawSpriteDataSC(uint8_t bitnr, int16_t xpos, uint8_t ypos,
                           uint8_t *data, uint8_t color)
{
  uint16_t tftcolor = color;
  uint16_t idx = xpos + ypos * 320;
  for (uint8_t x = 0; x < 3; x++)
  {
    uint8_t d = *data++;
    uint8_t bitval = 128;
    for (uint8_t i = 0; i < 8; i++)
    {
      if (xpos < 0)
      {
        idx++;
        xpos++;
        continue;
      }
      else if (xpos >= 320)
      {
        return;
      }
      if (d & bitval)
      {
        uint8_t bgspriteprio = vicreg[0x1b] & bitnr;
        if (spritedatacoll[xpos])
        {
          // sprite - data collision
          vicreg[0x1f] |= bitnr;
        }
        if (bgspriteprio && spritedatacoll[xpos])
        {
          // background prio
          idx++;
        }
        else
        {
          ((uint8_t *)canvas.getBuffer())[idx++] = tftcolor;
          DEBUG_VIC;
        }
        uint8_t sprcoll = spritespritecoll[xpos];
        if (sprcoll != 0)
        {
          // sprite - sprite collision
          vicreg[0x1e] |= sprcoll | bitnr;
        }
        spritespritecoll[xpos++] = sprcoll | bitnr;
      }
      else
      {
        idx++;
        xpos++;
      }
      bitval >>= 1;
    }
  }
}

void VIC::drawSpriteDataSCDS(uint8_t bitnr, int16_t xpos, uint8_t ypos,
                             uint8_t *data, uint8_t color)
{
  uint16_t tftcolor = color;
  uint16_t idx = xpos + ypos * 320;
  for (uint8_t x = 0; x < 3; x++)
  {
    uint8_t d = *data++;
    uint8_t bitval = 128;
    for (uint8_t i = 0; i < 8; i++)
    {
      if (xpos < 0)
      {
        idx += 2;
        xpos += 2;
        continue;
      }
      else if (xpos >= 320)
      {
        return;
      }
      if (d & bitval)
      {
        uint8_t bgspriteprio = vicreg[0x1b] & bitnr;
        if (spritedatacoll[xpos])
        {
          // sprite - data collision
          vicreg[0x1f] |= bitnr;
        }
        if (bgspriteprio && spritedatacoll[xpos])
        {
          // background prio
          idx++;
        }
        else
        {
          ((uint8_t *)canvas.getBuffer())[idx++] = tftcolor;
          DEBUG_VIC;
        }
        if (bgspriteprio && spritedatacoll[xpos + 1])
        {
          // background prio
          idx++;
        }
        else
        {
          ((uint8_t *)canvas.getBuffer())[idx++] = tftcolor;
          DEBUG_VIC;
        }
        uint8_t sprcoll = spritespritecoll[xpos];
        if (sprcoll != 0)
        {
          // sprite - sprite collision
          vicreg[0x1e] |= sprcoll | bitnr;
        }
        spritespritecoll[xpos++] = sprcoll | bitnr;
      }
      else
      {
        idx += 2;
        xpos += 2;
      }
      bitval >>= 1;
    }
  }
}

void VIC::drawSpriteDataMC2Bits(uint8_t idxc, uint16_t &idx, int16_t &xpos,
                                uint8_t bitnr, uint16_t *tftcolor)
{
  if (xpos < 0)
  {
    idx += 2;
    xpos += 2;
    return;
  }
  else if (xpos >= 320)
  {
    return;
  }
  if (idxc)
  {
    uint8_t bgspriteprio = vicreg[0x1b] & bitnr;
    if (spritedatacoll[xpos] || spritedatacoll[xpos + 1])
    {
      // sprite - data collision
      vicreg[0x1f] |= bitnr;
    }
    if (bgspriteprio && spritedatacoll[xpos])
    {
      // background prio
      idx++;
    }
    else
    {
      ((uint8_t *)canvas.getBuffer())[idx++] = tftcolor[idxc];
      DEBUG_VIC;
    }
    if (bgspriteprio && spritedatacoll[xpos + 1])
    {
      // background prio
      idx++;
    }
    else
    {
      ((uint8_t *)canvas.getBuffer())[idx++] = tftcolor[idxc];
      DEBUG_VIC;
    }
    uint8_t bitnrcollxpos0 = spritespritecoll[xpos];
    uint8_t bitnrcollxpos1 = spritespritecoll[xpos + 1];
    if (bitnrcollxpos0 != 0)
    {
      // sprite - sprite collision
      vicreg[0x1e] |= bitnrcollxpos0 | bitnr;
    }
    if (bitnrcollxpos1 != 0)
    {
      // sprite - sprite collision
      vicreg[0x1e] |= bitnrcollxpos1 | bitnr;
    }
    spritespritecoll[xpos++] = bitnrcollxpos0 | bitnr;
    spritespritecoll[xpos++] = bitnrcollxpos1 | bitnr;
  }
  else
  {
    idx += 2;
    xpos += 2;
  }
}

void VIC::drawSpriteDataMC(uint8_t bitnr, int16_t xpos, uint8_t ypos,
                           uint8_t *data, uint8_t color10, uint8_t color01,
                           uint8_t color11)
{
  uint16_t tftcolor[4] = {0, color01,
                          color10,
                          color11};
  uint16_t idx = xpos + ypos * 320;
  for (uint8_t x = 0; x < 3; x++)
  {
    uint8_t d = *data++;
    uint8_t idxc = (d & 192) >> 6;
    drawSpriteDataMC2Bits(idxc, idx, xpos, bitnr, tftcolor);
    idxc = (d & 48) >> 4;
    drawSpriteDataMC2Bits(idxc, idx, xpos, bitnr, tftcolor);
    idxc = (d & 12) >> 2;
    drawSpriteDataMC2Bits(idxc, idx, xpos, bitnr, tftcolor);
    idxc = (d & 3);
    drawSpriteDataMC2Bits(idxc, idx, xpos, bitnr, tftcolor);
  }
}

void VIC::drawSpriteDataMCDS(uint8_t bitnr, int16_t xpos, uint8_t ypos,
                             uint8_t *data, uint8_t color10, uint8_t color01,
                             uint8_t color11)
{
  uint16_t tftcolor[4] = {0, color01,
                          color10,
                          color11};
  uint16_t idx = xpos + ypos * 320;
  for (uint8_t x = 0; x < 3; x++)
  {
    uint8_t d = *data++;
    uint8_t idxc = (d & 192) >> 6;
    drawSpriteDataMC2Bits(idxc, idx, xpos, bitnr, tftcolor);
    xpos -= 2;
    drawSpriteDataMC2Bits(idxc, idx, xpos, bitnr, tftcolor);
    idxc = (d & 48) >> 4;
    drawSpriteDataMC2Bits(idxc, idx, xpos, bitnr, tftcolor);
    xpos -= 2;
    drawSpriteDataMC2Bits(idxc, idx, xpos, bitnr, tftcolor);
    idxc = (d & 12) >> 2;
    drawSpriteDataMC2Bits(idxc, idx, xpos, bitnr, tftcolor);
    xpos -= 2;
    drawSpriteDataMC2Bits(idxc, idx, xpos, bitnr, tftcolor);
    idxc = (d & 3);
    drawSpriteDataMC2Bits(idxc, idx, xpos, bitnr, tftcolor);
    xpos -= 2;
    drawSpriteDataMC2Bits(idxc, idx, xpos, bitnr, tftcolor);
  }
}

void VIC::drawSprites(uint8_t line)
{
  uint8_t spritesenabled = vicreg[0x15];
  uint8_t spritesdoubley = vicreg[0x17];
  uint8_t spritesdoublex = vicreg[0x1d];
  uint8_t multicolorreg = vicreg[0x1c];
  uint8_t color01 = vicreg[0x25] & 0x0f;
  uint8_t color11 = vicreg[0x26] & 0x0f;
  memset(spritespritecoll, 0, sizeof(spritespritecoll));
  uint8_t bitval = 128;
  for (int8_t nr = 7; nr >= 0; nr--)
  {
    if (spritesenabled & bitval)
    {
      uint8_t facysize = (spritesdoubley & bitval) ? 2 : 1;
      uint16_t y = vicreg[0x01 + nr * 2];
      if ((line >= y) && (line < (y + 21 * facysize)))
      {
        int16_t x = vicreg[0x00 + nr * 2] - 24;
        if (vicreg[0x10] & bitval)
        {
          x += 256;
        }
        uint8_t ypos = line - 50;
        uint16_t dataaddr = ram[screenmemstart + 1016 + nr] * 64;
        uint8_t *data = ram + vicmem + dataaddr + ((line - y) / facysize) * 3;
        uint8_t col = vicreg[0x27 + nr] & 0x0f;
        if (multicolorreg & bitval)
        {
          if (spritesdoublex & bitval)
          {
            drawSpriteDataMCDS(bitval, x, ypos, data, col, color01, color11);
          }
          else
          {
            drawSpriteDataMC(bitval, x, ypos, data, col, color01, color11);
          }
        }
        else
        {
          if (spritesdoublex & bitval)
          {
            drawSpriteDataSCDS(bitval, x, ypos, data, col);
          }
          else
          {
            drawSpriteDataSC(bitval, x, ypos, data, col);
          }
        }
      }
    }
    bitval >>= 1;
  }
  if (vicreg[0x1f] != 0)
  {
    if (vicreg[0x1a] & 2)
    {
      vicreg[0x19] |= 0x82;
    }
    else
    {
      vicreg[0x19] |= 0x02;
    }
  }
  if (vicreg[0x1e] != 0)
  {
    if (vicreg[0x1a] & 4)
    {
      vicreg[0x19] |= 0x84;
    }
    else
    {
      vicreg[0x19] |= 0x04;
    }
  }
}

void VIC::initVarsAndRegs()
{
  for (uint8_t i = 0; i < 0x40; i++)
  {
    vicreg[i] = 0;
  }
  vicreg[0x11] = 0x1b;
  vicreg[0x16] = 0xc8;
  vicreg[0x18] = 0x15;
  vicreg[0x19] = 0x71;
  vicreg[0x1a] = 0xf0;

  cntRefreshs = 0;
  // syncd020 = 0;
  vicmem = 0;
  bitmapstart = 0x2000;
  screenmemstart = 1024;
  cntRefreshs = 0;
  rasterline = 0;
  charset = chrom;
}

void VIC::init(uint8_t *ram, uint8_t *charrom)
{
  this->ram = ram;
  this->chrom = charrom;

  // allocate bitmap memory to be transfered to LCD
  // (consider xscroll and yscroll offset)
  // bitmap = new uint8_t[320 * (200 + 7) + 7]();
  colormap = new uint8_t[40 * 25]();

  canvas.setColorDepth(lgfx::v1::palette_8bit);
  canvas.createPalette();
  canvas.createSprite(320, 208);
  canvas.setPivot(160, 100);

  canvas.setPaletteColor(0, lgfx::v1::bgr888_t(0x00, 0x00, 0x00));
  canvas.setPaletteColor(1, lgfx::v1::bgr888_t(0xff, 0xff, 0xff));
  canvas.setPaletteColor(2, lgfx::v1::bgr888_t(0xab, 0x31, 0x26));
  canvas.setPaletteColor(3, lgfx::v1::bgr888_t(0x66, 0xda, 0xff));
  canvas.setPaletteColor(4, lgfx::v1::bgr888_t(0xbb, 0x3f, 0xb8));
  canvas.setPaletteColor(5, lgfx::v1::bgr888_t(0x55, 0xce, 0x58));
  canvas.setPaletteColor(6, lgfx::v1::bgr888_t(0x1d, 0x0e, 0x97));
  canvas.setPaletteColor(7, lgfx::v1::bgr888_t(0xea, 0xf5, 0x7c));
  canvas.setPaletteColor(8, lgfx::v1::bgr888_t(0xb9, 0x74, 0x18));
  canvas.setPaletteColor(9, lgfx::v1::bgr888_t(0x78, 0x53, 0x00));
  canvas.setPaletteColor(10, lgfx::v1::bgr888_t(0xdd, 0x93, 0x87));
  canvas.setPaletteColor(11, lgfx::v1::bgr888_t(0x5b, 0x5b, 0x5b));
  canvas.setPaletteColor(12, lgfx::v1::bgr888_t(0x8b, 0x8b, 0x8b));
  canvas.setPaletteColor(13, lgfx::v1::bgr888_t(0xb0, 0xf4, 0xac));
  canvas.setPaletteColor(14, lgfx::v1::bgr888_t(0xaa, 0x9d, 0xef));
  canvas.setPaletteColor(15, lgfx::v1::bgr888_t(0xb8, 0xb8, 0xb8));

  initVarsAndRegs();

  // init LCD driver
  // ST7789V::init();
}

// void VIC::checkFrameColor()
//{
//   //uint8_t framecol = vicreg[0x20] & 15;
//   //if (framecol != syncd020)
//   //{
//   //  syncd020 = framecol;
//   //  // ST7789V::drawFrame(framecol]);
//   //}
// }

void VIC::refresh(bool refreshframecolor)
{
  canvas.setClipRect(0, 0, 320, 200);
  canvas.pushRotateZoomWithAA(&M5Cardputer.Display, M5Cardputer.Display.width() / 2, M5Cardputer.Display.height() / 2, 0, 0.675, 0.675);
  // ST7789V::drawBitmap();
  // if (refreshframecolor) {
  //   checkFrameColor();
  // }

  cntRefreshs++;
}

uint8_t VIC::nextRasterline()
{
  rasterline++;
  if (rasterline > 311)
  {
    rasterline = 0;
  }
  uint8_t ld011 = (rasterline >= 256) ? 0x80 : 0;
  vicreg[0x11] &= 0x7f;
  vicreg[0x11] |= ld011;
  uint8_t vicregd012 = (rasterline & 0xff);
  vicreg[0x12] = vicregd012;
  if ((latchd012 == vicregd012) && ((latchd011 & 0x80) == ld011))
  {
    if (vicreg[0x1a] & 1)
    {
      vicreg[0x19] |= 0x81;
    }
    else
    {
      vicreg[0x19] |= 0x01;
    }
  }
  // badline?
  if (((vicreg[0x11] & 7) == (vicregd012 & 7)) && (vicregd012 >= 0x30) &&
      (vicregd012 <= 0xf7))
  {
    return 40;
  }
  return 0;
}

void VIC::drawRasterline()
{
  uint16_t line = rasterline;
  uint8_t d011 = vicreg[0x11];
  uint8_t deltay = d011 & 7;
  if ((line >= 50) && (line < 250))
  {
    uint8_t d016 = vicreg[0x16];
    uint8_t deltax = d016 & 7;
    uint8_t dline = line - 50;
    int32_t idx = (dline + deltay - 3) * 320;
    if ((idx >= 0) && (idx <= 63680))
    {
      memset(spritedatacoll, false, sizeof(bool) * sizeof(spritedatacoll));
      bool bmm = d011 & 32;
      bool ecm = d011 & 64;
      bool mcm = d016 & 16;
      if (bmm)
      {
        if (mcm)
        {
          drawMCBitmapMode(ram + bitmapstart, ram + screenmemstart,
                           vicreg[0x21], dline, deltay - 3, deltax, idx);
        }
        else
        {
          drawStdBitmapMode(ram + bitmapstart, ram + screenmemstart, dline,
                            deltay - 3, deltax, idx);
        }
      }
      else
      {
        if ((!ecm) && (!mcm))
        {
          drawStdCharMode(ram + screenmemstart, vicreg[0x21], dline, deltay - 3,
                          deltax, idx);
        }
        else if ((!ecm) && mcm)
        {
          drawMCCharMode(ram + screenmemstart, vicreg[0x21], vicreg[0x22],
                         vicreg[0x23], dline, deltay - 3, deltax, idx);
        }
        else if (ecm && (!mcm))
        {
          uint8_t bgColArr[] = {vicreg[0x21], vicreg[0x22], vicreg[0x23],
                                vicreg[0x24]};
          drawExtBGColCharMode(ram + screenmemstart, bgColArr, dline,
                               deltay - 3, deltax, idx);
        }
      }
    }
    drawSprites(line + deltay - 3);
  }
}
