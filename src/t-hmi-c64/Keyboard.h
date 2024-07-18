#pragma once

#include <M5Cardputer.h>
#include <queue>
#include <unordered_map>
#include <mutex>

#include "SDL_scancode.h"

#define KEY_SIZE 0xFF
#define INLINE IRAM_ATTR inline __attribute__((always_inline, flatten))
#define SDLK_SCANCODE_MASK (1 << 30)
#define SDL_SCANCODE_TO_KEYCODE(X) (X | SDLK_SCANCODE_MASK)
typedef int32_t SDL_Keycode;

class Keyboard
{
private:
  bool kb_state[KEY_SIZE] = {0};
  uint8_t keyboard_matrix_[8];
  std::atomic<bool> debug_;
  bool retval_ = true;
  /* keyboard mappings */
  std::unordered_map<SDL_Keycode, std::pair<int, int>> keymap_;
  std::unordered_map<char, std::vector<SDL_Keycode>> charmap_;
  enum kKeyEvent
  {
    kPress,
    kRelease,
  };
  /* key events */
  std::queue<std::pair<kKeyEvent, SDL_Keycode>> key_event_queue_;
  unsigned int next_key_event_at_;

public:
  INLINE uint8_t keyboard_matrix_row(int col) { return keyboard_matrix_[col]; };
  INLINE bool debug() { return debug_; };
  void init();
  void handleKeyboard();
  void handleKeyUp(SDL_Keycode k);
  void handleKeyDown(SDL_Keycode k);
};