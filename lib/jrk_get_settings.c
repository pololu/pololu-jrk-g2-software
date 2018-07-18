// Function for reading settings from the Jrk over USB.

#include "jrk_internal.h"

static void write_buffer_to_settings(const uint8_t * buf, jrk_settings * settings)
{
  uint32_t product = jrk_settings_get_product(settings);

  // Beginning of auto-generated buffer-to-settings code.

  {
    uint8_t input_mode = buf[JRK_SETTING_INPUT_MODE];
    jrk_settings_set_input_mode(settings, input_mode);
  }

  {
    uint16_t input_error_minimum = read_uint16_t(buf + JRK_SETTING_INPUT_ERROR_MINIMUM);
    jrk_settings_set_input_error_minimum(settings, input_error_minimum);
  }

  {
    uint16_t input_error_maximum = read_uint16_t(buf + JRK_SETTING_INPUT_ERROR_MAXIMUM);
    jrk_settings_set_input_error_maximum(settings, input_error_maximum);
  }

  {
    uint16_t input_minimum = read_uint16_t(buf + JRK_SETTING_INPUT_MINIMUM);
    jrk_settings_set_input_minimum(settings, input_minimum);
  }

  {
    uint16_t input_maximum = read_uint16_t(buf + JRK_SETTING_INPUT_MAXIMUM);
    jrk_settings_set_input_maximum(settings, input_maximum);
  }

  {
    uint16_t input_neutral_minimum = read_uint16_t(buf + JRK_SETTING_INPUT_NEUTRAL_MINIMUM);
    jrk_settings_set_input_neutral_minimum(settings, input_neutral_minimum);
  }

  {
    uint16_t input_neutral_maximum = read_uint16_t(buf + JRK_SETTING_INPUT_NEUTRAL_MAXIMUM);
    jrk_settings_set_input_neutral_maximum(settings, input_neutral_maximum);
  }

  {
    uint16_t output_minimum = read_uint16_t(buf + JRK_SETTING_OUTPUT_MINIMUM);
    jrk_settings_set_output_minimum(settings, output_minimum);
  }

  {
    uint16_t output_neutral = read_uint16_t(buf + JRK_SETTING_OUTPUT_NEUTRAL);
    jrk_settings_set_output_neutral(settings, output_neutral);
  }

  {
    uint16_t output_maximum = read_uint16_t(buf + JRK_SETTING_OUTPUT_MAXIMUM);
    jrk_settings_set_output_maximum(settings, output_maximum);
  }

  {
    bool input_invert = buf[JRK_SETTING_OPTIONS_BYTE2] >> JRK_OPTIONS_BYTE2_INPUT_INVERT & 1;
    jrk_settings_set_input_invert(settings, input_invert);
  }

  {
    uint8_t input_scaling_degree = buf[JRK_SETTING_INPUT_SCALING_DEGREE];
    jrk_settings_set_input_scaling_degree(settings, input_scaling_degree);
  }

  {
    bool input_detect_disconnect = buf[JRK_SETTING_OPTIONS_BYTE2] >> JRK_OPTIONS_BYTE2_INPUT_DETECT_DISCONNECT & 1;
    jrk_settings_set_input_detect_disconnect(settings, input_detect_disconnect);
  }

  {
    uint8_t input_analog_samples_exponent = buf[JRK_SETTING_INPUT_ANALOG_SAMPLES_EXPONENT];
    jrk_settings_set_input_analog_samples_exponent(settings, input_analog_samples_exponent);
  }

  {
    uint8_t feedback_mode = buf[JRK_SETTING_FEEDBACK_MODE];
    jrk_settings_set_feedback_mode(settings, feedback_mode);
  }

  {
    uint16_t feedback_error_minimum = read_uint16_t(buf + JRK_SETTING_FEEDBACK_ERROR_MINIMUM);
    jrk_settings_set_feedback_error_minimum(settings, feedback_error_minimum);
  }

  {
    uint16_t feedback_error_maximum = read_uint16_t(buf + JRK_SETTING_FEEDBACK_ERROR_MAXIMUM);
    jrk_settings_set_feedback_error_maximum(settings, feedback_error_maximum);
  }

  {
    uint16_t feedback_minimum = read_uint16_t(buf + JRK_SETTING_FEEDBACK_MINIMUM);
    jrk_settings_set_feedback_minimum(settings, feedback_minimum);
  }

  {
    uint16_t feedback_maximum = read_uint16_t(buf + JRK_SETTING_FEEDBACK_MAXIMUM);
    jrk_settings_set_feedback_maximum(settings, feedback_maximum);
  }

  {
    bool feedback_invert = buf[JRK_SETTING_OPTIONS_BYTE2] >> JRK_OPTIONS_BYTE2_FEEDBACK_INVERT & 1;
    jrk_settings_set_feedback_invert(settings, feedback_invert);
  }

  {
    bool feedback_detect_disconnect = buf[JRK_SETTING_OPTIONS_BYTE2] >> JRK_OPTIONS_BYTE2_FEEDBACK_DETECT_DISCONNECT & 1;
    jrk_settings_set_feedback_detect_disconnect(settings, feedback_detect_disconnect);
  }

  {
    uint8_t feedback_dead_zone = buf[JRK_SETTING_FEEDBACK_DEAD_ZONE];
    jrk_settings_set_feedback_dead_zone(settings, feedback_dead_zone);
  }

  {
    uint8_t feedback_analog_samples_exponent = buf[JRK_SETTING_FEEDBACK_ANALOG_SAMPLES_EXPONENT];
    jrk_settings_set_feedback_analog_samples_exponent(settings, feedback_analog_samples_exponent);
  }

  {
    bool feedback_wraparound = buf[JRK_SETTING_OPTIONS_BYTE2] >> JRK_OPTIONS_BYTE2_FEEDBACK_WRAPAROUND & 1;
    jrk_settings_set_feedback_wraparound(settings, feedback_wraparound);
  }

  {
    uint8_t serial_mode = buf[JRK_SETTING_SERIAL_MODE];
    jrk_settings_set_serial_mode(settings, serial_mode);
  }

  {
    uint16_t serial_device_number = read_uint16_t(buf + JRK_SETTING_SERIAL_DEVICE_NUMBER);
    jrk_settings_set_serial_device_number(settings, serial_device_number);
  }

  {
    bool never_sleep = buf[JRK_SETTING_OPTIONS_BYTE1] >> JRK_OPTIONS_BYTE1_NEVER_SLEEP & 1;
    jrk_settings_set_never_sleep(settings, never_sleep);
  }

  {
    bool serial_enable_crc = buf[JRK_SETTING_OPTIONS_BYTE1] >> JRK_OPTIONS_BYTE1_SERIAL_ENABLE_CRC & 1;
    jrk_settings_set_serial_enable_crc(settings, serial_enable_crc);
  }

  {
    bool serial_enable_14bit_device_number = buf[JRK_SETTING_OPTIONS_BYTE1] >> JRK_OPTIONS_BYTE1_SERIAL_ENABLE_14BIT_DEVICE_NUMBER & 1;
    jrk_settings_set_serial_enable_14bit_device_number(settings, serial_enable_14bit_device_number);
  }

  {
    bool serial_disable_compact_protocol = buf[JRK_SETTING_OPTIONS_BYTE1] >> JRK_OPTIONS_BYTE1_SERIAL_DISABLE_COMPACT_PROTOCOL & 1;
    jrk_settings_set_serial_disable_compact_protocol(settings, serial_disable_compact_protocol);
  }

  {
    uint16_t proportional_multiplier = read_uint16_t(buf + JRK_SETTING_PROPORTIONAL_MULTIPLIER);
    jrk_settings_set_proportional_multiplier(settings, proportional_multiplier);
  }

  {
    uint8_t proportional_exponent = buf[JRK_SETTING_PROPORTIONAL_EXPONENT];
    jrk_settings_set_proportional_exponent(settings, proportional_exponent);
  }

  {
    uint16_t integral_multiplier = read_uint16_t(buf + JRK_SETTING_INTEGRAL_MULTIPLIER);
    jrk_settings_set_integral_multiplier(settings, integral_multiplier);
  }

  {
    uint8_t integral_exponent = buf[JRK_SETTING_INTEGRAL_EXPONENT];
    jrk_settings_set_integral_exponent(settings, integral_exponent);
  }

  {
    uint16_t derivative_multiplier = read_uint16_t(buf + JRK_SETTING_DERIVATIVE_MULTIPLIER);
    jrk_settings_set_derivative_multiplier(settings, derivative_multiplier);
  }

  {
    uint8_t derivative_exponent = buf[JRK_SETTING_DERIVATIVE_EXPONENT];
    jrk_settings_set_derivative_exponent(settings, derivative_exponent);
  }

  {
    uint16_t pid_period = read_uint16_t(buf + JRK_SETTING_PID_PERIOD);
    jrk_settings_set_pid_period(settings, pid_period);
  }

  {
    uint8_t integral_divider_exponent = buf[JRK_SETTING_INTEGRAL_DIVIDER_EXPONENT];
    jrk_settings_set_integral_divider_exponent(settings, integral_divider_exponent);
  }

  {
    uint16_t integral_limit = read_uint16_t(buf + JRK_SETTING_INTEGRAL_LIMIT);
    jrk_settings_set_integral_limit(settings, integral_limit);
  }

  {
    bool reset_integral = buf[JRK_SETTING_OPTIONS_BYTE3] >> JRK_OPTIONS_BYTE3_RESET_INTEGRAL & 1;
    jrk_settings_set_reset_integral(settings, reset_integral);
  }

  {
    uint8_t pwm_frequency = buf[JRK_SETTING_PWM_FREQUENCY];
    jrk_settings_set_pwm_frequency(settings, pwm_frequency);
  }

  {
    uint8_t current_samples_exponent = buf[JRK_SETTING_CURRENT_SAMPLES_EXPONENT];
    jrk_settings_set_current_samples_exponent(settings, current_samples_exponent);
  }

  if (product != JRK_PRODUCT_UMC06A)
  {
    uint8_t hard_overcurrent_threshold = buf[JRK_SETTING_HARD_OVERCURRENT_THRESHOLD];
    jrk_settings_set_hard_overcurrent_threshold(settings, hard_overcurrent_threshold);
  }

  {
    int16_t current_offset_calibration = read_int16_t(buf + JRK_SETTING_CURRENT_OFFSET_CALIBRATION);
    jrk_settings_set_current_offset_calibration(settings, current_offset_calibration);
  }

  {
    int16_t current_scale_calibration = read_int16_t(buf + JRK_SETTING_CURRENT_SCALE_CALIBRATION);
    jrk_settings_set_current_scale_calibration(settings, current_scale_calibration);
  }

  {
    bool motor_invert = buf[JRK_SETTING_OPTIONS_BYTE2] >> JRK_OPTIONS_BYTE2_MOTOR_INVERT & 1;
    jrk_settings_set_motor_invert(settings, motor_invert);
  }

  {
    uint16_t max_duty_cycle_while_feedback_out_of_range = read_uint16_t(buf + JRK_SETTING_MAX_DUTY_CYCLE_WHILE_FEEDBACK_OUT_OF_RANGE);
    jrk_settings_set_max_duty_cycle_while_feedback_out_of_range(settings, max_duty_cycle_while_feedback_out_of_range);
  }

  {
    uint16_t max_acceleration_forward = read_uint16_t(buf + JRK_SETTING_MAX_ACCELERATION_FORWARD);
    jrk_settings_set_max_acceleration_forward(settings, max_acceleration_forward);
  }

  {
    uint16_t max_acceleration_reverse = read_uint16_t(buf + JRK_SETTING_MAX_ACCELERATION_REVERSE);
    jrk_settings_set_max_acceleration_reverse(settings, max_acceleration_reverse);
  }

  {
    uint16_t max_deceleration_forward = read_uint16_t(buf + JRK_SETTING_MAX_DECELERATION_FORWARD);
    jrk_settings_set_max_deceleration_forward(settings, max_deceleration_forward);
  }

  {
    uint16_t max_deceleration_reverse = read_uint16_t(buf + JRK_SETTING_MAX_DECELERATION_REVERSE);
    jrk_settings_set_max_deceleration_reverse(settings, max_deceleration_reverse);
  }

  {
    uint16_t max_duty_cycle_forward = read_uint16_t(buf + JRK_SETTING_MAX_DUTY_CYCLE_FORWARD);
    jrk_settings_set_max_duty_cycle_forward(settings, max_duty_cycle_forward);
  }

  {
    uint16_t max_duty_cycle_reverse = read_uint16_t(buf + JRK_SETTING_MAX_DUTY_CYCLE_REVERSE);
    jrk_settings_set_max_duty_cycle_reverse(settings, max_duty_cycle_reverse);
  }

  if (product != JRK_PRODUCT_UMC06A)
  {
    uint16_t encoded_hard_current_limit_forward = read_uint16_t(buf + JRK_SETTING_ENCODED_HARD_CURRENT_LIMIT_FORWARD);
    jrk_settings_set_encoded_hard_current_limit_forward(settings, encoded_hard_current_limit_forward);
  }

  if (product != JRK_PRODUCT_UMC06A)
  {
    uint16_t encoded_hard_current_limit_reverse = read_uint16_t(buf + JRK_SETTING_ENCODED_HARD_CURRENT_LIMIT_REVERSE);
    jrk_settings_set_encoded_hard_current_limit_reverse(settings, encoded_hard_current_limit_reverse);
  }

  {
    uint16_t soft_current_limit_forward = read_uint16_t(buf + JRK_SETTING_SOFT_CURRENT_LIMIT_FORWARD);
    jrk_settings_set_soft_current_limit_forward(settings, soft_current_limit_forward);
  }

  {
    uint16_t soft_current_limit_reverse = read_uint16_t(buf + JRK_SETTING_SOFT_CURRENT_LIMIT_REVERSE);
    jrk_settings_set_soft_current_limit_reverse(settings, soft_current_limit_reverse);
  }

  if (product == JRK_PRODUCT_UMC06A)
  {
    uint16_t soft_current_regulation_level_forward = read_uint16_t(buf + JRK_SETTING_SOFT_CURRENT_REGULATION_LEVEL_FORWARD);
    jrk_settings_set_soft_current_regulation_level_forward(settings, soft_current_regulation_level_forward);
  }

  if (product == JRK_PRODUCT_UMC06A)
  {
    uint16_t soft_current_regulation_level_reverse = read_uint16_t(buf + JRK_SETTING_SOFT_CURRENT_REGULATION_LEVEL_REVERSE);
    jrk_settings_set_soft_current_regulation_level_reverse(settings, soft_current_regulation_level_reverse);
  }

  {
    bool coast_when_off = buf[JRK_SETTING_OPTIONS_BYTE3] >> JRK_OPTIONS_BYTE3_COAST_WHEN_OFF & 1;
    jrk_settings_set_coast_when_off(settings, coast_when_off);
  }

  {
    uint16_t error_enable = read_uint16_t(buf + JRK_SETTING_ERROR_ENABLE);
    jrk_settings_set_error_enable(settings, error_enable);
  }

  {
    uint16_t error_latch = read_uint16_t(buf + JRK_SETTING_ERROR_LATCH);
    jrk_settings_set_error_latch(settings, error_latch);
  }

  {
    uint16_t error_hard = read_uint16_t(buf + JRK_SETTING_ERROR_HARD);
    jrk_settings_set_error_hard(settings, error_hard);
  }

  {
    int16_t vin_calibration = read_int16_t(buf + JRK_SETTING_VIN_CALIBRATION);
    jrk_settings_set_vin_calibration(settings, vin_calibration);
  }

  {
    bool disable_i2c_pullups = buf[JRK_SETTING_OPTIONS_BYTE1] >> JRK_OPTIONS_BYTE1_DISABLE_I2C_PULLUPS & 1;
    jrk_settings_set_disable_i2c_pullups(settings, disable_i2c_pullups);
  }

  {
    bool analog_sda_pullup = buf[JRK_SETTING_OPTIONS_BYTE1] >> JRK_OPTIONS_BYTE1_ANALOG_SDA_PULLUP & 1;
    jrk_settings_set_analog_sda_pullup(settings, analog_sda_pullup);
  }

  {
    bool always_analog_sda = buf[JRK_SETTING_OPTIONS_BYTE1] >> JRK_OPTIONS_BYTE1_ALWAYS_ANALOG_SDA & 1;
    jrk_settings_set_always_analog_sda(settings, always_analog_sda);
  }

  {
    bool always_analog_fba = buf[JRK_SETTING_OPTIONS_BYTE1] >> JRK_OPTIONS_BYTE1_ALWAYS_ANALOG_FBA & 1;
    jrk_settings_set_always_analog_fba(settings, always_analog_fba);
  }

  {
    uint8_t fbt_method = buf[JRK_SETTING_FBT_METHOD];
    jrk_settings_set_fbt_method(settings, fbt_method);
  }

  {
    uint8_t fbt_timing_clock = buf[JRK_SETTING_FBT_OPTIONS] >> JRK_FBT_OPTIONS_TIMING_CLOCK & JRK_FBT_OPTIONS_TIMING_CLOCK_MASK;
    jrk_settings_set_fbt_timing_clock(settings, fbt_timing_clock);
  }

  {
    bool fbt_timing_polarity = buf[JRK_SETTING_FBT_OPTIONS] >> JRK_FBT_OPTIONS_TIMING_POLARITY & 1;
    jrk_settings_set_fbt_timing_polarity(settings, fbt_timing_polarity);
  }

  {
    uint16_t fbt_timing_timeout = read_uint16_t(buf + JRK_SETTING_FBT_TIMING_TIMEOUT);
    jrk_settings_set_fbt_timing_timeout(settings, fbt_timing_timeout);
  }

  {
    uint8_t fbt_samples = buf[JRK_SETTING_FBT_SAMPLES];
    jrk_settings_set_fbt_samples(settings, fbt_samples);
  }

  {
    uint8_t fbt_divider_exponent = buf[JRK_SETTING_FBT_DIVIDER_EXPONENT];
    jrk_settings_set_fbt_divider_exponent(settings, fbt_divider_exponent);
  }

  // End of auto-generated buffer-to-settings code.

  {
    uint16_t brg = read_uint16_t(buf + JRK_SETTING_SERIAL_BAUD_RATE_GENERATOR);
    uint32_t baud_rate = jrk_baud_rate_from_brg(brg);
    jrk_settings_set_serial_baud_rate(settings, baud_rate);
  }

  {
    uint32_t duration = buf[JRK_SETTING_BRAKE_DURATION_FORWARD]
      * JRK_BRAKE_DURATION_UNITS;
    jrk_settings_set_brake_duration_forward(settings, duration);
  }

  {
    uint32_t duration = buf[JRK_SETTING_BRAKE_DURATION_REVERSE]
      * JRK_BRAKE_DURATION_UNITS;
    jrk_settings_set_brake_duration_reverse(settings, duration);
  }

  {
    uint32_t timeout = read_uint16_t(buf + JRK_SETTING_SERIAL_TIMEOUT)
      * JRK_SERIAL_TIMEOUT_UNITS;
    jrk_settings_set_serial_timeout(settings, timeout);
  }
}

jrk_error * jrk_get_eeprom_settings(jrk_handle * handle, jrk_settings ** settings)
{
  if (settings == NULL)
  {
    return jrk_error_create("Settings output pointer is null.");
  }

  *settings = NULL;

  if (handle == NULL)
  {
    return jrk_error_create("Handle is null.");
  }

  jrk_error * error = NULL;

  uint8_t product = jrk_device_get_product(jrk_handle_get_device(handle));

  // Allocate the new settings object.
  jrk_settings * new_settings = NULL;
  if (error == NULL)
  {
    error = jrk_settings_create(&new_settings);
  }

  // Specify what product these settings are for.
  if (error == NULL)
  {
    jrk_settings_set_product(new_settings, product);
  }

  // Set the context here because any error from jrk_get_setting_segment will
  // already have got context.
  if (error != NULL)
  {
    error = jrk_error_add(error,
      "There was an error reading settings from the device.");
  }

  // Read all the settings from the device into a buffer.
  uint8_t buf[JRK_SETTINGS_SIZE];
  if (error == NULL)
  {
    memset(buf, 0, sizeof(buf));
    size_t index = 1;
    while (index < sizeof(buf) && error == NULL)
    {
      size_t length = JRK_MAX_USB_RESPONSE_SIZE;
      if (index + length > sizeof(buf))
      {
        length = sizeof(buf) - index;
      }
      error = jrk_get_eeprom_setting_segment(handle, index, length, buf + index);
      index += length;
    }
  }

  // Pass the new settings to the caller.
  if (error == NULL)
  {
    write_buffer_to_settings(buf, new_settings);
    *settings = new_settings;
    new_settings = NULL;
  }

  jrk_settings_free(new_settings);

  return error;
}

jrk_error * jrk_get_ram_settings(jrk_handle * handle, jrk_settings ** settings)
{
  if (settings == NULL)
  {
    return jrk_error_create("RAM settings output pointer is null.");
  }

  *settings = NULL;

  if (handle == NULL)
  {
    return jrk_error_create("Handle is null.");
  }

  jrk_error * error = NULL;

  uint8_t product = jrk_device_get_product(jrk_handle_get_device(handle));

  // Allocate the new settings object.
  jrk_settings * new_settings = NULL;
  if (error == NULL)
  {
    error = jrk_settings_create(&new_settings);
  }

  // Specify what product these settings are for.
  if (error == NULL)
  {
    jrk_settings_set_product(new_settings, product);
  }

  // Set the context here because any error from jrk_get_setting_segment will
  // already have got context.
  if (error != NULL)
  {
    error = jrk_error_add(error,
      "There was an error reading RAM settings from the device.");
  }

  // Read all the settings from the device into a buffer.
  uint8_t buf[JRK_SETTINGS_SIZE];
  if (error == NULL)
  {
    memset(buf, 0, sizeof(buf));
    size_t index = 1;
    while (index < sizeof(buf) && error == NULL)
    {
      size_t length = JRK_MAX_USB_RESPONSE_SIZE;
      if (index + length > sizeof(buf))
      {
        length = sizeof(buf) - index;
      }
      error = jrk_get_ram_setting_segment(handle,
        index, length, buf + index);
      index += length;
    }
  }

  // Pass the new settings to the caller.
  if (error == NULL)
  {
    write_buffer_to_settings(buf, new_settings);
    *settings = new_settings;
    new_settings = NULL;
  }

  jrk_settings_free(new_settings);

  return error;
}
