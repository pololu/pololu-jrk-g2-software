// Function for reading settings from the Jrk over USB.

#include "jrk_internal.h"

static void write_buffer_to_overridable_settings(const uint8_t * buf,
  jrk_overridable_settings * settings)
{
  // Beginning of auto-generated buffer-to-overridable-settings code.

  {
    uint16_t proportional_multiplier = read_uint16_t(
      buf + (JRK_SETTING_PROPORTIONAL_MULTIPLIER - JRK_OVERRIDABLE_SETTINGS_START));
    jrk_overridable_settings_set_proportional_multiplier(settings, proportional_multiplier);
  }

  {
    uint8_t proportional_exponent =
      buf[JRK_SETTING_PROPORTIONAL_EXPONENT - JRK_OVERRIDABLE_SETTINGS_START];
    jrk_overridable_settings_set_proportional_exponent(settings, proportional_exponent);
  }

  {
    uint16_t integral_multiplier = read_uint16_t(
      buf + (JRK_SETTING_INTEGRAL_MULTIPLIER - JRK_OVERRIDABLE_SETTINGS_START));
    jrk_overridable_settings_set_integral_multiplier(settings, integral_multiplier);
  }

  {
    uint8_t integral_exponent =
      buf[JRK_SETTING_INTEGRAL_EXPONENT - JRK_OVERRIDABLE_SETTINGS_START];
    jrk_overridable_settings_set_integral_exponent(settings, integral_exponent);
  }

  {
    uint16_t derivative_multiplier = read_uint16_t(
      buf + (JRK_SETTING_DERIVATIVE_MULTIPLIER - JRK_OVERRIDABLE_SETTINGS_START));
    jrk_overridable_settings_set_derivative_multiplier(settings, derivative_multiplier);
  }

  {
    uint8_t derivative_exponent =
      buf[JRK_SETTING_DERIVATIVE_EXPONENT - JRK_OVERRIDABLE_SETTINGS_START];
    jrk_overridable_settings_set_derivative_exponent(settings, derivative_exponent);
  }

  {
    uint16_t pid_period = read_uint16_t(
      buf + (JRK_SETTING_PID_PERIOD - JRK_OVERRIDABLE_SETTINGS_START));
    jrk_overridable_settings_set_pid_period(settings, pid_period);
  }

  {
    uint16_t pid_integral_limit = read_uint16_t(
      buf + (JRK_SETTING_PID_INTEGRAL_LIMIT - JRK_OVERRIDABLE_SETTINGS_START));
    jrk_overridable_settings_set_pid_integral_limit(settings, pid_integral_limit);
  }

  {
    bool pid_reset_integral =
      buf[JRK_SETTING_OPTIONS_BYTE3 - JRK_OVERRIDABLE_SETTINGS_START] >> JRK_OPTIONS_BYTE3_PID_RESET_INTEGRAL & 1;
    jrk_overridable_settings_set_pid_reset_integral(settings, pid_reset_integral);
  }

  {
    uint16_t motor_max_duty_cycle_while_feedback_out_of_range = read_uint16_t(
      buf + (JRK_SETTING_MOTOR_MAX_DUTY_CYCLE_WHILE_FEEDBACK_OUT_OF_RANGE - JRK_OVERRIDABLE_SETTINGS_START));
    jrk_overridable_settings_set_motor_max_duty_cycle_while_feedback_out_of_range(settings, motor_max_duty_cycle_while_feedback_out_of_range);
  }

  {
    uint16_t motor_max_acceleration_forward = read_uint16_t(
      buf + (JRK_SETTING_MOTOR_MAX_ACCELERATION_FORWARD - JRK_OVERRIDABLE_SETTINGS_START));
    jrk_overridable_settings_set_motor_max_acceleration_forward(settings, motor_max_acceleration_forward);
  }

  {
    uint16_t motor_max_acceleration_reverse = read_uint16_t(
      buf + (JRK_SETTING_MOTOR_MAX_ACCELERATION_REVERSE - JRK_OVERRIDABLE_SETTINGS_START));
    jrk_overridable_settings_set_motor_max_acceleration_reverse(settings, motor_max_acceleration_reverse);
  }

  {
    uint16_t motor_max_deceleration_forward = read_uint16_t(
      buf + (JRK_SETTING_MOTOR_MAX_DECELERATION_FORWARD - JRK_OVERRIDABLE_SETTINGS_START));
    jrk_overridable_settings_set_motor_max_deceleration_forward(settings, motor_max_deceleration_forward);
  }

  {
    uint16_t motor_max_deceleration_reverse = read_uint16_t(
      buf + (JRK_SETTING_MOTOR_MAX_DECELERATION_REVERSE - JRK_OVERRIDABLE_SETTINGS_START));
    jrk_overridable_settings_set_motor_max_deceleration_reverse(settings, motor_max_deceleration_reverse);
  }

  {
    uint16_t motor_max_duty_cycle_forward = read_uint16_t(
      buf + (JRK_SETTING_MOTOR_MAX_DUTY_CYCLE_FORWARD - JRK_OVERRIDABLE_SETTINGS_START));
    jrk_overridable_settings_set_motor_max_duty_cycle_forward(settings, motor_max_duty_cycle_forward);
  }

  {
    uint16_t motor_max_duty_cycle_reverse = read_uint16_t(
      buf + (JRK_SETTING_MOTOR_MAX_DUTY_CYCLE_REVERSE - JRK_OVERRIDABLE_SETTINGS_START));
    jrk_overridable_settings_set_motor_max_duty_cycle_reverse(settings, motor_max_duty_cycle_reverse);
  }

  {
    uint16_t motor_current_limit_code_forward = read_uint16_t(
      buf + (JRK_SETTING_MOTOR_CURRENT_LIMIT_CODE_FORWARD - JRK_OVERRIDABLE_SETTINGS_START));
    jrk_overridable_settings_set_motor_current_limit_code_forward(settings, motor_current_limit_code_forward);
  }

  {
    uint16_t motor_current_limit_code_reverse = read_uint16_t(
      buf + (JRK_SETTING_MOTOR_CURRENT_LIMIT_CODE_REVERSE - JRK_OVERRIDABLE_SETTINGS_START));
    jrk_overridable_settings_set_motor_current_limit_code_reverse(settings, motor_current_limit_code_reverse);
  }

  {
    bool motor_coast_when_off =
      buf[JRK_SETTING_OPTIONS_BYTE3 - JRK_OVERRIDABLE_SETTINGS_START] >> JRK_OPTIONS_BYTE3_MOTOR_COAST_WHEN_OFF & 1;
    jrk_overridable_settings_set_motor_coast_when_off(settings, motor_coast_when_off);
  }

  // End of auto-generated buffer-to-overridable-settings code.

  {
    uint32_t duration = buf[JRK_SETTING_MOTOR_BRAKE_DURATION_FORWARD -
      JRK_OVERRIDABLE_SETTINGS_START] * JRK_BRAKE_DURATION_UNITS;
    jrk_overridable_settings_set_motor_brake_duration_forward(settings, duration);
  }

  {
    uint32_t duration = buf[JRK_SETTING_MOTOR_BRAKE_DURATION_REVERSE -
      JRK_OVERRIDABLE_SETTINGS_START] * JRK_BRAKE_DURATION_UNITS;
    jrk_overridable_settings_set_motor_brake_duration_reverse(settings, duration);
  }
}

jrk_error * jrk_get_overridable_settings(jrk_handle * handle,
  jrk_overridable_settings ** settings)
{
  if (settings == NULL)
  {
    return jrk_error_create("Overridable settings output pointer is null.");
  }

  *settings = NULL;

  if (handle == NULL)
  {
    return jrk_error_create("Handle is null.");
  }

  jrk_error * error = NULL;

  // Allocate the new settings object.
  jrk_overridable_settings * new_settings = NULL;
  if (error == NULL)
  {
    error = jrk_overridable_settings_create(&new_settings);
  }

  // Read all the settings from the device into a buffer.
  uint8_t buf[JRK_SETTINGS_SIZE - JRK_OVERRIDABLE_SETTINGS_START];
  memset(buf, 0, sizeof(buf));
  if (error == NULL)
  {
    error = jrk_get_overridable_setting_segment(handle, 0, sizeof(buf), buf);
  }

  // Store the settings in the new settings object.
  if (error == NULL)
  {
    write_buffer_to_overridable_settings(buf, new_settings);
  }

  // Pass the new settings to the caller.
  if (error == NULL)
  {
    *settings = new_settings;
    new_settings = NULL;
  }

  jrk_overridable_settings_free(new_settings);

  if (error != NULL)
  {
    error = jrk_error_add(error,
      "There was an error reading settings from the device.");
  }

  return error;
}
