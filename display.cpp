#include "display.h"


Display::Display() : lcd(this->DISPLAY_DEFAULT_IIC_ADDRESS, 16, 2), display_state(INIT_DISPLAY)
{
  Kernel::OS.MessageQueue.Subscribe(MSG_ID_INIT_COMPLETE, this);
  Kernel::OS.MessageQueue.Subscribe(MSG_ID_NEW_ACTUAL_RPS, this);
  Kernel::OS.MessageQueue.Subscribe(MSG_ID_KEY_PRESSED, this);
}


void Display::TaskLoop(void)
{
  char print_value[16];

  switch (this->display_state)
  {
    case INIT_DISPLAY:
      this->lcd.init();
      this->lcd.backlight();
      this->lcd.clear();
      this->lcd.setCursor(0, 0);
      this->lcd.print("Starting...");

      if (this->init_complete)
        this->display_state = REFRESH_DISPLAY;

      break;

    case REFRESH_DISPLAY:
      this->lcd.clear();

      this->lcd.setCursor(0, 0);
      sprintf(print_value, "DEMAND RPS: %03d", this->demand_rps);
      this->lcd.print(print_value);

      this->lcd.setCursor(0, 1);
      sprintf(print_value, "ACTUAL RPS: %03d", this->actual_rps);
      this->lcd.print(print_value);

      this->display_state = IDLE_DISPLAY;

      break;

    case IDLE_DISPLAY:
      // ignore backspace & enter keys (* 10>)
      if (this->is_key_pressed && this->current_key < 0b00001010)
      {
        this->is_key_pressed = false;
        this->display_state = this->FIRST_KEY_PRESS;
      }

      else if (this->actual_rps != this->current_rps)
      {
        this->current_rps = this->actual_rps;
        this->display_state = NEW_ACTUAL_RPS;
      }

      break;

    case NEW_ACTUAL_RPS:
      this->lcd.setCursor(0, 1);
      sprintf(print_value, "ACTUAL RPS: %03d", this->actual_rps);
      this->lcd.print(print_value);

      this->display_state = IDLE_DISPLAY;

      break;

    case FIRST_KEY_PRESS:
      this->cursor_position = 9;

      this->lcd.clear();
      this->lcd.setCursor(0, 0);
      this->lcd.print("New RPS: 000");

      sprintf(this->user_input_values, "%03d", this->demand_rps);
      memset(this->user_input_values, 0, sizeof(this->user_input_values));
      this->user_input_values[0] = this->current_key + 0b00110000; //int to ascii (adding 0b00110000)
      this->lcd.setCursor(this->cursor_position++, 0);
      this->lcd.blink();
      this->lcd.print(this->user_input_values);

      this->display_state = NEXT_KEY_PRESS;

      break;

    case NEXT_KEY_PRESS:
      if (!this->is_key_pressed)
        break;

      this->is_key_pressed = false;

      if ((this->current_key == 0b00001010) && (this->cursor_position > 9)) //backspace
      {
        this->lcd.setCursor(--this->cursor_position, 0);
        this->user_input_values[this->cursor_position - 9] = 0;
        this->lcd.print("0");
        this->lcd.setCursor(this->cursor_position, 0);
      }

      else if (this->current_key == 0b00001011) //enter
      {
        this->lcd.noCursor();
        this->lcd.noBlink();
        for (auto& it : this->user_input_values)
          if (it == 0) it = '0';
        sscanf(this->user_input_values, "%d", &this->input_rps);
        this->display_state = VALIDATE_INPUT;
      }

      else if ((this->cursor_position < 12) && (this->current_key < 10)) //enter next  num
      {
        this->user_input_values[this->cursor_position++ - 9] = this->current_key + '0';
        this->lcd.setCursor(9, 0);
        this->lcd.print(this->user_input_values);
      }

      break;

    case VALIDATE_INPUT:
      if (this->input_rps == 0 || (this->input_rps >= Control::RPS_MIN && this->input_rps <= Control::RPS_MAX))
      {
        this->demand_rps = this->input_rps;
        Kernel::OS.MessageQueue.Post(MSG_ID_NEW_RPS_ENTERED, (void*)this->input_rps, Kernel::MQ_OWNER_CALLER, Kernel::MQ_CONTEXT_TASK);
        this->display_state = REFRESH_DISPLAY;
        break;
      }

      this->error_timer.Set(2000);
      this->lcd.clear();
      this->lcd.setCursor(0, 0);
      this->lcd.print("ERROR:");
      this->lcd.setCursor(0, 1);
      this->lcd.print("INVALID INPUT!");

      this->display_state = DISPLAY_ERROR;
      break;

    case DISPLAY_ERROR:
      if (! error_timer.isExpired())
        break;

      this->lcd.setCursor(9, 0);
      this->lcd.print(user_input_values);
      this->lcd.setCursor(9, 0);
      this->lcd.noBlink();

      this->display_state = REFRESH_DISPLAY;
      break;

    default:
      this->display_state = IDLE_DISPLAY;
      break;
  }
}


void Display::EventHandler(int _posted_msg_id, void * _context)
{
  switch (_posted_msg_id) {

    case MSG_ID_INIT_COMPLETE:
      this->init_complete = true;
      break;

    case MSG_ID_NEW_ACTUAL_RPS:
      this->actual_rps = (uint16_t)_context;
      break;

    case MSG_ID_KEY_PRESSED:
      this->is_key_pressed = true;
      this->current_key = (uint16_t)_context;
      break;

    default: break;
  }
}
