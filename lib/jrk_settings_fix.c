#include "jrk_internal.h"

static void jrk_settings_fix_core(jrk_settings * settings, jrk_string * warnings)
{
  // Beginning of auto-generated settings fixing code.

  {
    uint8_t input_mode = jrk_settings_get_input_mode(settings);
    if (input_mode > JRK_INPUT_MODE_PULSE_WIDTH)
    {
      input_mode = JRK_INPUT_MODE_SERIAL;
      jrk_sprintf(warnings,
        "Warning: The input mode was invalid "
        "so it will be changed to serial.");
    }
    jrk_settings_set_input_mode(settings, input_mode);
  }

  {
    uint16_t input_absolute_minimum = jrk_settings_get_input_absolute_minimum(settings);
    if (input_absolute_minimum > 4095)
    {
      input_absolute_minimum = 4095;
      jrk_sprintf(warnings,
        "Warning: The input absolute minimum was too high "
        "so it will be changed to %u.\n", input_absolute_minimum);
    }
    jrk_settings_set_input_absolute_minimum(settings, input_absolute_minimum);
  }

  {
    uint16_t input_absolute_maximum = jrk_settings_get_input_absolute_maximum(settings);
    if (input_absolute_maximum > 4095)
    {
      input_absolute_maximum = 4095;
      jrk_sprintf(warnings,
        "Warning: The input absolute maximum was too high "
        "so it will be changed to %u.\n", input_absolute_maximum);
    }
    jrk_settings_set_input_absolute_maximum(settings, input_absolute_maximum);
  }

  {
    uint16_t input_minimum = jrk_settings_get_input_minimum(settings);
    if (input_minimum > 4095)
    {
      input_minimum = 4095;
      jrk_sprintf(warnings,
        "Warning: The input minimum was too high "
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
        "Warning: The input maximum was too high "
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
        "Warning: The input neutral minimum was too high "
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
        "Warning: The input neutral maximum was too high "
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
        "Warning: The output minimum was too high "
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
        "Warning: The output neutral was too high "
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
        "Warning: The output maximum was too high "
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
        "Warning: The input scaling degree was invalid "
        "so it will be changed to linear.");
    }
    jrk_settings_set_input_scaling_degree(settings, input_scaling_degree);
  }

  {
    uint8_t input_analog_samples_exponent = jrk_settings_get_input_analog_samples_exponent(settings);
    if (input_analog_samples_exponent > 8)
    {
      input_analog_samples_exponent = 8;
      jrk_sprintf(warnings,
        "Warning: The input analog samples exponent was too high "
        "so it will be changed to %u.\n", input_analog_samples_exponent);
    }
    jrk_settings_set_input_analog_samples_exponent(settings, input_analog_samples_exponent);
  }

  {
    uint8_t feedback_mode = jrk_settings_get_feedback_mode(settings);
    if (feedback_mode > JRK_FEEDBACK_MODE_FREQUENCY)
    {
      feedback_mode = JRK_FEEDBACK_MODE_ANALOG;
      jrk_sprintf(warnings,
        "Warning: The feedback mode was invalid "
        "so it will be changed to analog.");
    }
    jrk_settings_set_feedback_mode(settings, feedback_mode);
  }

  {
    uint16_t feedback_absolute_minimum = jrk_settings_get_feedback_absolute_minimum(settings);
    if (feedback_absolute_minimum > 4095)
    {
      feedback_absolute_minimum = 4095;
      jrk_sprintf(warnings,
        "Warning: The feedback absolute minimum was too high "
        "so it will be changed to %u.\n", feedback_absolute_minimum);
    }
    jrk_settings_set_feedback_absolute_minimum(settings, feedback_absolute_minimum);
  }

  {
    uint16_t feedback_absolute_maximum = jrk_settings_get_feedback_absolute_maximum(settings);
    if (feedback_absolute_maximum > 4095)
    {
      feedback_absolute_maximum = 4095;
      jrk_sprintf(warnings,
        "Warning: The feedback absolute maximum was too high "
        "so it will be changed to %u.\n", feedback_absolute_maximum);
    }
    jrk_settings_set_feedback_absolute_maximum(settings, feedback_absolute_maximum);
  }

  {
    uint16_t feedback_minimum = jrk_settings_get_feedback_minimum(settings);
    if (feedback_minimum > 4095)
    {
      feedback_minimum = 4095;
      jrk_sprintf(warnings,
        "Warning: The feedback minimum was too high "
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
        "Warning: The feedback maximum was too high "
        "so it will be changed to %u.\n", feedback_maximum);
    }
    jrk_settings_set_feedback_maximum(settings, feedback_maximum);
  }

  {
    uint8_t feedback_dead_zone = jrk_settings_get_feedback_dead_zone(settings);
    jrk_settings_set_feedback_dead_zone(settings, feedback_dead_zone);
  }

  {
    uint8_t feedback_analog_samples_exponent = jrk_settings_get_feedback_analog_samples_exponent(settings);
    if (feedback_analog_samples_exponent > 8)
    {
      feedback_analog_samples_exponent = 8;
      jrk_sprintf(warnings,
        "Warning: The feedback analog samples exponent was too high "
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
        "Warning: The serial mode was invalid "
        "so it will be changed to USB dual port.");
    }
    jrk_settings_set_serial_mode(settings, serial_mode);
  }

  {
    uint16_t serial_timeout = jrk_settings_get_serial_timeout(settings);
    jrk_settings_set_serial_timeout(settings, serial_timeout);
  }

  {
    uint16_t serial_device_number = jrk_settings_get_serial_device_number(settings);
    if (serial_device_number > 16383)
    {
      serial_device_number = 16383;
      jrk_sprintf(warnings,
        "Warning: The serial device number was too high "
        "so it will be changed to %u.\n", serial_device_number);
    }
    jrk_settings_set_serial_device_number(settings, serial_device_number);
  }

  {
    uint16_t proportional_multiplier = jrk_settings_get_proportional_multiplier(settings);
    if (proportional_multiplier > 1023)
    {
      proportional_multiplier = 1023;
      jrk_sprintf(warnings,
        "Warning: The proportional multiplier was too high "
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
        "Warning: The proportional exponent was too high "
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
        "Warning: The integral multiplier was too high "
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
        "Warning: The integral exponent was too high "
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
        "Warning: The derivative multiplier was too high "
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
        "Warning: The derivative exponent was too high "
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
        "Warning: The pid period was too low "
        "so it will be changed to %u.\n", pid_period);
    }
    if (pid_period > 8191)
    {
      pid_period = 8191;
      jrk_sprintf(warnings,
        "Warning: The pid period was too high "
        "so it will be changed to %u.\n", pid_period);
    }
    jrk_settings_set_pid_period(settings, pid_period);
  }

  {
    uint16_t pid_integral_limit = jrk_settings_get_pid_integral_limit(settings);
    if (pid_integral_limit > 32767)
    {
      pid_integral_limit = 32767;
      jrk_sprintf(warnings,
        "Warning: The pid integral limit was too high "
        "so it will be changed to %u.\n", pid_integral_limit);
    }
    jrk_settings_set_pid_integral_limit(settings, pid_integral_limit);
  }

  {
    uint8_t motor_pwm_frequency = jrk_settings_get_motor_pwm_frequency(settings);
    if (motor_pwm_frequency > JRK_MOTOR_PWM_FREQUENCY_5)
    {
      motor_pwm_frequency = JRK_MOTOR_PWM_FREQUENCY_20;
      jrk_sprintf(warnings,
        "Warning: The motor pwm frequency was invalid "
        "so it will be changed to 20 kHz.");
    }
    jrk_settings_set_motor_pwm_frequency(settings, motor_pwm_frequency);
  }

  {
    uint16_t motor_max_duty_cycle_while_feedback_out_of_range = jrk_settings_get_motor_max_duty_cycle_while_feedback_out_of_range(settings);
    if (motor_max_duty_cycle_while_feedback_out_of_range < 1)
    {
      motor_max_duty_cycle_while_feedback_out_of_range = 1;
      jrk_sprintf(warnings,
        "Warning: The motor max duty cycle while feedback out of range was too low "
        "so it will be changed to %u.\n", motor_max_duty_cycle_while_feedback_out_of_range);
    }
    if (motor_max_duty_cycle_while_feedback_out_of_range > 600)
    {
      motor_max_duty_cycle_while_feedback_out_of_range = 600;
      jrk_sprintf(warnings,
        "Warning: The motor max duty cycle while feedback out of range was too high "
        "so it will be changed to %u.\n", motor_max_duty_cycle_while_feedback_out_of_range);
    }
    jrk_settings_set_motor_max_duty_cycle_while_feedback_out_of_range(settings, motor_max_duty_cycle_while_feedback_out_of_range);
  }

  {
    uint16_t motor_max_acceleration_forward = jrk_settings_get_motor_max_acceleration_forward(settings);
    if (motor_max_acceleration_forward < 1)
    {
      motor_max_acceleration_forward = 1;
      jrk_sprintf(warnings,
        "Warning: The motor max acceleration forward was too low "
        "so it will be changed to %u.\n", motor_max_acceleration_forward);
    }
    if (motor_max_acceleration_forward > 600)
    {
      motor_max_acceleration_forward = 600;
      jrk_sprintf(warnings,
        "Warning: The motor max acceleration forward was too high "
        "so it will be changed to %u.\n", motor_max_acceleration_forward);
    }
    jrk_settings_set_motor_max_acceleration_forward(settings, motor_max_acceleration_forward);
  }

  {
    uint16_t motor_max_acceleration_reverse = jrk_settings_get_motor_max_acceleration_reverse(settings);
    if (motor_max_acceleration_reverse < 1)
    {
      motor_max_acceleration_reverse = 1;
      jrk_sprintf(warnings,
        "Warning: The motor max acceleration reverse was too low "
        "so it will be changed to %u.\n", motor_max_acceleration_reverse);
    }
    if (motor_max_acceleration_reverse > 600)
    {
      motor_max_acceleration_reverse = 600;
      jrk_sprintf(warnings,
        "Warning: The motor max acceleration reverse was too high "
        "so it will be changed to %u.\n", motor_max_acceleration_reverse);
    }
    jrk_settings_set_motor_max_acceleration_reverse(settings, motor_max_acceleration_reverse);
  }

  {
    uint16_t motor_max_deceleration_forward = jrk_settings_get_motor_max_deceleration_forward(settings);
    if (motor_max_deceleration_forward < 1)
    {
      motor_max_deceleration_forward = 1;
      jrk_sprintf(warnings,
        "Warning: The motor max deceleration forward was too low "
        "so it will be changed to %u.\n", motor_max_deceleration_forward);
    }
    if (motor_max_deceleration_forward > 600)
    {
      motor_max_deceleration_forward = 600;
      jrk_sprintf(warnings,
        "Warning: The motor max deceleration forward was too high "
        "so it will be changed to %u.\n", motor_max_deceleration_forward);
    }
    jrk_settings_set_motor_max_deceleration_forward(settings, motor_max_deceleration_forward);
  }

  {
    uint16_t motor_max_deceleration_reverse = jrk_settings_get_motor_max_deceleration_reverse(settings);
    if (motor_max_deceleration_reverse < 1)
    {
      motor_max_deceleration_reverse = 1;
      jrk_sprintf(warnings,
        "Warning: The motor max deceleration reverse was too low "
        "so it will be changed to %u.\n", motor_max_deceleration_reverse);
    }
    if (motor_max_deceleration_reverse > 600)
    {
      motor_max_deceleration_reverse = 600;
      jrk_sprintf(warnings,
        "Warning: The motor max deceleration reverse was too high "
        "so it will be changed to %u.\n", motor_max_deceleration_reverse);
    }
    jrk_settings_set_motor_max_deceleration_reverse(settings, motor_max_deceleration_reverse);
  }

  {
    uint16_t motor_max_duty_cycle_forward = jrk_settings_get_motor_max_duty_cycle_forward(settings);
    if (motor_max_duty_cycle_forward > 600)
    {
      motor_max_duty_cycle_forward = 600;
      jrk_sprintf(warnings,
        "Warning: The motor max duty cycle forward was too high "
        "so it will be changed to %u.\n", motor_max_duty_cycle_forward);
    }
    jrk_settings_set_motor_max_duty_cycle_forward(settings, motor_max_duty_cycle_forward);
  }

  {
    uint16_t motor_max_duty_cycle_reverse = jrk_settings_get_motor_max_duty_cycle_reverse(settings);
    if (motor_max_duty_cycle_reverse > 600)
    {
      motor_max_duty_cycle_reverse = 600;
      jrk_sprintf(warnings,
        "Warning: The motor max duty cycle reverse was too high "
        "so it will be changed to %u.\n", motor_max_duty_cycle_reverse);
    }
    jrk_settings_set_motor_max_duty_cycle_reverse(settings, motor_max_duty_cycle_reverse);
  }

  {
    uint8_t motor_max_current_forward = jrk_settings_get_motor_max_current_forward(settings);
    jrk_settings_set_motor_max_current_forward(settings, motor_max_current_forward);
  }

  {
    uint8_t motor_max_current_reverse = jrk_settings_get_motor_max_current_reverse(settings);
    jrk_settings_set_motor_max_current_reverse(settings, motor_max_current_reverse);
  }

  {
    int8_t motor_current_calibration_forward = jrk_settings_get_motor_current_calibration_forward(settings);
    jrk_settings_set_motor_current_calibration_forward(settings, motor_current_calibration_forward);
  }

  {
    int8_t motor_current_calibration_reverse = jrk_settings_get_motor_current_calibration_reverse(settings);
    jrk_settings_set_motor_current_calibration_reverse(settings, motor_current_calibration_reverse);
  }

  {
    uint8_t motor_brake_duration_forward = jrk_settings_get_motor_brake_duration_forward(settings);
    jrk_settings_set_motor_brake_duration_forward(settings, motor_brake_duration_forward);
  }

  {
    uint8_t motor_brake_duration_reverse = jrk_settings_get_motor_brake_duration_reverse(settings);
    jrk_settings_set_motor_brake_duration_reverse(settings, motor_brake_duration_reverse);
  }

  {
    uint16_t error_enable = jrk_settings_get_error_enable(settings);
    jrk_settings_set_error_enable(settings, error_enable);
  }

  {
    uint16_t error_latch = jrk_settings_get_error_latch(settings);
    jrk_settings_set_error_latch(settings, error_latch);
  }

  {
    int16_t vin_calibration = jrk_settings_get_vin_calibration(settings);
    if (vin_calibration < -500)
    {
      vin_calibration = -500;
      jrk_sprintf(warnings,
        "Warning: The VIN calibration was too low "
        "so it will be changed to %d.\n", vin_calibration);
    }
    if (vin_calibration > 500)
    {
      vin_calibration = 500;
      jrk_sprintf(warnings,
        "Warning: The VIN calibration was too high "
        "so it will be changed to %d.\n", vin_calibration);
    }
    jrk_settings_set_vin_calibration(settings, vin_calibration);
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
        "so it will be changed to %u.\n",
        baud);
    }

    baud = jrk_settings_achievable_serial_baud_rate(settings, baud);
    jrk_settings_set_serial_baud_rate(settings, baud);
  }

  // TODO: fix invalid pin configurations here
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
