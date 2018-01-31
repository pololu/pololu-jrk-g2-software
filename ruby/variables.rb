Variables = [
  {
    name: 'input',
    type: :uint16_t,
  },
  {
    name: 'target',
    type: :uint16_t,
  },
  {
    name: 'feedback',
    type: :uint16_t,
  },
  {
    name: 'scaled_feedback',
    type: :uint16_t,
  },
  {
    name: 'integral',
    type: :int16_t,
  },
  {
    name: 'duty_cycle_target',
    type: :int16_t,
  },
  {
    name: 'duty_cycle',
    type: :int16_t,
  },
  {
    name: 'current_low_res',
    type: :uint8_t,
    comment: <<EOF
This is the most-significant 8 bits of the 'current' variable returned
jrk_variables_get_current().

For most applications it is better to use jrk_calculate_measured_current_ma()
because it will always return the current in units of milliamps.
EOF
  },
  {
    name: 'pid_period_exceeded',
    type: :bool,
  },
  {
    name: 'pid_period_count',
    type: :uint16_t,
  },
  {
    name: 'error_flags_halting',
    type: :uint16_t,
  },
  {
    name: 'error_flags_occurred',
    type: :uint16_t,
  },
  {
    name: 'vin_voltage',
    type: :uint16_t,
  },
  {
    name: 'current',
    type: :uint16_t,
    comment: <<EOF
This is the measured current as calculated by the firmware.

For the umc04a jrk models, this is in units of milliamps.

For most applications it is better to use jrk_calculate_measured_current_ma()
because it will always return the current in units of milliamps.  This
function might return different units when used on different models of jrks
in the future.
EOF
  },
  {
    name: 'device_reset',
    type: :uint8_t,
  },
  {
    name: 'up_time',
    type: :uint32_t,
  },
  {
    name: 'rc_pulse_width',
    type: :uint16_t,
  },
  {
    name: 'tachometer_reading',
    type: :uint16_t,
  },
  {
    name: 'raw_current',
    type: :uint16_t,
    comment: <<EOF
This is an analog voltage reading from the motor driver's current sense pin.
EOF
  },
  {
    name: 'current_limit_code',
    type: :uint16_t,
  },
  {
    name: 'last_duty_cycle',
    type: :int16_t,
  },
  {
    name: 'current_chopping_consecutive_count',
    type: :uint8_t,
  },
  {
    name: 'current_chopping_occurrence_count',
    type: :uint8_t,
  },
]
