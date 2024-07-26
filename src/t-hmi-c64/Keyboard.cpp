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

#include "Keyboard.h"

static const char *TAG = "Keyboard";

void Keyboard::init()
{
  /* init keyboard matrix state */
  for (size_t i = 0; i < sizeof(keyboard_matrix_); i++)
  {
    keyboard_matrix_[i] = 0xff;
  }

  charmap_['\n'] = {CODE_RETURN};
  charmap_[' '] = {CODE_SPACE};
  charmap_['!'] = {CODE_BANG};
  charmap_['"'] = {CODE_QOUTE};
  charmap_['#'] = {CODE_HASH};
  charmap_['$'] = {CODE_DOLLAR};
  charmap_['%'] = {CODE_PERCENT};
  charmap_['&'] = {CODE_AND};
  charmap_[','] = {CODE_COMMA};
  charmap_['('] = {CODE_LEFTPARENTHESES};
  charmap_[')'] = {CODE_RIGHTPARENTHESES};
  charmap_['*'] = {CODE_STAR};
  charmap_['+'] = {CODE_PLUS};
  charmap_['-'] = {CODE_MINUS};
  charmap_['.'] = {CODE_PERIOD};
  charmap_['/'] = {CODE_SLASH};
  charmap_['0'] = {CODE_0};
  charmap_['1'] = {CODE_1};
  charmap_['2'] = {CODE_2};
  charmap_['3'] = {CODE_3};
  charmap_['4'] = {CODE_4};
  charmap_['5'] = {CODE_5};
  charmap_['6'] = {CODE_6};
  charmap_['7'] = {CODE_7};
  charmap_['8'] = {CODE_8};
  charmap_['9'] = {CODE_9};
  charmap_[':'] = {CODE_COLON};
  charmap_[';'] = {CODE_SEMICOLON};
  charmap_['<'] = {CODE_LEFTCHEVRONS};
  charmap_['='] = {CODE_EUQAL};
  charmap_['>'] = {CODE_RIGHTCHEVRONS};
  charmap_['?'] = {CODE_QUESTION};
  charmap_['@'] = {CODE_AT};
  charmap_['A'] = {CODE_A};
  charmap_['B'] = {CODE_B};
  charmap_['C'] = {CODE_C};
  charmap_['D'] = {CODE_D};
  charmap_['E'] = {CODE_E};
  charmap_['F'] = {CODE_F};
  charmap_['G'] = {CODE_G};
  charmap_['H'] = {CODE_H};
  charmap_['I'] = {CODE_I};
  charmap_['J'] = {CODE_J};
  charmap_['K'] = {CODE_K};
  charmap_['L'] = {CODE_L};
  charmap_['M'] = {CODE_M};
  charmap_['N'] = {CODE_N};
  charmap_['O'] = {CODE_O};
  charmap_['P'] = {CODE_P};
  charmap_['Q'] = {CODE_Q};
  charmap_['R'] = {CODE_R};
  charmap_['S'] = {CODE_S};
  charmap_['T'] = {CODE_T};
  charmap_['U'] = {CODE_U};
  charmap_['V'] = {CODE_V};
  charmap_['W'] = {CODE_W};
  charmap_['X'] = {CODE_X};
  charmap_['Y'] = {CODE_Y};
  charmap_['Z'] = {CODE_Z};
  charmap_['['] = {CODE_LEFTBRACES};
  charmap_[']'] = {CODE_RIGHTBRACES};
  charmap_['^'] = {CODE_CARET};
  charmap_['`'] = {CODE_BACKQOUTE};
  charmap_['\''] = {CODE_BACKQOUTE};
  // charmap_['a'] = {CODE_LA};
  // charmap_['b'] = {CODE_LB};
  // charmap_['c'] = {CODE_LC};
  // charmap_['d'] = {CODE_LD};
  // charmap_['e'] = {CODE_LE};
  // charmap_['f'] = {CODE_LF};
  // charmap_['g'] = {CODE_LG};
  // charmap_['h'] = {CODE_LH};
  // charmap_['i'] = {CODE_LI};
  // charmap_['j'] = {CODE_LJ};
  // charmap_['k'] = {CODE_LK};
  // charmap_['l'] = {CODE_LL};
  // charmap_['m'] = {CODE_LM};
  // charmap_['n'] = {CODE_LN};
  // charmap_['o'] = {CODE_LO};
  // charmap_['p'] = {CODE_LP};
  // charmap_['q'] = {CODE_LQ};
  // charmap_['r'] = {CODE_LR};
  // charmap_['s'] = {CODE_LS};
  // charmap_['t'] = {CODE_LT};
  // charmap_['u'] = {CODE_LU};
  // charmap_['v'] = {CODE_LV};
  // charmap_['w'] = {CODE_LW};
  // charmap_['x'] = {CODE_LX};
  // charmap_['y'] = {CODE_LY};
  // charmap_['z'] = {CODE_LZ};
  charmap_['{'] = {CODE_LEFTBRACES};
  charmap_['}'] = {CODE_RIGHTBRACES};

  joystickMode_ = false;
  reset_ = false;
}

void Keyboard::handleKeyDown(CODE k)
{
  try
  {
    CODE code = k;
    if (k > CODE_LOWER)
    {
      keyboard_matrix_[1] &= 0b01111111;
      if (k < CODE_EXT)
      {
        code = (CODE)((int)k - CODE_LOWER);
        keyboard_matrix_[7] &= 0b11011111;
      }
      else
      {
        code = (CODE)((int)k - CODE_EXT);
      }
    }
    uint8_t first = (k >> 3) & 0x7;
    uint8_t second = k & 0x7;
    uint8_t mask = ~(1 << second);
    keyboard_matrix_[first] &= mask;
  }
  catch (const std::out_of_range)
  {
  }
}

void Keyboard::handleKeyUp(CODE k)
{
  try
  {
    CODE code = k;
    if (k > CODE_LOWER)
    {
      keyboard_matrix_[1] |= 0b10000000;
      if (k < CODE_EXT)
      {
        code = (CODE)((int)k - CODE_EXT);
        keyboard_matrix_[7] |= 0b00100000;
      }
      else
      {
        code = (CODE)((int)k - CODE_LOWER);
      }
    }

    if (k > CODE_EXT)
    {
      keyboard_matrix_[1] |= 0b10000000;
    }
    uint8_t first = (k >> 3) & 0x7;
    uint8_t second = k & 0x7;
    uint8_t mask = 1 << second;
    keyboard_matrix_[first] |= mask;
  }
  catch (const std::out_of_range)
  {
  }
}

void Keyboard::handleKeyboard()
{
  if (!key_event_queue_.empty())
  {
    std::pair<kKeyEvent, CODE> &ev = key_event_queue_.front();
    key_event_queue_.pop();
    switch (ev.first)
    {
    case kPress:
      handleKeyDown(ev.second);
      break;
    case kRelease:
      handleKeyUp(ev.second);
      break;
    }
    return;
  }

  M5Cardputer.Keyboard.updateKeyList();
  M5Cardputer.Keyboard.updateKeysState();

  //if (M5Cardputer.BtnA.wasHold())
  //{
  //  reset_ = ~reset_;
  //}
  //if (M5Cardputer.BtnA.wasClicked())
  //{
  //  joystickMode_ = joystickMode_ ? false : true;
  //}

  std::vector<Point2D_t> keys = M5Cardputer.Keyboard.keyList();
  if (M5Cardputer.Keyboard.isKeyPressed('`'))
  {
    joystickMode_ = joystickMode_ < 2 ? joystickMode_ + 1 : 0;
  }

  if (joystickMode_ != 0)
  {
    joystickValue = 0xff;
    joystickFire = false;
    // up
    if (M5Cardputer.Keyboard.isKeyPressed(';'))
      joystickValue &= ~(1 << C64JOYUP);
    // left
    if (M5Cardputer.Keyboard.isKeyPressed(','))
      joystickValue &= ~(1 << C64JOYLEFT);
    // down
    if (M5Cardputer.Keyboard.isKeyPressed('.'))
      joystickValue &= ~(1 << C64JOYDOWN);
    // right
    if (M5Cardputer.Keyboard.isKeyPressed('/'))
      joystickValue &= ~(1 << C64JOYRIGHT);
    // fire
    if (M5Cardputer.Keyboard.isKeyPressed(KEY_TAB))
    {
      joystickValue &= ~(1 << C64JOYFIRE);
      joystickFire = true;
    }
  }
  else
  {
    bool new_kb_state[KEY_SIZE] = {0};
    // map key_code
    bool use_upper = false;

    uint8_t key_code;
    for (auto &i : keys)
    {
      key_code = kb_map[i.y][i.x];
      if (key_code == CODE_EXT)
      {
        use_upper = true;
        break;
      }
    }

    for (auto &i : keys)
    {
      key_code = use_upper ? kb_map_upper[i.y][i.x] : kb_map[i.y][i.x];
      if (key_code == CODE_INVALID || key_code == CODE_EXT)
      {
        continue;
      }
      new_kb_state[key_code] = true;
    }

    for (int i = 0; i < KEY_SIZE; i++)
    {
      if (new_kb_state[i] == true && kb_state[i] == false)
      {
        ESP_LOGI(TAG, "key down %d", i);
        handleKeyDown((CODE)i);
      }
      if (new_kb_state[i] == false && kb_state[i] == true)
      {
        ESP_LOGI(TAG, "key up %d", i);
        handleKeyUp((CODE)i);
      }
      kb_state[i] = new_kb_state[i];
    }
  }
}

void Keyboard::typeCharacter(char c)
{
  try
  {
    for (CODE &k : charmap_.at(toupper(c)))
      key_event_queue_.push(std::make_pair(kPress, k));
    for (CODE &k : charmap_.at(toupper(c)))
      key_event_queue_.push(std::make_pair(kRelease, k));
  }
  catch (const std::out_of_range)
  {
  }
}
