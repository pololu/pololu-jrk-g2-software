#include "jrk_internal.h"

// Note: When we have multiple jrk models with different units for their current
// settings, we will need some new functions for converting the settings from
// one product to another while keepign the currents roughly the same.

struct jrk_settings
{
  uint32_t product;

  // Beginning of auto-generated settings struct members.

  uint8_t input_mode;
  uint16_t input_absolute_minimum;
  uint16_t input_absolute_maximum;
  uint16_t input_minimum;
  uint16_t input_maximum;
  uint16_t input_neutral_minimum;
  uint16_t input_neutral_maximum;
  uint16_t output_minimum;
  uint16_t output_neutral;
  uint16_t output_maximum;
  bool input_invert;
  uint8_t input_scaling_degree;
  bool input_detect_disconnect;
  uint8_t input_analog_samples_exponent;
  uint8_t feedback_mode;
  uint16_t feedback_absolute_minimum;
  uint16_t feedback_absolute_maximum;
  uint16_t feedback_minimum;
  uint16_t feedback_maximum;
  bool feedback_invert;
  bool feedback_detect_disconnect;
  uint8_t feedback_dead_zone;
  uint8_t feedback_analog_samples_exponent;
  bool feedback_wraparound;
  uint8_t serial_mode;
  uint32_t serial_baud_rate;
  uint16_t serial_timeout;
  uint16_t serial_device_number;
  bool never_sleep;
  bool serial_enable_crc;
  bool serial_enable_14bit_device_number;
  bool serial_disable_compact_protocol;
  uint16_t proportional_multiplier;
  uint8_t proportional_exponent;
  uint16_t integral_multiplier;
  uint8_t integral_exponent;
  uint16_t derivative_multiplier;
  uint8_t derivative_exponent;
  uint16_t pid_period;
  uint16_t pid_integral_limit;
  bool pid_reset_integral;
  uint8_t motor_pwm_frequency;
  bool motor_invert;
  uint16_t motor_max_duty_cycle_while_feedback_out_of_range;
  uint16_t motor_max_acceleration_forward;
  uint16_t motor_max_acceleration_reverse;
  uint16_t motor_max_deceleration_forward;
  uint16_t motor_max_deceleration_reverse;
  uint16_t motor_max_duty_cycle_forward;
  uint16_t motor_max_duty_cycle_reverse;
  uint8_t motor_max_current_forward;
  uint8_t motor_max_current_reverse;
  int8_t motor_current_calibration_forward;
  int8_t motor_current_calibration_reverse;
  uint8_t motor_brake_duration_forward;
  uint8_t motor_brake_duration_reverse;
  bool motor_coast_when_off;
  uint16_t error_enable;
  uint16_t error_latch;
  int16_t vin_calibration;

  // End of auto-generated settings struct members.

  struct {
    uint8_t func;
    bool pullup;
    bool analog;
  } pin_settings[JRK_CONTROL_PIN_COUNT];
};

void jrk_settings_fill_with_defaults(jrk_settings * settings)
{
  if (settings == NULL) { return; }

  uint32_t product = settings->product;

  // The product should be set beforehand, and if it is not then we should do
  // nothing.
  if (product != JRK_PRODUCT_2017)
  {
    return;
  }

  // Reset all fields to zero.
  memset(settings, 0, sizeof(jrk_settings));

  jrk_settings_set_product(settings, product);

  // Beginning of auto-generated settings defaults.

  jrk_settings_set_input_absolute_maximum(settings, 4095);
  jrk_settings_set_input_maximum(settings, 4095);
  jrk_settings_set_input_neutral_minimum(settings, 2048);
  jrk_settings_set_input_neutral_maximum(settings, 2048);
  jrk_settings_set_output_neutral(settings, 2048);
  jrk_settings_set_output_maximum(settings, 4095);
  jrk_settings_set_input_analog_samples_exponent(settings, 5);
  jrk_settings_set_feedback_mode(settings, JRK_FEEDBACK_MODE_ANALOG);
  jrk_settings_set_feedback_absolute_maximum(settings, 4095);
  jrk_settings_set_feedback_maximum(settings, 4095);
  jrk_settings_set_feedback_analog_samples_exponent(settings, 5);
  jrk_settings_set_serial_device_number(settings, 11);
  jrk_settings_set_pid_period(settings, 10);
  jrk_settings_set_pid_integral_limit(settings, 1000);
  jrk_settings_set_motor_max_duty_cycle_while_feedback_out_of_range(settings, 600);
  jrk_settings_set_motor_max_acceleration_forward(settings, 600);
  jrk_settings_set_motor_max_acceleration_reverse(settings, 600);
  jrk_settings_set_motor_max_deceleration_forward(settings, 600);
  jrk_settings_set_motor_max_deceleration_reverse(settings, 600);
  jrk_settings_set_motor_max_duty_cycle_forward(settings, 600);
  jrk_settings_set_motor_max_duty_cycle_reverse(settings, 600);
  jrk_settings_set_motor_max_current_forward(settings, 42);
  jrk_settings_set_motor_max_current_reverse(settings, 42);

  // End of auto-generated settings defaults.
}

jrk_error * jrk_settings_create(jrk_settings ** settings)
{
  if (settings == NULL)
  {
    return jrk_error_create("Settings output pointer is null.");
  }

  *settings = NULL;

  jrk_error * error = NULL;

  jrk_settings * new_settings = NULL;
  if (error == NULL)
  {
    new_settings = (jrk_settings *)calloc(1, sizeof(jrk_settings));
    if (new_settings == NULL) { error = &jrk_error_no_memory; }
  }

  if (error == NULL)
  {
    *settings = new_settings;
    new_settings = NULL;
  }

  jrk_settings_free(new_settings);

  return error;
}

void jrk_settings_free(jrk_settings * settings)
{
  free(settings);
}

jrk_error * jrk_settings_copy(const jrk_settings * source, jrk_settings ** dest)
{
  if (dest == NULL)
  {
    return jrk_error_create("Settings output pointer is null.");
  }

  *dest = NULL;

  if (source == NULL)
  {
    return NULL;
  }

  jrk_error * error = NULL;

  jrk_settings * new_settings = NULL;
  if (error == NULL)
  {
    new_settings = (jrk_settings *)calloc(1, sizeof(jrk_settings));
    if (new_settings == NULL) { error = &jrk_error_no_memory; }
  }

  if (error == NULL)
  {
    memcpy(new_settings, source, sizeof(jrk_settings));
  }

  if (error == NULL)
  {
    *dest = new_settings;
    new_settings = NULL;
  }

  jrk_settings_free(new_settings);

  return error;
}

void jrk_settings_set_product(jrk_settings * settings, uint32_t product)
{
  if (!settings) { return; }
  settings->product = product;
}

uint32_t jrk_settings_get_product(const jrk_settings * settings)
{
  if (!settings) { return 0; }
  return settings->product;
}

// Beginning of auto-generated settings accessors.

void jrk_settings_set_input_mode(jrk_settings * settings, uint8_t input_mode)
{
  if (settings == NULL) { return; }
  settings->input_mode = input_mode;
}

uint8_t jrk_settings_get_input_mode(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->input_mode;
}

void jrk_settings_set_input_absolute_minimum(jrk_settings * settings, uint16_t input_absolute_minimum)
{
  if (settings == NULL) { return; }
  settings->input_absolute_minimum = input_absolute_minimum;
}

uint16_t jrk_settings_get_input_absolute_minimum(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->input_absolute_minimum;
}

void jrk_settings_set_input_absolute_maximum(jrk_settings * settings, uint16_t input_absolute_maximum)
{
  if (settings == NULL) { return; }
  settings->input_absolute_maximum = input_absolute_maximum;
}

uint16_t jrk_settings_get_input_absolute_maximum(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->input_absolute_maximum;
}

void jrk_settings_set_input_minimum(jrk_settings * settings, uint16_t input_minimum)
{
  if (settings == NULL) { return; }
  settings->input_minimum = input_minimum;
}

uint16_t jrk_settings_get_input_minimum(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->input_minimum;
}

void jrk_settings_set_input_maximum(jrk_settings * settings, uint16_t input_maximum)
{
  if (settings == NULL) { return; }
  settings->input_maximum = input_maximum;
}

uint16_t jrk_settings_get_input_maximum(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->input_maximum;
}

void jrk_settings_set_input_neutral_minimum(jrk_settings * settings, uint16_t input_neutral_minimum)
{
  if (settings == NULL) { return; }
  settings->input_neutral_minimum = input_neutral_minimum;
}

uint16_t jrk_settings_get_input_neutral_minimum(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->input_neutral_minimum;
}

void jrk_settings_set_input_neutral_maximum(jrk_settings * settings, uint16_t input_neutral_maximum)
{
  if (settings == NULL) { return; }
  settings->input_neutral_maximum = input_neutral_maximum;
}

uint16_t jrk_settings_get_input_neutral_maximum(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->input_neutral_maximum;
}

void jrk_settings_set_output_minimum(jrk_settings * settings, uint16_t output_minimum)
{
  if (settings == NULL) { return; }
  settings->output_minimum = output_minimum;
}

uint16_t jrk_settings_get_output_minimum(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->output_minimum;
}

void jrk_settings_set_output_neutral(jrk_settings * settings, uint16_t output_neutral)
{
  if (settings == NULL) { return; }
  settings->output_neutral = output_neutral;
}

uint16_t jrk_settings_get_output_neutral(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->output_neutral;
}

void jrk_settings_set_output_maximum(jrk_settings * settings, uint16_t output_maximum)
{
  if (settings == NULL) { return; }
  settings->output_maximum = output_maximum;
}

uint16_t jrk_settings_get_output_maximum(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->output_maximum;
}

void jrk_settings_set_input_invert(jrk_settings * settings, bool input_invert)
{
  if (settings == NULL) { return; }
  settings->input_invert = input_invert;
}

bool jrk_settings_get_input_invert(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->input_invert;
}

void jrk_settings_set_input_scaling_degree(jrk_settings * settings, uint8_t input_scaling_degree)
{
  if (settings == NULL) { return; }
  settings->input_scaling_degree = input_scaling_degree;
}

uint8_t jrk_settings_get_input_scaling_degree(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->input_scaling_degree;
}

void jrk_settings_set_input_detect_disconnect(jrk_settings * settings, bool input_detect_disconnect)
{
  if (settings == NULL) { return; }
  settings->input_detect_disconnect = input_detect_disconnect;
}

bool jrk_settings_get_input_detect_disconnect(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->input_detect_disconnect;
}

void jrk_settings_set_input_analog_samples_exponent(jrk_settings * settings, uint8_t input_analog_samples_exponent)
{
  if (settings == NULL) { return; }
  settings->input_analog_samples_exponent = input_analog_samples_exponent;
}

uint8_t jrk_settings_get_input_analog_samples_exponent(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->input_analog_samples_exponent;
}

void jrk_settings_set_feedback_mode(jrk_settings * settings, uint8_t feedback_mode)
{
  if (settings == NULL) { return; }
  settings->feedback_mode = feedback_mode;
}

uint8_t jrk_settings_get_feedback_mode(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->feedback_mode;
}

void jrk_settings_set_feedback_absolute_minimum(jrk_settings * settings, uint16_t feedback_absolute_minimum)
{
  if (settings == NULL) { return; }
  settings->feedback_absolute_minimum = feedback_absolute_minimum;
}

uint16_t jrk_settings_get_feedback_absolute_minimum(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->feedback_absolute_minimum;
}

void jrk_settings_set_feedback_absolute_maximum(jrk_settings * settings, uint16_t feedback_absolute_maximum)
{
  if (settings == NULL) { return; }
  settings->feedback_absolute_maximum = feedback_absolute_maximum;
}

uint16_t jrk_settings_get_feedback_absolute_maximum(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->feedback_absolute_maximum;
}

void jrk_settings_set_feedback_minimum(jrk_settings * settings, uint16_t feedback_minimum)
{
  if (settings == NULL) { return; }
  settings->feedback_minimum = feedback_minimum;
}

uint16_t jrk_settings_get_feedback_minimum(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->feedback_minimum;
}

void jrk_settings_set_feedback_maximum(jrk_settings * settings, uint16_t feedback_maximum)
{
  if (settings == NULL) { return; }
  settings->feedback_maximum = feedback_maximum;
}

uint16_t jrk_settings_get_feedback_maximum(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->feedback_maximum;
}

void jrk_settings_set_feedback_invert(jrk_settings * settings, bool feedback_invert)
{
  if (settings == NULL) { return; }
  settings->feedback_invert = feedback_invert;
}

bool jrk_settings_get_feedback_invert(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->feedback_invert;
}

void jrk_settings_set_feedback_detect_disconnect(jrk_settings * settings, bool feedback_detect_disconnect)
{
  if (settings == NULL) { return; }
  settings->feedback_detect_disconnect = feedback_detect_disconnect;
}

bool jrk_settings_get_feedback_detect_disconnect(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->feedback_detect_disconnect;
}

void jrk_settings_set_feedback_dead_zone(jrk_settings * settings, uint8_t feedback_dead_zone)
{
  if (settings == NULL) { return; }
  settings->feedback_dead_zone = feedback_dead_zone;
}

uint8_t jrk_settings_get_feedback_dead_zone(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->feedback_dead_zone;
}

void jrk_settings_set_feedback_analog_samples_exponent(jrk_settings * settings, uint8_t feedback_analog_samples_exponent)
{
  if (settings == NULL) { return; }
  settings->feedback_analog_samples_exponent = feedback_analog_samples_exponent;
}

uint8_t jrk_settings_get_feedback_analog_samples_exponent(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->feedback_analog_samples_exponent;
}

void jrk_settings_set_feedback_wraparound(jrk_settings * settings, bool feedback_wraparound)
{
  if (settings == NULL) { return; }
  settings->feedback_wraparound = feedback_wraparound;
}

bool jrk_settings_get_feedback_wraparound(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->feedback_wraparound;
}

void jrk_settings_set_serial_mode(jrk_settings * settings, uint8_t serial_mode)
{
  if (settings == NULL) { return; }
  settings->serial_mode = serial_mode;
}

uint8_t jrk_settings_get_serial_mode(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->serial_mode;
}

void jrk_settings_set_serial_baud_rate(jrk_settings * settings, uint32_t serial_baud_rate)
{
  if (settings == NULL) { return; }
  settings->serial_baud_rate = serial_baud_rate;
}

uint32_t jrk_settings_get_serial_baud_rate(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->serial_baud_rate;
}

void jrk_settings_set_serial_timeout(jrk_settings * settings, uint16_t serial_timeout)
{
  if (settings == NULL) { return; }
  settings->serial_timeout = serial_timeout;
}

uint16_t jrk_settings_get_serial_timeout(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->serial_timeout;
}

void jrk_settings_set_serial_device_number(jrk_settings * settings, uint16_t serial_device_number)
{
  if (settings == NULL) { return; }
  settings->serial_device_number = serial_device_number;
}

uint16_t jrk_settings_get_serial_device_number(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->serial_device_number;
}

void jrk_settings_set_never_sleep(jrk_settings * settings, bool never_sleep)
{
  if (settings == NULL) { return; }
  settings->never_sleep = never_sleep;
}

bool jrk_settings_get_never_sleep(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->never_sleep;
}

void jrk_settings_set_serial_enable_crc(jrk_settings * settings, bool serial_enable_crc)
{
  if (settings == NULL) { return; }
  settings->serial_enable_crc = serial_enable_crc;
}

bool jrk_settings_get_serial_enable_crc(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->serial_enable_crc;
}

void jrk_settings_set_serial_enable_14bit_device_number(jrk_settings * settings, bool serial_enable_14bit_device_number)
{
  if (settings == NULL) { return; }
  settings->serial_enable_14bit_device_number = serial_enable_14bit_device_number;
}

bool jrk_settings_get_serial_enable_14bit_device_number(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->serial_enable_14bit_device_number;
}

void jrk_settings_set_serial_disable_compact_protocol(jrk_settings * settings, bool serial_disable_compact_protocol)
{
  if (settings == NULL) { return; }
  settings->serial_disable_compact_protocol = serial_disable_compact_protocol;
}

bool jrk_settings_get_serial_disable_compact_protocol(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->serial_disable_compact_protocol;
}

void jrk_settings_set_proportional_multiplier(jrk_settings * settings, uint16_t proportional_multiplier)
{
  if (settings == NULL) { return; }
  settings->proportional_multiplier = proportional_multiplier;
}

uint16_t jrk_settings_get_proportional_multiplier(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->proportional_multiplier;
}

void jrk_settings_set_proportional_exponent(jrk_settings * settings, uint8_t proportional_exponent)
{
  if (settings == NULL) { return; }
  settings->proportional_exponent = proportional_exponent;
}

uint8_t jrk_settings_get_proportional_exponent(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->proportional_exponent;
}

void jrk_settings_set_integral_multiplier(jrk_settings * settings, uint16_t integral_multiplier)
{
  if (settings == NULL) { return; }
  settings->integral_multiplier = integral_multiplier;
}

uint16_t jrk_settings_get_integral_multiplier(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->integral_multiplier;
}

void jrk_settings_set_integral_exponent(jrk_settings * settings, uint8_t integral_exponent)
{
  if (settings == NULL) { return; }
  settings->integral_exponent = integral_exponent;
}

uint8_t jrk_settings_get_integral_exponent(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->integral_exponent;
}

void jrk_settings_set_derivative_multiplier(jrk_settings * settings, uint16_t derivative_multiplier)
{
  if (settings == NULL) { return; }
  settings->derivative_multiplier = derivative_multiplier;
}

uint16_t jrk_settings_get_derivative_multiplier(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->derivative_multiplier;
}

void jrk_settings_set_derivative_exponent(jrk_settings * settings, uint8_t derivative_exponent)
{
  if (settings == NULL) { return; }
  settings->derivative_exponent = derivative_exponent;
}

uint8_t jrk_settings_get_derivative_exponent(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->derivative_exponent;
}

void jrk_settings_set_pid_period(jrk_settings * settings, uint16_t pid_period)
{
  if (settings == NULL) { return; }
  settings->pid_period = pid_period;
}

uint16_t jrk_settings_get_pid_period(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->pid_period;
}

void jrk_settings_set_pid_integral_limit(jrk_settings * settings, uint16_t pid_integral_limit)
{
  if (settings == NULL) { return; }
  settings->pid_integral_limit = pid_integral_limit;
}

uint16_t jrk_settings_get_pid_integral_limit(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->pid_integral_limit;
}

void jrk_settings_set_pid_reset_integral(jrk_settings * settings, bool pid_reset_integral)
{
  if (settings == NULL) { return; }
  settings->pid_reset_integral = pid_reset_integral;
}

bool jrk_settings_get_pid_reset_integral(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->pid_reset_integral;
}

void jrk_settings_set_motor_pwm_frequency(jrk_settings * settings, uint8_t motor_pwm_frequency)
{
  if (settings == NULL) { return; }
  settings->motor_pwm_frequency = motor_pwm_frequency;
}

uint8_t jrk_settings_get_motor_pwm_frequency(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->motor_pwm_frequency;
}

void jrk_settings_set_motor_invert(jrk_settings * settings, bool motor_invert)
{
  if (settings == NULL) { return; }
  settings->motor_invert = motor_invert;
}

bool jrk_settings_get_motor_invert(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->motor_invert;
}

void jrk_settings_set_motor_max_duty_cycle_while_feedback_out_of_range(jrk_settings * settings, uint16_t motor_max_duty_cycle_while_feedback_out_of_range)
{
  if (settings == NULL) { return; }
  settings->motor_max_duty_cycle_while_feedback_out_of_range = motor_max_duty_cycle_while_feedback_out_of_range;
}

uint16_t jrk_settings_get_motor_max_duty_cycle_while_feedback_out_of_range(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->motor_max_duty_cycle_while_feedback_out_of_range;
}

void jrk_settings_set_motor_max_acceleration_forward(jrk_settings * settings, uint16_t motor_max_acceleration_forward)
{
  if (settings == NULL) { return; }
  settings->motor_max_acceleration_forward = motor_max_acceleration_forward;
}

uint16_t jrk_settings_get_motor_max_acceleration_forward(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->motor_max_acceleration_forward;
}

void jrk_settings_set_motor_max_acceleration_reverse(jrk_settings * settings, uint16_t motor_max_acceleration_reverse)
{
  if (settings == NULL) { return; }
  settings->motor_max_acceleration_reverse = motor_max_acceleration_reverse;
}

uint16_t jrk_settings_get_motor_max_acceleration_reverse(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->motor_max_acceleration_reverse;
}

void jrk_settings_set_motor_max_deceleration_forward(jrk_settings * settings, uint16_t motor_max_deceleration_forward)
{
  if (settings == NULL) { return; }
  settings->motor_max_deceleration_forward = motor_max_deceleration_forward;
}

uint16_t jrk_settings_get_motor_max_deceleration_forward(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->motor_max_deceleration_forward;
}

void jrk_settings_set_motor_max_deceleration_reverse(jrk_settings * settings, uint16_t motor_max_deceleration_reverse)
{
  if (settings == NULL) { return; }
  settings->motor_max_deceleration_reverse = motor_max_deceleration_reverse;
}

uint16_t jrk_settings_get_motor_max_deceleration_reverse(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->motor_max_deceleration_reverse;
}

void jrk_settings_set_motor_max_duty_cycle_forward(jrk_settings * settings, uint16_t motor_max_duty_cycle_forward)
{
  if (settings == NULL) { return; }
  settings->motor_max_duty_cycle_forward = motor_max_duty_cycle_forward;
}

uint16_t jrk_settings_get_motor_max_duty_cycle_forward(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->motor_max_duty_cycle_forward;
}

void jrk_settings_set_motor_max_duty_cycle_reverse(jrk_settings * settings, uint16_t motor_max_duty_cycle_reverse)
{
  if (settings == NULL) { return; }
  settings->motor_max_duty_cycle_reverse = motor_max_duty_cycle_reverse;
}

uint16_t jrk_settings_get_motor_max_duty_cycle_reverse(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->motor_max_duty_cycle_reverse;
}

void jrk_settings_set_motor_max_current_forward(jrk_settings * settings, uint8_t motor_max_current_forward)
{
  if (settings == NULL) { return; }
  settings->motor_max_current_forward = motor_max_current_forward;
}

uint8_t jrk_settings_get_motor_max_current_forward(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->motor_max_current_forward;
}

void jrk_settings_set_motor_max_current_reverse(jrk_settings * settings, uint8_t motor_max_current_reverse)
{
  if (settings == NULL) { return; }
  settings->motor_max_current_reverse = motor_max_current_reverse;
}

uint8_t jrk_settings_get_motor_max_current_reverse(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->motor_max_current_reverse;
}

void jrk_settings_set_motor_current_calibration_forward(jrk_settings * settings, int8_t motor_current_calibration_forward)
{
  if (settings == NULL) { return; }
  settings->motor_current_calibration_forward = motor_current_calibration_forward;
}

int8_t jrk_settings_get_motor_current_calibration_forward(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->motor_current_calibration_forward;
}

void jrk_settings_set_motor_current_calibration_reverse(jrk_settings * settings, int8_t motor_current_calibration_reverse)
{
  if (settings == NULL) { return; }
  settings->motor_current_calibration_reverse = motor_current_calibration_reverse;
}

int8_t jrk_settings_get_motor_current_calibration_reverse(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->motor_current_calibration_reverse;
}

void jrk_settings_set_motor_brake_duration_forward(jrk_settings * settings, uint8_t motor_brake_duration_forward)
{
  if (settings == NULL) { return; }
  settings->motor_brake_duration_forward = motor_brake_duration_forward;
}

uint8_t jrk_settings_get_motor_brake_duration_forward(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->motor_brake_duration_forward;
}

void jrk_settings_set_motor_brake_duration_reverse(jrk_settings * settings, uint8_t motor_brake_duration_reverse)
{
  if (settings == NULL) { return; }
  settings->motor_brake_duration_reverse = motor_brake_duration_reverse;
}

uint8_t jrk_settings_get_motor_brake_duration_reverse(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->motor_brake_duration_reverse;
}

void jrk_settings_set_motor_coast_when_off(jrk_settings * settings, bool motor_coast_when_off)
{
  if (settings == NULL) { return; }
  settings->motor_coast_when_off = motor_coast_when_off;
}

bool jrk_settings_get_motor_coast_when_off(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->motor_coast_when_off;
}

void jrk_settings_set_error_enable(jrk_settings * settings, uint16_t error_enable)
{
  if (settings == NULL) { return; }
  settings->error_enable = error_enable;
}

uint16_t jrk_settings_get_error_enable(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->error_enable;
}

void jrk_settings_set_error_latch(jrk_settings * settings, uint16_t error_latch)
{
  if (settings == NULL) { return; }
  settings->error_latch = error_latch;
}

uint16_t jrk_settings_get_error_latch(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->error_latch;
}

void jrk_settings_set_vin_calibration(jrk_settings * settings, int16_t vin_calibration)
{
  if (settings == NULL) { return; }
  settings->vin_calibration = vin_calibration;
}

int16_t jrk_settings_get_vin_calibration(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->vin_calibration;
}

// End of auto-generated settings accessors.

uint32_t jrk_settings_achievable_serial_baud_rate(
  const jrk_settings * settings, uint32_t baud)
{
  if (settings == NULL) { return 0; }
  uint16_t brg = jrk_baud_rate_to_brg(baud);
  return jrk_baud_rate_from_brg(brg);
}

void jrk_settings_set_pin_func(jrk_settings * settings, uint8_t pin, uint8_t func)
{
  if (!settings || pin >= JRK_CONTROL_PIN_COUNT) { return; }
  settings->pin_settings[pin].func = func;
}

uint8_t jrk_settings_get_pin_func(const jrk_settings * settings, uint8_t pin)
{
  if (!settings || pin >= JRK_CONTROL_PIN_COUNT) { return 0; }
  return settings->pin_settings[pin].func;
}

void jrk_settings_set_pin_pullup(jrk_settings * settings, uint8_t pin, bool pullup)
{
  if (!settings || pin >= JRK_CONTROL_PIN_COUNT) { return; }
  settings->pin_settings[pin].pullup = pullup;
}

bool jrk_settings_get_pin_pullup(const jrk_settings * settings, uint8_t pin)
{
  if (!settings || pin >= JRK_CONTROL_PIN_COUNT) { return 0; }
  return settings->pin_settings[pin].pullup;
}

void jrk_settings_set_pin_analog(jrk_settings * settings, uint8_t pin, bool analog)
{
  if (!settings || pin >= JRK_CONTROL_PIN_COUNT) { return; }
  settings->pin_settings[pin].analog = analog;
}

bool jrk_settings_get_pin_analog(const jrk_settings * settings, uint8_t pin)
{
  if (!settings || pin >= JRK_CONTROL_PIN_COUNT) { return 0; }
  return settings->pin_settings[pin].analog;
}
