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
    name: 'error_sum',
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
    name: 'current',
    type: :uint8_t,
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
    name: 'current_high_res',
    type: :uint16_t,
  },
  {
    name: 'current_chopping_log',
    type: :uint16_t,
  },
]
