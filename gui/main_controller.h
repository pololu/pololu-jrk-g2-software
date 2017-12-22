#pragma once

#include "jrk.hpp"

class main_window;

class main_controller
{
public:
  // Stores a pointer to the window so we can update the window.
  void set_window(main_window *);

  // This is called when the program starts up.
  void start();

  // This is called when the user issues a connect command.
  void connect_device_with_os_id(std::string const & id);

  // This is called when the user issues a disconnect command. Returns true
  // on completion or false if the user cancels when prompted about settings
  // that have not been applied.
  bool disconnect_device();

  // This is called when the user issues a command to reload settings from the
  // device.
  void reload_settings(bool ask = true);

  // This is called when the user wants to restore the device to its default
  // settings.
  void restore_default_settings();

  // This is called when the user wants to upgrade some firmware.
  void upgrade_firmware();

  // This is called regularly to do various updates.
  void update();

  // This is called when the user tries to exit the program.  Returns true if
  // the program is actually allowed to exit.
  bool exit();

  // This is called whenever something in the model has changed that might
  // require the window to be updated.  It includes no details about what
  // exactly changed.
  void handle_model_changed();  // TODO: make this private

private:
  void connect_device(jrk::device const & device);
  void disconnect_device_by_error(std::string const & error_message);
  void really_disconnect();
  void set_connection_error(std::string const & error_message);

  // Returns true for success, false for failure.
  bool update_device_list();

  // True if device_list changed the last time update_device_list() was
  // called.
  bool device_list_changed;

  void show_exception(std::exception const & e, std::string const & context = "");

public:
  // This is called when the user wants to apply the settings.
  void apply_settings();

  void stop_motor();
  void run_motor();
  void set_target(uint16_t);

  void open_settings_from_file(std::string filename);
  void save_settings_to_file(std::string filename);

  // These are called when the user changes a setting.
  // [all-settings]

  void handle_serial_baud_rate_input(uint32_t serial_baud_rate);
  void handle_serial_baud_rate_input_finished();
  void handle_serial_device_number_input(uint8_t serial_device_number);
  void handle_serial_crc_enabled_input(bool serial_crc_enabled);
  void handle_command_timeout_input(uint16_t command_timeout);

  void handle_input_mode_input(uint8_t input_mode);
  void handle_input_analog_samples_input(uint8_t input_analog_samples);
  void handle_input_detect_disconnect_input(bool detect_disconnect);
  void handle_input_invert_input(bool input_invert);
  void handle_input_absolute_minimum_input(uint16_t input_absolute_minimum);
  void handle_input_absolute_maximum_input(uint16_t input_absolute_maximum);
  void handle_input_minimum_input(uint16_t input_minimum);
  void handle_input_maximum_input(uint16_t input_maximum);
  void handle_input_neutral_minimum_input(uint16_t input_neutral_minimum);
  void handle_input_neutral_maximum_input(uint16_t input_neutral_maximum);
  void handle_output_minimum_input(uint16_t output_minimum);
  void handle_output_neutral_input(uint16_t output_neutral);
  void handle_output_maximum_input(uint16_t output_maximum);
  void handle_input_scaling_degree_input(uint8_t input_scaling_degree);

  void handle_feedback_mode_input(uint8_t value);

  void handle_invert_motor_input(bool invert_motor);
  void handle_motor_pwm_frequency_input(uint8_t);

  void handle_motor_max_duty_cycle_forward_input(uint16_t);
  void handle_motor_max_duty_cycle_reverse_input(uint16_t);
  void handle_motor_max_acceleration_forward_input(uint16_t);
  void handle_motor_max_acceleration_reverse_input(uint16_t);
  void handle_motor_max_deceleration_forward_input(uint16_t);
  void handle_motor_max_deceleration_reverse_input(uint16_t);
  void handle_motor_max_current_forward_input(uint16_t);
  void handle_motor_max_current_reverse_input(uint16_t);

  void handle_motor_coast_when_off(bool);

  void handle_never_sleep_input(bool never_sleep);
  void handle_vin_calibration_input(int16_t vin_calibration);

  void handle_upload_complete();

private:
  // This is called whenever it is possible that we have connected to a
  // different device.
  void handle_device_changed();
  void handle_variables_changed();
  void handle_settings_changed();
  void handle_settings_applied();

  void initialize_input_scaling();
  // void update_motor_status_message(bool prompt_to_resume);

  // Holds a list of the relevant devices that are connected to the computer.
  std::vector<jrk::device> device_list;

  // Holds an open handle to a device or a null handle if we are not connected.
  jrk::handle device_handle;

  // True if the last connection or connection attempt resulted in an error.  If
  // true, connection_error_essage provides some information about the error.
  bool connection_error = false;
  std::string connection_error_message;

  // True if we are disconnected now and the last connection was terminated by
  // the user.
  bool disconnected_by_user = false;

  // Holds a working copy of the settings from the device, including any
  // unapplied changes.
  jrk::settings settings;

  // Holds a cached copy of the settings from the device, without any unapplied
  // changes.
  jrk::settings cached_settings;

  // True if the working settings have been modified by user and could be
  // different from what is cached and on the device.
  bool settings_modified = false;

  // Holds the variables/status of the device.
  jrk::variables variables;

  // True if the last attempt to update the variables failed (typically due
  // to a USB error).
  bool variables_update_failed = false;

  // The number of updates to wait for before updating the
  // device list again (saves CPU time).
  uint32_t update_device_list_counter = 1;

  void reload_variables();

  // Returns true if we are currently connected to a device.
  bool connected() const { return device_handle; }

  bool input_mode_is_analog() const;
  bool input_mode_is_pulse_width() const;

  main_window * window;
};
