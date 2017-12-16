#include "jrk_internal.h"

static void print_pin_config_to_yaml(jrk_string * str,
  const jrk_settings * settings, uint8_t pin,  const char * config_name)
{
  assert(str != NULL);
  assert(config_name != NULL);

  const char * pullup_str = "";
  if (jrk_settings_get_pin_pullup(settings, pin)) { pullup_str = " pullup"; }

  const char * analog_str = "";
  if (jrk_settings_get_pin_analog(settings, pin)) { analog_str = " analog"; }

  const char * func_str = "";
  jrk_code_to_name(jrk_pin_func_names,
    jrk_settings_get_pin_func(settings, pin), &func_str);

  jrk_sprintf(str, "%s: %s%s%s\n", config_name, func_str,
    pullup_str, analog_str);
}

jrk_error * jrk_settings_to_string(const jrk_settings * settings, char ** string)
{
  if (string == NULL)
  {
    return jrk_error_create("String output pointer is null.");
  }

  *string = NULL;

  if (settings == NULL)
  {
    return jrk_error_create("Settings pointer is null.");
  }

  jrk_error * error = NULL;

  jrk_string str;
  jrk_string_setup(&str);

  jrk_sprintf(&str, "# Pololu jrk settings file.\n");
  jrk_sprintf(&str, "# " DOCUMENTATION_URL "\n");

  uint8_t product = jrk_settings_get_product(settings);

  {
    const char * product_str = jrk_look_up_product_name_short(product);
    jrk_sprintf(&str, "product: %s\n", product_str);
  }

  // Beginning of auto-generated settings file printing code.

  {
    uint8_t input_mode = jrk_settings_get_input_mode(settings);
    const char * value_str = "";
    jrk_code_to_name(jrk_input_mode_names_short, input_mode, &value_str);
    jrk_sprintf(&str, "input_mode: %s\n", value_str);
  }

  {
    uint16_t input_absolute_minimum = jrk_settings_get_input_absolute_minimum(settings);
    jrk_sprintf(&str, "input_absolute_minimum: %u\n", input_absolute_minimum);
  }

  {
    uint16_t input_absolute_maximum = jrk_settings_get_input_absolute_maximum(settings);
    jrk_sprintf(&str, "input_absolute_maximum: %u\n", input_absolute_maximum);
  }

  {
    uint16_t input_minimum = jrk_settings_get_input_minimum(settings);
    jrk_sprintf(&str, "input_minimum: %u\n", input_minimum);
  }

  {
    uint16_t input_maximum = jrk_settings_get_input_maximum(settings);
    jrk_sprintf(&str, "input_maximum: %u\n", input_maximum);
  }

  {
    uint16_t input_neutral_minimum = jrk_settings_get_input_neutral_minimum(settings);
    jrk_sprintf(&str, "input_neutral_minimum: %u\n", input_neutral_minimum);
  }

  {
    uint16_t input_neutral_maximum = jrk_settings_get_input_neutral_maximum(settings);
    jrk_sprintf(&str, "input_neutral_maximum: %u\n", input_neutral_maximum);
  }

  {
    uint16_t output_minimum = jrk_settings_get_output_minimum(settings);
    jrk_sprintf(&str, "output_minimum: %u\n", output_minimum);
  }

  {
    uint16_t output_neutral = jrk_settings_get_output_neutral(settings);
    jrk_sprintf(&str, "output_neutral: %u\n", output_neutral);
  }

  {
    uint16_t output_maximum = jrk_settings_get_output_maximum(settings);
    jrk_sprintf(&str, "output_maximum: %u\n", output_maximum);
  }

  {
    bool input_invert = jrk_settings_get_input_invert(settings);
    jrk_sprintf(&str, "input_invert: %s\n",
      input_invert ? "true" : "false");
  }

  {
    uint8_t input_scaling_degree = jrk_settings_get_input_scaling_degree(settings);
    const char * value_str = "";
    jrk_code_to_name(jrk_input_scaling_degree_names_short, input_scaling_degree, &value_str);
    jrk_sprintf(&str, "input_scaling_degree: %s\n", value_str);
  }

  {
    bool input_detect_disconnect = jrk_settings_get_input_detect_disconnect(settings);
    jrk_sprintf(&str, "input_detect_disconnect: %s\n",
      input_detect_disconnect ? "true" : "false");
  }

  {
    uint8_t input_analog_samples_exponent = jrk_settings_get_input_analog_samples_exponent(settings);
    jrk_sprintf(&str, "input_analog_samples_exponent: %u\n", input_analog_samples_exponent);
  }

  {
    uint8_t feedback_mode = jrk_settings_get_feedback_mode(settings);
    const char * value_str = "";
    jrk_code_to_name(jrk_feedback_mode_names_short, feedback_mode, &value_str);
    jrk_sprintf(&str, "feedback_mode: %s\n", value_str);
  }

  {
    uint16_t feedback_absolute_minimum = jrk_settings_get_feedback_absolute_minimum(settings);
    jrk_sprintf(&str, "feedback_absolute_minimum: %u\n", feedback_absolute_minimum);
  }

  {
    uint16_t feedback_absolute_maximum = jrk_settings_get_feedback_absolute_maximum(settings);
    jrk_sprintf(&str, "feedback_absolute_maximum: %u\n", feedback_absolute_maximum);
  }

  {
    uint16_t feedback_minimum = jrk_settings_get_feedback_minimum(settings);
    jrk_sprintf(&str, "feedback_minimum: %u\n", feedback_minimum);
  }

  {
    uint16_t feedback_maximum = jrk_settings_get_feedback_maximum(settings);
    jrk_sprintf(&str, "feedback_maximum: %u\n", feedback_maximum);
  }

  {
    bool feedback_invert = jrk_settings_get_feedback_invert(settings);
    jrk_sprintf(&str, "feedback_invert: %s\n",
      feedback_invert ? "true" : "false");
  }

  {
    bool feedback_detect_disconnect = jrk_settings_get_feedback_detect_disconnect(settings);
    jrk_sprintf(&str, "feedback_detect_disconnect: %s\n",
      feedback_detect_disconnect ? "true" : "false");
  }

  {
    uint8_t feedback_dead_zone = jrk_settings_get_feedback_dead_zone(settings);
    jrk_sprintf(&str, "feedback_dead_zone: %u\n", feedback_dead_zone);
  }

  {
    uint8_t feedback_analog_samples_exponent = jrk_settings_get_feedback_analog_samples_exponent(settings);
    jrk_sprintf(&str, "feedback_analog_samples_exponent: %u\n", feedback_analog_samples_exponent);
  }

  {
    bool feedback_wraparound = jrk_settings_get_feedback_wraparound(settings);
    jrk_sprintf(&str, "feedback_wraparound: %s\n",
      feedback_wraparound ? "true" : "false");
  }

  {
    uint8_t serial_mode = jrk_settings_get_serial_mode(settings);
    const char * value_str = "";
    jrk_code_to_name(jrk_serial_mode_names_short, serial_mode, &value_str);
    jrk_sprintf(&str, "serial_mode: %s\n", value_str);
  }

  {
    uint32_t serial_baud_rate = jrk_settings_get_serial_baud_rate(settings);
    jrk_sprintf(&str, "serial_baud_rate: %u\n", serial_baud_rate);
  }

  {
    uint16_t serial_timeout = jrk_settings_get_serial_timeout(settings);
    jrk_sprintf(&str, "serial_timeout: %u\n", serial_timeout);
  }

  {
    uint16_t serial_device_number = jrk_settings_get_serial_device_number(settings);
    jrk_sprintf(&str, "serial_device_number: %u\n", serial_device_number);
  }

  {
    bool never_sleep = jrk_settings_get_never_sleep(settings);
    jrk_sprintf(&str, "never_sleep: %s\n",
      never_sleep ? "true" : "false");
  }

  {
    bool serial_enable_crc = jrk_settings_get_serial_enable_crc(settings);
    jrk_sprintf(&str, "serial_enable_crc: %s\n",
      serial_enable_crc ? "true" : "false");
  }

  {
    bool serial_enable_14bit_device_number = jrk_settings_get_serial_enable_14bit_device_number(settings);
    jrk_sprintf(&str, "serial_enable_14bit_device_number: %s\n",
      serial_enable_14bit_device_number ? "true" : "false");
  }

  {
    bool serial_disable_compact_protocol = jrk_settings_get_serial_disable_compact_protocol(settings);
    jrk_sprintf(&str, "serial_disable_compact_protocol: %s\n",
      serial_disable_compact_protocol ? "true" : "false");
  }

  {
    uint16_t proportional_multiplier = jrk_settings_get_proportional_multiplier(settings);
    jrk_sprintf(&str, "proportional_multiplier: %u\n", proportional_multiplier);
  }

  {
    uint8_t proportional_exponent = jrk_settings_get_proportional_exponent(settings);
    jrk_sprintf(&str, "proportional_exponent: %u\n", proportional_exponent);
  }

  {
    uint16_t integral_multiplier = jrk_settings_get_integral_multiplier(settings);
    jrk_sprintf(&str, "integral_multiplier: %u\n", integral_multiplier);
  }

  {
    uint8_t integral_exponent = jrk_settings_get_integral_exponent(settings);
    jrk_sprintf(&str, "integral_exponent: %u\n", integral_exponent);
  }

  {
    uint16_t derivative_multiplier = jrk_settings_get_derivative_multiplier(settings);
    jrk_sprintf(&str, "derivative_multiplier: %u\n", derivative_multiplier);
  }

  {
    uint8_t derivative_exponent = jrk_settings_get_derivative_exponent(settings);
    jrk_sprintf(&str, "derivative_exponent: %u\n", derivative_exponent);
  }

  {
    uint16_t pid_period = jrk_settings_get_pid_period(settings);
    jrk_sprintf(&str, "pid_period: %u\n", pid_period);
  }

  {
    uint16_t pid_integral_limit = jrk_settings_get_pid_integral_limit(settings);
    jrk_sprintf(&str, "pid_integral_limit: %u\n", pid_integral_limit);
  }

  {
    bool pid_reset_integral = jrk_settings_get_pid_reset_integral(settings);
    jrk_sprintf(&str, "pid_reset_integral: %s\n",
      pid_reset_integral ? "true" : "false");
  }

  {
    uint8_t motor_pwm_frequency = jrk_settings_get_motor_pwm_frequency(settings);
    const char * value_str = "";
    jrk_code_to_name(jrk_motor_pwm_frequency_names_short, motor_pwm_frequency, &value_str);
    jrk_sprintf(&str, "motor_pwm_frequency: %s\n", value_str);
  }

  {
    bool motor_invert = jrk_settings_get_motor_invert(settings);
    jrk_sprintf(&str, "motor_invert: %s\n",
      motor_invert ? "true" : "false");
  }

  {
    uint16_t motor_max_duty_cycle_while_feedback_out_of_range = jrk_settings_get_motor_max_duty_cycle_while_feedback_out_of_range(settings);
    jrk_sprintf(&str, "motor_max_duty_cycle_while_feedback_out_of_range: %u\n", motor_max_duty_cycle_while_feedback_out_of_range);
  }

  {
    uint16_t motor_max_acceleration_forward = jrk_settings_get_motor_max_acceleration_forward(settings);
    jrk_sprintf(&str, "motor_max_acceleration_forward: %u\n", motor_max_acceleration_forward);
  }

  {
    uint16_t motor_max_acceleration_reverse = jrk_settings_get_motor_max_acceleration_reverse(settings);
    jrk_sprintf(&str, "motor_max_acceleration_reverse: %u\n", motor_max_acceleration_reverse);
  }

  {
    uint16_t motor_max_deceleration_forward = jrk_settings_get_motor_max_deceleration_forward(settings);
    jrk_sprintf(&str, "motor_max_deceleration_forward: %u\n", motor_max_deceleration_forward);
  }

  {
    uint16_t motor_max_deceleration_reverse = jrk_settings_get_motor_max_deceleration_reverse(settings);
    jrk_sprintf(&str, "motor_max_deceleration_reverse: %u\n", motor_max_deceleration_reverse);
  }

  {
    uint16_t motor_max_duty_cycle_forward = jrk_settings_get_motor_max_duty_cycle_forward(settings);
    jrk_sprintf(&str, "motor_max_duty_cycle_forward: %u\n", motor_max_duty_cycle_forward);
  }

  {
    uint16_t motor_max_duty_cycle_reverse = jrk_settings_get_motor_max_duty_cycle_reverse(settings);
    jrk_sprintf(&str, "motor_max_duty_cycle_reverse: %u\n", motor_max_duty_cycle_reverse);
  }

  {
    uint8_t motor_max_current_forward = jrk_settings_get_motor_max_current_forward(settings);
    jrk_sprintf(&str, "motor_max_current_forward: %u\n", motor_max_current_forward);
  }

  {
    uint8_t motor_max_current_reverse = jrk_settings_get_motor_max_current_reverse(settings);
    jrk_sprintf(&str, "motor_max_current_reverse: %u\n", motor_max_current_reverse);
  }

  {
    int8_t motor_current_calibration_forward = jrk_settings_get_motor_current_calibration_forward(settings);
    jrk_sprintf(&str, "motor_current_calibration_forward: %d\n", motor_current_calibration_forward);
  }

  {
    int8_t motor_current_calibration_reverse = jrk_settings_get_motor_current_calibration_reverse(settings);
    jrk_sprintf(&str, "motor_current_calibration_reverse: %d\n", motor_current_calibration_reverse);
  }

  {
    uint8_t motor_brake_duration_forward = jrk_settings_get_motor_brake_duration_forward(settings);
    jrk_sprintf(&str, "motor_brake_duration_forward: %u\n", motor_brake_duration_forward);
  }

  {
    uint8_t motor_brake_duration_reverse = jrk_settings_get_motor_brake_duration_reverse(settings);
    jrk_sprintf(&str, "motor_brake_duration_reverse: %u\n", motor_brake_duration_reverse);
  }

  {
    bool motor_coast_when_off = jrk_settings_get_motor_coast_when_off(settings);
    jrk_sprintf(&str, "motor_coast_when_off: %s\n",
      motor_coast_when_off ? "true" : "false");
  }

  {
    uint16_t error_enable = jrk_settings_get_error_enable(settings);
    jrk_sprintf(&str, "error_enable: %u\n", error_enable);
  }

  {
    uint16_t error_latch = jrk_settings_get_error_latch(settings);
    jrk_sprintf(&str, "error_latch: %u\n", error_latch);
  }

  {
    int16_t vin_calibration = jrk_settings_get_vin_calibration(settings);
    jrk_sprintf(&str, "vin_calibration: %d\n", vin_calibration);
  }

  // End of auto-generated settings file printing code.

  {
    print_pin_config_to_yaml(&str, settings, JRK_PIN_NUM_SCL, "pin_config_scl");
    print_pin_config_to_yaml(&str, settings, JRK_PIN_NUM_SDA, "pin_config_sda");
    print_pin_config_to_yaml(&str, settings, JRK_PIN_NUM_TX, "pin_config_tx");
    print_pin_config_to_yaml(&str, settings, JRK_PIN_NUM_RX, "pin_config_rx");
    print_pin_config_to_yaml(&str, settings, JRK_PIN_NUM_RC, "pin_config_rc");
    print_pin_config_to_yaml(&str, settings, JRK_PIN_NUM_AUX, "pin_config_aux");
    print_pin_config_to_yaml(&str, settings, JRK_PIN_NUM_FBA, "pin_config_fba");
    print_pin_config_to_yaml(&str, settings, JRK_PIN_NUM_FBT, "pin_config_fbt");
  }

  if (error == NULL && str.data == NULL)
  {
    error = &jrk_error_no_memory;
  }

  if (error == NULL)
  {
    *string = str.data;
    str.data = NULL;
  }

  jrk_string_free(str.data);

  return error;
}
