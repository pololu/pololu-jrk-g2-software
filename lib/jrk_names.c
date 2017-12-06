// Functions for converting between names and numeric codes.

#include "jrk_internal.h"

const jrk_name jrk_bool_names[] =
{
  { "true", 1 },
  { "false", 0 },
  { NULL, 0 },
};

const jrk_name jrk_product_names_short[] =
{
  { "2017", JRK_PRODUCT_2017 }, // TODO: real name
  { NULL, 0 },
};

const jrk_name jrk_product_names_ui[] =
{
  { "Jrk 2017 USB Motor Controller with Feedback", JRK_PRODUCT_2017 },  // TODO: real name
  { NULL, 0 },
};

const jrk_name jrk_input_mode_names_short[] =
{
  { "serial", JRK_INPUT_MODE_SERIAL },
  { "analog", JRK_INPUT_MODE_ANALOG },
  { "pulse_width", JRK_INPUT_MODE_PULSE_WIDTH },
  { NULL, 0 },
};

const jrk_name jrk_input_scaling_degree_names_short[] =
{
  { "linear", JRK_SCALING_DEGREE_LINEAR },
  { "quadratic", JRK_SCALING_DEGREE_QUADRATIC },
  { "cubic", JRK_SCALING_DEGREE_CUBIC },
  { "quartic", JRK_SCALING_DEGREE_QUARTIC },
  { "quintic", JRK_SCALING_DEGREE_QUINTIC },
  { NULL, 0 },
};

const jrk_name jrk_feedback_mode_names_short[] =
{
  { "none", JRK_FEEDBACK_MODE_NONE },
  { "analog", JRK_FEEDBACK_MODE_ANALOG },
  { "tachometer", JRK_FEEDBACK_MODE_FREQUENCY },
  { NULL, 0 },
};

const jrk_name jrk_serial_mode_names_short[] =
{
  { "usb_dual_port", JRK_SERIAL_MODE_USB_DUAL_PORT },
  { "usb_chained", JRK_SERIAL_MODE_USB_CHAINED },
  { "uart", JRK_SERIAL_MODE_UART },
  { NULL, 0 },
};

const jrk_name jrk_motor_pwm_frequency_names_short[] =
{
  { "20", JRK_MOTOR_PWM_FREQUENCY_20 },
  { "5", JRK_MOTOR_PWM_FREQUENCY_5 },
  { NULL, 0 },
};

const jrk_name jrk_error_names_ui[] =  // TODO
{
  { NULL, 0 },
};

const jrk_name jrk_device_reset_names_ui[] =
{
  { "Power-on reset", JRK_RESET_POWER_UP },
  { "Brown-out reset", JRK_RESET_BROWNOUT },
  { "Reset pin driven low", JRK_RESET_RESET_LINE },
  { "Watchdog reset", JRK_RESET_WATCHDOG },
  { "Software reset (bootloader)", JRK_RESET_SOFTWARE },
  { "Stack overflow", JRK_RESET_STACK_OVERFLOW },
  { "Stack underflow", JRK_RESET_STACK_UNDERFLOW },
  { NULL, 0 },
};

const char * jrk_look_up_product_name_short(uint32_t product)
{
  const char * str = "";
  jrk_code_to_name(jrk_product_names_short, product, &str);
  return str;
}

const char * jrk_look_up_product_name_ui(uint32_t product)
{
  const char * str = "(Unknown)";
  jrk_code_to_name(jrk_product_names_ui, product, &str);
  return str;
}

const char * jrk_look_up_error_name_ui(uint32_t error)
{
  const char * str = "(Unknown)";
  jrk_code_to_name(jrk_error_names_ui, error, &str);
  return str;
}

const char * jrk_look_up_device_reset_name_ui(uint8_t device_reset)
{
  const char * str = "(Unknown)";
  jrk_code_to_name(jrk_device_reset_names_ui, device_reset, &str);
  return str;
}

bool jrk_name_to_code(const jrk_name * table, const char * name, uint32_t * code)
{
  if (code) { *code = 0; }

  if (!table) { return false; }
  if (!name) { return false; }

  for (const jrk_name * p = table; p->name; p++)
  {
    if (!strcmp(p->name, name))
    {
      if (code) { *code = p->code; }
      return true;
    }
  }

  return false;
}

bool jrk_code_to_name(const jrk_name * table, uint32_t code, const char ** name)
{
  if (!table) { return false; }

  for (const jrk_name * p = table; p->name; p++)
  {
    if (p->code == code)
    {
      if (name) { *name = p->name; }
      return true;
    }
  }

  return false;
}
