#pragma once

#include "qcustomplot.h"
#include "graph_widget.h"
#include "graph_window.h"
#include "elided_label.h"
#include "pid_constant_control.h"
#include "nice_spin_box.h"
#include "popout_window.h"

#include "jrk.hpp"

#include <QMainWindow>
#include <QGroupBox>
#include <QButtonGroup>

#include <array>

class QAction;
class QCloseEvent;
class QComboBox;
class QDoubleSpinBox;
class QGridLayout;
class QGroupBox;
class QHBoxLayout;
class QLineEdit;
class QMenu;
class QRadioButton;
class QShortcut;
class QShowEvent;
class QSpacerItem;
class QSpinBox;

class pid_constant_control;
class pid_constant_validator;
class main_controller;
class target_slider;

class main_window : public QMainWindow
{
  Q_OBJECT

  struct error_row
  {
    unsigned int count = 0;
    QLabel * bit_mask_label = NULL;
    QLabel * error_label = NULL;
    QRadioButton * disabled_radio = NULL;
    QRadioButton * enabled_radio = NULL;
    QRadioButton * latched_radio = NULL;
    QCheckBox * error_hard;
    QLabel * stopping_value = NULL;
    QLabel * count_value = NULL;
    QWidget * frame = NULL;
    QButtonGroup * error_enable_group = NULL;
    bool always_enabled = false;
    bool always_latched = false;
    bool always_hard = false;
    int error_number = 0;
  };

public:
  main_window(QWidget * parent = 0);
  void set_controller(main_controller * controller);

  // This causes the window to call the controller's update() function
  // periodically, on the same thread as everything else.
  //
  // interval_ms is the amount of time between updates, in milliseconds.
  void set_update_timer_interval(uint32_t interval_ms);
  void start_update_timer();

  // Show an OK/Cancel dialog, return true if the user selects OK.
  bool confirm(const std::string & question);

  void show_error_message(const std::string & message);
  void show_info_message(const std::string & message);
  void show_warning_message(const std::string & message);

  void open_bootloader_window();

  void set_device_name(const std::string & name, bool link_enabled);
  void set_serial_number(const std::string & serial_number);
  void set_firmware_version(const std::string & firmware_version);
  void set_cmd_port(const std::string & cmd_port);
  void set_ttl_port(const std::string & ttl_port);
  void set_device_reset(const std::string & device_reset);
  void set_up_time(uint32_t);
  void set_input(uint16_t, uint8_t);
  void set_target(uint16_t);
  void set_feedback(uint16_t, uint8_t);
  void set_scaled_feedback(uint16_t);
  void set_feedback_not_applicable();
  void set_error(int16_t);
  void set_integral(int16_t);
  void set_duty_cycle_target(int16_t);
  void set_duty_cycle(int16_t);
  void set_raw_current_mv64(uint32_t);
  void set_current(int32_t);
  void set_current_chopping_now(bool);
  void set_current_chopping_count(uint32_t);
  void set_vin_voltage(uint16_t);
  void set_pid_period_count(uint16_t);
  void set_pid_period_exceeded(bool);

  void set_device_list_contents(const std::vector<jrk::device> & device_list);
  void set_device_list_selected(const jrk::device & device);

  // Sets the label that shows the connection status/error.
  void set_connection_status(const std::string & status, bool error);

  void adjust_ui_for_product(uint32_t product);

  void set_manual_target_mode(uint8_t input_mode, uint8_t feedback_mode);
  void set_manual_target_inputs(uint16_t target);
  uint16_t get_manual_target_numeric_input();

  // Controls whether the apply settings action/button is enabled or
  // disabled.
  void set_apply_settings_enabled(bool enabled);

  void set_motor_status_message(const std::string & message,
    uint16_t error_flag = 0);

  void set_apply_settings_button_stylesheet(int offset);
  void animate_apply_settings_button();

  void set_input_mode(uint8_t input_mode);
  void set_input_invert(bool input_invert);
  void set_input_analog_samples_exponent(uint8_t value);
  void set_input_detect_disconnect(bool input_detect_disconnect);
  void set_input_serial_mode(uint8_t value);
  void set_input_baud_rate(uint32_t value);
  void set_input_enable_crc(bool enabled);
  void set_input_device_number(uint16_t value);
  void set_input_enable_device_number(bool enabled);
  void set_serial_timeout(uint32_t value);
  void set_input_compact_protocol(bool enabled);  // TODO: fix this name and the others around here
  void set_input_error_minimum(uint16_t input_error_minimum);
  void set_input_error_maximum(uint16_t input_error_maximum);
  void set_input_minimum(uint16_t input_minimum);
  void set_input_maximum(uint16_t input_maximum);
  void set_input_neutral_minimum(uint16_t input_neutral_minimum);
  void set_input_neutral_maximum(uint16_t input_neutral_maximum);
  void set_input_output_minimum(uint16_t input_output_minimum);
  void set_input_output_neutral(uint16_t input_output_neutral);
  void set_input_output_maximum(uint16_t input_output_maximum);
  void set_input_scaling_degree(uint8_t input_scaling_degree);
  void update_deadband_label();
  void update_input_scaling_order_warning_label();
  void update_input_tab_enables();

  void set_feedback_mode(uint8_t feedback_mode);
  void set_feedback_invert(bool feedback_invert);
  void set_feedback_error_minimum(uint16_t value);
  void set_feedback_error_maximum(uint16_t value);
  void set_feedback_minimum(uint16_t value);
  void set_feedback_maximum(uint16_t value);
  void update_feedback_scaling_order_warning_label();
  void set_feedback_analog_samples_exponent(uint8_t value);
  void set_feedback_detect_disconnect(bool value);
  void set_feedback_wraparound(bool value);
  void set_fbt_method(uint8_t);
  void set_fbt_timing_clock(uint8_t);
  void set_fbt_timing_polarity(bool);
  void set_fbt_timing_timeout(uint16_t);
  void set_fbt_samples(uint8_t);
  void set_fbt_divider_exponent(uint8_t);
  void set_fbt_range_display(const std::string & message, bool invalid);
  void update_feedback_related_enables();

  void set_pid_proportional(uint16_t multiplier, uint8_t exponent);
  void set_pid_integral(uint16_t multiplier, uint8_t exponent);
  void set_pid_derivative(uint16_t multiplier, uint8_t exponent);
  void set_pid_period(uint16_t value);
  void set_integral_limit(uint16_t value);
  void set_integral_divider_exponent(uint8_t exponent);
  void set_reset_integral(bool enabled);
  void set_feedback_dead_zone(uint8_t value);

  void set_pwm_frequency(uint8_t pwm_frequency);
  void set_motor_invert(bool enabled);
  void set_motor_asymmetric(bool checked);
  void set_max_duty_cycle_forward(uint16_t);
  void set_max_duty_cycle_reverse(uint16_t);
  void set_max_acceleration_forward(uint16_t);
  void set_max_acceleration_reverse(uint16_t);
  void set_max_deceleration_forward(uint16_t);
  void set_max_deceleration_reverse(uint16_t);
  void set_brake_duration_forward(uint32_t);
  void set_brake_duration_reverse(uint32_t);
  void set_encoded_hard_current_limit_code_forward(uint16_t);
  void set_encoded_hard_current_limit_code_reverse(uint16_t);
  void update_hard_current_limit_controls(uint32_t product);
  void set_soft_current_limit_forward(uint16_t);
  void set_soft_current_limit_reverse(uint16_t);
  void set_current_offset_calibration(int16_t);
  void set_current_scale_calibration(int16_t);
  void set_current_samples_exponent(uint8_t);
  void set_overcurrent_threshold(uint8_t);
  void set_max_duty_cycle_while_feedback_out_of_range(uint16_t);
  void set_coast_when_off(bool value);

  void set_error_enable(uint16_t enable, uint16_t latch);
  void set_error_hard(uint16_t hard);
  void set_error_flags_halting(uint16_t error_flags_halting);
  void increment_errors_occurred(uint16_t errors_occurred);
  void reset_error_counts();

  void set_disable_i2c_pullups(bool enabled);
  void set_analog_sda_pullup(bool enabled);
  void set_always_analog_sda(bool enabled);
  void set_always_analog_fba(bool enabled);
  void set_never_sleep(bool enabled);
  void set_vin_calibration(int16_t vin_calibration);

  void set_serial_baud_rate(uint32_t serial_baud_rate);
  void set_serial_device_number(uint8_t serial_device_number);

  void set_tab_pages_enabled(bool enabled);
  void set_open_save_settings_enabled(bool enabled);
  void set_disconnect_enabled(bool enabled);
  void set_reload_settings_enabled(bool enabled);
  void set_restore_defaults_enabled(bool enabled);
  void set_stop_motor_enabled(bool enabled);
  void set_run_motor_enabled(bool enabled);

  bool motor_asymmetric_checked();

  void update_graph(uint32_t up_time);

  void reset_graph();

  bool suppress_events = false;
  main_controller * controller;

private:
  // Helper method for setting the index of a combo box, given the desired
  // uint8_t item value. Sets index of -1 for no selection if the specified
  // value is not found.
  void set_u8_combobox(QComboBox * combo, uint8_t value);
  void set_spin_box(QSpinBox * box, int value);
  void set_double_spin_box(QDoubleSpinBox * spin, double value);
  void set_check_box(QCheckBox * check, bool value);

  void center_at_startup_if_needed();

signals:
  void input_changed(uint16_t);
  void feedback_changed(uint16_t);
  void duty_cycle_changed(int16_t);
  void controller_updated();

protected:
  // void context_menu_event(QContextMenuEvent *event);

  // This is called by Qt just before the window is shown for the first time,
  // and is also called whenever the window becomes unminimized.
  void showEvent(QShowEvent *) override;

  // This is called by Qt when the "close" slot is triggered, meaning that
  // the user wants to close the window.
  void closeEvent(QCloseEvent *) override;

  bool eventFilter(QObject *, QEvent *) override;

private slots:
  void on_update_timer_timeout();
  void restore_graph_preview();
  void open_graph_window();
  void on_device_name_value_linkActivated();
  void on_documentation_action_triggered();
  void on_about_action_triggered();
  void on_device_list_value_currentIndexChanged(int index);
  void on_apply_settings_action_triggered();
  void on_upgrade_firmware_action_triggered();
  void upgrade_firmware_complete();
  void on_run_motor_action_triggered();
  void on_stop_motor_action_triggered();
  void on_clear_current_chopping_count_action_triggered();
  void on_set_target_button_clicked();
  void on_center_target_button_clicked();
  void on_auto_set_target_check_stateChanged(int state);
  void on_manual_target_slider_valueChanged(int value);
  void on_manual_target_entry_value_valueChanged(int value);
  void on_manual_target_return_key_shortcut_activated();
  void on_open_settings_action_triggered();
  void on_save_settings_action_triggered();
  void on_disconnect_action_triggered();
  void on_reload_settings_action_triggered();
  void on_restore_defaults_action_triggered();

  void on_input_mode_combobox_currentIndexChanged(int index);
  void on_input_analog_samples_combobox_currentIndexChanged(int index);
  void on_input_invert_checkbox_stateChanged(int state);
  void on_input_detect_disconnect_checkbox_stateChanged(int state);
  void on_input_serial_mode_button_group_buttonToggled(int id, bool checked);
  void on_input_uart_fixed_baud_spinbox_valueChanged(int value);
  void on_input_enable_crc_checkbox_stateChanged(int state);
  void on_input_device_spinbox_valueChanged(int value);
  void on_input_device_number_checkbox_stateChanged(int state);
  void on_input_timeout_spinbox_valueChanged(double value);
  void on_input_disable_compact_protocol_checkbox_stateChanged(int state);
  void on_input_error_minimum_spinbox_valueChanged(int value);
  void on_input_error_maximum_spinbox_valueChanged(int value);
  void on_input_minimum_spinbox_valueChanged(int value);
  void on_input_maximum_spinbox_valueChanged(int value);
  void on_input_neutral_minimum_spinbox_valueChanged(int value);
  void on_input_neutral_maximum_spinbox_valueChanged(int value);
  void on_input_output_minimum_spinbox_valueChanged(int value);
  void on_input_output_neutral_spinbox_valueChanged(int value);
  void on_input_output_maximum_spinbox_valueChanged(int value);
  void on_input_scaling_degree_combobox_currentIndexChanged(int index);
  void on_input_reset_range_button_clicked();
  void on_input_wizard_button_clicked();

  void on_feedback_mode_combobox_currentIndexChanged(int index);
  void on_feedback_invert_checkbox_stateChanged(int state);
  void on_feedback_error_minimum_spinbox_valueChanged(int value);
  void on_feedback_error_maximum_spinbox_valueChanged(int value);
  void on_feedback_maximum_spinbox_valueChanged(int value);
  void on_feedback_minimum_spinbox_valueChanged(int value);
  void on_feedback_reset_range_button_clicked();
  void on_feedback_analog_samples_combobox_currentIndexChanged(int index);
  void on_feedback_detect_disconnect_checkbox_stateChanged(int state);
  void on_feedback_wraparound_checkbox_stateChanged(int state);
  void on_fbt_method_combobox_currentIndexChanged(int index);
  void on_fbt_timing_clock_combobox_currentIndexChanged(int index);
  void on_fbt_timing_polarity_combobox_currentIndexChanged(int index);
  void on_fbt_timing_timeout_spinbox_valueChanged(int value);
  void on_fbt_samples_spinbox_valueChanged(int value);
  void on_fbt_divider_combobox_currentIndexChanged(int index);
  void on_feedback_wizard_button_clicked();

  void on_pid_proportional_control_values_changed(int multiplier, int exponent);
  void on_pid_integral_control_values_changed(int multiplier, int exponent);
  void on_pid_derivative_control_values_changed(int multiplier, int exponent);

  void on_pid_period_spinbox_valueChanged(int value);
  void on_integral_limit_spinbox_valueChanged(int value);
  void on_integral_divider_combobox_currentIndexChanged(int value);
  void on_reset_integral_checkbox_stateChanged(int state);
  void on_feedback_dead_zone_spinbox_valueChanged(int value);

  void on_pwm_frequency_combobox_currentIndexChanged(int index);
  void on_motor_invert_checkbox_stateChanged(int state);
  void on_motor_asymmetric_checkbox_stateChanged(int state);
  void on_max_duty_cycle_forward_spinbox_valueChanged(int value);
  void on_max_duty_cycle_reverse_spinbox_valueChanged(int value);
  void on_max_acceleration_forward_spinbox_valueChanged(int value);
  void on_max_acceleration_reverse_spinbox_valueChanged(int value);
  void on_max_deceleration_forward_spinbox_valueChanged(int value);
  void on_max_deceleration_reverse_spinbox_valueChanged(int value);
  void on_brake_duration_forward_spinbox_valueChanged(int value);
  void on_brake_duration_reverse_spinbox_valueChanged(int value);
  void on_hard_current_limit_forward_spinbox_valueChanged(int value);
  void on_hard_current_limit_reverse_spinbox_valueChanged(int value);
  void on_soft_current_limit_forward_spinbox_valueChanged(int value);
  void on_soft_current_limit_reverse_spinbox_valueChanged(int value);
  void on_current_offset_calibration_spinbox_valueChanged(int value);
  void on_current_scale_calibration_spinbox_valueChanged(int value);
  void on_current_samples_combobox_currentIndexChanged(int value);
  void on_overcurrent_threshold_spinbox_valueChanged(int value);
  void on_max_duty_cycle_while_feedback_out_of_range_spinbox_valueChanged(int value);
  void on_coast_when_off_button_group_buttonToggled(int id, bool checked);

  void error_enable_group_buttonToggled(int id, int index);
  void error_hard_stateChanged(int state, int index);
  void on_errors_clear_errors_clicked();
  void on_errors_reset_counts_clicked();

  void on_disable_i2c_pullups_stateChanged(int state);
  void on_analog_sda_pullup_stateChanged(int state);
  void on_always_analog_sda_stateChanged(int state);
  void on_always_analog_fba_stateChanged(int state);
  void on_never_sleep_checkbox_stateChanged(int state);
  void on_vin_calibration_value_valueChanged(int value);

private:
  void setup_ui();
  void setup_style_sheet();
  void setup_menu_bar();

  QSpacerItem * setup_vertical_spacer();

  QWidget * setup_status_tab();
  QWidget * setup_graph();
  QWidget * setup_variables_box();
  QWidget * setup_manual_target_box();

  QWidget * setup_input_tab();
  QWidget * setup_input_analog_groupbox();
  QWidget * setup_input_serial_groupbox();
  QWidget * setup_input_scaling_groupbox();

  QWidget * setup_feedback_tab();
  QWidget * setup_feedback_scaling_groupbox();
  QWidget * setup_feedback_analog_groupbox();
  QWidget * setup_feedback_fbt_groupbox();

  QWidget * setup_pid_tab();

  QWidget * setup_motor_tab();

  QWidget * setup_errors_tab();
  void setup_error_row(int error_number,
    bool always_enabled, bool always_latched, bool always_hard);

  QWidget * setup_advanced_tab();
  QWidget * setup_pin_configuration_groupbox();
  QWidget * setup_advanced_miscellaneous_groupbox();

  QWidget * setup_info_tab();

  QTimer *update_timer = NULL;

  QWidget * central_widget;
  QVBoxLayout * main_window_layout;
  graph_widget * graph;
  graph_window * graph_wind;
  QFrame * graph_preview_frame;
  QHBoxLayout * graph_preview_frame_layout;

  QMenuBar * menu_bar;
  QMenu * file_menu;
  QAction * open_settings_action;
  QAction * save_settings_action;
  QAction * exit_action;
  QMenu * device_menu;
  QAction * disconnect_action;
  QAction * stop_motor_action;
  QAction * run_motor_action;
  QAction * clear_current_chopping_count_action;
  QAction * reload_settings_action;
  QAction * restore_defaults_action;
  QAction * apply_settings_action;
  QAction * upgrade_firmware_action;
  QMenu * window_menu;
  QAction * graph_action;
  QAction * variables_window_action;
  QMenu * help_menu;
  QAction * documentation_action;
  QAction * about_action;

  QHBoxLayout * header_layout;
  QLabel * device_list_label;
  QComboBox * device_list_value;
  QLabel * connection_status_value;
  elided_label * motor_status_value;

  QTabWidget *tab_widget;

  // status tab

  QWidget * status_page_widget;
  QGroupBox * variables_box;
  popout_window * variables_window;
  QGridLayout * variables_layout;
  QLabel * up_time_label;
  QLabel * up_time_value;
  QLabel * input_label;
  QLabel * input_value;
  QLabel * target_label;
  QLabel * target_value;
  QLabel * feedback_label;
  QLabel * feedback_value;
  QLabel * scaled_feedback_label;
  QLabel * scaled_feedback_value;
  QLabel * error_label;
  QLabel * error_value;
  QLabel * integral_label;
  QLabel * integral_value;
  QLabel * duty_cycle_target_label;
  QLabel * duty_cycle_target_value;
  QLabel * duty_cycle_label;
  QLabel * duty_cycle_value;
  QLabel * current_label;
  QLabel * current_value;
  QLabel * raw_current_label;
  QLabel * raw_current_value;
  QLabel * scaled_current_label;
  QLabel * scaled_current_value;
  QLabel * current_chopping_count_label;
  QLabel * current_chopping_count_value;
  QLabel * vin_voltage_label;
  QLabel * vin_voltage_value;
  QLabel * pid_period_count_label;
  QLabel * pid_period_count_value;
  QLabel * pid_period_exceeded_label;
  QLabel * pid_period_exceeded_value;
  QLabel * error_flags_halting_label;
  QLabel * error_flags_halting_value;

  // Manually set target box

  bool manual_target_enabled = false;
  QGroupBox * manual_target_box;
  target_slider * manual_target_slider;
  QLabel * manual_target_min_label;
  QLabel * manual_target_max_label;
  QSpinBox * manual_target_entry_value;
  QPushButton * set_target_button;
  QPushButton * center_target_button;
  QCheckBox * auto_set_target_check;
  QShortcut * manual_target_return_key_shortcut;
  QShortcut * manual_target_enter_key_shortcut;

  // input tab

  QWidget *input_page_widget;
  QGridLayout *input_page_layout;
  QLabel *input_mode_label;
  QComboBox *input_mode_combobox;

  // input tab "Analog input" groupbox

  QGroupBox *input_analog_groupbox;
  QLabel *input_analog_samples_label;
  QCheckBox *input_detect_disconnect_checkbox;
  QComboBox *input_analog_samples_combobox;

  // input tab "Scaling" groupbox

  QGroupBox * input_scaling_groupbox;
  QPushButton * input_wizard_button;
  QPushButton * input_reset_range_button;
  QLabel * input_scaling_order_warning_label;
  QLabel * deadband_label;
  QLabel * input_error_max_label;
  QSpinBox * input_error_maximum_spinbox;
  QLabel * input_maximum_label;
  QSpinBox * input_maximum_spinbox;
  QLabel * input_neutral_max_label;
  QSpinBox * input_neutral_maximum_spinbox;
  QLabel * input_neutral_min_label;
  QSpinBox * input_neutral_minimum_spinbox;
  QLabel * input_minimum_label;
  QSpinBox * input_minimum_spinbox;
  QLabel * input_error_min_label;
  QSpinBox * input_error_minimum_spinbox;
  QLabel * input_degree_label;
  QComboBox * input_scaling_degree_combobox;
  QCheckBox * input_invert_checkbox;
  QLabel * input_input_label;
  QLabel * input_target_label;
  QSpinBox * input_output_maximum_spinbox;
  QSpinBox * input_output_neutral_spinbox;
  QSpinBox * input_output_minimum_spinbox;

  // input tab "Serial interface" groupbox

  QGroupBox *input_serial_groupbox;
  QButtonGroup *input_serial_mode_button_group;
  QRadioButton *input_usb_dual_port_radio;
  QRadioButton *input_usb_chained_radio;
  QLabel *input_device_label;
  QSpinBox *input_device_spinbox;
  QSpinBox *input_uart_fixed_baud_spinbox;;
  QCheckBox *input_enable_crc_checkbox;
  QCheckBox *input_device_number_checkbox;
  QLabel *input_timeout_label;  // TODO: rename to serial_timeout_label
  QRadioButton *input_uart_fixed_baud_radio;
  QDoubleSpinBox *input_timeout_spinbox;  // TODO: rename to serial_timeout_spinbox
  QCheckBox *input_disable_compact_protocol_checkbox;

  // feedback tab

  QWidget *feedback_page_widget;
  QGridLayout *feedback_page_layout;
  QLabel *feedback_mode_label;
  QComboBox *feedback_mode_combobox;

  // feedback tab "Scaling (Analog and Tachometer mode only)" groupbox

  QGroupBox * feedback_scaling_groupbox;
  QPushButton * feedback_wizard_button;
  QPushButton * feedback_reset_range_button;
  QCheckBox * feedback_invert_checkbox;
  QLabel * feedback_error_max_label;
  QLabel * feedback_maximum_label;
  QLabel * feedback_minimum_label;
  QLabel * feedback_error_min_label;
  QLabel * feedback_calibration_label;
  QLabel * feedback_scaling_order_warning_label;
  QSpinBox * feedback_error_maximum_spinbox;
  QSpinBox * feedback_maximum_spinbox;
  QSpinBox * feedback_minimum_spinbox;
  QSpinBox * feedback_error_minimum_spinbox;

  // feedback tab "Analog to digital conversion" groupbox

  QGroupBox * feedback_analog_groupbox;
  QLabel * feedback_analog_samples_label;
  QComboBox * feedback_analog_samples_combobox;
  QCheckBox * feedback_detect_disconnect_checkbox;
  QCheckBox * feedback_wraparound_checkbox;

  // feedback tab "FBT options" groupbox
  QComboBox * fbt_method_combobox;
  QLabel * fbt_timing_clock_label;
  QComboBox * fbt_timing_clock_combobox;
  QLabel * fbt_timing_polarity_label;
  QComboBox * fbt_timing_polarity_combobox;
  QLabel * fbt_timing_timeout_label;
  QSpinBox * fbt_timing_timeout_spinbox;
  QSpinBox * fbt_samples_spinbox;
  QLabel * fbt_divider_label;
  QComboBox * fbt_divider_combobox;
  QLabel * fbt_range_label;

  // pid tab

  QWidget * pid_page_widget;
  QLabel * pid_period_label;
  QSpinBox * pid_period_spinbox;
  QLabel * integral_limit_label;
  QSpinBox * integral_limit_spinbox;
  QLabel * integral_divider_label;
  QComboBox * integral_divider_combobox;
  QCheckBox * reset_integral_checkbox;
  QLabel * feedback_dead_zone_label;
  QSpinBox * feedback_dead_zone_spinbox;

  // pid tab constant controls
  pid_constant_control * pid_proportional_control;
  pid_constant_control * pid_integral_control;
  pid_constant_control * pid_derivative_control;

  // motor tab

  QWidget * motor_page_widget;
  QVBoxLayout * motor_page_layout;
  QButtonGroup * coast_when_off_button_group;
  QLabel * pwm_frequency_label;
  QComboBox * pwm_frequency_combobox;
  QCheckBox * motor_invert_checkbox;
  QPushButton * detect_motor_button;
  QCheckBox * motor_asymmetric_checkbox;
  QLabel * motor_forward_label;
  QLabel * motor_reverse_label;
  QLabel * max_duty_cycle_label;
  QSpinBox * max_duty_cycle_forward_spinbox;
  QSpinBox * max_duty_cycle_reverse_spinbox;
  QLabel * max_duty_cycle_means_label;
  QLabel * max_acceleration_label;
  QSpinBox * max_acceleration_forward_spinbox;
  QSpinBox * max_acceleration_reverse_spinbox;
  QLabel * max_acceleration_means_label;
  QLabel * max_deceleration_label;
  QSpinBox * max_deceleration_forward_spinbox;
  QSpinBox * max_deceleration_reverse_spinbox;
  QLabel * max_deceleration_means_label;
  QLabel * brake_duration_label;
  QSpinBox * brake_duration_forward_spinbox;
  QSpinBox * brake_duration_reverse_spinbox;
  QLabel * hard_current_limit_label;
  nice_spin_box * hard_current_limit_forward_spinbox;
  nice_spin_box * hard_current_limit_reverse_spinbox;
  QLabel * hard_current_limit_means_label;
  QLabel * soft_current_limit_label;
  nice_spin_box * soft_current_limit_forward_spinbox;
  nice_spin_box * soft_current_limit_reverse_spinbox;
  QLabel * soft_current_limit_means_label;
  QLabel * current_offset_calibration_label;
  QSpinBox * current_offset_calibration_spinbox;
  QLabel * current_scale_calibration_label;
  QSpinBox * current_scale_calibration_spinbox;
  QLabel * current_samples_label;
  QComboBox * current_samples_combobox;
  QLabel * overcurrent_threshold_label;
  QSpinBox * overcurrent_threshold_spinbox;
  QLabel * max_duty_cycle_while_feedback_out_of_range_label;
  QSpinBox * max_duty_cycle_while_feedback_out_of_range_spinbox;
  QLabel * max_duty_cycle_while_feedback_out_of_range_means_label;
  QLabel * motor_off_label;
  QRadioButton * motor_brake_radio;
  QRadioButton * motor_coast_radio;

  // errors tab

  QWidget * errors_page_widget;
  QGridLayout * errors_page_layout;
  QLabel * errors_bit_mask_label;
  QLabel * errors_error_label;
  QLabel * errors_setting_label;
  QLabel * errors_hard_label;
  QLabel * errors_stopping_motor_label;
  QLabel * errors_occurrence_count_label;
  QPushButton * errors_clear_errors;
  QPushButton * errors_reset_counts;
  QWidget * new_error_row;
  QList<error_row> error_rows;

  // footer

  QHBoxLayout * footer_layout;
  QPushButton * stop_motor_button;
  QPushButton * run_motor_button;
  QLabel * apply_settings_label;
  QPushButton * apply_settings_button;
  uint32_t apply_settings_animation_count = 0;

  // advanced tab

  QWidget * advanced_page_widget;

  QGroupBox * pin_configuration_groupbox;
  QCheckBox * disable_i2c_pullups;
  QCheckBox * analog_sda_pullup;
  QCheckBox * always_analog_sda;
  QCheckBox * always_analog_fba;

  QGroupBox * advanced_miscellaneous_groupbox;
  QCheckBox * never_sleep_checkbox;
  QSpinBox * vin_calibration_value;

  // info tab

  QLabel * device_name_label;
  QLabel * device_name_value;
  QLabel * serial_number_label;
  QLabel * serial_number_value;
  QLabel * firmware_version_label;
  QLabel * firmware_version_value;
  QLabel * cmd_port_label;
  QLabel * cmd_port_value;
  QLabel * ttl_port_label;
  QLabel * ttl_port_value;
  QLabel * device_reset_label;
  QLabel * device_reset_value;

  bool start_event_reported = false;

  QString directory_hint;
};

// TODO: fix these names in the main_window class for settings that were renamed:
/**
-  void set_overcurrent_threshold(uint8_t);
+  void set_hard_overcurrent_threshold(uint8_t);
**/
