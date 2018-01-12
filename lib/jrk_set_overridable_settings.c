#include "jrk_internal.h"

static void jrk_write_overridable_settings_to_buffer(
  const jrk_overridable_settings * settings, uint8_t * buf)
{
  assert(settings != NULL);
  assert(buf != NULL);

  // Beginning of auto-generated overridable-settings-to-buffer code.

  {
    uint16_t proportional_multiplier = jrk_overridable_settings_get_proportional_multiplier(settings);
    write_uint16_t(buf + (JRK_SETTING_PROPORTIONAL_MULTIPLIER - JRK_OVERRIDABLE_SETTINGS_START),
      proportional_multiplier);
  }

  {
    uint8_t proportional_exponent = jrk_overridable_settings_get_proportional_exponent(settings);
    buf[JRK_SETTING_PROPORTIONAL_EXPONENT - JRK_OVERRIDABLE_SETTINGS_START] =
      proportional_exponent;
  }

  {
    uint16_t integral_multiplier = jrk_overridable_settings_get_integral_multiplier(settings);
    write_uint16_t(buf + (JRK_SETTING_INTEGRAL_MULTIPLIER - JRK_OVERRIDABLE_SETTINGS_START),
      integral_multiplier);
  }

  {
    uint8_t integral_exponent = jrk_overridable_settings_get_integral_exponent(settings);
    buf[JRK_SETTING_INTEGRAL_EXPONENT - JRK_OVERRIDABLE_SETTINGS_START] =
      integral_exponent;
  }

  {
    uint16_t derivative_multiplier = jrk_overridable_settings_get_derivative_multiplier(settings);
    write_uint16_t(buf + (JRK_SETTING_DERIVATIVE_MULTIPLIER - JRK_OVERRIDABLE_SETTINGS_START),
      derivative_multiplier);
  }

  {
    uint8_t derivative_exponent = jrk_overridable_settings_get_derivative_exponent(settings);
    buf[JRK_SETTING_DERIVATIVE_EXPONENT - JRK_OVERRIDABLE_SETTINGS_START] =
      derivative_exponent;
  }

  {
    uint16_t pid_period = jrk_overridable_settings_get_pid_period(settings);
    write_uint16_t(buf + (JRK_SETTING_PID_PERIOD - JRK_OVERRIDABLE_SETTINGS_START),
      pid_period);
  }

  {
    uint16_t pid_integral_limit = jrk_overridable_settings_get_pid_integral_limit(settings);
    write_uint16_t(buf + (JRK_SETTING_PID_INTEGRAL_LIMIT - JRK_OVERRIDABLE_SETTINGS_START),
      pid_integral_limit);
  }

  {
    bool pid_reset_integral = jrk_overridable_settings_get_pid_reset_integral(settings);
    buf[JRK_SETTING_OPTIONS_BYTE3 - JRK_OVERRIDABLE_SETTINGS_START] |=
      pid_reset_integral << JRK_OPTIONS_BYTE3_PID_RESET_INTEGRAL;
  }

  {
    uint16_t motor_max_duty_cycle_while_feedback_out_of_range = jrk_overridable_settings_get_motor_max_duty_cycle_while_feedback_out_of_range(settings);
    write_uint16_t(buf + (JRK_SETTING_MOTOR_MAX_DUTY_CYCLE_WHILE_FEEDBACK_OUT_OF_RANGE - JRK_OVERRIDABLE_SETTINGS_START),
      motor_max_duty_cycle_while_feedback_out_of_range);
  }

  {
    uint16_t motor_max_acceleration_forward = jrk_overridable_settings_get_motor_max_acceleration_forward(settings);
    write_uint16_t(buf + (JRK_SETTING_MOTOR_MAX_ACCELERATION_FORWARD - JRK_OVERRIDABLE_SETTINGS_START),
      motor_max_acceleration_forward);
  }

  {
    uint16_t motor_max_acceleration_reverse = jrk_overridable_settings_get_motor_max_acceleration_reverse(settings);
    write_uint16_t(buf + (JRK_SETTING_MOTOR_MAX_ACCELERATION_REVERSE - JRK_OVERRIDABLE_SETTINGS_START),
      motor_max_acceleration_reverse);
  }

  {
    uint16_t motor_max_deceleration_forward = jrk_overridable_settings_get_motor_max_deceleration_forward(settings);
    write_uint16_t(buf + (JRK_SETTING_MOTOR_MAX_DECELERATION_FORWARD - JRK_OVERRIDABLE_SETTINGS_START),
      motor_max_deceleration_forward);
  }

  {
    uint16_t motor_max_deceleration_reverse = jrk_overridable_settings_get_motor_max_deceleration_reverse(settings);
    write_uint16_t(buf + (JRK_SETTING_MOTOR_MAX_DECELERATION_REVERSE - JRK_OVERRIDABLE_SETTINGS_START),
      motor_max_deceleration_reverse);
  }

  {
    uint16_t motor_max_duty_cycle_forward = jrk_overridable_settings_get_motor_max_duty_cycle_forward(settings);
    write_uint16_t(buf + (JRK_SETTING_MOTOR_MAX_DUTY_CYCLE_FORWARD - JRK_OVERRIDABLE_SETTINGS_START),
      motor_max_duty_cycle_forward);
  }

  {
    uint16_t motor_max_duty_cycle_reverse = jrk_overridable_settings_get_motor_max_duty_cycle_reverse(settings);
    write_uint16_t(buf + (JRK_SETTING_MOTOR_MAX_DUTY_CYCLE_REVERSE - JRK_OVERRIDABLE_SETTINGS_START),
      motor_max_duty_cycle_reverse);
  }

  {
    bool motor_coast_when_off = jrk_overridable_settings_get_motor_coast_when_off(settings);
    buf[JRK_SETTING_OPTIONS_BYTE3 - JRK_OVERRIDABLE_SETTINGS_START] |=
      motor_coast_when_off << JRK_OPTIONS_BYTE3_MOTOR_COAST_WHEN_OFF;
  }

  // End of auto-generated overridable-settings-to-buffer code.

  {
    buf[JRK_SETTING_MOTOR_BRAKE_DURATION_FORWARD - JRK_OVERRIDABLE_SETTINGS_START] =
      jrk_overridable_settings_get_motor_brake_duration_forward(settings)
      / JRK_BRAKE_DURATION_UNITS;
  }

  {
    buf[JRK_SETTING_MOTOR_BRAKE_DURATION_REVERSE - JRK_OVERRIDABLE_SETTINGS_START] =
      jrk_overridable_settings_get_motor_brake_duration_reverse(settings)
      / JRK_BRAKE_DURATION_UNITS;
  }

  {
    uint8_t motor_max_current_forward =
      jrk_overridable_settings_get_motor_max_current_forward(settings);
    buf[JRK_SETTING_MOTOR_MAX_CURRENT_FORWARD - JRK_OVERRIDABLE_SETTINGS_START] =
      motor_max_current_forward * TMPHAX_CURRENT_UNITS;
  }

  {
    uint8_t motor_max_current_reverse =
      jrk_overridable_settings_get_motor_max_current_reverse(settings);
    buf[JRK_SETTING_MOTOR_MAX_CURRENT_REVERSE - JRK_OVERRIDABLE_SETTINGS_START] =
      motor_max_current_reverse * TMPHAX_CURRENT_UNITS;
  }
}

jrk_error * jrk_set_overridable_settings(jrk_handle * handle,
  const jrk_overridable_settings * settings)
{
  if (handle == NULL)
  {
    return jrk_error_create("Handle is null.");
  }

  if (settings == NULL)
  {
    return jrk_error_create("Overridable settings object is null.");
  }

  // Note: Checking settings ranges here would be nice.

  // Construct a buffer holding the bytes we want to write.
  uint8_t buf[JRK_SETTINGS_SIZE - JRK_OVERRIDABLE_SETTINGS_START];
  memset(buf, 0, sizeof(buf));
  jrk_write_overridable_settings_to_buffer(settings, buf);

  jrk_error * error = NULL;

  // Write the bytes to the device.
  if (error == NULL)
  {
    error = jrk_set_overridable_setting_segment(handle, 0, sizeof(buf), buf);
  }

  if (error != NULL)
  {
    error = jrk_error_add(error,
      "There was an error overriding settings on the device.");
  }

  return error;
}
