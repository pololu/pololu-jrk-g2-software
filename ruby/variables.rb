Variables = [
  {
    name: 'input',
    type: :uint16_t,
    comment: <<END
The input variable is a raw, unscaled value representing a measurement taken
by the Jrk of the input to the system.  In serial input mode, the input is
equal to the target, which can be set to any value from 0 to 4095 using
serial commands.  In analog input mode, the input is a measurement the
voltage on the RX pin, where 0 is 0 V and 4092 is a voltage equal the Jrk's
5V pin (approximately 4.8 V).  In pulse width input mode, the input is the
duration of the last pulse measured, in units of 2/3 us.
END
  },
  {
    name: 'target',
    type: :uint16_t,
    comment: <<END
In serial input mode, the target is set directly with serial commands.  In
the other input modes, the target is computed by scaling the input, using the
configurable input scaling settings.
END
  },
  {
    name: 'feedback',
    type: :uint16_t,
    comment: <<END
The feedback variable is a raw, unscaled feedback value, representing a
measurement taken by the Jrk of the output of the system.  In analog mode,
the feedback is a measurement of the voltage on the FBA pin, where 0 is 0 V
and 4092 is a voltage equal to the Jrk's 5V pin (approximately 4.8 V).  In
frequency feedback mode, the feedback is 2048 plus or minus a measurement of
the frequency of pulses on the FBT pin.  In feedback mode none (open-loop
speed control mode), the feedback is always zero.
END
  },
  {
    name: 'scaled_feedback',
    type: :uint16_t,
    comment: <<END
The scaled feedback is calculated the feedback using the Jrk's configurable
feedback scaling settings.
END
  },
  {
    name: 'integral',
    type: :int16_t,
    comment: <<END
In general, every PID period, the error (scaled feedback minus target) is
added to the integral (also known as error sum).  There are several settings
to configure the behavior of this variable, and it is used in the PID
calculation.
END
  },
  {
    name: 'duty_cycle_target',
    type: :int16_t,
    comment: <<END

In general, this is the duty cycle that the jrk is trying to achieve.  A
value of -600 or less means full speed reverse, while a value of 600 or more
means full speed forward.  A value of 0 means stopped (braking or coasting).
In no feedback mode (open-loop speed control mode), the duty cycle target is
normally the target minus 2048. In other feedback modes, the duty cycle
target is normally the sum of the proportional, integral, and derivative
terms of the PID algorithm.  In any mode, the duty cycle target can be
overridden with a Force Duty Cycle Target command.

If an error is stopping the motor, the duty cycle target variable will not be
affected, but the duty cycle variable will change/decelerate to zero.
END
  },
  {
    name: 'duty_cycle',
    type: :int16_t,
    comment: <<END
The duty cycle variable is the duty cycle at which the jrk is currently
driving the motor.  A value of -600 means full speed reverse, while a
value of 600 means full speed forward.  A value of 0 means stopped
(braking or coasting).  The duty cycle could be different from the duty
cycle target because it normally takes into account the Jrk's configurable
motor limits and errors.  The duty cycle can be overridden with a Force
Duty Cycle command.
END
  },
  {
    name: 'current_low_res',
    type: :uint8_t,
    comment: <<END
This is the most-significant 8 bits of the 'current' variable returned by
jrk_variables_get_current().

The Jrk G2 has this variable mainly to be compatible with older Jrk models.
In new applications, we recommend using jrk_variables_get_current(), which
provides a higher-resolution measurement.
END
  },
  {
    name: 'pid_period_exceeded',
    type: :bool,
    comment: <<END
This variable is true if the Jrk's most recent PID cycle took more time than
the configured PID period.  This indicates that the Jrk does not have time to
perform all of its tasks at the desired rate.  Most often, this is caused by
the requested number of analog samples for input, feedback, or current
sensing being too high for the configured PID period.
END
  },
  {
    name: 'pid_period_count',
    type: :uint16_t,
    comment: <<END
This is the number of PID periods that have elapsed.  It resets to 0 after
reaching 65535.  The duration of the PID period can be configured.
END
  },
  {
    name: 'error_flags_halting',
    type: :uint16_t,
    comment: <<END
This variable indicates which errors are currently stopping the motor.

Each bit in the variable represents a different error.  The bits are defined
by the JRK_ERROR_* macros.
END
  },
  {
    name: 'error_flags_occurred',
    type: :uint16_t,
    comment: <<END
This variable indicates which errors have occurred since the last time the
variable was cleared.

Like error flags halting, each bit in the variable represents a different
error.  The bits are defined by the JRK_ERROR_* macros.
END
  },
  {
    name: 'vin_voltage',
    type: :uint16_t,
    comment: <<END
This is a measurement of the VIN voltage, in millivolts.
END
  },
  {
    name: 'current',
    type: :uint16_t,
    comment: <<EOF
This is the Jrk's measurement of the current running through the motor, in
milliamps.
EOF
  },
  {
    name: 'device_reset',
    type: :uint8_t,
    comment: <<EOF
This is the cause of the Jrk's last full microcontroller reset.

It should be equal to one of the JRK_RESET_* macros.
EOF
  },
  {
    name: 'up_time',
    type: :uint32_t,
    comment: <<EOF
This is the time since the last full reset of the Jrk's microcontroller, in
milliseconds.
EOF
  },
  {
    name: 'rc_pulse_width',
    type: :uint16_t,
    comment: <<EOF
This is the raw RC pulse width measured on the Jrk's RC input, in units of
twelfths of a microsecond.

Returns 0 if the RC input is missing or invalid.
EOF
  },
  {
    name: 'fbt_reading',
    type: :uint16_t,
    comment: <<EOF
This is the raw pulse rate or pulse width measured on the Jrk's FBT
(tachometer feedback) pin.

In pulse counting mode, this will be the number of pulses on the FBT pin seen
in the last N PID periods, where N is the "Pulse samples" setting.

In pulse timing mode, this will be a measurement of the width of pulses on
the FBT pin.  This measurement is affected by several configurable settings.
EOF
  },
  {
    name: 'raw_current',
    type: :uint16_t,
    comment: <<EOF
This is an analog voltage reading from the Jrk's current sense pin.  The
units of the reading depend on what hard current limit is being used
(jrk_variable_get_encoded_hard_current_limit()).
EOF
  },
  {
    name: 'encoded_hard_current_limit',
    type: :uint16_t,
    comment: <<EOF
This is the encoded value representing the hardware current limit the jrk is
currently using.
EOF
  },
  {
    name: 'last_duty_cycle',
    type: :int16_t,
    comment: <<EOF
This is the duty cycle the Jrk drove the motor with during the last PID
period.

This can be useful for converting the raw current reading into milliamps.
EOF
  },
  {
    name: 'current_chopping_consecutive_count',
    type: :uint8_t,
    comment: <<EOF
This is the number of consecutive PID periods during which current chopping
due to the hard current limit has been active.
EOF
  },
  {
    name: 'current_chopping_occurrence_count',
    type: :uint8_t,
    comment: <<EOF
This is the number of PID periods during which current chopping due to the
hard current limit has been active, since the last time the variable was
cleared.
EOF
  },
]
