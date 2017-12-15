#include "jrk_internal.h"

static void jrk_write_settings_to_buffer(const jrk_settings * settings, uint8_t * buf)
{
  assert(settings != NULL);
  assert(buf != NULL);

  // Beginning of auto-generated settings-to-buffer code.

  {
    uint8_t input_mode = jrk_settings_get_input_mode(settings);
    buf[JRK_SETTING_INPUT_MODE] = input_mode;
  }

  {
    uint16_t input_absolute_minimum = jrk_settings_get_input_absolute_minimum(settings);
    write_uint16_t(buf + JRK_SETTING_INPUT_ABSOLUTE_MINIMUM, input_absolute_minimum);
  }

  {
    uint16_t input_absolute_maximum = jrk_settings_get_input_absolute_maximum(settings);
    write_uint16_t(buf + JRK_SETTING_INPUT_ABSOLUTE_MAXIMUM, input_absolute_maximum);
  }

  {
    uint16_t input_minimum = jrk_settings_get_input_minimum(settings);
    write_uint16_t(buf + JRK_SETTING_INPUT_MINIMUM, input_minimum);
  }

  {
    uint16_t input_maximum = jrk_settings_get_input_maximum(settings);
    write_uint16_t(buf + JRK_SETTING_INPUT_MAXIMUM, input_maximum);
  }

  {
    uint16_t input_neutral_minimum = jrk_settings_get_input_neutral_minimum(settings);
    write_uint16_t(buf + JRK_SETTING_INPUT_NEUTRAL_MINIMUM, input_neutral_minimum);
  }

  {
    uint16_t input_neutral_maximum = jrk_settings_get_input_neutral_maximum(settings);
    write_uint16_t(buf + JRK_SETTING_INPUT_NEUTRAL_MAXIMUM, input_neutral_maximum);
  }

  {
    uint16_t output_minimum = jrk_settings_get_output_minimum(settings);
    write_uint16_t(buf + JRK_SETTING_OUTPUT_MINIMUM, output_minimum);
  }

  {
    uint16_t output_neutral = jrk_settings_get_output_neutral(settings);
    write_uint16_t(buf + JRK_SETTING_OUTPUT_NEUTRAL, output_neutral);
  }

  {
    uint16_t output_maximum = jrk_settings_get_output_maximum(settings);
    write_uint16_t(buf + JRK_SETTING_OUTPUT_MAXIMUM, output_maximum);
  }

  {
    bool input_invert = jrk_settings_get_input_invert(settings);
    buf[JRK_SETTING_INPUT_INVERT] |= input_invert;
  }

  {
    uint8_t input_scaling_degree = jrk_settings_get_input_scaling_degree(settings);
    buf[JRK_SETTING_INPUT_SCALING_DEGREE] = input_scaling_degree;
  }

  {
    bool input_power_with_aux = jrk_settings_get_input_power_with_aux(settings);
    buf[JRK_SETTING_INPUT_POWER_WITH_AUX] |= input_power_with_aux;
  }

  {
    uint8_t input_analog_samples_exponent = jrk_settings_get_input_analog_samples_exponent(settings);
    buf[JRK_SETTING_INPUT_ANALOG_SAMPLES_EXPONENT] = input_analog_samples_exponent;
  }

  {
    uint8_t feedback_mode = jrk_settings_get_feedback_mode(settings);
    buf[JRK_SETTING_FEEDBACK_MODE] = feedback_mode;
  }

  {
    uint16_t feedback_absolute_minimum = jrk_settings_get_feedback_absolute_minimum(settings);
    write_uint16_t(buf + JRK_SETTING_FEEDBACK_ABSOLUTE_MINIMUM, feedback_absolute_minimum);
  }

  {
    uint16_t feedback_absolute_maximum = jrk_settings_get_feedback_absolute_maximum(settings);
    write_uint16_t(buf + JRK_SETTING_FEEDBACK_ABSOLUTE_MAXIMUM, feedback_absolute_maximum);
  }

  {
    uint16_t feedback_minimum = jrk_settings_get_feedback_minimum(settings);
    write_uint16_t(buf + JRK_SETTING_FEEDBACK_MINIMUM, feedback_minimum);
  }

  {
    uint16_t feedback_maximum = jrk_settings_get_feedback_maximum(settings);
    write_uint16_t(buf + JRK_SETTING_FEEDBACK_MAXIMUM, feedback_maximum);
  }

  {
    bool feedback_invert = jrk_settings_get_feedback_invert(settings);
    buf[JRK_SETTING_FEEDBACK_INVERT] |= feedback_invert;
  }

  {
    bool feedback_power_with_aux = jrk_settings_get_feedback_power_with_aux(settings);
    buf[JRK_SETTING_FEEDBACK_POWER_WITH_AUX] |= feedback_power_with_aux;
  }

  {
    uint8_t feedback_dead_zone = jrk_settings_get_feedback_dead_zone(settings);
    buf[JRK_SETTING_FEEDBACK_DEAD_ZONE] = feedback_dead_zone;
  }

  {
    uint8_t feedback_analog_samples_exponent = jrk_settings_get_feedback_analog_samples_exponent(settings);
    buf[JRK_SETTING_FEEDBACK_ANALOG_SAMPLES_EXPONENT] = feedback_analog_samples_exponent;
  }

  {
    bool feedback_wraparound = jrk_settings_get_feedback_wraparound(settings);
    buf[JRK_SETTING_FEEDBACK_WRAPAROUND] |= feedback_wraparound;
  }

  {
    uint8_t serial_mode = jrk_settings_get_serial_mode(settings);
    buf[JRK_SETTING_SERIAL_MODE] = serial_mode;
  }

  {
    uint16_t serial_timeout = jrk_settings_get_serial_timeout(settings);
    write_uint16_t(buf + JRK_SETTING_SERIAL_TIMEOUT, serial_timeout);
  }

  {
    uint16_t serial_device_number = jrk_settings_get_serial_device_number(settings);
    write_uint16_t(buf + JRK_SETTING_SERIAL_DEVICE_NUMBER, serial_device_number);
  }

  {
    bool never_sleep = jrk_settings_get_never_sleep(settings);
    buf[JRK_SETTING_OPTIONS_BYTE1] |= never_sleep << JRK_OPTIONS_BYTE1_NEVER_SLEEP;
  }

  {
    bool serial_enable_crc = jrk_settings_get_serial_enable_crc(settings);
    buf[JRK_SETTING_OPTIONS_BYTE1] |= serial_enable_crc << JRK_OPTIONS_BYTE1_SERIAL_ENABLE_CRC;
  }

  {
    bool serial_enable_14bit_device_number = jrk_settings_get_serial_enable_14bit_device_number(settings);
    buf[JRK_SETTING_OPTIONS_BYTE1] |= serial_enable_14bit_device_number << JRK_OPTIONS_BYTE1_SERIAL_ENABLE_14BIT_DEVICE_NUMBER;
  }

  {
    bool serial_disable_compact_protocol = jrk_settings_get_serial_disable_compact_protocol(settings);
    buf[JRK_SETTING_OPTIONS_BYTE1] |= serial_disable_compact_protocol << JRK_OPTIONS_BYTE1_SERIAL_DISABLE_COMPACT_PROTOCOL;
  }

  {
    uint16_t proportional_multiplier = jrk_settings_get_proportional_multiplier(settings);
    write_uint16_t(buf + JRK_SETTING_PROPORTIONAL_MULTIPLIER, proportional_multiplier);
  }

  {
    uint8_t proportional_exponent = jrk_settings_get_proportional_exponent(settings);
    buf[JRK_SETTING_PROPORTIONAL_EXPONENT] = proportional_exponent;
  }

  {
    uint16_t integral_multiplier = jrk_settings_get_integral_multiplier(settings);
    write_uint16_t(buf + JRK_SETTING_INTEGRAL_MULTIPLIER, integral_multiplier);
  }

  {
    uint8_t integral_exponent = jrk_settings_get_integral_exponent(settings);
    buf[JRK_SETTING_INTEGRAL_EXPONENT] = integral_exponent;
  }

  {
    uint16_t derivative_multiplier = jrk_settings_get_derivative_multiplier(settings);
    write_uint16_t(buf + JRK_SETTING_DERIVATIVE_MULTIPLIER, derivative_multiplier);
  }

  {
    uint8_t derivative_exponent = jrk_settings_get_derivative_exponent(settings);
    buf[JRK_SETTING_DERIVATIVE_EXPONENT] = derivative_exponent;
  }

  {
    uint16_t pid_period = jrk_settings_get_pid_period(settings);
    write_uint16_t(buf + JRK_SETTING_PID_PERIOD, pid_period);
  }

  {
    uint16_t pid_integral_limit = jrk_settings_get_pid_integral_limit(settings);
    write_uint16_t(buf + JRK_SETTING_PID_INTEGRAL_LIMIT, pid_integral_limit);
  }

  {
    bool pid_reset_integral = jrk_settings_get_pid_reset_integral(settings);
    buf[JRK_SETTING_PID_RESET_INTEGRAL] |= pid_reset_integral;
  }

  {
    uint8_t motor_pwm_frequency = jrk_settings_get_motor_pwm_frequency(settings);
    buf[JRK_SETTING_MOTOR_PWM_FREQUENCY] = motor_pwm_frequency;
  }

  {
    bool motor_invert = jrk_settings_get_motor_invert(settings);
    buf[JRK_SETTING_MOTOR_INVERT] |= motor_invert;
  }

  {
    uint16_t motor_max_duty_cycle_while_feedback_out_of_range = jrk_settings_get_motor_max_duty_cycle_while_feedback_out_of_range(settings);
    write_uint16_t(buf + JRK_SETTING_MOTOR_MAX_DUTY_CYCLE_WHILE_FEEDBACK_OUT_OF_RANGE, motor_max_duty_cycle_while_feedback_out_of_range);
  }

  {
    uint16_t motor_max_acceleration_forward = jrk_settings_get_motor_max_acceleration_forward(settings);
    write_uint16_t(buf + JRK_SETTING_MOTOR_MAX_ACCELERATION_FORWARD, motor_max_acceleration_forward);
  }

  {
    uint16_t motor_max_acceleration_reverse = jrk_settings_get_motor_max_acceleration_reverse(settings);
    write_uint16_t(buf + JRK_SETTING_MOTOR_MAX_ACCELERATION_REVERSE, motor_max_acceleration_reverse);
  }

  {
    uint16_t motor_max_deceleration_forward = jrk_settings_get_motor_max_deceleration_forward(settings);
    write_uint16_t(buf + JRK_SETTING_MOTOR_MAX_DECELERATION_FORWARD, motor_max_deceleration_forward);
  }

  {
    uint16_t motor_max_deceleration_reverse = jrk_settings_get_motor_max_deceleration_reverse(settings);
    write_uint16_t(buf + JRK_SETTING_MOTOR_MAX_DECELERATION_REVERSE, motor_max_deceleration_reverse);
  }

  {
    uint16_t motor_max_duty_cycle_forward = jrk_settings_get_motor_max_duty_cycle_forward(settings);
    write_uint16_t(buf + JRK_SETTING_MOTOR_MAX_DUTY_CYCLE_FORWARD, motor_max_duty_cycle_forward);
  }

  {
    uint16_t motor_max_duty_cycle_reverse = jrk_settings_get_motor_max_duty_cycle_reverse(settings);
    write_uint16_t(buf + JRK_SETTING_MOTOR_MAX_DUTY_CYCLE_REVERSE, motor_max_duty_cycle_reverse);
  }

  {
    uint8_t motor_max_current_forward = jrk_settings_get_motor_max_current_forward(settings);
    buf[JRK_SETTING_MOTOR_MAX_CURRENT_FORWARD] = motor_max_current_forward;
  }

  {
    uint8_t motor_max_current_reverse = jrk_settings_get_motor_max_current_reverse(settings);
    buf[JRK_SETTING_MOTOR_MAX_CURRENT_REVERSE] = motor_max_current_reverse;
  }

  {
    int8_t motor_current_calibration_forward = jrk_settings_get_motor_current_calibration_forward(settings);
    buf[JRK_SETTING_MOTOR_CURRENT_CALIBRATION_FORWARD] = motor_current_calibration_forward;
  }

  {
    int8_t motor_current_calibration_reverse = jrk_settings_get_motor_current_calibration_reverse(settings);
    buf[JRK_SETTING_MOTOR_CURRENT_CALIBRATION_REVERSE] = motor_current_calibration_reverse;
  }

  {
    uint8_t motor_brake_duration_forward = jrk_settings_get_motor_brake_duration_forward(settings);
    buf[JRK_SETTING_MOTOR_BRAKE_DURATION_FORWARD] = motor_brake_duration_forward;
  }

  {
    uint8_t motor_brake_duration_reverse = jrk_settings_get_motor_brake_duration_reverse(settings);
    buf[JRK_SETTING_MOTOR_BRAKE_DURATION_REVERSE] = motor_brake_duration_reverse;
  }

  {
    bool motor_coast_when_off = jrk_settings_get_motor_coast_when_off(settings);
    buf[JRK_SETTING_MOTOR_COAST_WHEN_OFF] |= motor_coast_when_off;
  }

  {
    uint16_t error_enable = jrk_settings_get_error_enable(settings);
    write_uint16_t(buf + JRK_SETTING_ERROR_ENABLE, error_enable);
  }

  {
    uint16_t error_latch = jrk_settings_get_error_latch(settings);
    write_uint16_t(buf + JRK_SETTING_ERROR_LATCH, error_latch);
  }

  {
    int16_t vin_calibration = jrk_settings_get_vin_calibration(settings);
    write_int16_t(buf + JRK_SETTING_VIN_CALIBRATION, vin_calibration);
  }

  // End of auto-generated settings-to-buffer code.

  {
    uint32_t baud_rate = jrk_settings_get_serial_baud_rate(settings);
    uint16_t brg = jrk_baud_rate_to_brg(baud_rate);
    write_uint16_t(buf + JRK_SETTING_SERIAL_BAUD_RATE_GENERATOR, brg);
  }
}

jrk_error * jrk_set_settings(jrk_handle * handle, const jrk_settings * settings)
{
  if (handle == NULL)
  {
    return jrk_error_create("Handle is null.");
  }

  if (settings == NULL)
  {
    return jrk_error_create("Settings object is null.");
  }

  jrk_error * error = NULL;

  jrk_settings * fixed_settings = NULL;

  // Copy the settings so we can fix them without modifying the input ones,
  // which would be surprising to the caller.
  if (error == NULL)
  {
    error = jrk_settings_copy(settings, &fixed_settings);
  }

  // Set the product code of the settings and fix the settings silently to make
  // sure we don't apply invalid settings to the device.  A good app will set
  // the product and call jrk_settings_fix on its own before calling this
  // function, so there should be nothing to fix here.
  if (error == NULL)
  {
    uint8_t product = jrk_device_get_product(jrk_handle_get_device(handle));
    jrk_settings_set_product(fixed_settings, product);

    error = jrk_settings_fix(fixed_settings, NULL);
  }

  // Construct a buffer holding the bytes we want to write.
  uint8_t buf[JRK_SETTINGS_SIZE];
  memset(buf, 0, sizeof(buf));
  jrk_write_settings_to_buffer(fixed_settings, buf);

  // Write the bytes to the device.
  for (uint8_t i = 1; i < sizeof(buf) && error == NULL; i++)
  {
    error = jrk_set_setting_byte(handle, i, buf[i]);
  }

  jrk_settings_free(fixed_settings);

  if (error != NULL)
  {
    error = jrk_error_add(error,
      "There was an error applying settings to the device.");
  }

  return error;
}
