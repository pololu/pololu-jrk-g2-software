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
  uint16_t integral_limit;
  bool reset_integral;
  uint16_t max_duty_cycle_while_feedback_out_of_range;
  uint16_t max_acceleration_forward;
  uint16_t max_acceleration_reverse;
  uint16_t max_deceleration_forward;
  uint16_t max_deceleration_reverse;
  uint16_t max_duty_cycle_forward;
  uint16_t max_duty_cycle_reverse;
  uint16_t current_limit_code_forward;
  uint16_t current_limit_code_reverse;
  uint32_t brake_duration_forward;
  uint32_t brake_duration_reverse;
  uint16_t max_current_forward;
  uint16_t max_current_reverse;
  bool coast_when_off;

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

void jrk_overridable_settings_set_integral_limit(
  jrk_overridable_settings * settings,
  uint16_t integral_limit)
{
  if (settings == NULL) { return; }
  settings->integral_limit = integral_limit;
}

uint16_t jrk_overridable_settings_get_integral_limit(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->integral_limit;
}

void jrk_overridable_settings_set_reset_integral(
  jrk_overridable_settings * settings,
  bool reset_integral)
{
  if (settings == NULL) { return; }
  settings->reset_integral = reset_integral;
}

bool jrk_overridable_settings_get_reset_integral(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->reset_integral;
}

void jrk_overridable_settings_set_max_duty_cycle_while_feedback_out_of_range(
  jrk_overridable_settings * settings,
  uint16_t max_duty_cycle_while_feedback_out_of_range)
{
  if (settings == NULL) { return; }
  settings->max_duty_cycle_while_feedback_out_of_range = max_duty_cycle_while_feedback_out_of_range;
}

uint16_t jrk_overridable_settings_get_max_duty_cycle_while_feedback_out_of_range(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->max_duty_cycle_while_feedback_out_of_range;
}

void jrk_overridable_settings_set_max_acceleration_forward(
  jrk_overridable_settings * settings,
  uint16_t max_acceleration_forward)
{
  if (settings == NULL) { return; }
  settings->max_acceleration_forward = max_acceleration_forward;
}

uint16_t jrk_overridable_settings_get_max_acceleration_forward(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->max_acceleration_forward;
}

void jrk_overridable_settings_set_max_acceleration_reverse(
  jrk_overridable_settings * settings,
  uint16_t max_acceleration_reverse)
{
  if (settings == NULL) { return; }
  settings->max_acceleration_reverse = max_acceleration_reverse;
}

uint16_t jrk_overridable_settings_get_max_acceleration_reverse(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->max_acceleration_reverse;
}

void jrk_overridable_settings_set_max_deceleration_forward(
  jrk_overridable_settings * settings,
  uint16_t max_deceleration_forward)
{
  if (settings == NULL) { return; }
  settings->max_deceleration_forward = max_deceleration_forward;
}

uint16_t jrk_overridable_settings_get_max_deceleration_forward(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->max_deceleration_forward;
}

void jrk_overridable_settings_set_max_deceleration_reverse(
  jrk_overridable_settings * settings,
  uint16_t max_deceleration_reverse)
{
  if (settings == NULL) { return; }
  settings->max_deceleration_reverse = max_deceleration_reverse;
}

uint16_t jrk_overridable_settings_get_max_deceleration_reverse(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->max_deceleration_reverse;
}

void jrk_overridable_settings_set_max_duty_cycle_forward(
  jrk_overridable_settings * settings,
  uint16_t max_duty_cycle_forward)
{
  if (settings == NULL) { return; }
  settings->max_duty_cycle_forward = max_duty_cycle_forward;
}

uint16_t jrk_overridable_settings_get_max_duty_cycle_forward(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->max_duty_cycle_forward;
}

void jrk_overridable_settings_set_max_duty_cycle_reverse(
  jrk_overridable_settings * settings,
  uint16_t max_duty_cycle_reverse)
{
  if (settings == NULL) { return; }
  settings->max_duty_cycle_reverse = max_duty_cycle_reverse;
}

uint16_t jrk_overridable_settings_get_max_duty_cycle_reverse(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->max_duty_cycle_reverse;
}

void jrk_overridable_settings_set_current_limit_code_forward(
  jrk_overridable_settings * settings,
  uint16_t current_limit_code_forward)
{
  if (settings == NULL) { return; }
  settings->current_limit_code_forward = current_limit_code_forward;
}

uint16_t jrk_overridable_settings_get_current_limit_code_forward(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->current_limit_code_forward;
}

void jrk_overridable_settings_set_current_limit_code_reverse(
  jrk_overridable_settings * settings,
  uint16_t current_limit_code_reverse)
{
  if (settings == NULL) { return; }
  settings->current_limit_code_reverse = current_limit_code_reverse;
}

uint16_t jrk_overridable_settings_get_current_limit_code_reverse(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->current_limit_code_reverse;
}

void jrk_overridable_settings_set_brake_duration_forward(
  jrk_overridable_settings * settings,
  uint32_t brake_duration_forward)
{
  if (settings == NULL) { return; }
  settings->brake_duration_forward = brake_duration_forward;
}

uint32_t jrk_overridable_settings_get_brake_duration_forward(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->brake_duration_forward;
}

void jrk_overridable_settings_set_brake_duration_reverse(
  jrk_overridable_settings * settings,
  uint32_t brake_duration_reverse)
{
  if (settings == NULL) { return; }
  settings->brake_duration_reverse = brake_duration_reverse;
}

uint32_t jrk_overridable_settings_get_brake_duration_reverse(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->brake_duration_reverse;
}

void jrk_overridable_settings_set_max_current_forward(
  jrk_overridable_settings * settings,
  uint16_t max_current_forward)
{
  if (settings == NULL) { return; }
  settings->max_current_forward = max_current_forward;
}

uint16_t jrk_overridable_settings_get_max_current_forward(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->max_current_forward;
}

void jrk_overridable_settings_set_max_current_reverse(
  jrk_overridable_settings * settings,
  uint16_t max_current_reverse)
{
  if (settings == NULL) { return; }
  settings->max_current_reverse = max_current_reverse;
}

uint16_t jrk_overridable_settings_get_max_current_reverse(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->max_current_reverse;
}

void jrk_overridable_settings_set_coast_when_off(
  jrk_overridable_settings * settings,
  bool coast_when_off)
{
  if (settings == NULL) { return; }
  settings->coast_when_off = coast_when_off;
}

bool jrk_overridable_settings_get_coast_when_off(
  const jrk_overridable_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->coast_when_off;
}

// End of auto-generated overridable settings accessors.
