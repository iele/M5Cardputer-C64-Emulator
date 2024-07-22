#include "Keyboard.h"

static const char *TAG = "Keyboard";

const char kb_map[4][14] = {
    {SDL_SCANCODE_BACKSPACE, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4, SDL_SCANCODE_5, SDL_SCANCODE_6, SDL_SCANCODE_7, SDL_SCANCODE_8, SDL_SCANCODE_9, SDL_SCANCODE_0, SDL_SCANCODE_MINUS, SDL_SCANCODE_EQUALS, SDL_SCANCODE_BACKSPACE},
    {SDL_SCANCODE_SPACE, SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R, SDL_SCANCODE_T, SDL_SCANCODE_Y, SDL_SCANCODE_U, SDL_SCANCODE_I, SDL_SCANCODE_O, SDL_SCANCODE_P, SDL_SCANCODE_LEFTBRACKET, SDL_SCANCODE_RIGHTBRACKET, SDL_SCANCODE_BACKSLASH},
    {SDL_SCANCODE_LGUI, SDL_SCANCODE_LSHIFT, SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F, SDL_SCANCODE_G, SDL_SCANCODE_H, SDL_SCANCODE_J, SDL_SCANCODE_K, SDL_SCANCODE_L, SDL_SCANCODE_SEMICOLON, SDL_SCANCODE_COMMA, SDL_SCANCODE_RETURN},
    {SDL_SCANCODE_F1, SDL_SCANCODE_F3, SDL_SCANCODE_RSHIFT, SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V, SDL_SCANCODE_B, SDL_SCANCODE_N, SDL_SCANCODE_M, SDL_SCANCODE_COMMA, SDL_SCANCODE_PERIOD, SDL_SCANCODE_SLASH, SDL_SCANCODE_SPACE}};

void Keyboard::init()
{
  /* init keyboard matrix state */
  for (size_t i = 0; i < sizeof(keyboard_matrix_); i++)
  {
    keyboard_matrix_[i] = 0xff;
  }
  /* character to sdl key map */
  charmap_['A'] = {SDL_SCANCODE_A};
  charmap_['B'] = {SDL_SCANCODE_B};
  charmap_['C'] = {SDL_SCANCODE_C};
  charmap_['D'] = {SDL_SCANCODE_D};
  charmap_['E'] = {SDL_SCANCODE_E};
  charmap_['F'] = {SDL_SCANCODE_F};
  charmap_['G'] = {SDL_SCANCODE_G};
  charmap_['H'] = {SDL_SCANCODE_H};
  charmap_['I'] = {SDL_SCANCODE_I};
  charmap_['J'] = {SDL_SCANCODE_J};
  charmap_['K'] = {SDL_SCANCODE_K};
  charmap_['L'] = {SDL_SCANCODE_L};
  charmap_['M'] = {SDL_SCANCODE_M};
  charmap_['N'] = {SDL_SCANCODE_N};
  charmap_['O'] = {SDL_SCANCODE_O};
  charmap_['P'] = {SDL_SCANCODE_P};
  charmap_['Q'] = {SDL_SCANCODE_Q};
  charmap_['R'] = {SDL_SCANCODE_R};
  charmap_['S'] = {SDL_SCANCODE_S};
  charmap_['T'] = {SDL_SCANCODE_T};
  charmap_['U'] = {SDL_SCANCODE_U};
  charmap_['V'] = {SDL_SCANCODE_V};
  charmap_['W'] = {SDL_SCANCODE_W};
  charmap_['X'] = {SDL_SCANCODE_X};
  charmap_['Y'] = {SDL_SCANCODE_Y};
  charmap_['Z'] = {SDL_SCANCODE_Z};
  charmap_['1'] = {SDL_SCANCODE_1};
  charmap_['2'] = {SDL_SCANCODE_2};
  charmap_['3'] = {SDL_SCANCODE_3};
  charmap_['4'] = {SDL_SCANCODE_4};
  charmap_['5'] = {SDL_SCANCODE_5};
  charmap_['6'] = {SDL_SCANCODE_6};
  charmap_['7'] = {SDL_SCANCODE_7};
  charmap_['8'] = {SDL_SCANCODE_8};
  charmap_['9'] = {SDL_SCANCODE_9};
  charmap_['0'] = {SDL_SCANCODE_0};
  charmap_['\n'] = {SDL_SCANCODE_RETURN};
  charmap_[' '] = {SDL_SCANCODE_SPACE};
  charmap_[','] = {SDL_SCANCODE_COMMA};
  charmap_['.'] = {SDL_SCANCODE_PERIOD};
  charmap_['/'] = {SDL_SCANCODE_SLASH};
  charmap_[';'] = {SDL_SCANCODE_SEMICOLON};
  charmap_['='] = {SDL_SCANCODE_EQUALS};
  charmap_['-'] = {SDL_SCANCODE_MINUS};
  charmap_[':'] = {SDL_SCANCODE_BACKSLASH};
  charmap_['+'] = {SDL_SCANCODE_LEFTBRACKET};
  charmap_['*'] = {SDL_SCANCODE_RIGHTBRACKET};
  charmap_['@'] = {SDL_SCANCODE_APOSTROPHE};
  charmap_['('] = {SDL_SCANCODE_LSHIFT, SDL_SCANCODE_8};
  charmap_[')'] = {SDL_SCANCODE_LSHIFT, SDL_SCANCODE_9};
  charmap_['<'] = {SDL_SCANCODE_LSHIFT, SDL_SCANCODE_COMMA};
  charmap_['>'] = {SDL_SCANCODE_LSHIFT, SDL_SCANCODE_PERIOD};
  charmap_['"'] = {SDL_SCANCODE_LSHIFT, SDL_SCANCODE_2};
  charmap_['$'] = {SDL_SCANCODE_LSHIFT, SDL_SCANCODE_4};
  /* keymap letters */
  keymap_[SDL_SCANCODE_A] = std::make_pair(1, 2);
  keymap_[SDL_SCANCODE_B] = std::make_pair(3, 4);
  keymap_[SDL_SCANCODE_C] = std::make_pair(2, 4);
  keymap_[SDL_SCANCODE_D] = std::make_pair(2, 2);
  keymap_[SDL_SCANCODE_E] = std::make_pair(1, 6);
  keymap_[SDL_SCANCODE_F] = std::make_pair(2, 5);
  keymap_[SDL_SCANCODE_G] = std::make_pair(3, 2);
  keymap_[SDL_SCANCODE_H] = std::make_pair(3, 5);
  keymap_[SDL_SCANCODE_I] = std::make_pair(4, 1);
  keymap_[SDL_SCANCODE_J] = std::make_pair(4, 2);
  keymap_[SDL_SCANCODE_K] = std::make_pair(4, 5);
  keymap_[SDL_SCANCODE_L] = std::make_pair(5, 2);
  keymap_[SDL_SCANCODE_M] = std::make_pair(4, 4);
  keymap_[SDL_SCANCODE_N] = std::make_pair(4, 7);
  keymap_[SDL_SCANCODE_O] = std::make_pair(4, 6);
  keymap_[SDL_SCANCODE_P] = std::make_pair(5, 1);
  keymap_[SDL_SCANCODE_Q] = std::make_pair(7, 6);
  keymap_[SDL_SCANCODE_R] = std::make_pair(2, 1);
  keymap_[SDL_SCANCODE_S] = std::make_pair(1, 5);
  keymap_[SDL_SCANCODE_T] = std::make_pair(2, 6);
  keymap_[SDL_SCANCODE_U] = std::make_pair(3, 6);
  keymap_[SDL_SCANCODE_V] = std::make_pair(3, 7);
  keymap_[SDL_SCANCODE_W] = std::make_pair(1, 1);
  keymap_[SDL_SCANCODE_X] = std::make_pair(2, 7);
  keymap_[SDL_SCANCODE_Y] = std::make_pair(3, 1);
  keymap_[SDL_SCANCODE_Z] = std::make_pair(1, 4);
  /* keymap numbers */
  keymap_[SDL_SCANCODE_1] = std::make_pair(7, 0);
  keymap_[SDL_SCANCODE_2] = std::make_pair(7, 3);
  keymap_[SDL_SCANCODE_3] = std::make_pair(1, 0);
  keymap_[SDL_SCANCODE_4] = std::make_pair(1, 3);
  keymap_[SDL_SCANCODE_5] = std::make_pair(2, 0);
  keymap_[SDL_SCANCODE_6] = std::make_pair(2, 3);
  keymap_[SDL_SCANCODE_7] = std::make_pair(3, 0);
  keymap_[SDL_SCANCODE_8] = std::make_pair(3, 3);
  keymap_[SDL_SCANCODE_9] = std::make_pair(4, 0);
  keymap_[SDL_SCANCODE_0] = std::make_pair(4, 3);
  /* keymap function keys */
  keymap_[SDL_SCANCODE_F1] = std::make_pair(0, 4);
  keymap_[SDL_SCANCODE_F3] = std::make_pair(0, 4);
  keymap_[SDL_SCANCODE_F5] = std::make_pair(0, 4);
  keymap_[SDL_SCANCODE_F7] = std::make_pair(0, 4);
  /* keymap: other */
  keymap_[SDL_SCANCODE_RETURN] = std::make_pair(0, 1);
  keymap_[SDL_SCANCODE_SPACE] = std::make_pair(7, 4);
  keymap_[SDL_SCANCODE_LSHIFT] = std::make_pair(1, 7);
  keymap_[SDL_SCANCODE_RSHIFT] = std::make_pair(6, 4);
  keymap_[SDL_SCANCODE_COMMA] = std::make_pair(5, 7);
  keymap_[SDL_SCANCODE_PERIOD] = std::make_pair(5, 4);
  keymap_[SDL_SCANCODE_SLASH] = std::make_pair(6, 7);
  keymap_[SDL_SCANCODE_SEMICOLON] = std::make_pair(6, 2);
  keymap_[SDL_SCANCODE_EQUALS] = std::make_pair(6, 5);
  keymap_[SDL_SCANCODE_BACKSPACE] = std::make_pair(0, 0);
  keymap_[SDL_SCANCODE_MINUS] = std::make_pair(5, 3);
  /* keymap: these are mapped to other keys */
  keymap_[SDL_SCANCODE_BACKSLASH] = std::make_pair(5, 5);    // :
  keymap_[SDL_SCANCODE_LEFTBRACKET] = std::make_pair(5, 0);  // +
  keymap_[SDL_SCANCODE_RIGHTBRACKET] = std::make_pair(6, 1); // *
  keymap_[SDL_SCANCODE_APOSTROPHE] = std::make_pair(5, 6);   // @
  keymap_[SDL_SCANCODE_LGUI] = std::make_pair(7, 5);         // commodore key

  joysitckMode_ = false;
  reset_ = false;
}

void Keyboard::handleKeyDown(SDL_Keycode k)
{
  try
  {
    uint8_t mask = ~(1 << keymap_.at(k).second);
    keyboard_matrix_[keymap_.at(k).first] &= mask;
  }
  catch (const std::out_of_range)
  {
  }
}

void Keyboard::handleKeyUp(SDL_Keycode k)
{
  try
  {
    uint8_t mask = (1 << keymap_.at(k).second);
    keyboard_matrix_[keymap_.at(k).first] |= mask;
  }
  catch (const std::out_of_range)
  {
  }
}

void Keyboard::handleKeyboard()
{
  M5Cardputer.update();
  if (M5Cardputer.BtnA.wasHold())
  {
    reset_ = ~reset_;
  }
  if (M5Cardputer.BtnA.wasClicked())
  {
    joysitckMode_ = joysitckMode_ ? false : true;
  }
  std::vector<Point2D_t> keys = M5Cardputer.Keyboard.keyList();
  if (joysitckMode_)
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
    if (M5Cardputer.Keyboard.isKeyPressed('a')) {
      joystickValue &= ~(1 << C64JOYFIRE);
      joystickFire = true;
    }
  }
  else
  {
    bool new_kb_state[KEY_SIZE] = {0};
    // map key_code
    for (auto &i : keys)
    {
      uint8_t key_code;
      key_code = kb_map[i.y][i.x];
      if (key_code == 0)
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
        handleKeyDown(i);
      }
      if (new_kb_state[i] == false && kb_state[i] == true)
      {
        ESP_LOGI(TAG, "key up %d", i);
        handleKeyUp(i);
      }
      kb_state[i] = new_kb_state[i];
    }
  }

  if (!key_event_queue_.empty())
  {
    std::pair<kKeyEvent, SDL_Keycode> &ev = key_event_queue_.front();
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
  }
}

void Keyboard::typeCharacter(char c)
{
  try
  {
    for (SDL_Keycode &k : charmap_.at(toupper(c)))
      key_event_queue_.push(std::make_pair(kPress, k));
    for (SDL_Keycode &k : charmap_.at(toupper(c)))
      key_event_queue_.push(std::make_pair(kRelease, k));
  }
  catch (const std::out_of_range)
  {
  }
}
