#include "cli.h"

static const int left_column_width = 30;
static auto left_column = std::setw(left_column_width);

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

  if (pin == JRK_PIN_NUM_SDA || pin == JRK_PIN_NUM_FBA)
  {
    std::cout << left_column << "  Analog reading: "
      << analog_reading_format(vars.get_analog_reading(pin)) << std::endl;
  }

  std::cout << left_column << "  Digital reading: "
    << vars.get_digital_reading(pin) << std::endl;
}

void print_status(const jrk::variables & vars,
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
    << convert_up_time_to_string(vars.get_up_time())
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

  if (full_output)
  {
    std::cout << left_column << "Last duty cycle: "
      << vars.get_last_duty_cycle()
      << std::endl;
  }

  std::cout << left_column << "Current: "
    << vars.get_current() << " mA"
    << std::endl;

  if (full_output)
  {
    std::cout << left_column << "Raw current: "
      << jrk::calculate_raw_current_mv64(settings, vars) / 64 << " mV"
      << std::endl;

    std::cout << left_column << "Hard current limit: "
      << convert_current_limit_ma_to_string(
         jrk::current_limit_decode(settings,
           vars.get_encoded_hard_current_limit()))
      << std::endl;

    std::cout << left_column << "Encoded hard current limit: "
      << vars.get_encoded_hard_current_limit()
      << std::endl;

    std::cout << "Current chopping:" << std::endl;
    std::cout << left_column << "  Consecutive count: "
      << (uint32_t)vars.get_current_chopping_consecutive_count()
      << std::endl;

    std::cout << left_column << "  Occurrence count: "
      << (uint32_t)vars.get_current_chopping_occurrence_count()
      << std::endl;

    std::cout << left_column << "RC pulse width: "
      << vars.get_rc_pulse_width()
      << std::endl;

    std::cout << left_column << "FBT reading: "
      << vars.get_fbt_reading()
      << std::endl;
  }

  std::cout << left_column << "VIN voltage: "
    << convert_mv_to_v_string(vars.get_vin_voltage(), full_output)
    << std::endl;

  std::cout << left_column << "PID period exceeded: "
    << (vars.get_pid_period_exceeded() ? "Yes" : "No")
    << std::endl;

  std::cout << left_column << "PID period count: "
    << vars.get_pid_period_count()
    << std::endl;

  if (full_output)
  {
    std::cout << left_column << "Force mode: "
      << jrk_look_up_force_mode_name_ui(vars.get_force_mode())
      << std::endl;
  }

  std::cout << std::endl;

  print_errors(vars.get_error_flags_halting(),
    "Errors currently stopping the motor");
  print_errors(vars.get_error_flags_occurred(),
    "Errors that occurred since last check");
  std::cout << std::endl;

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
    std::cout << std::endl;
  }

  std::string diagnosis;
  try
  {
    diagnosis = jrk::diagnose(settings, vars);
  }
  catch (const jrk::error & error)
  {
    diagnosis = "Error: " + error.message();
  }

  // This will be a long line, so don't make it longer using 'left_column'.
  // Use quotes so that YAML parsers don't complain about special characters
  // like colons.
  std::cout << "Overall status: \'" << diagnosis << "\'" << std::endl;
}
