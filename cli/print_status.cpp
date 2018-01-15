#include "cli.h"

static const int left_column_width = 30;
static auto left_column = std::setw(left_column_width);

static std::string pretty_up_time(uint32_t up_time)
{
  std::ostringstream ss;
  uint32_t seconds = up_time / 1000;
  uint32_t minutes = seconds / 60;
  uint16_t hours = minutes / 60;

  ss << hours <<
    ":" << std::setfill('0') << std::setw(2) << minutes % 60 <<
    ":" << std::setfill('0') << std::setw(2) << seconds % 60;
  return ss.str();
}

static std::string convert_mv_to_v_string(uint32_t mv, bool full_output)
{
  std::ostringstream ss;

  if (full_output)
  {
    ss << (mv / 1000) << "." << (mv / 100 % 10) << (mv / 10 % 10) << (mv % 10);
  }
  else
  {
    uint32_t dv = (mv + 50) / 100;
    ss << (dv / 10) << "." << (dv % 10);
  }
  ss << " V";
  return ss.str();
}

static void print_errors(uint32_t errors, const char * error_set_name)
{
  if (!errors)
  {
    std::cout << error_set_name << ": None" << std::endl;
    return;
  }

  std::cout << error_set_name << ":" << std::endl;
  for (uint32_t i = 0; i < 32; i++)
  {
    uint32_t error = (1 << i);
    if (errors & error)
    {
      std::cout << "  - " << jrk_look_up_error_name_ui(error) << std::endl;
    }
  }
}

static std::string analog_reading_format(uint16_t reading)
{
  if (reading == 0xFFFF) { return std::string("N/A"); }
  return std::to_string(reading);
}

static void print_pin_info(const jrk::variables & vars,
  uint8_t pin, const char * pin_name)
{
  std::cout << pin_name << " pin:" << std::endl;

  if (pin != JRK_PIN_NUM_RC)
  {
    std::cout << left_column << "  State: "
      << jrk_look_up_pin_state_name_ui(vars.get_pin_state(pin))
      << std::endl;
  }

  if (pin != JRK_PIN_NUM_RC && pin != JRK_PIN_NUM_FBT)
  {
    std::cout << left_column << "  Analog reading: "
      << analog_reading_format(vars.get_analog_reading(pin)) << std::endl;
  }

  std::cout << left_column << "  Digital reading: "
    << vars.get_digital_reading(pin) << std::endl;
}

void print_status(const jrk::variables & vars,
  const jrk::overridable_settings & osettings,
  const jrk::settings & settings,
  const std::string & name,
  const std::string & serial_number,
  const std::string & firmware_version,
  const std::string & cmd_port,
  const std::string & ttl_port,
  bool full_output)
{
  // The output here is YAML so that people can more easily write scripts that
  // use it.

  std::cout << std::left << std::setfill(' ');

  std::cout << left_column << "Name: "
    << name << std::endl;

  std::cout << left_column << "Serial number: "
    << serial_number << std::endl;

  std::cout << left_column << "Firmware version: "
    << firmware_version << std::endl;

  std::cout << left_column << "Command port: "
    << cmd_port << std::endl;

  std::cout << left_column << "TTL port: "
    << ttl_port << std::endl;

  std::cout << left_column << "Last reset: "
    << jrk_look_up_device_reset_name_ui(vars.get_device_reset())
    << std::endl;

  std::cout << left_column << "Up time: "
    << pretty_up_time(vars.get_up_time())
    << std::endl;

  std::cout << std::endl;

  std::cout << left_column << "Input: "
    << vars.get_input()
    << std::endl;

  std::cout << left_column << "Target: "
    << vars.get_target()
    << std::endl;

  std::cout << left_column << "Feedback: "
    << vars.get_feedback()
    << std::endl;

  std::cout << left_column << "Scaled feedback: "
    << vars.get_scaled_feedback()
    << std::endl;

  std::cout << left_column << "Error: "
    << vars.get_error()
    << std::endl;

  std::cout << left_column << "Integral: "
    << vars.get_integral()
    << std::endl;

  std::cout << left_column << "Duty cycle target: "
    << vars.get_duty_cycle_target()
    << std::endl;

  std::cout << left_column << "Duty cycle: "
    << vars.get_duty_cycle()
    << std::endl;

  std::cout << left_column << "Current (mA): "
    << jrk::calculate_measured_current_ma(settings, vars)
    << std::endl;

  std::cout << left_column << "VIN voltage: "
    << convert_mv_to_v_string(vars.get_vin_voltage(), full_output)
    << std::endl;

  std::cout << left_column << "PID period exceeded: "
    << (vars.get_pid_period_exceeded() ? "Yes" : "No")
    << std::endl;

  std::cout << left_column << "PID period count: "
    << vars.get_pid_period_count()
    << std::endl;

  std::cout << std::endl;

  print_errors(vars.get_error_flags_halting(),
    "Errors currently stopping the motor");
  print_errors(vars.get_error_flags_occurred(),
    "Errors that occurred since last check");
  std::cout << std::endl;

  if (full_output)
  {
    // Overwritable variables

    std::cout << left_column
      << "Reset integral when proportional term exceeds max duty cycle: "
      << (osettings.get_reset_integral() ? "Yes" : "No")
      << std::endl;

    std::cout << left_column << "Coast when motor is off: "
      << (osettings.get_coast_when_off() ? "Yes" : "No")
      << std::endl;

    std::cout << left_column << "Proportional multiplier: "
      << osettings.get_proportional_multiplier()
      << std::endl;

    std::cout << left_column << "Proportional exponent: "
      << (uint32_t)osettings.get_proportional_exponent()
      << std::endl;

    std::cout << left_column << "Integral multiplier: "
      << osettings.get_integral_multiplier()
      << std::endl;

    std::cout << left_column << "Integral exponent: "
      << (uint32_t)osettings.get_integral_exponent()
      << std::endl;

    std::cout << left_column << "Derivative multiplier: "
      << osettings.get_derivative_multiplier()
      << std::endl;

    std::cout << left_column << "Derivative exponent: "
      << (uint32_t)osettings.get_derivative_exponent()
      << std::endl;

    std::cout << left_column << "PID period: "
      << osettings.get_pid_period()
      << " ms" << std::endl;

    std::cout << left_column << "Integral limit: "
      << osettings.get_integral_limit()
      << std::endl;

    std::cout << left_column << "Max. duty cycle while feedback is out of range: "
      << osettings.get_max_duty_cycle_while_feedback_out_of_range()
      << std::endl;

    std::cout << left_column << "Max. duty cycle forward: "
      << osettings.get_max_duty_cycle_forward()
      << std::endl;

    std::cout << left_column << "Max. duty cycle reverse: "
      << osettings.get_max_duty_cycle_reverse()
      << std::endl;

    std::cout << left_column << "Max. acceleration forward: "
      << osettings.get_max_acceleration_forward()
      << std::endl;

    std::cout << left_column << "Max. acceleration reverse: "
      << osettings.get_max_acceleration_reverse()
      << std::endl;

    std::cout << left_column << "Max. deceleration forward: "
      << osettings.get_max_deceleration_forward()
      << std::endl;

    std::cout << left_column << "Max. deceleration reverse: "
      << osettings.get_max_deceleration_reverse()
      << std::endl;

    // TODO: fix the current readings below; they are 0 because we marked the
    // current settings as custom_eeprom.

    // TODO: format in amps
    std::cout << left_column << "Current limit forward (mA): "
      << jrk::current_limit_code_to_ma(settings,
        osettings.get_current_limit_code_forward())
      << std::endl;

    // TODO: format in amps
    std::cout << left_column << "Current limit reverse (mA): "
      << jrk::current_limit_code_to_ma(settings,
        osettings.get_current_limit_code_reverse())
      << std::endl;

    std::cout << left_column << "Current limit forward (code): "
      << (uint32_t)osettings.get_current_limit_code_forward()
      << std::endl;

    std::cout << left_column << "Current limit reverse (code): "
      << (uint32_t)osettings.get_current_limit_code_reverse()
      << std::endl;

    std::cout << left_column << "Brake duration forward: "
      << osettings.get_brake_duration_forward()
      << " ms" << std::endl;

    std::cout << left_column << "Brake duration reverse: "
      << osettings.get_brake_duration_reverse()
      << " ms" << std::endl;

    std::cout << std::endl;
  }

  if (full_output)
  {
    std::cout << left_column << "RC pulse width: "
      << vars.get_rc_pulse_width()
      << std::endl;

    std::cout << left_column << "Tachometer reading: "
      << vars.get_tachometer_reading()
      << std::endl;

    std::cout << left_column << "Last duty cycle: "
      << vars.get_last_duty_cycle()
      << std::endl;

    // TODO: format in amps
    std::cout << left_column << "Current limit: "
      << "code " << vars.get_current_limit_code()
      << std::endl;

    std::cout << left_column << "Current chopping consecutive count: "
      << (uint32_t)vars.get_current_chopping_consecutive_count()
      << std::endl;

    std::cout << left_column << "Current chopping occurrence count: "
      << (uint32_t)vars.get_current_chopping_occurrence_count()
      << std::endl;

    std::cout << std::endl;
  }

  if (full_output)
  {
    print_pin_info(vars, JRK_PIN_NUM_SCL, "SCL");
    print_pin_info(vars, JRK_PIN_NUM_SDA, "SDA");
    print_pin_info(vars, JRK_PIN_NUM_TX, "TX");
    print_pin_info(vars, JRK_PIN_NUM_RX, "RX");
    print_pin_info(vars, JRK_PIN_NUM_RC, "RC");
    print_pin_info(vars, JRK_PIN_NUM_AUX, "AUX");
    print_pin_info(vars, JRK_PIN_NUM_FBA, "FBA");
    print_pin_info(vars, JRK_PIN_NUM_FBT, "FBT");
  }
}
