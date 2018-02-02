#include "jrk_internal.h"

#define ERRORS_ALWAYS_LATCHED ( \
  (1 << JRK_ERROR_AWAITING_COMMAND) | \
  (1 << JRK_ERROR_SERIAL_SIGNAL) | \
  (1 << JRK_ERROR_SERIAL_CRC) | \
  (1 << JRK_ERROR_SERIAL_PROTOCOL) | \
  (1 << JRK_ERROR_SERIAL_TIMEOUT) | \
  (1 << JRK_ERROR_SERIAL_BUFFER_FULL))

#define ERRORS_IGNORED_WHEN_FORCING_DUTY_CYCLE ( \
  (1 << JRK_ERROR_INPUT_INVALID) |  \
  (1 << JRK_ERROR_INPUT_DISCONNECT) | \
  (1 << JRK_ERROR_FEEDBACK_DISCONNECT))

// TODO: say when it's at duty cycle limit
// TODO: if duty cycle is 0, if it hasn't reached the target or limit, say accelerating

// Returns true if the device's PID coefficients are zero.
static bool pid_zero(
  const jrk_settings * settings,
  const jrk_overridable_settings * osettings)
{
  if (jrk_settings_get_feedback_mode(settings) == JRK_FEEDBACK_MODE_NONE)
  {
    return false;  // PID coefficients don't matter.
  }

  if (osettings)
  {
    return
      !jrk_overridable_settings_get_proportional_multiplier(osettings) &&
      !jrk_overridable_settings_get_integral_multiplier(osettings) &&
      !jrk_overridable_settings_get_derivative_multiplier(osettings);
  }
  else
  {
    return
      !jrk_settings_get_proportional_multiplier(settings) &&
      !jrk_settings_get_integral_multiplier(settings) &&
      !jrk_settings_get_derivative_multiplier(settings);
  }
}

jrk_error * jrk_diagnose(
  const jrk_settings * settings,
  const jrk_overridable_settings * osettings,
  const jrk_variables * vars,
  uint32_t flags,
  char ** diagnosis)
{
  (void)flags;

  if (diagnosis == NULL)
  {
    return jrk_error_create("Diagnosis output pointer is null.");
  }

  *diagnosis = NULL;

  if (settings == NULL)
  {
    return jrk_error_create("Settings object is null.");
  }

  // osettings is optional, don't check it here

  if (vars == NULL)
  {
    return jrk_error_create("Variables object is null.");
  }

  uint16_t errors_latched = jrk_settings_get_error_latch(settings)
    | ERRORS_ALWAYS_LATCHED;

  uint8_t force_mode = jrk_variables_get_force_mode(vars);

  uint16_t errors_halting = jrk_variables_get_error_flags_halting(vars);
  if (force_mode == JRK_FORCE_MODE_DUTY_CYCLE)
  {
    errors_halting &= ~ERRORS_IGNORED_WHEN_FORCING_DUTY_CYCLE;
  }

  uint16_t real_errors_halting = errors_halting & ~(1 << JRK_ERROR_AWAITING_COMMAND);

  uint16_t real_errors_latched =
    errors_halting & errors_latched & ~(1 << JRK_ERROR_AWAITING_COMMAND);

  int16_t duty_cycle = jrk_variables_get_duty_cycle(vars);

  int16_t duty_cycle_target = jrk_variables_get_duty_cycle_target(vars);

  jrk_string str;
  jrk_string_setup(&str);

  if (real_errors_latched)
  {
    // A latched error is active other than awaiting command.
    // We don't know if the error is actually still happening, so we
    // can't go into great detail here.
    if (__builtin_popcount(real_errors_latched) > 1)
    {
      jrk_sprintf(&str, "Motor stopped due to latched errors.");
    }
    else
    {
      jrk_sprintf(&str, "Motor stopped due to a latched error.");
    }
  }
  else if (errors_halting == (1 << JRK_ERROR_AWAITING_COMMAND))
  {
    jrk_sprintf(&str, "Motor stopped: waiting for a command.");
  }
  else if (real_errors_halting == (1 << JRK_ERROR_NO_POWER))
  {
    if (jrk_variables_get_vin_voltage(vars) < 2000)
    {
      jrk_sprintf(&str, "Motor stopped: VIN is disconnected.");
    }
    else
    {
      jrk_sprintf(&str, "Motor stopped: VIN is too low.");
    }
  }
  else if (real_errors_halting == (1 << JRK_ERROR_MOTOR_DRIVER))
  {
    jrk_sprintf(&str, "Motor stopped: motor driver error.");
  }
  else if (real_errors_halting == (1 << JRK_ERROR_INPUT_INVALID))
  {
    jrk_sprintf(&str, "Motor stopped: input is invalid.");
  }
  else if (real_errors_halting == (1 << JRK_ERROR_INPUT_DISCONNECT))
  {
    jrk_sprintf(&str, "Motor stopped: input is disconnected or out of range.");
  }
  else if (real_errors_halting == (1 << JRK_ERROR_FEEDBACK_DISCONNECT))
  {
    jrk_sprintf(&str, "Motor stopped: feedback is disconnected or out of range.");
  }
  else if (real_errors_halting)
  {
    if (__builtin_popcount(real_errors_halting) > 1)
    {
      jrk_sprintf(&str, "Motor stopped due to errors.");
    }
    else
    {
      jrk_sprintf(&str, "Motor stopped due to an error.");
    }
  }
  else if (force_mode == JRK_FORCE_MODE_DUTY_CYCLE)
  {
    if (duty_cycle == 0)
    {
      jrk_sprintf(&str, "Motor stopped: duty cycle is forced to 0.");
    }
    else
    {
      jrk_sprintf(&str, "Motor is running with a forced duty cycle.");
    }
  }
  else if (force_mode == JRK_FORCE_MODE_DUTY_CYCLE_TARGET)
  {
    if (duty_cycle_target == 0)
    {
      jrk_sprintf(&str, "Motor stopped: duty cycle target is forced to 0.");
    }
    else
    {
      jrk_sprintf(&str, "Motor is running with a forced duty cycle target.");
    }
  }
  else if (duty_cycle == 0 && pid_zero(settings,osettings))
  {
    jrk_sprintf(&str, "Motor stopped: PID coefficients are zero.");
  }
  else if (duty_cycle == 0)
  {
    jrk_sprintf(&str, "Motor is ready to run.");
  }
  else
  {
    jrk_sprintf(&str, "Motor is running.");
  }

  if (str.data == NULL)
  {
    return &jrk_error_no_memory;
  }
  else
  {
    *diagnosis = str.data;
    return NULL;
  }
}
