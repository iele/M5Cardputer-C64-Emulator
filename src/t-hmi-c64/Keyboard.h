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

#include <queue>
#include <unordered_map>
#include <mutex>
#include "M5Cardputer.h"
#include "utility/Button_Class.hpp"

#define KEY_SIZE 0xFF
#define INLINE IRAM_ATTR inline __attribute__((always_inline, flatten))

// clang-format off
typedef enum code {
  CODE_DELETE, CODE_RETURN, CODE_CRSRRT,    CODE_F7,    CODE_F1,     CODE_F3,    CODE_F5,    CODE_CRSRDN,
  CODE_3,      CODE_W,      CODE_A,         CODE_4,     CODE_Z,      CODE_S,     CODE_E,     CODE_LSHIFT,
  CODE_5,      CODE_R,      CODE_D,         CODE_6,     CODE_C,      CODE_F,     CODE_T,     CODE_X,
  CODE_7,      CODE_Y,      CODE_G,         CODE_8,     CODE_B,      CODE_H,     CODE_U,     CODE_V,
  CODE_9,      CODE_I,      CODE_J,         CODE_0,     CODE_M,      CODE_K,     CODE_O,     CODE_N,
  CODE_PLUS,   CODE_P,      CODE_L,         CODE_MINUS, CODE_PERIOD, CODE_COLON, CODE_AT,    CODE_COMMA,
  CODE_POUND,  CODE_STAR,   CODE_SEMICOLON, CODE_HOME,  CODE_RSHIFT, CODE_EQUALS, CODE_CARET, CODE_SLASH,
  CODE_1,      CODE_BACK,   CODE_CTRL,      CODE_2,     CODE_SPACE,  CODE_GUI,   CODE_Q,     CODE_STOP,

  //CODE_LW = 73, CODE_LA = 74, CODE_LZ = 76, CODE_LS = 77, CODE_LE = 78,
  //CODE_LR = 81, CODE_LD = 82, CODE_LC = 84, CODE_LF = 85, CODE_LT = 86,
  //CODE_LX = 87, CODE_LY = 89, CODE_LG = 90, CODE_LB = 92, CODE_LH = 93,
  //CODE_LU = 94, CODE_LV = 95, CODE_LI = 96, CODE_LJ = 97, CODE_LM = 98,
  //CODE_LK = 99, CODE_LO = 100, CODE_LN = 101, CODE_LP = 105, CODE_LL = 106,
  //CODE_LQ = 126,

  CODE_BANG = 184,            //!
  CODE_QUOTE = 187,           //"
  CODE_HASH = 136,            // #
  CODE_DOLLAR = 139,          //$
  CODE_PERCENT = 144,         //%
  CODE_AND = 147,             //&
  CODE_BACKQOUTE = 152,       //`
  CODE_LEFTPARENTHESES = 155, //(
  CODE_RIGHTPARENTHESES = 160,//)
  CODE_LEFTBRACES = 173,      //[
  CODE_RIGHTBRACES = 178,     //]
  CODE_LEFTCHEVRONS = 175,    //<
  CODE_RIGHTCHEVRONS = 172,   //>
  CODE_QUESTION= 183,         //?

  CODE_LOWER = 64,
  CODE_EXT = 128,
  CODE_RESTORE = 254,
  CODE_INVALID = 255,
} CODE;

// 0x00______ code
// 0x11111111 invalid
// 0x10______ SHIFT + code

const CODE kb_map[4][14] = {
    {CODE_STOP,    CODE_1,      CODE_2,      CODE_3, CODE_4, CODE_5, CODE_6, CODE_7, CODE_8, CODE_9, CODE_0,     CODE_HOME,       CODE_EQUALS,      CODE_BACK},
    {CODE_RESTORE, CODE_Q,      CODE_W,      CODE_E, CODE_R, CODE_T, CODE_Y, CODE_U, CODE_I, CODE_O, CODE_P,     CODE_LEFTBRACES, CODE_RIGHTBRACES, CODE_SLASH},
    {CODE_GUI,     CODE_EXT,    CODE_A,      CODE_S, CODE_D, CODE_F, CODE_G, CODE_H, CODE_J, CODE_K, CODE_L,     CODE_SEMICOLON,  CODE_BACKQOUTE,   CODE_RETURN},
    {CODE_CTRL,    CODE_LSHIFT, CODE_RSHIFT, CODE_Z, CODE_X, CODE_C, CODE_V, CODE_B, CODE_N, CODE_M, CODE_COMMA, CODE_PERIOD,     CODE_SLASH,       CODE_SPACE}};

// when Aa pressed
const CODE kb_map_upper[4][14] = {
    {CODE_F1, CODE_BANG,    CODE_AT,     CODE_HASH, CODE_DOLLAR, CODE_PERCENT, CODE_CARET, CODE_AND, CODE_STAR, CODE_LEFTPARENTHESES, CODE_RIGHTPARENTHESES, CODE_MINUS,         CODE_PLUS,     CODE_DELETE},
    {CODE_F3, CODE_Q,       CODE_W,      CODE_E,    CODE_R,      CODE_T,       CODE_Y,     CODE_U,   CODE_I,    CODE_O,               CODE_P,                CODE_INVALID,       CODE_INVALID,  CODE_POUND},
    {CODE_F5, CODE_INVALID, CODE_A,      CODE_S,    CODE_D,      CODE_F,       CODE_G,     CODE_H,   CODE_J,    CODE_K,               CODE_L,                CODE_COLON,         CODE_QUOTE,    CODE_RETURN},
    {CODE_F7, CODE_CRSRDN,  CODE_CRSRRT, CODE_Z,    CODE_X,      CODE_C,       CODE_V,     CODE_B,   CODE_N,    CODE_M,               CODE_LEFTCHEVRONS,     CODE_RIGHTCHEVRONS, CODE_QUESTION, CODE_SPACE}};
// clang-format on

static const uint8_t C64JOYUP = 0;
static const uint8_t C64JOYDOWN = 1;
static const uint8_t C64JOYLEFT = 2;
static const uint8_t C64JOYRIGHT = 3;
static const uint8_t C64JOYFIRE = 4;

class Keyboard
{
private:
  bool kb_state[KEY_SIZE] = {0};
  uint8_t keyboard_matrix_[8];
  std::atomic<int> joystickMode_;
  std::atomic<bool> reset_;
  std::atomic<bool> restore_;
  bool retval_ = true;
  /* keyboard mappings */
  std::unordered_map<char, std::vector<CODE>> charmap_;
  enum kKeyEvent
  {
    kPress,
    kRelease,
  };
  /* key events */
  std::queue<std::pair<kKeyEvent, CODE>> key_event_queue_;

  uint8_t joystickValue;
  bool joystickFire;

  m5::Button_Class btnA;
public:
  INLINE uint8_t keyboard_matrix_row(int col) { return keyboard_matrix_[col]; };
  INLINE int joystickMode() { return joystickMode_; };
  INLINE bool reset() { return reset_; };
  INLINE bool restore() { return restore_; };
  void init();
  void handleKeyboard();
  void handleKeyUp(CODE k);
  void handleKeyDown(CODE k);
  void typeCharacter(char c);

  INLINE uint8_t getJoyStickValue(bool port2, uint8_t dc00, uint8_t dc02)
  {
    uint8_t value = joystickValue;
    if (port2 && ((dc02 & 0x7f) == 0x7f))
    {
      value &= 0x7f;
    }
    return value | (dc00 & 0x80);
  }

  INLINE bool getJoyStickFire() { return joystickFire; };
};