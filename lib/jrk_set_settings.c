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
    uint16_t input_error_minimum = jrk_settings_get_input_error_minimum(settings);
    write_uint16_t(buf + JRK_SETTING_INPUT_ERROR_MINIMUM, input_error_minimum);
  }

  {
    uint16_t input_error_maximum = jrk_settings_get_input_error_maximum(settings);
    write_uint16_t(buf + JRK_SETTING_INPUT_ERROR_MAXIMUM, input_error_maximum);
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
    buf[JRK_SETTING_OPTIONS_BYTE2] |= input_invert << JRK_OPTIONS_BYTE2_INPUT_INVERT;
  }

  {
    uint8_t input_scaling_degree = jrk_settings_get_input_scaling_degree(settings);
    buf[JRK_SETTING_INPUT_SCALING_DEGREE] = input_scaling_degree;
  }

  {
    bool input_detect_disconnect = jrk_settings_get_input_detect_disconnect(settings);
    buf[JRK_SETTING_OPTIONS_BYTE2] |= input_detect_disconnect << JRK_OPTIONS_BYTE2_INPUT_DETECT_DISCONNECT;
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
    uint16_t feedback_error_minimum = jrk_settings_get_feedback_error_minimum(settings);
    write_uint16_t(buf + JRK_SETTING_FEEDBACK_ERROR_MINIMUM, feedback_error_minimum);
  }

  {
    uint16_t feedback_error_maximum = jrk_settings_get_feedback_error_maximum(settings);
    write_uint16_t(buf + JRK_SETTING_FEEDBACK_ERROR_MAXIMUM, feedback_error_maximum);
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
    buf[JRK_SETTING_OPTIONS_BYTE2] |= feedback_invert << JRK_OPTIONS_BYTE2_FEEDBACK_INVERT;
  }

  {
    bool feedback_detect_disconnect = jrk_settings_get_feedback_detect_disconnect(settings);
    buf[JRK_SETTING_OPTIONS_BYTE2] |= feedback_detect_disconnect << JRK_OPTIONS_BYTE2_FEEDBACK_DETECT_DISCONNECT;
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
    buf[JRK_SETTING_OPTIONS_BYTE2] |= feedback_wraparound << JRK_OPTIONS_BYTE2_FEEDBACK_WRAPAROUND;
  }

  {
    uint8_t serial_mode = jrk_settings_get_serial_mode(settings);
    buf[JRK_SETTING_SERIAL_MODE] = serial_mode;
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
    uint8_t integral_reduction_exponent = jrk_settings_get_integral_reduction_exponent(settings);
    buf[JRK_SETTING_INTEGRAL_REDUCTION_EXPONENT] = integral_reduction_exponent;
  }

  {
    uint16_t integral_limit = jrk_settings_get_integral_limit(settings);
    write_uint16_t(buf + JRK_SETTING_INTEGRAL_LIMIT, integral_limit);
  }

  {
    bool reset_integral = jrk_settings_get_reset_integral(settings);
    buf[JRK_SETTING_OPTIONS_BYTE3] |= reset_integral << JRK_OPTIONS_BYTE3_RESET_INTEGRAL;
  }

  {
    uint8_t pwm_frequency = jrk_settings_get_pwm_frequency(settings);
    buf[JRK_SETTING_PWM_FREQUENCY] = pwm_frequency;
  }

  {
    uint8_t current_samples_exponent = jrk_settings_get_current_samples_exponent(settings);
    buf[JRK_SETTING_CURRENT_SAMPLES_EXPONENT] = current_samples_exponent;
  }

  {
    uint8_t overcurrent_threshold = jrk_settings_get_overcurrent_threshold(settings);
    buf[JRK_SETTING_OVERCURRENT_THRESHOLD] = overcurrent_threshold;
  }

  {
    int16_t current_offset_calibration = jrk_settings_get_current_offset_calibration(settings);
    write_int16_t(buf + JRK_SETTING_CURRENT_OFFSET_CALIBRATION, current_offset_calibration);
  }

  {
    int16_t current_scale_calibration = jrk_settings_get_current_scale_calibration(settings);
    write_int16_t(buf + JRK_SETTING_CURRENT_SCALE_CALIBRATION, current_scale_calibration);
  }

  {
    bool motor_invert = jrk_settings_get_motor_invert(settings);
    buf[JRK_SETTING_OPTIONS_BYTE2] |= motor_invert << JRK_OPTIONS_BYTE2_MOTOR_INVERT;
  }

  {
    uint16_t max_duty_cycle_while_feedback_out_of_range = jrk_settings_get_max_duty_cycle_while_feedback_out_of_range(settings);
    write_uint16_t(buf + JRK_SETTING_MAX_DUTY_CYCLE_WHILE_FEEDBACK_OUT_OF_RANGE, max_duty_cycle_while_feedback_out_of_range);
  }

  {
    uint16_t max_acceleration_forward = jrk_settings_get_max_acceleration_forward(settings);
    write_uint16_t(buf + JRK_SETTING_MAX_ACCELERATION_FORWARD, max_acceleration_forward);
  }

  {
    uint16_t max_acceleration_reverse = jrk_settings_get_max_acceleration_reverse(settings);
    write_uint16_t(buf + JRK_SETTING_MAX_ACCELERATION_REVERSE, max_acceleration_reverse);
  }

  {
    uint16_t max_deceleration_forward = jrk_settings_get_max_deceleration_forward(settings);
    write_uint16_t(buf + JRK_SETTING_MAX_DECELERATION_FORWARD, max_deceleration_forward);
  }

  {
    uint16_t max_deceleration_reverse = jrk_settings_get_max_deceleration_reverse(settings);
    write_uint16_t(buf + JRK_SETTING_MAX_DECELERATION_REVERSE, max_deceleration_reverse);
  }

  {
    uint16_t max_duty_cycle_forward = jrk_settings_get_max_duty_cycle_forward(settings);
    write_uint16_t(buf + JRK_SETTING_MAX_DUTY_CYCLE_FORWARD, max_duty_cycle_forward);
  }

  {
    uint16_t max_duty_cycle_reverse = jrk_settings_get_max_duty_cycle_reverse(settings);
    write_uint16_t(buf + JRK_SETTING_MAX_DUTY_CYCLE_REVERSE, max_duty_cycle_reverse);
  }

  {
    uint16_t current_limit_code_forward = jrk_settings_get_current_limit_code_forward(settings);
    write_uint16_t(buf + JRK_SETTING_CURRENT_LIMIT_CODE_FORWARD, current_limit_code_forward);
  }

  {
    uint16_t current_limit_code_reverse = jrk_settings_get_current_limit_code_reverse(settings);
    write_uint16_t(buf + JRK_SETTING_CURRENT_LIMIT_CODE_REVERSE, current_limit_code_reverse);
  }

  {
    uint16_t max_current_forward = jrk_settings_get_max_current_forward(settings);
    write_uint16_t(buf + JRK_SETTING_MAX_CURRENT_FORWARD, max_current_forward);
  }

  {
    uint16_t max_current_reverse = jrk_settings_get_max_current_reverse(settings);
    write_uint16_t(buf + JRK_SETTING_MAX_CURRENT_REVERSE, max_current_reverse);
  }

  {
    bool coast_when_off = jrk_settings_get_coast_when_off(settings);
    buf[JRK_SETTING_OPTIONS_BYTE3] |= coast_when_off << JRK_OPTIONS_BYTE3_COAST_WHEN_OFF;
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
    uint16_t error_hard = jrk_settings_get_error_hard(settings);
    write_uint16_t(buf + JRK_SETTING_ERROR_HARD, error_hard);
  }

  {
    int16_t vin_calibration = jrk_settings_get_vin_calibration(settings);
    write_int16_t(buf + JRK_SETTING_VIN_CALIBRATION, vin_calibration);
  }

  {
    bool disable_i2c_pullups = jrk_settings_get_disable_i2c_pullups(settings);
    buf[JRK_SETTING_OPTIONS_BYTE1] |= disable_i2c_pullups << JRK_OPTIONS_BYTE1_DISABLE_I2C_PULLUPS;
  }

  {
    bool analog_sda_pullup = jrk_settings_get_analog_sda_pullup(settings);
    buf[JRK_SETTING_OPTIONS_BYTE1] |= analog_sda_pullup << JRK_OPTIONS_BYTE1_ANALOG_SDA_PULLUP;
  }

  {
    bool always_analog_sda = jrk_settings_get_always_analog_sda(settings);
    buf[JRK_SETTING_OPTIONS_BYTE1] |= always_analog_sda << JRK_OPTIONS_BYTE1_ALWAYS_ANALOG_SDA;
  }

  {
    bool always_analog_fba = jrk_settings_get_always_analog_fba(settings);
    buf[JRK_SETTING_OPTIONS_BYTE1] |= always_analog_fba << JRK_OPTIONS_BYTE1_ALWAYS_ANALOG_FBA;
  }

  {
    uint8_t fbt_mode = jrk_settings_get_fbt_mode(settings);
    buf[JRK_SETTING_FBT_MODE] = fbt_mode;
  }

  {
    uint8_t fbt_timing_clock = jrk_settings_get_fbt_timing_clock(settings);
    buf[JRK_SETTING_FBT_OPTIONS] |= (fbt_timing_clock & JRK_FBT_OPTIONS_TIMING_CLOCK_MASK) << JRK_FBT_OPTIONS_TIMING_CLOCK;
  }

  {
    bool fbt_timing_polarity = jrk_settings_get_fbt_timing_polarity(settings);
    buf[JRK_SETTING_FBT_OPTIONS] |= fbt_timing_polarity << JRK_FBT_OPTIONS_TIMING_POLARITY;
  }

  {
    uint16_t fbt_timing_timeout = jrk_settings_get_fbt_timing_timeout(settings);
    write_uint16_t(buf + JRK_SETTING_FBT_TIMING_TIMEOUT, fbt_timing_timeout);
  }

  {
    uint8_t fbt_averaging_count = jrk_settings_get_fbt_averaging_count(settings);
    buf[JRK_SETTING_FBT_AVERAGING_COUNT] = fbt_averaging_count;
  }

  {
    bool fbt_reciprocal = jrk_settings_get_fbt_reciprocal(settings);
    buf[JRK_SETTING_FBT_OPTIONS] |= fbt_reciprocal << JRK_FBT_OPTIONS_RECIPROCAL;
  }

  {
    uint8_t fbt_divider_exponent = jrk_settings_get_fbt_divider_exponent(settings);
    buf[JRK_SETTING_FBT_DIVIDER_EXPONENT] = fbt_divider_exponent;
  }

  // End of auto-generated settings-to-buffer code.

  {
    uint32_t baud_rate = jrk_settings_get_serial_baud_rate(settings);
    uint16_t brg = jrk_baud_rate_to_brg(baud_rate);
    write_uint16_t(buf + JRK_SETTING_SERIAL_BAUD_RATE_GENERATOR, brg);
  }

  {
    buf[JRK_SETTING_BRAKE_DURATION_FORWARD] =
      jrk_settings_get_brake_duration_forward(settings)
      / JRK_BRAKE_DURATION_UNITS;
  }

  {
    buf[JRK_SETTING_BRAKE_DURATION_REVERSE] =
      jrk_settings_get_brake_duration_reverse(settings)
      / JRK_BRAKE_DURATION_UNITS;
  }

  {
    write_uint16_t(buf + JRK_SETTING_SERIAL_TIMEOUT,
      jrk_settings_get_serial_timeout(settings)
      / JRK_SERIAL_TIMEOUT_UNITS);
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
  if (error == NULL)
  {
    jrk_write_settings_to_buffer(fixed_settings, buf);
  }

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
