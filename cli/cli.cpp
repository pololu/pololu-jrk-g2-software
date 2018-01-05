#include "cli.h"

static const char help[] =
  CLI_NAME ": Pololu Jrk Command-line Utility\n"
  "Version " SOFTWARE_VERSION_STRING "\n"
  "Usage: " CLI_NAME " OPTIONS\n"
  "\n"
  "General options:\n"
  "  -s, --status                 Show device settings and info.\n"
  "  --full                       When used with --status, shows more.\n"
  "  -d SERIALNUMBER              Specifies the serial number of the device.\n"
  "  -l, --list                   List devices connected to computer.\n"
  "  --cmd-port                   Print the name of the command port.\n"
  "  --ttl-port                   Print the name of the TTL port.\n"
  "  --pause                      Pause program at the end.\n"
  "  --pause-on-error             Pause program at the end if an error happens.\n"
  "  -h, --help                   Show this help screen.\n"
  "\n"
  "Control commands:\n"
  "  --target NUM                 Set the target value (if input mode is Serial)\n"
  "  --speed NUM                  Set the target value to 2048 plus NUM\n"
  "  --override NUM TIMEOUT       Ignore PID and target, set duty cycle target\n"
  "                               to NUM for TIMEOUT PID periods.\n"
  "  --stop                       Stop the motor\n"
  "  --run                        Run the motor\n"
  "  --clear-errors               Clear latched errors\n"
  "\n"
  //"Temporary settings:\n"
  // TODO: stuff for setting PID variables
  // TODO: stuff for temporarily setting duty cycle
  //"\n"
  "Permanent settings:\n"
  "  --restore-defaults           Restore device's factory settings\n"
  "  --settings FILE              Load settings file into device.\n"
  "  --get-settings FILE          Read device settings and write to file.\n"
  "  --fix-settings IN OUT        Read settings from a file and fix them.\n"
  "\n"
  "For more help, see: " DOCUMENTATION_URL "\n"
  "\n";

// TODO: add --stream, like the old jrkcmd, for streaming variables

struct arguments
{
  bool show_status = false;

  bool full_output = false;

  bool serial_number_specified = false;
  std::string serial_number;

  bool show_list = false;

  bool show_cmd_port = false;

  bool show_ttl_port = false;

  bool pause = false;

  bool pause_on_error = false;

  bool show_help = false;

  bool set_target = false;
  uint16_t target;

  bool override_duty_cycle = false;
  int16_t override_duty_cycle_value;
  uint8_t override_duty_cycle_timeout;

  bool stop_motor = false;

  bool run_motor = false;

  bool clear_errors = false;

  bool restore_defaults = false;

  bool set_settings = false;
  std::string set_settings_filename;

  bool get_settings = false;
  std::string get_settings_filename;

  bool fix_settings = false;
  std::string fix_settings_input_filename;
  std::string fix_settings_output_filename;

  bool get_debug_data = false;

  uint32_t test_procedure = 0;

  bool action_specified() const
  {
    return show_status ||
      show_list ||
      show_cmd_port ||
      show_ttl_port ||
      show_help ||
      set_target ||
      override_duty_cycle ||
      stop_motor ||
      run_motor ||
      clear_errors ||
      restore_defaults ||
      set_settings ||
      get_settings ||
      fix_settings ||
      get_debug_data ||
      test_procedure;
  }
};

template <typename T>
static T parse_arg_int(arg_reader & arg_reader)
{
  const char * value_c = arg_reader.next();
  if (value_c == NULL)
  {
    throw exception_with_exit_code(EXIT_BAD_ARGS,
      "Expected a number after '" + std::string(arg_reader.last()) + "'.");
  }

  T result;
  uint8_t error = string_to_int(value_c, &result);
  if (error == STRING_TO_INT_ERR_SMALL)
  {
    throw exception_with_exit_code(EXIT_BAD_ARGS,
      "The number after '" + std::string(arg_reader.last()) + "' is too small.");
  }
  if (error == STRING_TO_INT_ERR_LARGE)
  {
    throw exception_with_exit_code(EXIT_BAD_ARGS,
      "The number after '" + std::string(arg_reader.last()) + "' is too large.");
  }
  if (error)
  {
    throw exception_with_exit_code(EXIT_BAD_ARGS,
      "The number after '" + std::string(arg_reader.last()) + "' is invalid.");
  }

  return result;
}

static std::string parse_arg_string(arg_reader & arg_reader)
{
    const char * value_c = arg_reader.next();
    if (value_c == NULL)
    {
      throw exception_with_exit_code(EXIT_BAD_ARGS,
        "Expected an argument after '" +
        std::string(arg_reader.last()) + "'.");
    }
    if (value_c[0] == 0)
    {
      throw exception_with_exit_code(EXIT_BAD_ARGS,
        "Expected a non-empty argument after '" +
        std::string(arg_reader.last()) + "'.");
    }
    return std::string(value_c);
}

static arguments parse_args(int argc, char ** argv)
{
  arg_reader arg_reader(argc, argv);
  arguments args;

  while (1)
  {
    const char * arg_c = arg_reader.next();
    if (arg_c == NULL)
    {
      break;  // Done reading arguments.
    }

    std::string arg = arg_c;

    if (arg == "-s" || arg == "--status")
    {
      args.show_status = true;
    }
    else if (arg == "--full")
    {
      args.full_output = true;
    }
    else if (arg == "-d" || arg == "--serial" || arg == "--device")
    {
      args.serial_number_specified = true;
      args.serial_number = parse_arg_string(arg_reader);

      // Remove a pound sign at the beginning of the string because people might
      // copy that from the GUI.
      if (args.serial_number[0] == '#')
      {
        args.serial_number.erase(0, 1);
      }
    }
    else if (arg == "--list" || arg == "-l")
    {
      args.show_list = true;
    }
    else if (arg == "--cmd-port" || arg == "--cmd-serial-port" ||
      arg == "--command-port" || arg == "--command-serial-port")
    {
      args.show_cmd_port = true;
    }
    else if (arg == "--ttl-port" || arg == "--ttl-serial-port")
    {
      args.show_ttl_port = true;
    }
    else if (arg == "--pause")
    {
      args.pause = true;
    }
    else if (arg == "--pause-on-error")
    {
      args.pause_on_error = true;
    }
    else if (arg == "-h" || arg == "--help" ||
      arg == "--h" || arg == "-help" || arg == "/help" || arg == "/h")
    {
      args.show_help = true;
    }
    else if (arg == "--target")
    {
      args.set_target = true;
      args.target = parse_arg_int<uint16_t>(arg_reader);
    }
    else if (arg == "--speed")
    {
      args.set_target = true;
      int32_t speed = parse_arg_int<int32_t>(arg_reader);
      if (speed < -2048) { args.target = 0; }
      else if (speed > 2047) { args.target = 4095; }
      else { args.target = 2048 + speed; }
    }
    else if (arg == "--override")
    {
      args.override_duty_cycle = true;
      args.override_duty_cycle_value = parse_arg_int<int16_t>(arg_reader);
      args.override_duty_cycle_timeout = parse_arg_int<uint8_t>(arg_reader);
    }
    else if (arg == "--stop" || arg == "--stop-motor" || arg == "--stopmotor"
      || arg == "--motor-off" || arg == "--motoroff")
    {
      args.stop_motor = true;
    }
    else if (arg == "--run" || arg == "--run-motor" || arg == "--runmotor")
    {
      args.run_motor = true;
    }
    else if (arg == "--clear-errors" || arg == "--clearerrors")
    {
      args.clear_errors = true;
    }
    else if (arg == "--restore-defaults" || arg == "--restoredefaults")
    {
      args.restore_defaults = true;
    }
    else if (arg == "--settings" || arg == "--set-settings" || arg == "--configure")
    {
      args.set_settings = true;
      args.set_settings_filename = parse_arg_string(arg_reader);
    }
    else if (arg == "--get-settings" || arg == "--getconf")
    {
      args.get_settings = true;
      args.get_settings_filename = parse_arg_string(arg_reader);
    }
    else if (arg == "--fix-settings")
    {
      args.fix_settings = true;
      args.fix_settings_input_filename = parse_arg_string(arg_reader);
      args.fix_settings_output_filename = parse_arg_string(arg_reader);
    }
    else if (arg == "--debug")
    {
      // This is an unadvertized option for helping customers troubleshoot
      // issues with their device.
      args.get_debug_data = true;
    }
    else
    {
      throw exception_with_exit_code(EXIT_BAD_ARGS,
        std::string("Unknown option: '") + arg + "'.");
    }
  }
  return args;
}

static jrk::handle handle(device_selector & selector)
{
  jrk::device device = selector.select_device();
  return jrk::handle(device);
}

static void print_list(device_selector & selector)
{
  for (const jrk::device & device : selector.list_devices())
  {
    std::cout << std::left << std::setfill(' ');
    std::cout << std::setw(17) << device.get_serial_number() + "," << " ";
    std::cout << std::setw(45) << jrk_look_up_product_name_ui(device.get_product());
    std::cout << std::endl;
  }
}

static void get_status(device_selector & selector, bool full_output)
{
  jrk::device device = selector.select_device();
  jrk::handle handle(device);

  // TODO: maybe we don't need the settings to print the status?
  jrk::settings settings = handle.get_settings();
  jrk::variables vars = handle.get_variables(false, true);

  std::string name = jrk_look_up_product_name_ui(device.get_product());
  std::string serial_number = device.get_serial_number();
  std::string firmware_version = handle.get_firmware_version_string();

  std::string cmd_port;
  try
  {
    cmd_port = device.get_cmd_port_name();
  }
  catch (const jrk::error &)
  {
    cmd_port = "?";
  }

  std::string ttl_port;
  try
  {
    ttl_port = device.get_ttl_port_name();
  }
  catch (const jrk::error &)
  {
    ttl_port = "?";
  }

  print_status(vars, settings, name, serial_number, firmware_version,
    cmd_port, ttl_port, full_output);
}

static void get_settings(device_selector & selector,
  const std::string & filename)
{
  jrk::settings settings = handle(selector).get_settings();

  std::string warnings;
  settings.fix(&warnings);
  std::cerr << warnings;

  std::string settings_string = settings.to_string();

  write_string_to_file_or_pipe(filename, settings_string);
}

static void set_settings(device_selector & selector,
  const std::string & filename)
{
  std::string settings_string = read_string_from_file_or_pipe(filename);
  jrk::settings settings = jrk::settings::read_from_string(settings_string);

  jrk::device device = selector.select_device();

  // Set the product of the settings object to match that of the device so we
  // can fix it properly.
  uint8_t product = device.get_product();
  jrk_settings_set_product(settings.get_pointer(), product);

  std::string warnings;
  settings.fix(&warnings);
  std::cerr << warnings;

  jrk::handle handle(device);
  handle.set_settings(settings);
  handle.reinitialize();
}

static void fix_settings(const std::string & input_filename,
  const std::string & output_filename)
{
  std::string in_str = read_string_from_file_or_pipe(input_filename);
  jrk::settings settings = jrk::settings::read_from_string(in_str);

  std::string warnings;
  settings.fix(&warnings);
  std::cerr << warnings;

  write_string_to_file_or_pipe(output_filename, settings.to_string());
}

static void print_debug_data(device_selector & selector)
{
  jrk::device device = selector.select_device();
  jrk::handle handle(device);

  std::vector<uint8_t> data(4096, 0);
  handle.get_debug_data(data);

  for (const uint8_t & byte : data)
  {
    std::cout << std::setfill('0') << std::setw(2) << std::hex
              << (unsigned int)byte << ' ';
  }
  std::cout << std::endl;
}

// A note about ordering: We want to do all the setting stuff first because it
// could affect subsequent options.  We want to show the status last, because it
// could be affected by options before it.
static void run(const arguments & args)
{
  if (args.show_help || !args.action_specified())
  {
    std::cout << help;
    return;
  }

  device_selector selector;
  if (args.serial_number_specified)
  {
    selector.specify_serial_number(args.serial_number);
  }

  if (args.show_list)
  {
    print_list(selector);
    return;
  }

  // Useful for connecting to the jrk from a script and useful for getting an
  // error message if the other displays of the port name are not working.
  if (args.show_cmd_port)
  {
    std::cout << selector.select_device().get_cmd_port_name() << std::endl;
  }

  if (args.show_ttl_port)
  {
    std::cout << selector.select_device().get_ttl_port_name() << std::endl;
  }

  if (args.fix_settings)
  {
    fix_settings(args.fix_settings_input_filename,
      args.fix_settings_output_filename);
  }

  if (args.get_settings)
  {
    get_settings(selector, args.get_settings_filename);
  }

  if (args.restore_defaults)
  {
    handle(selector).restore_defaults();
  }

  if (args.set_settings)
  {
    set_settings(selector, args.set_settings_filename);
  }

  if (args.clear_errors)
  {
    handle(selector).clear_errors();
  }

  if (args.set_target)
  {
    handle(selector).set_target(args.target);
  }

  if (args.override_duty_cycle)
  {
    handle(selector).override_duty_cycle(
      args.override_duty_cycle_value, args.override_duty_cycle_timeout);
  }

  if (args.run_motor)
  {
    handle(selector).run_motor();
  }

  if (args.stop_motor)
  {
    handle(selector).stop_motor();
  }

  if (args.get_debug_data)
  {
    print_debug_data(selector);
  }

  if (args.show_status)
  {
    get_status(selector, args.full_output);
  }
}

int main(int argc, char ** argv)
{
  int exit_code = 0;

  arguments args;
  try
  {
    args = parse_args(argc, argv);
    run(args);
  }
  catch (const exception_with_exit_code & error)
  {
    std::cerr << "Error: " << error.what() << std::endl;
    exit_code = error.get_code();
  }
  catch (const std::exception & error)
  {
    std::cerr << "Error: " << error.what() << std::endl;
    exit_code = EXIT_OPERATION_FAILED;
  }

  if (args.pause || (args.pause_on_error && exit_code))
  {
    std::cout << "Press enter to continue." << std::endl;
    char input;
    std::cin.get(input);
  }

  return exit_code;
}
