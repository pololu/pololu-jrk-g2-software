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

// Returns true if the duty cycle is non-zero and at the max duty cycle.
static bool duty_cycle_at_max_non_zero(
  const jrk_settings * settings,
  int16_t duty_cycle)
{
  if (duty_cycle == 0)
  {
    return false;
  }

  if (duty_cycle > 0)
  {
    return duty_cycle == jrk_settings_get_max_duty_cycle_forward(settings);
  }
  else
  {
    return duty_cycle == -jrk_settings_get_max_duty_cycle_reverse(settings);
  }
}

// Returns true if the duty cycle is 0 and the duty cycle target is not 0,
// and this is due to a max. duty cycle limit being 0.
static bool duty_cycle_at_max_zero(
  const jrk_settings * settings,
  int16_t duty_cycle,
  int16_t duty_cycle_target)
{
  if (!(duty_cycle == 0 && duty_cycle_target != 0))
  {
    return false;
  }

  if (duty_cycle_target > 0)
  {
    return jrk_settings_get_max_duty_cycle_forward(settings) == 0;
  }
  else
  {
    return jrk_settings_get_max_duty_cycle_reverse(settings) == 0;
  }
}

// Returns true if the device's setting indicate closed-loop feedback but the
// PID coefficients are zero.
static bool pid_zero(const jrk_settings * settings)
{
  if (jrk_settings_get_feedback_mode(settings) == JRK_FEEDBACK_MODE_NONE)
  {
    return false;  // PID coefficients don't matter.
  }

  return
    !jrk_settings_get_proportional_multiplier(settings) &&
    !jrk_settings_get_integral_multiplier(settings) &&
    !jrk_settings_get_derivative_multiplier(settings);
}

jrk_error * jrk_diagnose(
  const jrk_settings * settings,
  const jrk_variables * vars,
  uint32_t flags,
  char ** diagnosis)
{
  if (diagnosis == NULL)
  {
    return jrk_error_create("Diagnosis output pointer is null.");
  }

  *diagnosis = NULL;

  if (settings == NULL)
  {
    return jrk_error_create("Settings object is null.");
  }

  if (vars == NULL)
  {
    return jrk_error_create("Variables object is null.");
  }

  bool feedback_wizard = (flags & JRK_DIAGNOSE_FLAG_FEEDBACK_WIZARD) ? 1 : 0;

  uint8_t feedback_mode = jrk_settings_get_feedback_mode(settings);

  uint16_t errors_latched = jrk_settings_get_error_latch(settings)
    | ERRORS_ALWAYS_LATCHED;

  uint8_t force_mode = jrk_variables_get_force_mode(vars);

  bool open_loop = feedback_mode == JRK_FEEDBACK_MODE_NONE || force_mode;

  uint16_t errors_halting = jrk_variables_get_error_flags_halting(vars);
  if (force_mode == JRK_FORCE_MODE_DUTY_CYCLE)
  {
    errors_halting &= ~ERRORS_IGNORED_WHEN_FORCING_DUTY_CYCLE;
  }

  uint16_t real_errors_halting = errors_halting & ~(1 << JRK_ERROR_AWAITING_COMMAND);

  uint16_t real_errors_latched =
    errors_halting & errors_latched & ~(1 << JRK_ERROR_AWAITING_COMMAND);

  int16_t duty_cycle = jrk_variables_get_duty_cycle(vars);
  int16_t last_duty_cycle = jrk_variables_get_last_duty_cycle(vars);
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
    if (feedback_wizard)
    {
      jrk_sprintf(&str, "Motor stopped: click and hold one of the buttons above "
        "to drive the motor.");
    }
    else
    {
      jrk_sprintf(&str, "Motor stopped: waiting for a command.");
    }
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
  // Below this point, we know there are no errors.
  else if (force_mode == JRK_FORCE_MODE_DUTY_CYCLE)
  {
    if (duty_cycle == 0)
    {
      // It could be that a "Force duty cycle" command specified a speed of 0,
      // or it could be that one or more of the "Max. duty cycle" settings was
      // set to 0, and the "Force duty cycle" command tried to make the motor
      // go in that direction.
      jrk_sprintf(&str, "Motor stopped: duty cycle is forced to 0.");
    }
    else if (duty_cycle_at_max_non_zero(settings, duty_cycle))
    {
      jrk_sprintf(&str, "Motor is running, forced to max. duty cycle.");
    }
    else
    {
      jrk_sprintf(&str, "Motor is running with a forced duty cycle.");
    }
  }
  // Below this point, there are no errors and no forced duty cycle, so we
  // know the jrk is trying to reach the duty cycle target.
  else if (duty_cycle_at_max_zero(settings, duty_cycle, duty_cycle_target))
  {
    jrk_sprintf(&str, "Motor stopped because max. duty cycle is 0.");
  }
  else if (duty_cycle_target != duty_cycle &&
    duty_cycle_at_max_non_zero(settings, duty_cycle))
  {
    jrk_sprintf(&str, "Motor running at max. duty cycle.");
  }
  else if (duty_cycle_target != duty_cycle && duty_cycle != last_duty_cycle &&
    open_loop)
  {
    // These messages would oscillate pretty quickly during closed loop feedback
    // and be annoying, so don't show them.
    if ((duty_cycle > 0 && duty_cycle > last_duty_cycle) ||
      (duty_cycle < 0 && duty_cycle < last_duty_cycle))
    {
      jrk_sprintf(&str, "Motor is accelerating.");
    }
    else
    {
      jrk_sprintf(&str, "Motor is decelerating.");
    }
  }
  else if (force_mode == JRK_FORCE_MODE_DUTY_CYCLE_TARGET)
  {
    if (duty_cycle_target == 0 && duty_cycle == 0)
    {
      if (feedback_wizard)
      {
        jrk_sprintf(&str, "Motor stopped: click and hold one of the buttons above "
          "to drive the motor.");
      }
      else
      {
        jrk_sprintf(&str, "Motor stopped: duty cycle target is forced to 0.");
      }
    }
    else
    {
      if (feedback_wizard)
      {
        jrk_sprintf(&str, "Motor is running.");
      }
      else
      {
        jrk_sprintf(&str, "Motor is running with a forced duty cycle target.");
      }
    }
  }
  // Below this point, we know this is normal operation (force_mode == 0).
  else if (duty_cycle == 0 && pid_zero(settings))
  {
    jrk_sprintf(&str, "Motor stopped: PID coefficients are zero.");
  }
  else if (duty_cycle == 0)
  {
    // Probably stopped intentionally, or maybe due to the "Max duty cycle while
    // feedback is out of range" setting.
    jrk_sprintf(&str, "Motor stopped.");
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
