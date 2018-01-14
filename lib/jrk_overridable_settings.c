#include "jrk_internal.h"

struct jrk_overridable_settings
{
  // Beginning of auto-generated overridable settings struct members.

  uint16_t proportional_multiplier;
  uint8_t proportional_exponent;
  uint16_t integral_multiplier;
  uint8_t integral_exponent;
  uint16_t derivative_multiplier;
  uint8_t derivative_exponent;
  uint16_t pid_period;
  uint16_t pid_integral_limit;
  bool pid_reset_integral;
  uint16_t motor_max_duty_cycle_while_feedback_out_of_range;
  uint16_t motor_max_acceleration_forward;
  uint16_t motor_max_acceleration_reverse;
  uint16_t motor_max_deceleration_forward;
  uint16_t motor_max_deceleration_reverse;
  uint16_t motor_max_duty_cycle_forward;
  uint16_t motor_max_duty_cycle_reverse;
  uint16_t motor_current_limit_code_forward;
  uint16_t motor_current_limit_code_reverse;
  uint32_t motor_brake_duration_forward;
  uint32_t motor_brake_duration_reverse;
  bool motor_coast_when_off;

  // End of auto-generated overridable settings struct members.
};

jrk_error * jrk_overridable_settings_create(jrk_overridable_settings ** settings)
{
  if (settings == NULL)
  {
    return jrk_error_create("Settings output pointer is null.");
  }

  *settings = NULL;

  jrk_error * error = NULL;

  jrk_overridable_settings * new_settings = NULL;
  if (error == NULL)
  {
    new_settings = (jrk_overridable_settings *)
      calloc(1, sizeof(jrk_overridable_settings));
    if (new_settings == NULL) { error = &jrk_error_no_memory; }
  }

  if (error == NULL)
  {
    *settings = new_settings;
    new_settings = NULL;
  }

  jrk_overridable_settings_free(new_settings);

  return error;
}

void jrk_overridable_settings_free(jrk_overridable_settings * s)
{
  free(s);
}

jrk_error * jrk_overridable_settings_copy(
  const jrk_overridable_settings * source,
  jrk_overridable_settings ** dest)
{
  if (dest == NULL)
  {
    return jrk_error_create("Overridable settings output pointer is null.");
  }

  *dest = NULL;

  if (source == NULL)
  {
    return NULL;
  }

  jrk_error * error = NULL;

  jrk_overridable_settings * new_settings = NULL;
  if (error == NULL)
  {
    new_settings = (jrk_overridable_settings *)
      calloc(1, sizeof(jrk_overridable_settings));
    if (new_settings == NULL) { error = &jrk_error_no_memory; }
  }

  if (error == NULL)
  {
    memcpy(new_settings, source, sizeof(jrk_overridable_settings));
  }

  if (error == NULL)
  {
    *dest = new_settings;
    new_settings = NULL;
  }

  jrk_overridable_settings_free(new_settings);

  return error;
}

// Beginning of auto-generated overridable settings accessors.

void jrk_overridable_settings_set_proportional_multiplier(
  jrk_overridable_settings * settings,
  uint16_t proportional_multiplier)
{
  if (settings == NULL) { return; }
  settings->proportional_multiplier = proportional_multiplier;
}

uint16_t jrk_overridable_settings_get_proportional_multiplier(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->proportional_multiplier;
}

void jrk_overridable_settings_set_proportional_exponent(
  jrk_overridable_settings * settings,
  uint8_t proportional_exponent)
{
  if (settings == NULL) { return; }
  settings->proportional_exponent = proportional_exponent;
}

uint8_t jrk_overridable_settings_get_proportional_exponent(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->proportional_exponent;
}

void jrk_overridable_settings_set_integral_multiplier(
  jrk_overridable_settings * settings,
  uint16_t integral_multiplier)
{
  if (settings == NULL) { return; }
  settings->integral_multiplier = integral_multiplier;
}

uint16_t jrk_overridable_settings_get_integral_multiplier(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->integral_multiplier;
}

void jrk_overridable_settings_set_integral_exponent(
  jrk_overridable_settings * settings,
  uint8_t integral_exponent)
{
  if (settings == NULL) { return; }
  settings->integral_exponent = integral_exponent;
}

uint8_t jrk_overridable_settings_get_integral_exponent(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->integral_exponent;
}

void jrk_overridable_settings_set_derivative_multiplier(
  jrk_overridable_settings * settings,
  uint16_t derivative_multiplier)
{
  if (settings == NULL) { return; }
  settings->derivative_multiplier = derivative_multiplier;
}

uint16_t jrk_overridable_settings_get_derivative_multiplier(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->derivative_multiplier;
}

void jrk_overridable_settings_set_derivative_exponent(
  jrk_overridable_settings * settings,
  uint8_t derivative_exponent)
{
  if (settings == NULL) { return; }
  settings->derivative_exponent = derivative_exponent;
}

uint8_t jrk_overridable_settings_get_derivative_exponent(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->derivative_exponent;
}

void jrk_overridable_settings_set_pid_period(
  jrk_overridable_settings * settings,
  uint16_t pid_period)
{
  if (settings == NULL) { return; }
  settings->pid_period = pid_period;
}

uint16_t jrk_overridable_settings_get_pid_period(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->pid_period;
}

void jrk_overridable_settings_set_pid_integral_limit(
  jrk_overridable_settings * settings,
  uint16_t pid_integral_limit)
{
  if (settings == NULL) { return; }
  settings->pid_integral_limit = pid_integral_limit;
}

uint16_t jrk_overridable_settings_get_pid_integral_limit(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->pid_integral_limit;
}

void jrk_overridable_settings_set_pid_reset_integral(
  jrk_overridable_settings * settings,
  bool pid_reset_integral)
{
  if (settings == NULL) { return; }
  settings->pid_reset_integral = pid_reset_integral;
}

bool jrk_overridable_settings_get_pid_reset_integral(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->pid_reset_integral;
}

void jrk_overridable_settings_set_motor_max_duty_cycle_while_feedback_out_of_range(
  jrk_overridable_settings * settings,
  uint16_t motor_max_duty_cycle_while_feedback_out_of_range)
{
  if (settings == NULL) { return; }
  settings->motor_max_duty_cycle_while_feedback_out_of_range = motor_max_duty_cycle_while_feedback_out_of_range;
}

uint16_t jrk_overridable_settings_get_motor_max_duty_cycle_while_feedback_out_of_range(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->motor_max_duty_cycle_while_feedback_out_of_range;
}

void jrk_overridable_settings_set_motor_max_acceleration_forward(
  jrk_overridable_settings * settings,
  uint16_t motor_max_acceleration_forward)
{
  if (settings == NULL) { return; }
  settings->motor_max_acceleration_forward = motor_max_acceleration_forward;
}

uint16_t jrk_overridable_settings_get_motor_max_acceleration_forward(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->motor_max_acceleration_forward;
}

void jrk_overridable_settings_set_motor_max_acceleration_reverse(
  jrk_overridable_settings * settings,
  uint16_t motor_max_acceleration_reverse)
{
  if (settings == NULL) { return; }
  settings->motor_max_acceleration_reverse = motor_max_acceleration_reverse;
}

uint16_t jrk_overridable_settings_get_motor_max_acceleration_reverse(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->motor_max_acceleration_reverse;
}

void jrk_overridable_settings_set_motor_max_deceleration_forward(
  jrk_overridable_settings * settings,
  uint16_t motor_max_deceleration_forward)
{
  if (settings == NULL) { return; }
  settings->motor_max_deceleration_forward = motor_max_deceleration_forward;
}

uint16_t jrk_overridable_settings_get_motor_max_deceleration_forward(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->motor_max_deceleration_forward;
}

void jrk_overridable_settings_set_motor_max_deceleration_reverse(
  jrk_overridable_settings * settings,
  uint16_t motor_max_deceleration_reverse)
{
  if (settings == NULL) { return; }
  settings->motor_max_deceleration_reverse = motor_max_deceleration_reverse;
}

uint16_t jrk_overridable_settings_get_motor_max_deceleration_reverse(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->motor_max_deceleration_reverse;
}

void jrk_overridable_settings_set_motor_max_duty_cycle_forward(
  jrk_overridable_settings * settings,
  uint16_t motor_max_duty_cycle_forward)
{
  if (settings == NULL) { return; }
  settings->motor_max_duty_cycle_forward = motor_max_duty_cycle_forward;
}

uint16_t jrk_overridable_settings_get_motor_max_duty_cycle_forward(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->motor_max_duty_cycle_forward;
}

void jrk_overridable_settings_set_motor_max_duty_cycle_reverse(
  jrk_overridable_settings * settings,
  uint16_t motor_max_duty_cycle_reverse)
{
  if (settings == NULL) { return; }
  settings->motor_max_duty_cycle_reverse = motor_max_duty_cycle_reverse;
}

uint16_t jrk_overridable_settings_get_motor_max_duty_cycle_reverse(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->motor_max_duty_cycle_reverse;
}

void jrk_overridable_settings_set_motor_current_limit_code_forward(
  jrk_overridable_settings * settings,
  uint16_t motor_current_limit_code_forward)
{
  if (settings == NULL) { return; }
  settings->motor_current_limit_code_forward = motor_current_limit_code_forward;
}

uint16_t jrk_overridable_settings_get_motor_current_limit_code_forward(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->motor_current_limit_code_forward;
}

void jrk_overridable_settings_set_motor_current_limit_code_reverse(
  jrk_overridable_settings * settings,
  uint16_t motor_current_limit_code_reverse)
{
  if (settings == NULL) { return; }
  settings->motor_current_limit_code_reverse = motor_current_limit_code_reverse;
}

uint16_t jrk_overridable_settings_get_motor_current_limit_code_reverse(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->motor_current_limit_code_reverse;
}

void jrk_overridable_settings_set_motor_brake_duration_forward(
  jrk_overridable_settings * settings,
  uint32_t motor_brake_duration_forward)
{
  if (settings == NULL) { return; }
  settings->motor_brake_duration_forward = motor_brake_duration_forward;
}

uint32_t jrk_overridable_settings_get_motor_brake_duration_forward(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->motor_brake_duration_forward;
}

void jrk_overridable_settings_set_motor_brake_duration_reverse(
  jrk_overridable_settings * settings,
  uint32_t motor_brake_duration_reverse)
{
  if (settings == NULL) { return; }
  settings->motor_brake_duration_reverse = motor_brake_duration_reverse;
}

uint32_t jrk_overridable_settings_get_motor_brake_duration_reverse(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->motor_brake_duration_reverse;
}

void jrk_overridable_settings_set_motor_coast_when_off(
  jrk_overridable_settings * settings,
  bool motor_coast_when_off)
{
  if (settings == NULL) { return; }
  settings->motor_coast_when_off = motor_coast_when_off;
}

bool jrk_overridable_settings_get_motor_coast_when_off(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->motor_coast_when_off;
}

// End of auto-generated overridable settings accessors.
