#include "jrk_internal.h"

#define PIN_COUNT 5

struct jrk_variables {
  uint8_t avoid_struct_size_zero;  // TODO: remove, put real variables here

  // Beginning of auto-generated variables struct members.

  uint16_t input;
  uint16_t target;
  uint16_t feedback;
  uint16_t scaled_feedback;
  int16_t error_sum;
  int16_t duty_cycle_target;
  int16_t duty_cycle;
  uint8_t current;
  bool pid_period_exceeded;
  uint16_t pid_period_count;
  uint16_t error_flags_halting;
  uint16_t error_flags_occurred;
  uint16_t vin_voltage;
  uint8_t device_reset;
  uint32_t up_time;
  uint16_t rc_pulse_width;
  uint16_t tachometer_reading;
  uint16_t current_high_res;
  uint16_t max_current;
  int16_t last_duty_cycle;
  uint8_t current_chopping_consecutive_count;
  uint8_t current_chopping_occurrence_count;

  // End of auto-generated variables struct members.

  struct {
    uint16_t analog_reading;
    bool digital_reading;
    uint8_t pin_state;
  } pin_info[JRK_CONTROL_PIN_COUNT];
};

jrk_error * jrk_variables_create(jrk_variables ** variables)
{
  if (variables == NULL)
  {
    return jrk_error_create("Variables output pointer is null.");
  }

  *variables = NULL;

  jrk_error * error = NULL;

  jrk_variables * new_variables = NULL;
  if (error == NULL)
  {
    new_variables = (jrk_variables *)calloc(1, sizeof(jrk_variables));
    if (new_variables == NULL) { error = &jrk_error_no_memory; }
  }

  if (error == NULL)
  {
    *variables = new_variables;
    new_variables = NULL;
  }

  jrk_variables_free(new_variables);

  return error;
}

jrk_error * jrk_variables_copy(const jrk_variables * source, jrk_variables ** dest)
{
  if (dest == NULL)
  {
    return jrk_error_create("Variables output pointer is null.");
  }

  *dest = NULL;

  if (source == NULL)
  {
    return NULL;
  }

  jrk_error * error = NULL;

  jrk_variables * new_variables = NULL;
  if (error == NULL)
  {
    new_variables = (jrk_variables *)calloc(1, sizeof(jrk_variables));
    if (new_variables == NULL) { error = &jrk_error_no_memory; }
  }

  if (error == NULL)
  {
    memcpy(new_variables, source, sizeof(jrk_variables));
  }

  if (error == NULL)
  {
    *dest = new_variables;
    new_variables = NULL;
  }

  jrk_variables_free(new_variables);

  return error;
}

void jrk_variables_free(jrk_variables * variables)
{
  free(variables);
}

static void write_buffer_to_variables(const uint8_t * buf, jrk_variables * vars)
{
  assert(vars != NULL);
  assert(buf != NULL);

  // Beginning of auto-generated buffer-to-variables code.

  vars->input = read_uint16_t(buf + JRK_VAR_INPUT);
  vars->target = read_uint16_t(buf + JRK_VAR_TARGET);
  vars->feedback = read_uint16_t(buf + JRK_VAR_FEEDBACK);
  vars->scaled_feedback = read_uint16_t(buf + JRK_VAR_SCALED_FEEDBACK);
  vars->error_sum = read_int16_t(buf + JRK_VAR_ERROR_SUM);
  vars->duty_cycle_target = read_int16_t(buf + JRK_VAR_DUTY_CYCLE_TARGET);
  vars->duty_cycle = read_int16_t(buf + JRK_VAR_DUTY_CYCLE);
  vars->current = buf[JRK_VAR_CURRENT];
  vars->pid_period_exceeded = buf[JRK_VAR_PID_PERIOD_EXCEEDED] & 1;
  vars->pid_period_count = read_uint16_t(buf + JRK_VAR_PID_PERIOD_COUNT);
  vars->error_flags_halting = read_uint16_t(buf + JRK_VAR_ERROR_FLAGS_HALTING);
  vars->error_flags_occurred = read_uint16_t(buf + JRK_VAR_ERROR_FLAGS_OCCURRED);
  vars->vin_voltage = read_uint16_t(buf + JRK_VAR_VIN_VOLTAGE);
  vars->device_reset = buf[JRK_VAR_DEVICE_RESET];
  vars->up_time = read_uint32_t(buf + JRK_VAR_UP_TIME);
  vars->rc_pulse_width = read_uint16_t(buf + JRK_VAR_RC_PULSE_WIDTH);
  vars->tachometer_reading = read_uint16_t(buf + JRK_VAR_TACHOMETER_READING);
  vars->current_high_res = read_uint16_t(buf + JRK_VAR_CURRENT_HIGH_RES);
  vars->max_current = read_uint16_t(buf + JRK_VAR_MAX_CURRENT);
  vars->last_duty_cycle = read_int16_t(buf + JRK_VAR_LAST_DUTY_CYCLE);
  vars->current_chopping_consecutive_count = buf[JRK_VAR_CURRENT_CHOPPING_CONSECUTIVE_COUNT];
  vars->current_chopping_occurrence_count = buf[JRK_VAR_CURRENT_CHOPPING_OCCURRENCE_COUNT];

  // End of auto-generated buffer-to-variables code.

  {
    // Digital readings.
    uint8_t d = buf[JRK_VAR_DIGITAL_READINGS];
    vars->pin_info[JRK_PIN_NUM_SCL].digital_reading = d >> JRK_PIN_NUM_SCL & 1;
    vars->pin_info[JRK_PIN_NUM_SDA].digital_reading = d >> JRK_PIN_NUM_SDA & 1;
    vars->pin_info[JRK_PIN_NUM_TX].digital_reading = d >> JRK_PIN_NUM_TX & 1;
    vars->pin_info[JRK_PIN_NUM_RX].digital_reading = d >> JRK_PIN_NUM_RX & 1;
    vars->pin_info[JRK_PIN_NUM_RC].digital_reading = d >> JRK_PIN_NUM_RC & 1;
    vars->pin_info[JRK_PIN_NUM_AUX].digital_reading = d >> JRK_PIN_NUM_AUX & 1;
    vars->pin_info[JRK_PIN_NUM_FBA].digital_reading = d >> JRK_PIN_NUM_FBA & 1;
    vars->pin_info[JRK_PIN_NUM_FBT].digital_reading = d >> JRK_PIN_NUM_FBT & 1;
  }

  {
    vars->pin_info[JRK_PIN_NUM_SCL].analog_reading =
      read_uint16_t(buf + JRK_VAR_ANALOG_READING_SCL);

    vars->pin_info[JRK_PIN_NUM_SDA].analog_reading =
      read_uint16_t(buf + JRK_VAR_ANALOG_READING_SDA);

    vars->pin_info[JRK_PIN_NUM_TX].analog_reading =
      read_uint16_t(buf + JRK_VAR_ANALOG_READING_TX);

    vars->pin_info[JRK_PIN_NUM_RX].analog_reading =
      read_uint16_t(buf + JRK_VAR_ANALOG_READING_RX);

    vars->pin_info[JRK_PIN_NUM_RC].analog_reading = 0xFFFF;

    vars->pin_info[JRK_PIN_NUM_AUX].analog_reading =
      read_uint16_t(buf + JRK_VAR_ANALOG_READING_AUX);

    vars->pin_info[JRK_PIN_NUM_FBA].analog_reading =
      read_uint16_t(buf + JRK_VAR_ANALOG_READING_FBA);

    vars->pin_info[JRK_PIN_NUM_FBT].analog_reading = 0xFFFF;
  }

  {
    uint16_t s = read_uint16_t(buf + JRK_VAR_PIN_STATES);
    vars->pin_info[JRK_PIN_NUM_SCL].pin_state = s >> (JRK_PIN_NUM_SCL * 2) & 3;
    vars->pin_info[JRK_PIN_NUM_SDA].pin_state = s >> (JRK_PIN_NUM_SDA * 2) & 3;
    vars->pin_info[JRK_PIN_NUM_TX].pin_state = s >> (JRK_PIN_NUM_TX * 2) & 3;
    vars->pin_info[JRK_PIN_NUM_RX].pin_state = s >> (JRK_PIN_NUM_RX * 2) & 3;
    vars->pin_info[JRK_PIN_NUM_RC].pin_state = s >> (JRK_PIN_NUM_RC * 2) & 3;
    vars->pin_info[JRK_PIN_NUM_AUX].pin_state = s >> (JRK_PIN_NUM_AUX * 2) & 3;
    vars->pin_info[JRK_PIN_NUM_FBA].pin_state = s >> (JRK_PIN_NUM_FBA * 2) & 3;
    vars->pin_info[JRK_PIN_NUM_FBT].pin_state = s >> (JRK_PIN_NUM_FBT * 2) & 3;
  }
}

jrk_error * jrk_get_variables(jrk_handle * handle, jrk_variables ** variables,
  uint16_t flags)
{
  if (variables == NULL)
  {
    return jrk_error_create("Variables output pointer is null.");
  }

  *variables = NULL;

  if (handle == NULL)
  {
    return jrk_error_create("Handle is null.");
  }

  jrk_error * error = NULL;

  // Create a variables object.
  jrk_variables * new_variables = NULL;
  if (error == NULL)
  {
    error = jrk_variables_create(&new_variables);
  }

  // Read all the variables from the device.
  uint8_t buf[JRK_VARIABLES_SIZE];
  if (error == NULL)
  {
    size_t index = 0;
    error = jrk_get_variable_segment(handle, index, sizeof(buf), buf, flags);
  }

  // Store the variables in the new variables object.
  if (error == NULL)
  {
    write_buffer_to_variables(buf, new_variables);
  }

  // Pass the new variables to the caller.
  if (error == NULL)
  {
    *variables = new_variables;
    new_variables = NULL;
  }

  jrk_variables_free(new_variables);

  if (error != NULL)
  {
    error = jrk_error_add(error,
      "There was an error reading variables from the device.");
  }

  return error;
}

// Beginning of auto-generated variables getters.

uint16_t jrk_variables_get_input(const jrk_variables * vars)
{
  if (vars == NULL) { return 0; }
  return vars->input;
}

uint16_t jrk_variables_get_target(const jrk_variables * vars)
{
  if (vars == NULL) { return 0; }
  return vars->target;
}

uint16_t jrk_variables_get_feedback(const jrk_variables * vars)
{
  if (vars == NULL) { return 0; }
  return vars->feedback;
}

uint16_t jrk_variables_get_scaled_feedback(const jrk_variables * vars)
{
  if (vars == NULL) { return 0; }
  return vars->scaled_feedback;
}

int16_t jrk_variables_get_error_sum(const jrk_variables * vars)
{
  if (vars == NULL) { return 0; }
  return vars->error_sum;
}

int16_t jrk_variables_get_duty_cycle_target(const jrk_variables * vars)
{
  if (vars == NULL) { return 0; }
  return vars->duty_cycle_target;
}

int16_t jrk_variables_get_duty_cycle(const jrk_variables * vars)
{
  if (vars == NULL) { return 0; }
  return vars->duty_cycle;
}

uint8_t jrk_variables_get_current(const jrk_variables * vars)
{
  if (vars == NULL) { return 0; }
  return vars->current;
}

bool jrk_variables_get_pid_period_exceeded(const jrk_variables * vars)
{
  if (vars == NULL) { return 0; }
  return vars->pid_period_exceeded;
}

uint16_t jrk_variables_get_pid_period_count(const jrk_variables * vars)
{
  if (vars == NULL) { return 0; }
  return vars->pid_period_count;
}

uint16_t jrk_variables_get_error_flags_halting(const jrk_variables * vars)
{
  if (vars == NULL) { return 0; }
  return vars->error_flags_halting;
}

uint16_t jrk_variables_get_error_flags_occurred(const jrk_variables * vars)
{
  if (vars == NULL) { return 0; }
  return vars->error_flags_occurred;
}

uint16_t jrk_variables_get_vin_voltage(const jrk_variables * vars)
{
  if (vars == NULL) { return 0; }
  return vars->vin_voltage;
}

uint8_t jrk_variables_get_device_reset(const jrk_variables * vars)
{
  if (vars == NULL) { return 0; }
  return vars->device_reset;
}

uint32_t jrk_variables_get_up_time(const jrk_variables * vars)
{
  if (vars == NULL) { return 0; }
  return vars->up_time;
}

uint16_t jrk_variables_get_rc_pulse_width(const jrk_variables * vars)
{
  if (vars == NULL) { return 0; }
  return vars->rc_pulse_width;
}

uint16_t jrk_variables_get_tachometer_reading(const jrk_variables * vars)
{
  if (vars == NULL) { return 0; }
  return vars->tachometer_reading;
}

uint16_t jrk_variables_get_current_high_res(const jrk_variables * vars)
{
  if (vars == NULL) { return 0; }
  return vars->current_high_res;
}

uint16_t jrk_variables_get_max_current(const jrk_variables * vars)
{
  if (vars == NULL) { return 0; }
  return vars->max_current;
}

int16_t jrk_variables_get_last_duty_cycle(const jrk_variables * vars)
{
  if (vars == NULL) { return 0; }
  return vars->last_duty_cycle;
}

uint8_t jrk_variables_get_current_chopping_consecutive_count(const jrk_variables * vars)
{
  if (vars == NULL) { return 0; }
  return vars->current_chopping_consecutive_count;
}

uint8_t jrk_variables_get_current_chopping_occurrence_count(const jrk_variables * vars)
{
  if (vars == NULL) { return 0; }
  return vars->current_chopping_occurrence_count;
}

// End of auto-generated variables getters.

int16_t jrk_variables_get_error(const jrk_variables * vars)
{
  if (vars == NULL) { return 0; }
  return vars->scaled_feedback - vars->target;
}

uint16_t jrk_variables_get_raw_current_mv(const jrk_variables * vars)
{
  return jrk_variables_get_current_high_res(vars) >> 4;
}

int32_t jrk_variables_get_scaled_current_mv(const jrk_variables * vars, int32_t offset_mv)
{
  int16_t duty_cycle = jrk_variables_get_duty_cycle(vars);
  uint16_t raw_current_mv = jrk_variables_get_raw_current_mv(vars);

  // We can't divide by the duty cycle if it's zero.
  if (duty_cycle == 0) { return 0; }

  // Disallow negative offsets for now because it could cause overflows below,
  // and does not make sense.
  if (offset_mv < 0) { offset_mv = 0; }

  // Subtract the offset, making sure we don't make the raw current negative.
  if (raw_current_mv < offset_mv)
  {
    raw_current_mv = 0;
  }
  else
  {
    raw_current_mv -= offset_mv;
  }

  // Divide by the duty cycle as a number between 0 and 1.  The multiplication
  // cannot overflow because 0xFFFF * 600 is expressible as an int32_t.
  return (int32_t)raw_current_mv * 600 / duty_cycle;
}

uint16_t jrk_variables_get_analog_reading(const jrk_variables * variables,
  uint8_t pin)
{
  if (variables == NULL || pin >= JRK_CONTROL_PIN_COUNT) { return 0xFFFF; }
  return variables->pin_info[pin].analog_reading;
}

bool jrk_variables_get_digital_reading(const jrk_variables * variables,
  uint8_t pin)
{
  if (variables == NULL || pin >= JRK_CONTROL_PIN_COUNT) { return 0; }
  return variables->pin_info[pin].digital_reading;
}

uint8_t jrk_variables_get_pin_state(const jrk_variables * variables,
  uint8_t pin)
{
  if (variables == NULL || pin >= PIN_COUNT) { return 0; }
  return variables->pin_info[pin].pin_state;
}
