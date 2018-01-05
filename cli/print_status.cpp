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

  (void)settings;  // TODO: either use this for real or remove it

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

  std::cout << left_column << "RC pulse width: "
    << vars.get_rc_pulse_width()
    << std::endl;

  std::cout << left_column << "Tachometer reading: "
    << vars.get_tachometer_reading()
    << std::endl;

  std::cout << left_column << "VIN voltage: "
            << convert_mv_to_v_string(vars.get_vin_voltage(), full_output)
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

  std::cout << left_column << "Error sum: "
    << vars.get_error_sum()
    << std::endl;

  std::cout << left_column << "Duty cycle target: "
    << vars.get_duty_cycle_target()
    << std::endl;

  std::cout << left_column << "Duty cycle: "
    << vars.get_duty_cycle()
    << std::endl;

  std::cout << left_column << "Current: "
    << vars.get_current_high_res()  // TODO: properly format this in mA or A
    << std::endl;

  std::cout << left_column << "Current chopping log: "
    << vars.get_current_chopping_log()
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

  // TODO: print analog readings from pins and the pin states
}
