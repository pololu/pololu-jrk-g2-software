#include "jrk_internal.h"

static void jrk_settings_fix_core(jrk_settings * settings, jrk_string * warnings)
{
  uint8_t product = jrk_settings_get_product(settings);

  // Beginning of auto-generated settings fixing code.

  {
    uint8_t input_mode = jrk_settings_get_input_mode(settings);
    if (input_mode > JRK_INPUT_MODE_RC)
    {
      input_mode = JRK_INPUT_MODE_SERIAL;
      jrk_sprintf(warnings,
        "Warning: The input mode is invalid "
        "so it will be changed to serial.\n");
    }
    jrk_settings_set_input_mode(settings, input_mode);
  }

  {
    uint16_t input_error_minimum = jrk_settings_get_input_error_minimum(settings);
    if (input_error_minimum > 4095)
    {
      input_error_minimum = 4095;
      jrk_sprintf(warnings,
        "Warning: The input error minimum is too high "
        "so it will be changed to %u.\n", input_error_minimum);
    }
    jrk_settings_set_input_error_minimum(settings, input_error_minimum);
  }

  {
    uint16_t input_error_maximum = jrk_settings_get_input_error_maximum(settings);
    if (input_error_maximum > 4095)
    {
      input_error_maximum = 4095;
      jrk_sprintf(warnings,
        "Warning: The input error maximum is too high "
        "so it will be changed to %u.\n", input_error_maximum);
    }
    jrk_settings_set_input_error_maximum(settings, input_error_maximum);
  }

  {
    uint16_t input_minimum = jrk_settings_get_input_minimum(settings);
    if (input_minimum > 4095)
    {
      input_minimum = 4095;
      jrk_sprintf(warnings,
        "Warning: The input minimum is too high "
        "so it will be changed to %u.\n", input_minimum);
    }
    jrk_settings_set_input_minimum(settings, input_minimum);
  }

  {
    uint16_t input_maximum = jrk_settings_get_input_maximum(settings);
    if (input_maximum > 4095)
    {
      input_maximum = 4095;
      jrk_sprintf(warnings,
        "Warning: The input maximum is too high "
        "so it will be changed to %u.\n", input_maximum);
    }
    jrk_settings_set_input_maximum(settings, input_maximum);
  }

  {
    uint16_t input_neutral_minimum = jrk_settings_get_input_neutral_minimum(settings);
    if (input_neutral_minimum > 4095)
    {
      input_neutral_minimum = 4095;
      jrk_sprintf(warnings,
        "Warning: The input neutral minimum is too high "
        "so it will be changed to %u.\n", input_neutral_minimum);
    }
    jrk_settings_set_input_neutral_minimum(settings, input_neutral_minimum);
  }

  {
    uint16_t input_neutral_maximum = jrk_settings_get_input_neutral_maximum(settings);
    if (input_neutral_maximum > 4095)
    {
      input_neutral_maximum = 4095;
      jrk_sprintf(warnings,
        "Warning: The input neutral maximum is too high "
        "so it will be changed to %u.\n", input_neutral_maximum);
    }
    jrk_settings_set_input_neutral_maximum(settings, input_neutral_maximum);
  }

  {
    uint16_t output_minimum = jrk_settings_get_output_minimum(settings);
    if (output_minimum > 4095)
    {
      output_minimum = 4095;
      jrk_sprintf(warnings,
        "Warning: The output minimum is too high "
        "so it will be changed to %u.\n", output_minimum);
    }
    jrk_settings_set_output_minimum(settings, output_minimum);
  }

  {
    uint16_t output_neutral = jrk_settings_get_output_neutral(settings);
    if (output_neutral > 4095)
    {
      output_neutral = 4095;
      jrk_sprintf(warnings,
        "Warning: The output neutral is too high "
        "so it will be changed to %u.\n", output_neutral);
    }
    jrk_settings_set_output_neutral(settings, output_neutral);
  }

  {
    uint16_t output_maximum = jrk_settings_get_output_maximum(settings);
    if (output_maximum > 4095)
    {
      output_maximum = 4095;
      jrk_sprintf(warnings,
        "Warning: The output maximum is too high "
        "so it will be changed to %u.\n", output_maximum);
    }
    jrk_settings_set_output_maximum(settings, output_maximum);
  }

  {
    uint8_t input_scaling_degree = jrk_settings_get_input_scaling_degree(settings);
    if (input_scaling_degree > JRK_SCALING_DEGREE_QUINTIC)
    {
      input_scaling_degree = JRK_SCALING_DEGREE_LINEAR;
      jrk_sprintf(warnings,
        "Warning: The input scaling degree is invalid "
        "so it will be changed to linear.\n");
    }
    jrk_settings_set_input_scaling_degree(settings, input_scaling_degree);
  }

  {
    uint8_t input_analog_samples_exponent = jrk_settings_get_input_analog_samples_exponent(settings);
    if (input_analog_samples_exponent > 10)
    {
      input_analog_samples_exponent = 10;
      jrk_sprintf(warnings,
        "Warning: The input analog samples exponent is too high "
        "so it will be changed to %u.\n", input_analog_samples_exponent);
    }
    jrk_settings_set_input_analog_samples_exponent(settings, input_analog_samples_exponent);
  }

  {
    uint8_t feedback_mode = jrk_settings_get_feedback_mode(settings);
    if (feedback_mode > JRK_FEEDBACK_MODE_FREQUENCY)
    {
      feedback_mode = JRK_FEEDBACK_MODE_NONE;
      jrk_sprintf(warnings,
        "Warning: The feedback mode is invalid "
        "so it will be changed to none.\n");
    }
    jrk_settings_set_feedback_mode(settings, feedback_mode);
  }

  {
    uint16_t feedback_error_minimum = jrk_settings_get_feedback_error_minimum(settings);
    if (feedback_error_minimum > 4095)
    {
      feedback_error_minimum = 4095;
      jrk_sprintf(warnings,
        "Warning: The feedback error minimum is too high "
        "so it will be changed to %u.\n", feedback_error_minimum);
    }
    jrk_settings_set_feedback_error_minimum(settings, feedback_error_minimum);
  }

  {
    uint16_t feedback_error_maximum = jrk_settings_get_feedback_error_maximum(settings);
    if (feedback_error_maximum > 4095)
    {
      feedback_error_maximum = 4095;
      jrk_sprintf(warnings,
        "Warning: The feedback error maximum is too high "
        "so it will be changed to %u.\n", feedback_error_maximum);
    }
    jrk_settings_set_feedback_error_maximum(settings, feedback_error_maximum);
  }

  {
    uint16_t feedback_minimum = jrk_settings_get_feedback_minimum(settings);
    if (feedback_minimum > 4095)
    {
      feedback_minimum = 4095;
      jrk_sprintf(warnings,
        "Warning: The feedback minimum is too high "
        "so it will be changed to %u.\n", feedback_minimum);
    }
    jrk_settings_set_feedback_minimum(settings, feedback_minimum);
  }

  {
    uint16_t feedback_maximum = jrk_settings_get_feedback_maximum(settings);
    if (feedback_maximum > 4095)
    {
      feedback_maximum = 4095;
      jrk_sprintf(warnings,
        "Warning: The feedback maximum is too high "
        "so it will be changed to %u.\n", feedback_maximum);
    }
    jrk_settings_set_feedback_maximum(settings, feedback_maximum);
  }

  {
    uint8_t feedback_analog_samples_exponent = jrk_settings_get_feedback_analog_samples_exponent(settings);
    if (feedback_analog_samples_exponent > 10)
    {
      feedback_analog_samples_exponent = 10;
      jrk_sprintf(warnings,
        "Warning: The feedback analog samples exponent is too high "
        "so it will be changed to %u.\n", feedback_analog_samples_exponent);
    }
    jrk_settings_set_feedback_analog_samples_exponent(settings, feedback_analog_samples_exponent);
  }

  {
    uint8_t serial_mode = jrk_settings_get_serial_mode(settings);
    if (serial_mode > JRK_SERIAL_MODE_UART)
    {
      serial_mode = JRK_SERIAL_MODE_USB_DUAL_PORT;
      jrk_sprintf(warnings,
        "Warning: The serial mode is invalid "
        "so it will be changed to USB dual port.\n");
    }
    jrk_settings_set_serial_mode(settings, serial_mode);
  }

  {
    uint16_t proportional_multiplier = jrk_settings_get_proportional_multiplier(settings);
    if (proportional_multiplier > 1023)
    {
      proportional_multiplier = 1023;
      jrk_sprintf(warnings,
        "Warning: The proportional multiplier is too high "
        "so it will be changed to %u.\n", proportional_multiplier);
    }
    jrk_settings_set_proportional_multiplier(settings, proportional_multiplier);
  }

  {
    uint8_t proportional_exponent = jrk_settings_get_proportional_exponent(settings);
    if (proportional_exponent > 18)
    {
      proportional_exponent = 18;
      jrk_sprintf(warnings,
        "Warning: The proportional exponent is too high "
        "so it will be changed to %u.\n", proportional_exponent);
    }
    jrk_settings_set_proportional_exponent(settings, proportional_exponent);
  }

  {
    uint16_t integral_multiplier = jrk_settings_get_integral_multiplier(settings);
    if (integral_multiplier > 1023)
    {
      integral_multiplier = 1023;
      jrk_sprintf(warnings,
        "Warning: The integral multiplier is too high "
        "so it will be changed to %u.\n", integral_multiplier);
    }
    jrk_settings_set_integral_multiplier(settings, integral_multiplier);
  }

  {
    uint8_t integral_exponent = jrk_settings_get_integral_exponent(settings);
    if (integral_exponent > 18)
    {
      integral_exponent = 18;
      jrk_sprintf(warnings,
        "Warning: The integral exponent is too high "
        "so it will be changed to %u.\n", integral_exponent);
    }
    jrk_settings_set_integral_exponent(settings, integral_exponent);
  }

  {
    uint16_t derivative_multiplier = jrk_settings_get_derivative_multiplier(settings);
    if (derivative_multiplier > 1023)
    {
      derivative_multiplier = 1023;
      jrk_sprintf(warnings,
        "Warning: The derivative multiplier is too high "
        "so it will be changed to %u.\n", derivative_multiplier);
    }
    jrk_settings_set_derivative_multiplier(settings, derivative_multiplier);
  }

  {
    uint8_t derivative_exponent = jrk_settings_get_derivative_exponent(settings);
    if (derivative_exponent > 18)
    {
      derivative_exponent = 18;
      jrk_sprintf(warnings,
        "Warning: The derivative exponent is too high "
        "so it will be changed to %u.\n", derivative_exponent);
    }
    jrk_settings_set_derivative_exponent(settings, derivative_exponent);
  }

  {
    uint16_t pid_period = jrk_settings_get_pid_period(settings);
    if (pid_period < 1)
    {
      pid_period = 1;
      jrk_sprintf(warnings,
        "Warning: The pid period is too low "
        "so it will be changed to %u.\n", pid_period);
    }
    if (pid_period > 8191)
    {
      pid_period = 8191;
      jrk_sprintf(warnings,
        "Warning: The pid period is too high "
        "so it will be changed to %u.\n", pid_period);
    }
    jrk_settings_set_pid_period(settings, pid_period);
  }

  {
    uint8_t integral_divider_exponent = jrk_settings_get_integral_divider_exponent(settings);
    if (integral_divider_exponent > 15)
    {
      integral_divider_exponent = 15;
      jrk_sprintf(warnings,
        "Warning: The integral divider exponent is too high "
        "so it will be changed to %u.\n", integral_divider_exponent);
    }
    jrk_settings_set_integral_divider_exponent(settings, integral_divider_exponent);
  }

  {
    uint16_t integral_limit = jrk_settings_get_integral_limit(settings);
    if (integral_limit > 32767)
    {
      integral_limit = 32767;
      jrk_sprintf(warnings,
        "Warning: The integral limit is too high "
        "so it will be changed to %u.\n", integral_limit);
    }
    jrk_settings_set_integral_limit(settings, integral_limit);
  }

  {
    uint8_t pwm_frequency = jrk_settings_get_pwm_frequency(settings);
    if (pwm_frequency > JRK_PWM_FREQUENCY_5)
    {
      pwm_frequency = JRK_PWM_FREQUENCY_20;
      jrk_sprintf(warnings,
        "Warning: The pwm frequency is invalid "
        "so it will be changed to 20 kHz.\n");
    }
    jrk_settings_set_pwm_frequency(settings, pwm_frequency);
  }

  {
    uint8_t current_samples_exponent = jrk_settings_get_current_samples_exponent(settings);
    if (current_samples_exponent > 10)
    {
      current_samples_exponent = 10;
      jrk_sprintf(warnings,
        "Warning: The current samples exponent is too high "
        "so it will be changed to %u.\n", current_samples_exponent);
    }
    jrk_settings_set_current_samples_exponent(settings, current_samples_exponent);
  }

  if (product != JRK_PRODUCT_UMC06A)
  {
    uint8_t hard_overcurrent_threshold = jrk_settings_get_hard_overcurrent_threshold(settings);
    if (hard_overcurrent_threshold < 1)
    {
      hard_overcurrent_threshold = 1;
      jrk_sprintf(warnings,
        "Warning: The hard overcurrent threshold is too low "
        "so it will be changed to %u.\n", hard_overcurrent_threshold);
    }
    jrk_settings_set_hard_overcurrent_threshold(settings, hard_overcurrent_threshold);
  }

  {
    uint16_t max_duty_cycle_while_feedback_out_of_range = jrk_settings_get_max_duty_cycle_while_feedback_out_of_range(settings);
    if (max_duty_cycle_while_feedback_out_of_range < 1)
    {
      max_duty_cycle_while_feedback_out_of_range = 1;
      jrk_sprintf(warnings,
        "Warning: The max duty cycle while feedback out of range is too low "
        "so it will be changed to %u.\n", max_duty_cycle_while_feedback_out_of_range);
    }
    if (max_duty_cycle_while_feedback_out_of_range > 600)
    {
      max_duty_cycle_while_feedback_out_of_range = 600;
      jrk_sprintf(warnings,
        "Warning: The max duty cycle while feedback out of range is too high "
        "so it will be changed to %u.\n", max_duty_cycle_while_feedback_out_of_range);
    }
    jrk_settings_set_max_duty_cycle_while_feedback_out_of_range(settings, max_duty_cycle_while_feedback_out_of_range);
  }

  {
    uint16_t max_acceleration_forward = jrk_settings_get_max_acceleration_forward(settings);
    if (max_acceleration_forward < 1)
    {
      max_acceleration_forward = 1;
      jrk_sprintf(warnings,
        "Warning: The max acceleration forward is too low "
        "so it will be changed to %u.\n", max_acceleration_forward);
    }
    if (max_acceleration_forward > 600)
    {
      max_acceleration_forward = 600;
      jrk_sprintf(warnings,
        "Warning: The max acceleration forward is too high "
        "so it will be changed to %u.\n", max_acceleration_forward);
    }
    jrk_settings_set_max_acceleration_forward(settings, max_acceleration_forward);
  }

  {
    uint16_t max_acceleration_reverse = jrk_settings_get_max_acceleration_reverse(settings);
    if (max_acceleration_reverse < 1)
    {
      max_acceleration_reverse = 1;
      jrk_sprintf(warnings,
        "Warning: The max acceleration reverse is too low "
        "so it will be changed to %u.\n", max_acceleration_reverse);
    }
    if (max_acceleration_reverse > 600)
    {
      max_acceleration_reverse = 600;
      jrk_sprintf(warnings,
        "Warning: The max acceleration reverse is too high "
        "so it will be changed to %u.\n", max_acceleration_reverse);
    }
    jrk_settings_set_max_acceleration_reverse(settings, max_acceleration_reverse);
  }

  {
    uint16_t max_deceleration_forward = jrk_settings_get_max_deceleration_forward(settings);
    if (max_deceleration_forward < 1)
    {
      max_deceleration_forward = 1;
      jrk_sprintf(warnings,
        "Warning: The max deceleration forward is too low "
        "so it will be changed to %u.\n", max_deceleration_forward);
    }
    if (max_deceleration_forward > 600)
    {
      max_deceleration_forward = 600;
      jrk_sprintf(warnings,
        "Warning: The max deceleration forward is too high "
        "so it will be changed to %u.\n", max_deceleration_forward);
    }
    jrk_settings_set_max_deceleration_forward(settings, max_deceleration_forward);
  }

  {
    uint16_t max_deceleration_reverse = jrk_settings_get_max_deceleration_reverse(settings);
    if (max_deceleration_reverse < 1)
    {
      max_deceleration_reverse = 1;
      jrk_sprintf(warnings,
        "Warning: The max deceleration reverse is too low "
        "so it will be changed to %u.\n", max_deceleration_reverse);
    }
    if (max_deceleration_reverse > 600)
    {
      max_deceleration_reverse = 600;
      jrk_sprintf(warnings,
        "Warning: The max deceleration reverse is too high "
        "so it will be changed to %u.\n", max_deceleration_reverse);
    }
    jrk_settings_set_max_deceleration_reverse(settings, max_deceleration_reverse);
  }

  {
    uint16_t max_duty_cycle_forward = jrk_settings_get_max_duty_cycle_forward(settings);
    if (max_duty_cycle_forward > 600)
    {
      max_duty_cycle_forward = 600;
      jrk_sprintf(warnings,
        "Warning: The max duty cycle forward is too high "
        "so it will be changed to %u.\n", max_duty_cycle_forward);
    }
    jrk_settings_set_max_duty_cycle_forward(settings, max_duty_cycle_forward);
  }

  {
    uint16_t max_duty_cycle_reverse = jrk_settings_get_max_duty_cycle_reverse(settings);
    if (max_duty_cycle_reverse > 600)
    {
      max_duty_cycle_reverse = 600;
      jrk_sprintf(warnings,
        "Warning: The max duty cycle reverse is too high "
        "so it will be changed to %u.\n", max_duty_cycle_reverse);
    }
    jrk_settings_set_max_duty_cycle_reverse(settings, max_duty_cycle_reverse);
  }

  if (product != JRK_PRODUCT_UMC06A)
  {
    uint16_t encoded_hard_current_limit_forward = jrk_settings_get_encoded_hard_current_limit_forward(settings);
    if (encoded_hard_current_limit_forward > 95)
    {
      encoded_hard_current_limit_forward = 95;
      jrk_sprintf(warnings,
        "Warning: The encoded hard current limit forward is too high "
        "so it will be changed to %u.\n", encoded_hard_current_limit_forward);
    }
    jrk_settings_set_encoded_hard_current_limit_forward(settings, encoded_hard_current_limit_forward);
  }

  if (product != JRK_PRODUCT_UMC06A)
  {
    uint16_t encoded_hard_current_limit_reverse = jrk_settings_get_encoded_hard_current_limit_reverse(settings);
    if (encoded_hard_current_limit_reverse > 95)
    {
      encoded_hard_current_limit_reverse = 95;
      jrk_sprintf(warnings,
        "Warning: The encoded hard current limit reverse is too high "
        "so it will be changed to %u.\n", encoded_hard_current_limit_reverse);
    }
    jrk_settings_set_encoded_hard_current_limit_reverse(settings, encoded_hard_current_limit_reverse);
  }

  {
    int16_t vin_calibration = jrk_settings_get_vin_calibration(settings);
    if (vin_calibration < -500)
    {
      vin_calibration = -500;
      jrk_sprintf(warnings,
        "Warning: The VIN calibration is too low "
        "so it will be changed to %d.\n", vin_calibration);
    }
    if (vin_calibration > 500)
    {
      vin_calibration = 500;
      jrk_sprintf(warnings,
        "Warning: The VIN calibration is too high "
        "so it will be changed to %d.\n", vin_calibration);
    }
    jrk_settings_set_vin_calibration(settings, vin_calibration);
  }

  {
    uint8_t fbt_method = jrk_settings_get_fbt_method(settings);
    if (fbt_method > JRK_FBT_METHOD_PULSE_TIMING)
    {
      fbt_method = JRK_FBT_METHOD_PULSE_COUNTING;
      jrk_sprintf(warnings,
        "Warning: The fbt method is invalid "
        "so it will be changed to pulse counting.\n");
    }
    jrk_settings_set_fbt_method(settings, fbt_method);
  }

  {
    uint8_t fbt_timing_clock = jrk_settings_get_fbt_timing_clock(settings);
    if (fbt_timing_clock > JRK_FBT_TIMING_CLOCK_24)
    {
      fbt_timing_clock = JRK_FBT_TIMING_CLOCK_1_5;
      jrk_sprintf(warnings,
        "Warning: The fbt timing clock is invalid "
        "so it will be changed to 1.5 MHz.\n");
    }
    jrk_settings_set_fbt_timing_clock(settings, fbt_timing_clock);
  }

  {
    uint16_t fbt_timing_timeout = jrk_settings_get_fbt_timing_timeout(settings);
    if (fbt_timing_timeout < 1)
    {
      fbt_timing_timeout = 1;
      jrk_sprintf(warnings,
        "Warning: The fbt timing timeout is too low "
        "so it will be changed to %u.\n", fbt_timing_timeout);
    }
    if (fbt_timing_timeout > 60000)
    {
      fbt_timing_timeout = 60000;
      jrk_sprintf(warnings,
        "Warning: The fbt timing timeout is too high "
        "so it will be changed to %u.\n", fbt_timing_timeout);
    }
    jrk_settings_set_fbt_timing_timeout(settings, fbt_timing_timeout);
  }

  {
    uint8_t fbt_samples = jrk_settings_get_fbt_samples(settings);
    if (fbt_samples < 1)
    {
      fbt_samples = 1;
      jrk_sprintf(warnings,
        "Warning: The fbt samples is too low "
        "so it will be changed to %u.\n", fbt_samples);
    }
    if (fbt_samples > JRK_MAX_ALLOWED_FBT_SAMPLES)
    {
      fbt_samples = JRK_MAX_ALLOWED_FBT_SAMPLES;
      jrk_sprintf(warnings,
        "Warning: The fbt samples is too high "
        "so it will be changed to %u.\n", fbt_samples);
    }
    jrk_settings_set_fbt_samples(settings, fbt_samples);
  }

  {
    uint8_t fbt_divider_exponent = jrk_settings_get_fbt_divider_exponent(settings);
    if (fbt_divider_exponent > 15)
    {
      fbt_divider_exponent = 15;
      jrk_sprintf(warnings,
        "Warning: The fbt divider exponent is too high "
        "so it will be changed to %u.\n", fbt_divider_exponent);
    }
    jrk_settings_set_fbt_divider_exponent(settings, fbt_divider_exponent);
  }

  // End of auto-generated settings fixing code.

  {
    uint32_t baud = jrk_settings_get_serial_baud_rate(settings);
    if (baud < JRK_MIN_ALLOWED_BAUD_RATE)
    {
      baud = JRK_MIN_ALLOWED_BAUD_RATE;
      jrk_sprintf(warnings,
        "Warning: The serial baud rate is too low "
        "so it will be changed to %u.\n", baud);
    }
    if (baud > JRK_MAX_ALLOWED_BAUD_RATE)
    {
      baud = JRK_MAX_ALLOWED_BAUD_RATE;
      jrk_sprintf(warnings,
        "Warning: The serial baud rate is too high "
        "so it will be changed to %u.\n", baud);
    }

    baud = jrk_settings_achievable_serial_baud_rate(settings, baud);
    jrk_settings_set_serial_baud_rate(settings, baud);
  }

  {
    uint32_t duration = jrk_settings_get_brake_duration_forward(settings);

    // Make it be a multiple of 5, rounding up.
    duration = (duration + 4) / JRK_BRAKE_DURATION_UNITS * JRK_BRAKE_DURATION_UNITS;

    if (duration > JRK_MAX_ALLOWED_BRAKE_DURATION)
    {
      duration = JRK_MAX_ALLOWED_BRAKE_DURATION;
      jrk_sprintf(warnings,
        "Warning: The brake duration forward is too high "
        "so it will be changed to %u.\n", duration);
    }

    jrk_settings_set_brake_duration_forward(settings, duration);
  }

  {
    uint32_t duration = jrk_settings_get_brake_duration_reverse(settings);

    // Make it be a multiple of 5, rounding up.
    duration = (duration + JRK_BRAKE_DURATION_UNITS - 1)
      / JRK_BRAKE_DURATION_UNITS * JRK_BRAKE_DURATION_UNITS;

    if (duration > JRK_MAX_ALLOWED_BRAKE_DURATION)
    {
      duration = JRK_MAX_ALLOWED_BRAKE_DURATION;
      jrk_sprintf(warnings,
        "Warning: The brake duration reverse is too high "
        "so it will be changed to %u.\n", duration);
    }

    jrk_settings_set_brake_duration_reverse(settings, duration);
  }

  {
    uint16_t serial_device_number = jrk_settings_get_serial_device_number(settings);

    uint16_t device_number_mask;
    if (jrk_settings_get_serial_enable_14bit_device_number(settings))
    {
      device_number_mask = 0x3FFF;
    }
    else
    {
      device_number_mask = 0x7F;
    }

    // Clear the ignored upper bits in the number to avoid user confusion.
    if (serial_device_number > device_number_mask)
    {
      serial_device_number &= device_number_mask;
      jrk_sprintf(warnings,
        "Warning: The serial device number is higher than %u "
        "so it will be changed to %u.\n", device_number_mask, serial_device_number);
    }

    jrk_settings_set_serial_device_number(settings, serial_device_number);
  }

  {
    uint32_t timeout = jrk_settings_get_serial_timeout(settings);

    // Make it be a multiple of 10, rounding up
    // (we want to avoid changing small timeouts to 0).
    timeout = (timeout + JRK_SERIAL_TIMEOUT_UNITS - 1)
      / JRK_SERIAL_TIMEOUT_UNITS * JRK_SERIAL_TIMEOUT_UNITS;

    if (timeout > JRK_MAX_ALLOWED_SERIAL_TIMEOUT)
    {
      timeout = JRK_MAX_ALLOWED_SERIAL_TIMEOUT;
      jrk_sprintf(warnings,
        "Warning: The serial timeout is too high "
        "so it will be changed to %u.\n", timeout);
    }

    jrk_settings_set_serial_timeout(settings, timeout);
  }

  {
    int16_t offset_calibration =
      jrk_settings_get_current_offset_calibration(settings);
    int16_t scale_calibration =
      jrk_settings_get_current_scale_calibration(settings);

    int16_t min_offset_calibration = -32768;
    int16_t max_offset_calibration = 32767;
    int16_t min_scale_calibration = -32768;
    int16_t max_scale_calibration = 32767;
    switch (product)
    {
    case JRK_PRODUCT_UMC04A_30V:
    case JRK_PRODUCT_UMC04A_40V:
    case JRK_PRODUCT_UMC05A_40V:
    case JRK_PRODUCT_UMC05A_30V:
      min_offset_calibration = -800;
      max_offset_calibration = 800;
      min_scale_calibration = -1875;
      max_scale_calibration = 1875;
      break;
    case JRK_PRODUCT_UMC06A:
      min_scale_calibration = -1136;
      max_scale_calibration = 1136;
      break;
    }

    if (offset_calibration < min_offset_calibration)
    {
      offset_calibration = min_offset_calibration;
      jrk_sprintf(warnings,
        "Warning: The current offset calibration is too low "
        "so it will be changed to %d.\n", offset_calibration);
    }
    if (offset_calibration > max_offset_calibration)
    {
      offset_calibration = max_offset_calibration;
      jrk_sprintf(warnings,
        "Warning: The current offset calibration is too high "
        "so it will be changed to %d.\n", offset_calibration);
    }
    if (scale_calibration < min_scale_calibration)
    {
      scale_calibration = min_scale_calibration;
      jrk_sprintf(warnings,
        "Warning: The current scale calibration is too low "
        "so it will be changed to %d.\n", scale_calibration);
    }
    if (scale_calibration > max_scale_calibration)
    {
      scale_calibration = max_scale_calibration;
      jrk_sprintf(warnings,
        "Warning: The current scale calibration is too high "
        "so it will be changed to %d.\n", scale_calibration);
    }

    jrk_settings_set_current_offset_calibration(settings, offset_calibration);
    jrk_settings_set_current_scale_calibration(settings, scale_calibration);
  }
}

jrk_error * jrk_settings_fix(jrk_settings * settings, char ** warnings)
{
  if (warnings) { *warnings = NULL; }

  if (settings == NULL)
  {
    return jrk_error_create("Jrk settings pointer is null.");
  }

  // Make a string to store the warnings we accumulate in this function.
  jrk_string str;
  if (warnings)
  {
    jrk_string_setup(&str);
  }
  else
  {
    jrk_string_setup_dummy(&str);
  }

  jrk_settings_fix_core(settings, &str);

  #ifndef NDEBUG
  {
    // In debug builds, assert that this function is idempotent.
    jrk_string str2;
    jrk_string_setup(&str2);
    jrk_settings_fix_core(settings, &str2);
    assert(str2.data != NULL && str2.data[0] == 0);
    jrk_string_free(str2.data);
  }
  #endif

  if (warnings && str.data == NULL)
  {
    // Memory allocation for the warning string failed at some point.
    return &jrk_error_no_memory;
  }

  if (warnings)
  {
    *warnings = str.data;
    str.data = NULL;
  }

  return NULL;
}
