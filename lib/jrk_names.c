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
  { "18v2x", JRK_PRODUCT_UMC04A_30V }, // TODO: real name
  { "24v2x", JRK_PRODUCT_UMC04A_40V }, // TODO: real name
  { "18v1x", JRK_PRODUCT_UMC05A_30V }, // TODO: real name
  { "24v1x", JRK_PRODUCT_UMC05A_40V }, // TODO: real name
  { NULL, 0 },
};

const jrk_name jrk_product_names_ui[] =
{
  { "Jrk G2 18v2x", JRK_PRODUCT_UMC04A_30V },  // TODO: real name
  { "Jrk G2 24v2x", JRK_PRODUCT_UMC04A_40V },  // TODO: real name
  { "Jrk G2 18v1x", JRK_PRODUCT_UMC05A_30V },  // TODO: real name
  { "Jrk G2 24v1x", JRK_PRODUCT_UMC05A_40V },  // TODO: real name
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
  { "frequency", JRK_FEEDBACK_MODE_FREQUENCY },
  { NULL, 0 },
};

const jrk_name jrk_serial_mode_names_short[] =
{
  { "usb_dual_port", JRK_SERIAL_MODE_USB_DUAL_PORT },
  { "usb_chained", JRK_SERIAL_MODE_USB_CHAINED },
  { "uart", JRK_SERIAL_MODE_UART },
  { NULL, 0 },
};

const jrk_name jrk_pwm_frequency_names_short[] =
{
  { "20", JRK_PWM_FREQUENCY_20 },
  { "5", JRK_PWM_FREQUENCY_5 },
  { NULL, 0 },
};

const jrk_name jrk_fbt_method_names_short[] =
{
  { "counting", JRK_FBT_METHOD_PULSE_COUNTING },
  { "timing", JRK_FBT_METHOD_PULSE_TIMING },
  { NULL, 0 },
};

const jrk_name jrk_fbt_timing_clock_names_short[] =
{
  { "12m", JRK_FBT_TIMING_CLOCK_12 },
  { "6m", JRK_FBT_TIMING_CLOCK_6 },
  { "3m", JRK_FBT_TIMING_CLOCK_3 },
  { "1m5", JRK_FBT_TIMING_CLOCK_1_5 },
  { "48m", JRK_FBT_TIMING_CLOCK_48 },
  { "24m", JRK_FBT_TIMING_CLOCK_24 },
  { NULL, 0 },
};

const jrk_name jrk_fbt_timing_clock_names_no_units[] =
{
  { "12", JRK_FBT_TIMING_CLOCK_12 },
  { "6", JRK_FBT_TIMING_CLOCK_6 },
  { "3", JRK_FBT_TIMING_CLOCK_3 },
  { "1.5", JRK_FBT_TIMING_CLOCK_1_5 },
  { "48", JRK_FBT_TIMING_CLOCK_48 },
  { "24", JRK_FBT_TIMING_CLOCK_24 },
  { NULL, 0 },
};

const jrk_name jrk_error_names_ui[] =
{
  { "Awaiting command", 1 << JRK_ERROR_AWAITING_COMMAND },
  { "No power", 1 << JRK_ERROR_NO_POWER },
  { "Motor driver error", 1 << JRK_ERROR_MOTOR_DRIVER },
  { "Input invalid", 1 << JRK_ERROR_INPUT_INVALID },
  { "Input disconnect", 1 << JRK_ERROR_INPUT_DISCONNECT },
  { "Feedback disconnect", 1 << JRK_ERROR_FEEDBACK_DISCONNECT },
  { "Max. current exceeded", 1 << JRK_ERROR_MAX_CURRENT_EXCEEDED },
  { "Serial signal error", 1 << JRK_ERROR_SERIAL_SIGNAL },
  { "Serial overrun", 1 << JRK_ERROR_SERIAL_OVERRUN },
  { "Serial RX buffer full", 1 << JRK_ERROR_SERIAL_BUFFER_FULL },
  { "Serial CRC error", 1 << JRK_ERROR_SERIAL_CRC },
  { "Serial protocol error", 1 << JRK_ERROR_SERIAL_PROTOCOL },
  { "Serial timeout error", 1 << JRK_ERROR_SERIAL_TIMEOUT },
  { "Overcurrent", 1 << JRK_ERROR_OVERCURRENT },
  { NULL, 0 },
};

const jrk_name jrk_force_mode_names_ui[] =
{
  { "None", JRK_FORCE_MODE_NONE },
  { "Duty cycle target", JRK_FORCE_MODE_DUTY_CYCLE_TARGET },
  { "Duty cycle", JRK_FORCE_MODE_DUTY_CYCLE },
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

const char * jrk_look_up_force_mode_name_ui(uint8_t force_mode)
{
  const char * str = "(Unknown)";
  jrk_code_to_name(jrk_force_mode_names_ui, force_mode, &str);
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
