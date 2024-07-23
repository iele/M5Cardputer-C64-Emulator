#include "Keyboard.h"

static const char *TAG = "Keyboard";

void Keyboard::init()
{
  /* init keyboard matrix state */
  for (size_t i = 0; i < sizeof(keyboard_matrix_); i++)
  {
    keyboard_matrix_[i] = 0xff;
  }

  joysitckMode_ = false;
  reset_ = false;
}

void Keyboard::handleKeyDown(CODE k)
{
  try
  {
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
      if (key_code == CODE_INVALID)
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
