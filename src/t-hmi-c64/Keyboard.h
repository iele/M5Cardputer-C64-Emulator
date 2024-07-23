#pragma once

#include <M5Cardputer.h>
#include <queue>
#include <unordered_map>
#include <mutex>

#define KEY_SIZE 0xFF
#define INLINE IRAM_ATTR inline __attribute__((always_inline, flatten))

typedef enum code {
  CODE_DELETE, CODE_RETURN, CODE_CRSRRT,    CODE_F7,    CODE_F1,     CODE_F3,    CODE_F5,    CODE_CRSRDN,
  CODE_3,      CODE_W,      CODE_A,         CODE_4,     CODE_Z,      CODE_S,     CODE_E,     CODE_LSHIFT,
  CODE_5,      CODE_R,      CODE_D,         CODE_6,     CODE_C,      CODE_F,     CODE_T,     CODE_X,
  CODE_7,      CODE_Y,      CODE_G,         CODE_8,     CODE_B,      CODE_H,     CODE_U,     CODE_V,
  CODE_9,      CODE_I,      CODE_J,         CODE_0,     CODE_M,      CODE_K,     CODE_O,     CODE_N,
  CODE_PLUS,   CODE_P,      CODE_L,         CODE_MINUS, CODE_PERIOD, CODE_COLON, CODE_AT,    CODE_COMMA,
  CODE_POUND,  CODE_STAR,   CODE_SEMICOLON, CODE_HOME,  CODE_RSHIFT, CODE_EUQAL, CODE_CARET, CODE_SLASH,
  CODE_1,      CODE_BACK,   CODE_CTRL,      CODE_2,     CODE_SPACE,  CODE_GUI,   CODE_Q,     CODE_STOP, CODE_INVALID,
} CODE;

const CODE kb_map[4][14] = {
    {CODE_INVALID, CODE_1, CODE_2, CODE_3, CODE_4, CODE_5, CODE_6, CODE_7, CODE_8, CODE_9, CODE_0, CODE_MINUS, CODE_EUQAL, CODE_DELETE},
    {CODE_INVALID, CODE_Q, CODE_W, CODE_E, CODE_R, CODE_T, CODE_Y, CODE_U, CODE_I, CODE_O, CODE_P, CODE_INVALID, CODE_INVALID, CODE_INVALID},
    {CODE_INVALID, CODE_INVALID, CODE_A, CODE_S, CODE_D, CODE_F, CODE_G, CODE_H, CODE_J, CODE_K, CODE_L, CODE_SEMICOLON, CODE_INVALID, CODE_RETURN},
    {CODE_INVALID, CODE_INVALID, CODE_INVALID, CODE_Z, CODE_X, CODE_C, CODE_V, CODE_B, CODE_N, CODE_M, CODE_COMMA, CODE_PERIOD, CODE_SLASH, CODE_SPACE}};


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
  std::atomic<bool> joysitckMode_;
  std::atomic<bool> reset_;
  bool retval_ = true;
  /* keyboard mappings */
  std::unordered_map<CODE, std::pair<int, int>> keymap_;
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

public:
  INLINE uint8_t keyboard_matrix_row(int col) { return keyboard_matrix_[col]; };
  INLINE bool joysitckMode() { return joysitckMode_; };
  INLINE bool reset() { return reset_; };
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