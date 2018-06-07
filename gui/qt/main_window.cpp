#include "config.h"
#include "main_window.h"
#include "main_controller.h"
#include "graph_window.h"
#include "bootloader_window.h"
#include "input_wizard.h"
#include "feedback_wizard.h"
#include "message_box.h"
#include "elided_label.h"
#include "pid_constant_control.h"
#include "nice_spin_box.h"

#include <to_string.h>

#include "qcustomplot.h"

#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QMessageBox>
#include <QProcessEnvironment>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollBar>
#include <QShortcut>
#include <QSpinBox>
#include <QTabWidget>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>
#include <QString>
#include <QFontMetrics>
#include <iostream>
#include <cassert>
#include <cmath>

#ifdef QT_STATIC
#include <QtPlugin>
#ifdef _WIN32
Q_IMPORT_PLUGIN (QWindowsIntegrationPlugin);
#endif
#ifdef __linux__
Q_IMPORT_PLUGIN (QXcbIntegrationPlugin);
Q_IMPORT_PLUGIN (QLinuxFbIntegrationPlugin);
#endif
#ifdef __APPLE__
Q_IMPORT_PLUGIN (QCocoaIntegrationPlugin);
#endif
#endif

#define UINT12_MAX 0xFFF // 4095

main_window::main_window(QWidget * parent)
  : QMainWindow(parent)
{
  setup_ui();
  popout_graph_window = 0;
}

void main_window::set_controller(main_controller * controller)
{
  this->controller = controller;
}

void main_window::set_update_timer_interval(uint32_t interval_ms)
{
  assert(update_timer);
  assert(interval_ms <= std::numeric_limits<int>::max());
  update_timer->setInterval(interval_ms);
}

void main_window::start_update_timer()
{
  assert(update_timer);
  update_timer->start();
}

bool main_window::confirm(const std::string & question)
{
  return ::confirm(question, this);
}

void main_window::show_error_message(const std::string & message)
{
  ::show_error_message(message, this);
}

void main_window::show_info_message(const std::string & message)
{
  ::show_info_message(message, this);
}

void main_window::show_warning_message(const std::string & message)
{
  ::show_warning_message(message, this);
}

void main_window::open_bootloader_window()
{
  // Note: Unlike the learning wizard dialogs, bootloader_window is not a
  // QDialog (maybe it should be), so we cannot show it in a blocking way, so we
  // must create it with 'new' and use signals to get its results.

  bootloader_window * window = new bootloader_window(this);
  connect(window, &bootloader_window::upload_complete,
    this, &main_window::upgrade_firmware_complete);
  window->setWindowModality(Qt::ApplicationModal);
  window->setAttribute(Qt::WA_DeleteOnClose);
  window->show();
}

void main_window::set_device_name(const std::string & name, bool link_enabled)
{
  QString text = QString::fromStdString(name);
  if (link_enabled)
  {
    text = "<a href=\"#doc\">" + text + "</a>";
  }
  device_name_value->setText(text);
}

void main_window::set_serial_number(const std::string & serial_number)
{
  serial_number_value->setText(QString::fromStdString(serial_number));
}

void main_window::set_firmware_version(const std::string & firmware_version)
{
  firmware_version_value->setText(QString::fromStdString(firmware_version));
}

void main_window::set_cmd_port(const std::string & cmd_port)
{
  cmd_port_value->setText(QString::fromStdString(cmd_port));
}

void main_window::set_ttl_port(const std::string & ttl_port)
{
  ttl_port_value->setText(QString::fromStdString(ttl_port));
}

void main_window::set_device_reset(const std::string & device_reset)
{
  device_reset_value->setText(QString::fromStdString(device_reset));
}

void main_window::set_up_time(uint32_t up_time)
{
  up_time_value->setText(QString::fromStdString(
    convert_up_time_to_string(up_time)));
}

void main_window::set_input(uint16_t input, uint8_t input_mode)
{
  graph->input.plot_value = input;

  QString input_pretty = "";

  if (input_mode == JRK_INPUT_MODE_RC)
  {
    input_pretty = " (" + QString::fromStdString(
      convert_rc_12bit_to_us_string(input)) + ")";
  }
  else if (input_mode == JRK_INPUT_MODE_ANALOG)
  {
    input_pretty = " (" + QString::fromStdString(
      convert_analog_12bit_to_v_string(input)) + ")";
  }

  input_value->setText(QString::number(input) + input_pretty);

  emit input_changed(input);
}

void main_window::set_target(uint16_t target)
{
  graph->target.plot_value = target;
  target_value->setText(QString::number(target));
}

void main_window::set_feedback(uint16_t feedback, uint8_t feedback_mode)
{
  graph->feedback.plot_value = feedback;

  QString feedback_pretty = "";

  if (feedback_mode == JRK_FEEDBACK_MODE_ANALOG)
  {
    feedback_pretty = " (" + QString::fromStdString(
      convert_analog_12bit_to_v_string(feedback)) + ")";
  }

  feedback_value->setText(QString::number(feedback) + feedback_pretty);
  emit feedback_changed(feedback);
}

void main_window::set_scaled_feedback(uint16_t scaled_feedback)
{
  graph->scaled_feedback.plot_value = scaled_feedback;
  scaled_feedback_value->setText(QString::number(scaled_feedback));
}

void main_window::set_feedback_not_applicable()
{
  graph->feedback.plot_value = 0;
  feedback_value->setText(tr("N/A"));

  graph->scaled_feedback.plot_value = 0;
  scaled_feedback_value->setText(tr("N/A"));

  graph->error.plot_value = 0;
  error_value->setText("N/A");

  graph->integral.plot_value = 0;
  integral_value->setText("N/A");
}

void main_window::set_error(int16_t error)
{
  graph->error.plot_value = error;
  error_value->setText(QString::number(error));
}

void main_window::set_integral(int16_t integral)
{
  graph->integral.plot_value = integral;
  integral_value->setText(QString::number(integral));
}

void main_window::set_duty_cycle_target(int16_t duty_cycle_target)
{
  graph->duty_cycle_target.plot_value = duty_cycle_target;
  duty_cycle_target_value->setText(QString::number(duty_cycle_target));
}

void main_window::set_duty_cycle(int16_t duty_cycle)
{
  graph->duty_cycle.plot_value = duty_cycle;

  QString duty_cycle_pretty = " (" + QString::fromStdString(
      convert_duty_cycle_to_percent_string(duty_cycle)) + ")";

  duty_cycle_value->setText(QString::number(duty_cycle) + duty_cycle_pretty);
  emit duty_cycle_changed(duty_cycle);
}

void main_window::set_raw_current_mv(uint16_t current)
{
  graph->raw_current.plot_value = current;
  raw_current_value->setText(QString::number(current) + " mV");
}

void main_window::set_current(int32_t current)
{
  graph->current.plot_value = current;
  current_value->setText(QString::number(current) + " mA");
}

void main_window::set_current_chopping_now(bool chopping)
{
  graph->current_chopping.plot_value = chopping;
}

void main_window::set_current_chopping_count(uint32_t count)
{
  current_chopping_count_value->setText(QString::number(count));
}

void main_window::set_vin_voltage(uint16_t vin_voltage)
{
  vin_voltage_value->setText(QString::fromStdString(
    convert_mv_to_v_string(vin_voltage)));
}

void main_window::set_pid_period_count(uint16_t count)
{
  pid_period_count_value->setText(QString::number(count));
}

void main_window::set_pid_period_exceeded(bool exceeded)
{
  pid_period_exceeded_value->setText(exceeded ? "Yes" : "No");
}

void main_window::set_device_list_contents(const std::vector<jrk::device> & device_list)
{
  suppress_events = true;
  device_list_value->clear();
  device_list_value->addItem(tr("Not connected"), QString()); // null value
  for (const jrk::device & device : device_list)
  {
    device_list_value->addItem(
      QString::fromStdString(
        std::string(jrk_look_up_product_name_short(device.get_product())) +
        " #" + device.get_serial_number()),
      QString::fromStdString(device.get_os_id()));
  }
  suppress_events = false;
}

void main_window::set_device_list_selected(const jrk::device & device)
{
  // TODO: show an error if we couldn't find the specified device
  // (findData returned -1)?
  suppress_events = true;
  int index = 0;
  if (device.is_present())
  {
    index = device_list_value->findData(QString::fromStdString(device.get_os_id()));
  }
  device_list_value->setCurrentIndex(index);
  suppress_events = false;
}

void main_window::set_connection_status(std::string const & status, bool error)
{
  if (error)
  {
    connection_status_value->setStyleSheet("color: red;");
  }
  else
  {
    connection_status_value->setStyleSheet("");
  }
  connection_status_value->setText(QString::fromStdString(status));
}

void main_window::set_manual_target_enabled(bool enabled)
{
  manual_target_box->setEnabled(enabled);
}

void main_window::set_manual_target_range(uint16_t min, uint16_t max)
{
  suppress_events = true;
  manual_target_min_label->setText(QString::number(min));
  manual_target_max_label->setText(QString::number(max));
  manual_target_scroll_bar->setRange(min, max);
  manual_target_entry_value->setRange(min, max);
  suppress_events = false;
}

void main_window::set_manual_target_inputs(uint16_t target)
{
  suppress_events = true;
  manual_target_scroll_bar->setValue(target);
  manual_target_entry_value->setValue(target);
  suppress_events = false;
}

uint16_t main_window::get_manual_target_numeric_input()
{
  return manual_target_entry_value->value();
}

void main_window::set_apply_settings_enabled(bool enabled)
{
  apply_settings_button->setEnabled(enabled);
  apply_settings_action->setEnabled(enabled);
}

void main_window::set_motor_status_message(
  const std::string & message, uint16_t error_flag)
{
  // setStyleSheet() is expensive, so only call it if something actually
  // changed. Check if there's currently a stylesheet applied and decide
  // whether we need to do anything based on that.
  bool styled = !motor_status_value->styleSheet().isEmpty();
  bool stopped = false;

  if (error_flag != 0)
  {
    stopped = true;
  }

  if (!styled && stopped)
  {
    motor_status_value->setStyleSheet("color: red;");
  }
  else if (styled && !stopped)
  {
    motor_status_value->setStyleSheet("");
  }

  QString message_qstr = QString::fromStdString(message);
  motor_status_value->setText(message_qstr);
}

void main_window::set_input_mode(uint8_t input_mode)
{
  set_u8_combobox(input_mode_combobox, input_mode);
}

void main_window::set_input_invert(bool input_invert)
{
  set_check_box(input_invert_checkbox, input_invert);
}

void main_window::set_input_analog_samples_exponent(uint8_t input_analog_samples)
{
  set_u8_combobox(input_analog_samples_combobox, input_analog_samples);
}

void main_window::set_input_detect_disconnect(bool input_detect_disconnect)
{
  set_check_box(input_detect_disconnect_checkbox, input_detect_disconnect);
}

void main_window::set_input_serial_mode(uint8_t value)
{
  suppress_events = true;
  QAbstractButton * radio = input_serial_mode_button_group->button(value);
  if (radio)
  {
    radio->setChecked(true);
  }
  else
  {
    // The value doesn't correspond with any of the radio buttons, so clear
    // the currently selected button, if any.
    QAbstractButton * checked = input_serial_mode_button_group->checkedButton();
    if (checked)
    {
      input_serial_mode_button_group->setExclusive(false);
      checked->setChecked(false);
      input_serial_mode_button_group->setExclusive(true);
    }
  }
  suppress_events = false;
}

void main_window::set_input_baud_rate(uint32_t value)
{
  set_spin_box(input_uart_fixed_baud_spinbox, value);
}

void main_window::set_input_enable_crc(bool enabled)
{
  set_check_box(input_enable_crc_checkbox, enabled);
}

void main_window::set_input_device_number(uint16_t value)
{
  set_spin_box(input_device_spinbox, value);
}

void main_window::set_input_enable_device_number(bool enabled)
{
  set_check_box(input_device_number_checkbox, enabled);
}

void main_window::set_serial_timeout(uint32_t value)
{
  set_double_spin_box(input_timeout_spinbox, value / 1000.0);
}

void main_window::set_input_compact_protocol(bool enabled)
{
  set_check_box(input_disable_compact_protocol_checkbox, enabled);
}

void main_window::set_input_error_minimum(uint16_t input_error_minimum)
{
  set_spin_box(input_error_minimum_spinbox, input_error_minimum);
}

void main_window::set_input_error_maximum(uint16_t input_error_maximum)
{
  set_spin_box(input_error_maximum_spinbox, input_error_maximum);
}

void main_window::set_input_minimum(uint16_t input_minimum)
{
  set_spin_box(input_minimum_spinbox, input_minimum);
}

void main_window::set_input_maximum(uint16_t input_maximum)
{
  set_spin_box(input_maximum_spinbox, input_maximum);
}

void main_window::set_input_neutral_minimum(uint16_t input_neutral_minimum)
{
  set_spin_box(input_neutral_minimum_spinbox, input_neutral_minimum);
}

void main_window::set_input_neutral_maximum(uint16_t input_neutral_maximum)
{
  set_spin_box(input_neutral_maximum_spinbox, input_neutral_maximum);
}

void main_window::set_input_output_minimum(uint16_t input_output_minimum)
{
  set_spin_box(input_output_minimum_spinbox, input_output_minimum);
}

void main_window::set_input_output_neutral(uint16_t input_output_neutral)
{
  set_spin_box(input_output_neutral_spinbox, input_output_neutral);
}

void main_window::set_input_output_maximum(uint16_t input_output_maximum)
{
  set_spin_box(input_output_maximum_spinbox, input_output_maximum);
}

void main_window::set_input_scaling_degree(uint8_t input_scaling_degree)
{
  set_u8_combobox(input_scaling_degree_combobox, input_scaling_degree);
}

static bool ordered(QList<int> p)
{
  for (int i = 0; i < p.size()-1; i++)
  {
    if (p[i] > p[i + 1])
      return false;
  }
  return true;
}

void main_window::set_input_scaling_order_warning_label()
{
  bool enabled = !ordered({
    input_error_minimum_spinbox->value(), input_minimum_spinbox->value(),
    input_neutral_minimum_spinbox->value(), input_neutral_maximum_spinbox->value(),
    input_maximum_spinbox->value(), input_error_maximum_spinbox->value()});
  input_scaling_order_warning_label->setVisible(enabled);
}

void main_window::run_input_wizard(uint8_t input_mode)
{
  // TODO: make the input wizard more like the feedback wizard
  input_wizard wizard(this, input_mode);
  connect(this, &main_window::input_changed, &wizard, &input_wizard::set_input);

  if (wizard.exec() != QDialog::Accepted) { return; }

  controller->handle_input_invert_input(wizard.result.invert);
  controller->handle_input_error_minimum_input(wizard.result.error_minimum);
  controller->handle_input_error_maximum_input(wizard.result.error_maximum);
  controller->handle_input_minimum_input(wizard.result.minimum);
  controller->handle_input_maximum_input(wizard.result.maximum);
  controller->handle_input_neutral_minimum_input(wizard.result.neutral_minimum);
  controller->handle_input_neutral_maximum_input(wizard.result.neutral_maximum);
}

void main_window::update_input_tab_enables()
{
  uint8_t input_mode = input_mode_combobox->currentData().toUInt();

  input_scaling_groupbox->setEnabled(input_mode != JRK_INPUT_MODE_SERIAL);

  bool analog = input_mode == JRK_FEEDBACK_MODE_ANALOG;
  input_analog_groupbox->setEnabled(analog || always_analog_sda->isChecked());
  input_detect_disconnect_checkbox->setEnabled(analog);
}

void main_window::set_feedback_mode(uint8_t feedback_mode)
{
  set_u8_combobox(feedback_mode_combobox, feedback_mode);
}

void main_window::set_feedback_invert(bool feedback_invert)
{
  set_check_box(feedback_invert_checkbox, feedback_invert);
}

void main_window::set_feedback_error_minimum(uint16_t value)
{
  set_spin_box(feedback_error_minimum_spinbox, value);
}

void main_window::set_feedback_error_maximum(uint16_t value)
{
  set_spin_box(feedback_error_maximum_spinbox, value);
}

void main_window::set_feedback_minimum(uint16_t value)
{
  set_spin_box(feedback_minimum_spinbox, value);
}

void main_window::set_feedback_maximum(uint16_t value)
{
  set_spin_box(feedback_maximum_spinbox, value);
}

void main_window::set_feedback_scaling_order_warning_label()
{
  bool enabled =
  !ordered({feedback_error_minimum_spinbox->value(), feedback_minimum_spinbox->value(),
    feedback_maximum_spinbox->value(), feedback_error_maximum_spinbox->value()});
  feedback_scaling_order_warning_label->setVisible(enabled);
}

void main_window::set_feedback_analog_samples_exponent(uint8_t feedback_analog_samples)
{
  set_u8_combobox(feedback_analog_samples_combobox, feedback_analog_samples);
}

void main_window::set_feedback_detect_disconnect(bool value)
{
  set_check_box(feedback_detect_disconnect_checkbox, value);
}

void main_window::set_feedback_wraparound(bool value)
{
  set_check_box(feedback_wraparound_checkbox, value);
}

void main_window::set_fbt_method(uint8_t value)
{
  set_u8_combobox(fbt_method_combobox, value);
}

void main_window::set_fbt_timing_clock(uint8_t value)
{
  set_u8_combobox(fbt_timing_clock_combobox, value);
}

void main_window::set_fbt_timing_polarity(bool polarity)
{
  set_u8_combobox(fbt_timing_polarity_combobox, polarity);
}

void main_window::set_fbt_timing_timeout(uint16_t value)
{
  set_spin_box(fbt_timing_timeout_spinbox, value);
}

void main_window::set_fbt_samples(uint8_t value)
{
  set_spin_box(fbt_samples_spinbox, value);
}

void main_window::set_fbt_divider_exponent(uint8_t value)
{
  set_u8_combobox(fbt_divider_combobox, value);
}

void main_window::set_fbt_range_display(const std::string & message, bool invalid)
{
  fbt_range_label->setText(QString::fromStdString(message));

  bool styled = !fbt_range_label->styleSheet().isEmpty();
  if (invalid && !styled)
  {
    fbt_range_label->setStyleSheet("color: red; }");
  }
  else if (!invalid && styled)
  {
    fbt_range_label->setStyleSheet("");
  }
}

void main_window::update_feedback_tab_enables()
{
  uint8_t feedback_mode = feedback_mode_combobox->currentData().toUInt();

  feedback_scaling_groupbox->setEnabled(
    feedback_mode != JRK_FEEDBACK_MODE_NONE);

  bool analog = feedback_mode == JRK_FEEDBACK_MODE_ANALOG;
  feedback_analog_groupbox->setEnabled(analog || always_analog_fba->isChecked());
  feedback_detect_disconnect_checkbox->setEnabled(analog);
  feedback_wraparound_checkbox->setEnabled(analog);

  bool frequency = feedback_mode == JRK_FEEDBACK_MODE_FREQUENCY;
  fbt_divider_label->setEnabled(frequency);
  fbt_divider_combobox->setEnabled(frequency);

  uint8_t fbt_method = fbt_method_combobox->currentData().toUInt();

  bool timing = fbt_method == JRK_FBT_METHOD_PULSE_TIMING;
  fbt_timing_clock_label->setEnabled(timing);
  fbt_timing_clock_combobox->setEnabled(timing);
  fbt_timing_polarity_label->setEnabled(timing);
  fbt_timing_polarity_combobox->setEnabled(timing);
  fbt_timing_timeout_label->setEnabled(timing);
  fbt_timing_timeout_spinbox->setEnabled(timing);
}

void main_window::set_pid_proportional(uint16_t multiplier, uint8_t exponent)
{
  suppress_events = true;
  pid_proportional_control->set_values(multiplier, exponent);
  suppress_events = false;
}

void main_window::set_pid_integral(uint16_t multiplier, uint8_t exponent)
{
  suppress_events = true;
  pid_integral_control->set_values(multiplier, exponent);
  suppress_events = false;
}

void main_window::set_pid_derivative(uint16_t multiplier, uint8_t exponent)
{
  suppress_events = true;
  pid_derivative_control->set_values(multiplier, exponent);
  suppress_events = false;
}

void main_window::set_pid_period(uint16_t value)
{
  set_spin_box(pid_period_spinbox, value);
}

void main_window::set_integral_limit(uint16_t value)
{
  set_spin_box(integral_limit_spinbox, value);
}

void main_window::set_integral_divider_exponent(uint8_t exponent)
{
  set_u8_combobox(integral_divider_combobox, exponent);
}

void main_window::set_reset_integral(bool enabled)
{
  set_check_box(reset_integral_checkbox, enabled);
}

void main_window::set_feedback_dead_zone(uint8_t value)
{
  set_spin_box(feedback_dead_zone_spinbox, value);
}

void main_window::set_pwm_frequency(uint8_t pwm_frequency)
{
  set_u8_combobox(pwm_frequency_combobox, pwm_frequency);
}

void main_window::set_motor_invert(bool enabled)
{
  set_check_box(motor_invert_checkbox, enabled);
}

void main_window::set_motor_asymmetric(bool checked)
{
  set_check_box(motor_asymmetric_checkbox, checked);
  max_duty_cycle_reverse_spinbox->setEnabled(checked);
  max_acceleration_reverse_spinbox->setEnabled(checked);
  max_deceleration_reverse_spinbox->setEnabled(checked);
  brake_duration_reverse_spinbox->setEnabled(checked);
  current_limit_reverse_spinbox->setEnabled(checked);
  max_current_reverse_spinbox->setEnabled(checked);
}

void main_window::set_max_duty_cycle_forward(uint16_t duty_cycle)
{
  set_spin_box(max_duty_cycle_forward_spinbox, duty_cycle);
}

void main_window::set_max_duty_cycle_reverse(uint16_t duty_cycle)
{
  set_spin_box(max_duty_cycle_reverse_spinbox, duty_cycle);
}

void main_window::set_max_acceleration_forward(uint16_t acceleration)
{
  set_spin_box(max_acceleration_forward_spinbox, acceleration);
}

void main_window::set_max_acceleration_reverse(uint16_t acceleration)
{
  set_spin_box(max_acceleration_reverse_spinbox, acceleration);
}

void main_window::set_max_deceleration_forward(uint16_t acceleration)
{
  set_spin_box(max_deceleration_forward_spinbox, acceleration);
}

void main_window::set_max_deceleration_reverse(uint16_t acceleration)
{
  set_spin_box(max_deceleration_reverse_spinbox, acceleration);
}

void main_window::set_brake_duration_forward(uint32_t brake_duration)
{
  set_spin_box(brake_duration_forward_spinbox, brake_duration);
}

void main_window::set_brake_duration_reverse(uint32_t brake_duration)
{
  set_spin_box(brake_duration_reverse_spinbox, brake_duration);
}

void main_window::set_current_limit_code_forward(uint16_t current_limit)
{
  set_spin_box(current_limit_forward_spinbox, current_limit);
}

void main_window::set_current_limit_code_reverse(uint16_t current_limit)
{
  set_spin_box(current_limit_reverse_spinbox, current_limit);
}

void main_window::get_recommended_current_limit_codes(uint32_t product)
{
  const std::vector<uint16_t> code_table =
    jrk::get_recommended_encoded_hard_current_limits(product);

  QMap<int, int> mapping;
  for (uint16_t code : code_table)
  {
    uint32_t current = controller->current_limit_code_to_ma(code);

    mapping.insert(code, current);
  }

  current_limit_forward_spinbox->set_mapping(mapping);
  current_limit_reverse_spinbox->set_mapping(mapping);
}

void main_window::set_max_current_forward(uint16_t current)
{
  set_spin_box(max_current_forward_spinbox, current);
}

void main_window::set_max_current_reverse(uint16_t current)
{
  set_spin_box(max_current_reverse_spinbox, current);
}

void main_window::set_current_offset_calibration(int16_t cal)
{
  set_spin_box(current_offset_calibration_spinbox, cal);
}

void main_window::set_current_scale_calibration(int16_t cal)
{
  set_spin_box(current_scale_calibration_spinbox, cal);
}

void main_window::set_current_samples_exponent(uint8_t exponent)
{
  set_u8_combobox(current_samples_combobox, exponent);
}

void main_window::set_overcurrent_threshold(uint8_t threshold)
{
  set_spin_box(overcurrent_threshold_spinbox, threshold);
}

void main_window::set_max_duty_cycle_while_feedback_out_of_range(uint16_t value)
{
  set_spin_box(max_duty_cycle_while_feedback_out_of_range_spinbox, value);
}

void main_window::set_coast_when_off(bool value)
{
  suppress_events = true;
  QAbstractButton * radio = coast_when_off_button_group->button(value);
  if (radio)
  {
    radio->setChecked(true);
  }
  else
  {
    // This should never happen.
    assert(0);
  }
  suppress_events = false;
}

void main_window::set_error_enable(uint16_t enable, uint16_t latch)
{
  suppress_events = true;

  for (error_row & row : error_rows)
  {
    if ((enable >> row.error_number & 1) || row.always_enabled)
    {
      if ((latch >> row.error_number & 1) || row.always_latched)
      {
        row.latched_radio->setChecked(true);
      }
      else
      {
        row.enabled_radio->setChecked(true);
      }
    }
    else
    {
      row.disabled_radio->setChecked(true);
    }
  }

  suppress_events = false;
}

void main_window::set_error_hard(uint16_t hard)
{
  for (error_row & row : error_rows)
  {
    bool is_hard = hard >> row.error_number & 1;
    set_check_box(row.error_hard, is_hard || row.always_hard);
  }
}

void main_window::set_error_flags_halting(uint16_t error_flags_halting)
{
  error_flags_halting_value->setText(QString::fromStdString(
    convert_error_flags_to_hex_string(error_flags_halting)));

  for (error_row & row : error_rows)
  {
    // setStyleSheet() is expensive, so only call it if something actually
    // changed. Check if there's currently a stylesheet applied and decide
    // whether we need to do anything based on that.
    bool styled = !row.stopping_value->styleSheet().isEmpty();

    if (error_flags_halting >> row.error_number & 1)
    {
      row.stopping_value->setText(tr("Yes"));
      if (!styled)
      {
        row.stopping_value->setStyleSheet(
          ":enabled { background-color: red; color: white; }");
      }
    }
    else
    {
      row.stopping_value->setText(tr("No"));
      if (styled)
      {
        row.stopping_value->setStyleSheet("");
      }
    }
  }
}

void main_window::increment_errors_occurred(uint16_t errors_occurred)
{
  for (error_row & row : error_rows)
  {
    if (errors_occurred >> row.error_number & 1)
    {
      row.count++;
      row.count_value->setText(QString::number(row.count));
    }
  }
}

void main_window::reset_error_counts()
{
  for (error_row & row : error_rows)
  {
    row.count = 0;
    row.count_value->setText("-");
  }
}

void main_window::set_disable_i2c_pullups(bool enabled)
{
  set_check_box(disable_i2c_pullups, enabled);
}

void main_window::set_analog_sda_pullup(bool enabled)
{
  set_check_box(analog_sda_pullup, enabled);
}

void main_window::set_always_analog_sda(bool enabled)
{
  set_check_box(always_analog_sda, enabled);
}

void main_window::set_always_analog_fba(bool enabled)
{
  set_check_box(always_analog_fba, enabled);
}

void main_window::set_never_sleep(bool enabled)
{
  set_check_box(never_sleep_checkbox, enabled);
}

void main_window::set_vin_calibration(int16_t vin_calibration)
{
  set_spin_box(vin_calibration_value, vin_calibration);
}

void main_window::set_serial_baud_rate(uint32_t serial_baud_rate)
{
  set_spin_box(input_uart_fixed_baud_spinbox, serial_baud_rate);
}

void main_window::set_serial_device_number(uint8_t serial_device_number)
{
  set_spin_box(input_device_spinbox, serial_device_number);
}

void main_window::set_tab_pages_enabled(bool enabled)
{
  variables_box->setEnabled(enabled);
  manual_target_box->setEnabled(enabled);

  for (int i = 1; i < tab_widget->count(); i++)
  {
    tab_widget->widget(i)->setEnabled(enabled);
  }
}

void main_window::set_restore_defaults_enabled(bool enabled)
{
  restore_defaults_action->setEnabled(enabled);
}

void main_window::set_reload_settings_enabled(bool enabled)
{
  reload_settings_action->setEnabled(enabled);
}

void main_window::set_open_save_settings_enabled(bool enabled)
{
  open_settings_action->setEnabled(enabled);
  save_settings_action->setEnabled(enabled);
}

void main_window::set_disconnect_enabled(bool enabled)
{
  disconnect_action->setEnabled(enabled);
}

void main_window::set_stop_motor_enabled(bool enabled)
{
  stop_motor_action->setEnabled(enabled);
  stop_motor_button->setEnabled(enabled);
}

void main_window::set_run_motor_enabled(bool enabled)
{
  run_motor_action->setEnabled(enabled);
  run_motor_button->setEnabled(enabled);
}

bool main_window::motor_asymmetric_checked()
{
  return motor_asymmetric_checkbox->isChecked();
}

void main_window::update_graph(uint32_t up_time)
{
  graph->plot_data(up_time);
}

void main_window::reset_graph()
{
  graph->clear_graphs();
}

void main_window::set_u8_combobox(QComboBox * combo, uint8_t value)
{
  suppress_events = true;
  combo->setCurrentIndex(combo->findData(value));
  suppress_events = false;
}

void main_window::set_spin_box(QSpinBox * spin, int value)
{
  // Only set the QSpinBox's value if the new value is numerically different.
  // This prevents, for example, a value of "0000" from being changed to "0"
  // while you're trying to change "10000" to "20000".
  if (spin->value() != value)
  {
    suppress_events = true;
    spin->setValue(value);
    suppress_events = false;
  }
}

void main_window::set_double_spin_box(QDoubleSpinBox * spin, double value)
{
  // Only set the QSpinBox's value if the new value is numerically different.
  // This prevents, for example, a value of "0000" from being changed to "0"
  // while you're trying to change "10000" to "20000".
  if (spin->value() != value)
  {
    suppress_events = true;
    spin->setValue(value);
    suppress_events = false;
  }
}

void main_window::set_check_box(QCheckBox * check, bool value)
{
  suppress_events = true;
  check->setChecked(value);
  suppress_events = false;
}

void main_window::center_at_startup_if_needed()
{
  // Center the window.  This fixes a strange bug on the Raspbian Jessie where
  // the window would appear in the upper left with its title bar off the
  // screen.  On other platforms, the default window position did not make much
  // sense, so it is nice to center it.
  //
  // In case this causes problems, you can set the JRK2GUI_CENTER environment
  // variable to "N".
  //
  // NOTE: This position issue on Raspbian is a bug in Qt that should be fixed.
  auto env = QProcessEnvironment::systemEnvironment();
  if (env.value("JRK2GUI_CENTER") != "N")
  {
    setGeometry(
      QStyle::alignedRect(
        Qt::LeftToRight,
        Qt::AlignCenter,
        size(),
        qApp->desktop()->availableGeometry()
        )
      );
  }
}

void main_window::showEvent(QShowEvent * event)
{
  if (!start_event_reported)
  {
    start_event_reported = true;
    center_at_startup_if_needed();
    controller->start();
  }
}

void main_window::closeEvent(QCloseEvent * event)
{
  if (!controller->exit())
  {
    // User canceled exit when prompted about settings that have not been applied.
    event->ignore();
  }
  else
    qApp->quit();
}

void main_window::on_update_timer_timeout()
{
  controller->update();
  emit controller_updated();
}

void main_window::restore_graph_preview()
{
  graph->set_preview_mode(true);

  horizontal_layout->addWidget(graph->custom_plot, 1);
  preview_frame->setFrameShape(QFrame::Box);
}

void main_window::preview_pane_clicked()
{
  horizontal_layout->removeWidget(graph);
  preview_frame->setFrameShape(QFrame::NoFrame);
  if(popout_graph_window == 0)
  {
    popout_graph_window = new graph_window(this);
    connect(popout_graph_window, SIGNAL(pass_widget()), this,
    SLOT(restore_graph_preview()));
  }

  popout_graph_window->receive_widget(graph);
  popout_graph_window->raise_window();
}

void main_window::on_device_name_value_linkActivated()
{
  on_documentation_action_triggered();
}

void main_window::on_documentation_action_triggered()
{
  QDesktopServices::openUrl(QUrl(DOCUMENTATION_URL));
}

void main_window::on_about_action_triggered()
{
  QMessageBox::about(this, tr("About") + " " + windowTitle(),
    QString("<h2>") + windowTitle() + "</h2>" +
    tr("<h4>Version %1</h4>"
      "<h4>Copyright &copy; %2 Pololu Corporation</h4>"
      "<p>See LICENSE.html for copyright and license information.</p>"
      "<p><a href=\"%3\">Online documentation</a></p>")
    .arg(SOFTWARE_VERSION_STRING, SOFTWARE_YEAR, DOCUMENTATION_URL));
}

void main_window::on_device_list_value_currentIndexChanged(int index)
{
  if (suppress_events) { return; }

  QString id = device_list_value->itemData(index).toString();
  controller->connect_device_with_os_id(id.toStdString());
}

void main_window::on_apply_settings_action_triggered()
{
  controller->apply_settings();
}

void main_window::on_upgrade_firmware_action_triggered()
{
  controller->upgrade_firmware();
}

void main_window::upgrade_firmware_complete()
{
  controller->upgrade_firmware_complete();
}

void main_window::on_run_motor_action_triggered()
{
  controller->run_motor();
}

void main_window::on_stop_motor_action_triggered()
{
  controller->stop_motor();
}

void main_window::on_clear_current_chopping_count_action_triggered()
{
  controller->clear_current_chopping_count();
}

void main_window::on_set_target_button_clicked()
{
  controller->set_target(manual_target_entry_value->value());
}

void main_window::on_auto_set_target_check_stateChanged(int state)
{
  if (suppress_events) { return; }

  if (state == Qt::Checked)
  {
    on_set_target_button_clicked();
  }
}

void main_window::on_manual_target_scroll_bar_valueChanged(int value)
{
  if (suppress_events) { return; }
  manual_target_entry_value->setValue(value);
}

void main_window::on_manual_target_entry_value_valueChanged(int value)
{
  if (suppress_events) { return; }

  suppress_events = true;
  manual_target_scroll_bar->setValue(value);
  suppress_events = false;

  if (auto_set_target_check->isChecked())
  {
    on_set_target_button_clicked();
  }
}

void main_window::on_manual_target_return_key_shortcut_activated()
{
  if (manual_target_scroll_bar->hasFocus())
  {
    // Enter was pressed on the scroll bar.
    on_set_target_button_clicked();
  }
  else if (manual_target_entry_value->hasFocus())
  {
    // Enter was pressed on the target entry box.
    suppress_events = true;
    manual_target_entry_value->interpretText();
    manual_target_entry_value->selectAll();
    suppress_events = false;
    on_set_target_button_clicked();
  }
}

void main_window::on_open_settings_action_triggered()
{
  QString filename = QFileDialog::getOpenFileName(this,
    tr("Open Settings File"), directory_hint + "/jrk_settings.txt",
    tr("Text files (*.txt)"));

  if (!filename.isNull())
  {
    directory_hint = QFileInfo(filename).canonicalPath();
    controller->open_settings_from_file(filename.toStdString());
  }
}

void main_window::on_save_settings_action_triggered()
{
  QString filename = QFileDialog::getSaveFileName(this,
    tr("Save Settings File"), directory_hint + "/jrk_settings.txt",
    tr("Text files (*.txt)"));

  if (!filename.isNull())
  {
    directory_hint = QFileInfo(filename).canonicalPath();
    controller->save_settings_to_file(filename.toStdString());
  }
}

void main_window::on_disconnect_action_triggered()
{
  controller->disconnect_device();
}

void main_window::on_reload_settings_action_triggered()
{
  controller->reload_settings();
}

void main_window::on_restore_defaults_action_triggered()
{
  controller->restore_default_settings();
}

void main_window::on_input_mode_combobox_currentIndexChanged(int index)
{
  if (suppress_events) { return; }
  uint8_t input_mode = input_mode_combobox->itemData(index).toUInt();
  controller->handle_input_mode_input(input_mode);
}

void main_window::on_input_analog_samples_combobox_currentIndexChanged(int index)
{
  if (suppress_events) { return; }
  uint8_t exponent = input_analog_samples_combobox->itemData(index).toUInt();
  controller->handle_input_analog_samples_exponent_input(exponent);
}

void main_window::on_input_invert_checkbox_stateChanged(int state)
{
  if (suppress_events) { return; }
  controller->handle_input_invert_input(state == Qt::Checked);
}

void main_window::on_input_detect_disconnect_checkbox_stateChanged(int state)
{
  if (suppress_events) { return; }
  controller->handle_input_detect_disconnect_input(state == Qt::Checked);
}

void main_window::on_input_serial_mode_button_group_buttonToggled(int id, bool checked)
{
  if (suppress_events) { return; }
  if (checked) { controller->handle_input_serial_mode_input(id); }
}

void main_window::on_input_uart_fixed_baud_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_input_uart_fixed_baud_input(value);
}

void main_window::on_input_enable_crc_checkbox_stateChanged(int state)
{
  if (suppress_events) { return; }
  controller->handle_input_enable_crc_input(state == Qt::Checked);
}

void main_window::on_input_device_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_input_device_input(value);
}

void main_window::on_input_device_number_checkbox_stateChanged(int state)
{
  if (suppress_events) { return; }
  controller->handle_input_device_number_input(state == Qt::Checked);
}

void main_window::on_input_timeout_spinbox_valueChanged(double value)
{
  if (suppress_events) { return; }
  controller->handle_serial_timeout_input(qRound(value * 100) * 10);
}

void main_window::on_input_disable_compact_protocol_checkbox_stateChanged(int state)
{
  if (suppress_events) { return; }
  controller->handle_input_disable_compact_protocol_input(state == Qt::Checked);
}

void main_window::on_input_error_minimum_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_input_error_minimum_input(value);
}

void main_window::on_input_error_maximum_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_input_error_maximum_input(value);
}

void main_window::on_input_minimum_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_input_minimum_input(value);
}

void main_window::on_input_maximum_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_input_maximum_input(value);
}

void main_window::on_input_neutral_minimum_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_input_neutral_minimum_input(value);
}

void main_window::on_input_neutral_maximum_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_input_neutral_maximum_input(value);
}

void main_window::on_input_output_minimum_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_output_minimum_input(value);
}

void main_window::on_input_output_neutral_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_output_neutral_input(value);
}

void main_window::on_input_output_maximum_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_output_maximum_input(value);
}

void main_window::on_input_scaling_degree_combobox_currentIndexChanged(int index)
{
  if (suppress_events) { return; }
  uint8_t input_scaling_degree = input_scaling_degree_combobox->itemData(index).toUInt();
  controller->handle_input_scaling_degree_input(input_scaling_degree);
}

void main_window::on_input_reset_range_button_clicked()
{
  controller->handle_input_error_minimum_input(0);
  controller->handle_input_error_maximum_input(4095);
  controller->handle_input_minimum_input(0);
  controller->handle_input_maximum_input(4095);
  controller->handle_input_neutral_minimum_input(2048);
  controller->handle_input_neutral_maximum_input(2048);
  controller->handle_output_minimum_input(0);
  controller->handle_output_neutral_input(2048);
  controller->handle_output_maximum_input(4095);
  controller->handle_input_invert_input(false);
}

void main_window::on_input_learn_button_clicked()
{
  controller->handle_input_learn();
}

void main_window::on_feedback_mode_combobox_currentIndexChanged(int index)
{
  if (suppress_events) { return; }
  uint8_t feedback_mode = feedback_mode_combobox->itemData(index).toUInt();
  controller->handle_feedback_mode_input(feedback_mode);
}

void main_window::on_feedback_invert_checkbox_stateChanged(int state)
{
  if (suppress_events) { return; }
  controller->handle_feedback_invert_input(state == Qt::Checked);
}

void main_window::on_feedback_error_minimum_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_feedback_error_minimum_input(value);
}

void main_window::on_feedback_error_maximum_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_feedback_error_maximum_input(value);
}

void main_window::on_feedback_maximum_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_feedback_maximum_input(value);
}

void main_window::on_feedback_minimum_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_feedback_minimum_input(value);
}

void main_window::on_feedback_reset_range_button_clicked()
{
  controller->handle_feedback_error_maximum_input(4095);
  controller->handle_feedback_maximum_input(4095);
  controller->handle_feedback_minimum_input(0);
  controller->handle_feedback_error_minimum_input(0);
}

void main_window::on_feedback_analog_samples_combobox_currentIndexChanged(int index)
{
  if (suppress_events) { return; }
  uint8_t exponent = feedback_analog_samples_combobox->itemData(index).toUInt();
  controller->handle_feedback_analog_samples_exponent_input(exponent);
}

void main_window::on_feedback_detect_disconnect_checkbox_stateChanged(int state)
{
  if (suppress_events) { return; }
  controller->handle_feedback_detect_disconnect_input(state == Qt::Checked);
}

void main_window::on_feedback_wraparound_checkbox_stateChanged(int state)
{
  if (suppress_events) { return; }
  controller->handle_feedback_wraparound_input(state == Qt::Checked);
}

void main_window::on_fbt_method_combobox_currentIndexChanged(int index)
{
  if (suppress_events) { return; }
  uint8_t mode = fbt_method_combobox->itemData(index).toUInt();
  controller->handle_fbt_method_input(mode);
}

void main_window::on_fbt_timing_clock_combobox_currentIndexChanged(int index)
{
  if (suppress_events) { return; }
  uint8_t clock = fbt_timing_clock_combobox->itemData(index).toUInt();
  controller->handle_fbt_timing_clock_input(clock);
}

void main_window::on_fbt_timing_polarity_combobox_currentIndexChanged(int index)
{
  if (suppress_events) { return; }
  bool polarity = fbt_timing_polarity_combobox->itemData(index).toUInt();
  controller->handle_fbt_timing_polarity_input(polarity);
}

void main_window::on_fbt_timing_timeout_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_fbt_timing_timeout_input(value);
}

void main_window::on_fbt_samples_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_fbt_samples_input(value);
}

void main_window::on_fbt_divider_combobox_currentIndexChanged(int index)
{
  if (suppress_events) { return; }
  uint8_t exponent = fbt_divider_combobox->itemData(index).toUInt();
  controller->handle_fbt_divider_exponent_input(exponent);
}

void main_window::on_feedback_learn_button_clicked()
{
  run_feedback_wizard(this);
}

void main_window::on_pid_proportional_control_values_changed(int multiplier, int exponent)
{
  if (suppress_events) { return; }
  controller->handle_pid_proportional_input(multiplier, exponent);
}

void main_window::on_pid_integral_control_values_changed(int multiplier, int exponent)
{
  if (suppress_events) { return; }
  controller->handle_pid_integral_input(multiplier, exponent);
}

void main_window::on_pid_derivative_control_values_changed(int multiplier, int exponent)
{
  if (suppress_events) { return; }
  controller->handle_pid_derivative_input(multiplier, exponent);
}

void main_window::on_pid_period_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_pid_period_input(value);
}

void main_window::on_integral_limit_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_integral_limit_input(value);
}

void main_window::on_integral_divider_combobox_currentIndexChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_integral_divider_exponent_input(value);
}

void main_window::on_reset_integral_checkbox_stateChanged(int state)
{
  if (suppress_events) { return; }
  controller->handle_reset_integral_input(state == Qt::Checked);
}

void main_window::on_feedback_dead_zone_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_feedback_dead_zone_input(value);
}

void main_window::on_pwm_frequency_combobox_currentIndexChanged(int index)
{
  if (suppress_events) { return; }
  uint8_t pwm_frequency = pwm_frequency_combobox->itemData(index).toUInt();
  controller->handle_pwm_frequency_input(pwm_frequency);
}

void main_window::on_motor_invert_checkbox_stateChanged(int state)
{
  if (suppress_events) { return; }
  controller->handle_motor_invert_input(state == Qt::Checked);
}

void main_window::on_motor_asymmetric_checkbox_stateChanged(int state)
{
  if (suppress_events) { return; }
  controller->handle_motor_asymmetric_input(state == Qt::Checked);
}

void main_window::on_max_duty_cycle_forward_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_max_duty_cycle_forward_input(value);
}

void main_window::on_max_duty_cycle_reverse_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_max_duty_cycle_reverse_input(value);
}

void main_window::on_max_acceleration_forward_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_max_acceleration_forward_input(value);
}

void main_window::on_max_acceleration_reverse_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_max_acceleration_reverse_input(value);
}

void main_window::on_max_deceleration_forward_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_max_deceleration_forward_input(value);
}

void main_window::on_max_deceleration_reverse_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_max_deceleration_reverse_input(value);
}

void main_window::on_brake_duration_forward_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_brake_duration_forward_input(value);
}

void main_window::on_brake_duration_reverse_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_brake_duration_reverse_input(value);
}

void main_window::on_current_limit_forward_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_current_limit_forward_input(value);
}

void main_window::on_current_limit_reverse_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_current_limit_reverse_input(value);
}

void main_window::on_max_current_forward_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_max_current_forward_input(value);
}

void main_window::on_max_current_reverse_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_max_current_reverse_input(value);
}
void main_window::on_current_offset_calibration_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_current_offset_calibration_input(value);
}

void main_window::on_current_scale_calibration_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_current_scale_calibration_input(value);
}

void main_window::on_current_samples_combobox_currentIndexChanged(int index)
{
  if (suppress_events) { return; }
  uint8_t exponent = current_samples_combobox->itemData(index).toUInt();
  controller->handle_current_samples_exponent_input(exponent);
}

void main_window::on_overcurrent_threshold_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_overcurrent_threshold_input(value);
}

void main_window::on_max_duty_cycle_while_feedback_out_of_range_spinbox_valueChanged(
  int value)
{
  if (suppress_events) { return; }
  controller->handle_max_duty_cycle_while_feedback_out_of_range_input(value);
}

void main_window::on_coast_when_off_button_group_buttonToggled(int id, bool checked)
{
  if (suppress_events) { return; }
  if (checked) { controller->handle_coast_when_off_input(id); }
}

void main_window::error_enable_group_buttonToggled(int id, int index)
{
  if (suppress_events) { return; }
  controller->handle_error_enable_input(index, id);
}

void main_window::error_hard_stateChanged(int state, int index)
{
  if (suppress_events) { return; }
  controller->handle_error_hard_input(index, state == Qt::Checked);
}

void main_window::on_errors_clear_errors_clicked()
{
  if (suppress_events) { return; }
  controller->handle_clear_errors_input();
}

void main_window::on_errors_reset_counts_clicked()
{
  if (suppress_events) { return; }
  reset_error_counts();
}

void main_window::on_disable_i2c_pullups_stateChanged(int state)
{
  if (suppress_events) { return; }
  controller->handle_disable_i2c_pullups_input(state == Qt::Checked);
}

void main_window::on_analog_sda_pullup_stateChanged(int state)
{
  if (suppress_events) { return; }
  controller->handle_analog_sda_pullup_input(state == Qt::Checked);
}

void main_window::on_always_analog_sda_stateChanged(int state)
{
  if (suppress_events) { return; }
  controller->handle_always_analog_sda_input(state == Qt::Checked);
}

void main_window::on_always_analog_fba_stateChanged(int state)
{
  if (suppress_events) { return; }
  controller->handle_always_analog_fba_input(state == Qt::Checked);
}

void main_window::on_never_sleep_checkbox_stateChanged(int state)
{
  if (suppress_events) { return; }
  controller->handle_never_sleep_input(state == Qt::Checked);
}

void main_window::on_vin_calibration_value_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_vin_calibration_input(value);
}

void main_window::setup_ui()
{
  setObjectName("main_window");
  setWindowTitle("Pololu Jrk G2 Configuration Utility");

  setup_style_sheet();

  directory_hint = QDir::homePath();

  update_timer = new QTimer(this);
  update_timer->setObjectName("update_timer");

  central_widget = new QWidget();
  central_widget->setObjectName("central_widget");
  setCentralWidget(central_widget);

  setup_menu_bar();

  header_layout = new QHBoxLayout();

  device_list_label = new QLabel();
  device_list_label->setText(tr("Connected to:"));

  device_list_value = new QComboBox();
  device_list_value->setObjectName("device_list_value");
  device_list_value->addItem(tr("Not connected"), QString()); // null value

  connection_status_value = new QLabel();

  // Make the device list wide enough to display the short name and serial
  // number of the Jrk.
  {
    QComboBox tmp_box;
    tmp_box.addItem("99v99 #1234567890123456");
    device_list_value->setMinimumWidth(tmp_box.sizeHint().width() * 105 / 100);
  }

  main_window_layout = new QVBoxLayout();
  main_window_layout->setObjectName("main_window_layout");

  tab_widget = new QTabWidget();
  tab_widget->addTab(setup_status_tab(), tr("Status"));
  tab_widget->addTab(setup_input_tab(), tr("Input"));
  tab_widget->addTab(setup_feedback_tab(), tr("Feedback"));
  tab_widget->addTab(setup_pid_tab(), tr("PID"));
  tab_widget->addTab(setup_motor_tab(), tr("Motor"));
  tab_widget->addTab(setup_errors_tab(), tr("Errors"));
  tab_widget->addTab(setup_advanced_tab(), tr("Advanced"));

  // Let the user specify which tab to start on.  Handy for development.
  auto env = QProcessEnvironment::systemEnvironment();
  tab_widget->setCurrentIndex(env.value("JRK2GUI_TAB").toInt());

  stop_motor_button = new QPushButton();
  stop_motor_button->setObjectName("stop_motor_button");
  stop_motor_button->setText(tr("&Stop motor"));
  stop_motor_button->setStyleSheet(
    ":enabled { background-color: red; color: white; font-weight: bold; }");

  run_motor_button = new QPushButton();
  run_motor_button->setObjectName("run_motor_button");
  run_motor_button->setText(tr("&Run motor"));
  run_motor_button->setStyleSheet(
    ":enabled { background-color: green; color: white; font-weight: bold; }");

  motor_status_value = new elided_label();

  apply_settings_button = new QPushButton();
  apply_settings_button->setObjectName("apply_settings");
  apply_settings_button->setText(tr("&Apply settings"));

  QHBoxLayout *footer_layout = new QHBoxLayout();
  footer_layout->addWidget(stop_motor_button, 0, Qt::AlignLeft);
  footer_layout->addWidget(run_motor_button, 0, Qt::AlignLeft);
  footer_layout->addWidget(motor_status_value, 1);
  footer_layout->addWidget(apply_settings_button, 0, Qt::AlignRight);

  QHBoxLayout *header_layout = new QHBoxLayout();
  header_layout->addWidget(device_list_label, 0);
  header_layout->addWidget(device_list_value, 0);
  header_layout->addWidget(connection_status_value, 1);

  main_window_layout->addLayout(header_layout, 0);
  main_window_layout->addWidget(tab_widget, 0);
  main_window_layout->addLayout(footer_layout, 0);

  connect(stop_motor_button, SIGNAL(clicked()),
    stop_motor_action, SLOT(trigger()));

  connect(run_motor_button, SIGNAL(clicked()),
    run_motor_action, SLOT(trigger()));

  connect(apply_settings_button, SIGNAL(clicked()),
    apply_settings_action, SLOT(trigger()));

  central_widget->setLayout(main_window_layout);

  QMetaObject::connectSlotsByName(this);
}

void main_window::setup_style_sheet()
{
  QString style_name = QApplication::style()->objectName();
  QString stylesheet;

  // Make buttons a little bit bigger so they're easier to click.  However, this
  // causes problems with the native Macintosh style, making the buttons
  // actually look narrower.
  if (style_name != "macintosh")
  {
    stylesheet += "QPushButton { padding: 0.3em 1em; }\n";
  }

  // By default, the fusion style makes the scroll bar look bad, having a border
  // on the top but no borders on the bottom.  This line seems to make it use a
  // totally different style which makes it look more like a normal Windows
  // scrollbar, and thus better.
  if (style_name == "fusion")
  {
    stylesheet += "QScrollBar#manual_target_scroll_bar { border: 0; }\n";
  }

  setStyleSheet(stylesheet);
}

void main_window::setup_menu_bar()
{
  menu_bar = new QMenuBar();
  setMenuBar(menu_bar);

  file_menu = menu_bar->addMenu("");
  file_menu->setTitle(tr("&File"));

  device_menu = menu_bar->addMenu("");
  device_menu->setTitle(tr("&Device"));

  window_menu = menu_bar->addMenu("");
  window_menu->setTitle(tr("&Window"));

  help_menu = menu_bar->addMenu("");
  help_menu->setTitle(tr("&Help"));

  open_settings_action = new QAction(this);
  open_settings_action->setObjectName("open_settings_action");
  open_settings_action->setText(tr("&Open settings file..."));
  open_settings_action->setShortcut(Qt::CTRL + Qt::Key_O);

  save_settings_action = new QAction(this);
  save_settings_action->setObjectName("save_settings_action");
  save_settings_action->setText(tr("&Save settings file..."));
  save_settings_action->setShortcut(Qt::CTRL + Qt::Key_S);

  exit_action = new QAction(this);
  exit_action->setObjectName("exit_action");
  exit_action->setText(tr("E&xit"));
  exit_action->setShortcut(QKeySequence::Quit);
  connect(exit_action, SIGNAL(triggered()), this, SLOT(close()));

  disconnect_action = new QAction(this);
  disconnect_action->setObjectName("disconnect_action");
  disconnect_action->setText(tr("&Disconnect"));
  disconnect_action->setShortcut(Qt::CTRL + Qt::Key_D);

  stop_motor_action = new QAction(this);
  stop_motor_action->setObjectName("stop_motor_action");
  stop_motor_action->setText(tr("&Stop motor"));

  run_motor_action = new QAction(this);
  run_motor_action->setObjectName("run_motor_action");
  run_motor_action->setText(tr("Run &motor"));

  clear_current_chopping_count_action = new QAction(this);
  clear_current_chopping_count_action->setObjectName(
    "clear_current_chopping_count_action");
  clear_current_chopping_count_action->setText(
    tr("&Clear current chopping count"));

  reload_settings_action = new QAction(this);
  reload_settings_action->setObjectName("reload_settings_action");
  reload_settings_action->setText(tr("Re&load settings from device"));

  restore_defaults_action = new QAction(this);
  restore_defaults_action->setObjectName("restore_defaults_action");
  restore_defaults_action->setText(tr("&Restore default settings"));

  apply_settings_action = new QAction(this);
  apply_settings_action->setObjectName("apply_settings_action");
  apply_settings_action->setText(tr("&Apply settings"));
  apply_settings_action->setShortcut(Qt::CTRL + Qt::Key_P);

  upgrade_firmware_action = new QAction(this);
  upgrade_firmware_action->setObjectName("upgrade_firmware_action");
  upgrade_firmware_action->setText(tr("&Upgrade firmware..."));

  graph_action = new QAction(this);
  graph_action->setObjectName("graph_action");
  graph_action->setText(tr("&Graph"));
  graph_action->setShortcut(Qt::CTRL + Qt::Key_G);
  connect(graph_action, SIGNAL(triggered()), this, SLOT(preview_pane_clicked()));

  documentation_action = new QAction(this);
  documentation_action->setObjectName("documentation_action");
  documentation_action->setText(tr("&Online documentation..."));
  documentation_action->setShortcut(QKeySequence::HelpContents);

  about_action = new QAction(this);
  about_action->setObjectName("about_action");
  about_action->setText(tr("&About..."));
  about_action->setShortcut(QKeySequence::WhatsThis);

  file_menu->addAction(open_settings_action);
  file_menu->addAction(save_settings_action);
  file_menu->addSeparator();
  file_menu->addAction(exit_action);

  device_menu->addAction(disconnect_action);
  device_menu->addSeparator();
  device_menu->addAction(stop_motor_action);
  device_menu->addAction(run_motor_action);
  device_menu->addAction(clear_current_chopping_count_action);
  device_menu->addSeparator();
  device_menu->addAction(reload_settings_action);
  device_menu->addAction(restore_defaults_action);
  device_menu->addAction(apply_settings_action);
  device_menu->addSeparator();
  device_menu->addAction(upgrade_firmware_action);

  window_menu->addAction(graph_action);

  help_menu->addAction(documentation_action);
  help_menu->addAction(about_action);
}

QSpacerItem * main_window::setup_vertical_spacer()
{
  return new QSpacerItem(1, fontMetrics().height());
}

// Sets up labels for a read-only text field.  This is intended for status
// displays that update frequently, so the value_size parameter is required for
// performance reasons.
static void setup_read_only_text_field(QGridLayout * layout,
  int row, int col, int value_col_span,
  const QSize & value_size, QLabel ** label, QLabel ** value)
{
  QLabel * new_value = new QLabel();
  new_value->setTextInteractionFlags(Qt::TextSelectableByMouse);
  new_value->setFixedSize(value_size);

  QLabel * new_label = new QLabel();
  new_label->setBuddy(new_value);

  layout->addWidget(new_label, row, col, Qt::AlignLeft);
  layout->addWidget(new_value, row, col + 1, 1, value_col_span, Qt::AlignLeft);

  if (label) { *label = new_label; }
  if (value) { *value = new_value; }
}

static void setup_read_only_text_field(QGridLayout * layout,
  int row, int col, const QSize & value_size,
  QLabel ** label, QLabel ** value)
{
  setup_read_only_text_field(layout, row, col, 1, value_size, label, value);
}

static void setup_read_only_text_field(QGridLayout * layout,
  int row, const QSize & value_size, QLabel ** label, QLabel ** value)
{
  setup_read_only_text_field(layout, row, 0, 1, value_size, label, value);
}

QWidget * main_window::setup_status_tab()
{
  status_page_widget = new QWidget();
  QGridLayout * layout = new QGridLayout();

  layout->addWidget(setup_variables_box(), 0, 0);
  layout->addWidget(setup_graph(), 0, 1);
  layout->addWidget(setup_manual_target_box(), 1, 0, 1, 3);

  layout->setRowStretch(2, 1);
  layout->setColumnStretch(1, 1);

  status_page_widget->setLayout(layout);
  return status_page_widget;
}

QWidget * main_window::setup_graph()
{
  graph = new graph_widget();
  graph->setObjectName(QStringLiteral("graph"));
  graph->set_preview_mode(true);

  preview_frame = new QFrame();
  preview_frame->setFrameStyle(QFrame::Box | QFrame::Plain);
  preview_frame->setLineWidth(1);

  horizontal_layout = new QHBoxLayout();
  horizontal_layout->addWidget(graph->custom_plot, 1);

  connect(graph->custom_plot, SIGNAL(mousePress(QMouseEvent*)), this,
    SLOT(preview_pane_clicked()));

  preview_frame->setLayout(horizontal_layout);

  return preview_frame;
}

QWidget * main_window::setup_variables_box()
{
  variables_box = new QGroupBox();
  variables_box->setTitle(tr("Variables"));  // TODO: better name?

  QGridLayout * layout = new QGridLayout();

  int row = 0;

  // We set all the value labels to have a fixed size for performance.
  // We set that size based on the largest text we expect any of
  // the labels to hold.
  QSize value_size = QLabel(tr("Software reset (bootloader)")).sizeHint();

  setup_read_only_text_field(layout, row++, value_size,
    &device_name_label, &device_name_value);
  device_name_value->setObjectName("device_name_value");
  device_name_value->setTextInteractionFlags(Qt::TextBrowserInteraction);
  device_name_label->setText(tr("Name:"));

  setup_read_only_text_field(layout, row++, value_size,
    &serial_number_label, &serial_number_value);
  serial_number_label->setText(tr("Serial number:"));

  setup_read_only_text_field(layout, row++, value_size,
    &firmware_version_label, &firmware_version_value);
  firmware_version_label->setText(tr("Firmware version:"));

  setup_read_only_text_field(layout, row++, value_size,
    &cmd_port_label, &cmd_port_value);
  cmd_port_label->setText(tr("Command port:"));

  setup_read_only_text_field(layout, row++, value_size,
    &ttl_port_label, &ttl_port_value);
  ttl_port_label->setText(tr("TTL port:"));

#ifdef __APPLE__
  {
    cmd_port_value->setText("/dev/cu.usbmodem01234567x");
    int width = cmd_port_value->sizeHint().width();
    cmd_port_value->setText("");
    cmd_port_value->setMinimumWidth(width);
    ttl_port_value->setMinimumWidth(width);
  }
#endif

  setup_read_only_text_field(layout, row++, value_size,
    &device_reset_label, &device_reset_value);
  device_reset_label->setText(tr("Last reset:"));

  setup_read_only_text_field(layout, row++, value_size,
    &up_time_label, &up_time_value);
  up_time_label->setText(tr("Up time:"));

  setup_read_only_text_field(layout, row++, value_size,
    &input_label, &input_value);
  input_label->setText(tr("Input:"));

  setup_read_only_text_field(layout, row++, value_size,
    &target_label, &target_value);
  target_label->setText(tr("Target:"));

  setup_read_only_text_field(layout, row++, value_size,
    &feedback_label, &feedback_value);
  feedback_label->setText(tr("Feedback:"));

  setup_read_only_text_field(layout, row++, value_size,
    &scaled_feedback_label, &scaled_feedback_value);
  scaled_feedback_label->setText(tr("Scaled feedback:"));

  setup_read_only_text_field(layout, row++, value_size,
    &error_label, &error_value);
  error_label->setText(tr("Error:"));

  setup_read_only_text_field(layout, row++, value_size,
    &integral_label, &integral_value);
  integral_label->setText(tr("Integral:"));

  setup_read_only_text_field(layout, row++, value_size,
    &duty_cycle_target_label, &duty_cycle_target_value);
  duty_cycle_target_label->setText(tr("Duty cycle target:"));

  setup_read_only_text_field(layout, row++, value_size,
    &duty_cycle_label, &duty_cycle_value);
  duty_cycle_label->setText(tr("Duty cycle:"));

  setup_read_only_text_field(layout, row++, value_size,
    &raw_current_label, &raw_current_value);
  raw_current_label->setText(tr("Raw current:"));

  setup_read_only_text_field(layout, row++, value_size,
    &current_label, &current_value);
  current_label->setText(tr("Current:"));

  // TODO: what kind of current chopping thing do we want to show here?
  // it's not really a log any more
  setup_read_only_text_field(layout, row++, value_size,
    &current_chopping_count_label, &current_chopping_count_value);
  current_chopping_count_label->setText(tr("Current chopping count:"));

  setup_read_only_text_field(layout, row++, value_size,
    &vin_voltage_label, &vin_voltage_value);
  vin_voltage_label->setText(tr("VIN voltage:"));

  setup_read_only_text_field(layout, row++, value_size,
    &pid_period_count_label, &pid_period_count_value);
  pid_period_count_label->setText(tr("PID period count:"));

  setup_read_only_text_field(layout, row++, value_size,
    &pid_period_exceeded_label, &pid_period_exceeded_value);
  pid_period_exceeded_label->setText(tr("PID period exceeded:"));

  setup_read_only_text_field(layout, row++, value_size,
    &error_flags_halting_label, &error_flags_halting_value);
  error_flags_halting_label->setText(tr("Errors:"));

  layout->setColumnStretch(2, 1);
  layout->setRowStretch(row, 1);
  variables_box->setLayout(layout);
  return variables_box;
}

QWidget * main_window::setup_manual_target_box()
{
  manual_target_box = new QGroupBox();
  manual_target_box->setTitle(tr("Manually set target (Serial mode only)"));
  QGridLayout * layout = new QGridLayout();

  manual_target_scroll_bar = new QScrollBar(Qt::Horizontal);
  manual_target_scroll_bar->setObjectName("manual_target_scroll_bar");
  manual_target_scroll_bar->setRange(0, 4095);
  manual_target_scroll_bar->setValue(2048);
  // Let the scroll bar be focused when people click on it, so they can
  // then press enter to set the target.
  manual_target_scroll_bar->setFocusPolicy(Qt::ClickFocus);

  manual_target_min_label = new QLabel("0");

  manual_target_max_label = new QLabel("4095");

  manual_target_entry_value = new QSpinBox();
  manual_target_entry_value->setObjectName("manual_target_entry_value");
  // Don't emit valueChanged events while user is typing (e.g. if the user
  // enters 500, we don't want to set targets of 5, 50, and 500).
  manual_target_entry_value->setKeyboardTracking(false);
  manual_target_entry_value->setRange(0, 4095);

  set_target_button = new QPushButton();
  set_target_button->setObjectName("set_target_button");
  set_target_button->setText(tr("Set &target"));

  auto_set_target_check = new QCheckBox();
  auto_set_target_check->setObjectName("auto_set_target_check");
  auto_set_target_check->setChecked(true);
  auto_set_target_check->setText(tr("Set target when slider or entry box are changed"));

  QHBoxLayout * spinbox_and_button = new QHBoxLayout();
  spinbox_and_button->addWidget(manual_target_entry_value, 0);
  spinbox_and_button->addWidget(set_target_button, 0);

  layout->addWidget(manual_target_scroll_bar, 0, 0, 1, 5);
  layout->addWidget(manual_target_min_label, 1, 0);
  layout->addLayout(spinbox_and_button, 1, 2);
  layout->addWidget(manual_target_max_label, 1, 4);

  // Align the checkbox to the left so that clicking far to the right of it
  // doesn't weirdly make it get the focus.
  layout->addWidget(auto_set_target_check, 2, 0, 1, 5, Qt::AlignLeft);

  layout->setRowStretch(2, 1);
  layout->setColumnStretch(1, 1);
  layout->setColumnStretch(3, 1);

  // Add shortcuts so we can take actions when enter/return is pressed.
  {
    manual_target_return_key_shortcut = new QShortcut(manual_target_box);
    manual_target_return_key_shortcut->setObjectName("manual_target_return_key_shortcut");
    manual_target_return_key_shortcut->setContext(Qt::WidgetWithChildrenShortcut);
    manual_target_return_key_shortcut->setKey(Qt::Key_Return);
    manual_target_enter_key_shortcut = new QShortcut(manual_target_box);
    manual_target_enter_key_shortcut->setObjectName("manual_target_enter_key_shortcut");
    manual_target_enter_key_shortcut->setContext(Qt::WidgetWithChildrenShortcut);
    manual_target_enter_key_shortcut->setKey(Qt::Key_Enter);

    // Handle both shortcuts with one slot.
    connect(manual_target_enter_key_shortcut, SIGNAL(activated()), this,
      SLOT(on_manual_target_return_key_shortcut_activated()));
  }

  manual_target_box->setLayout(layout);
  return manual_target_box;
}

QWidget * main_window::setup_input_tab()
{
  input_page_widget = new QWidget();

  input_mode_label = new QLabel(tr("Input mode:"));
  input_mode_label->setObjectName("input_mode_label");

  input_mode_combobox = new QComboBox();
  input_mode_combobox->setObjectName("input_mode_combobox");
  input_mode_combobox->addItem(
    "Serial\u2009/\u2009I\u00B2C\u2009/\u2009USB", JRK_INPUT_MODE_SERIAL);
  input_mode_combobox->addItem("Analog voltage", JRK_INPUT_MODE_ANALOG);
  input_mode_combobox->addItem("RC", JRK_INPUT_MODE_RC);

  QHBoxLayout *input_mode_layout = new QHBoxLayout();
  input_mode_layout->addWidget(input_mode_label);
  input_mode_layout->addWidget(input_mode_combobox);
  input_mode_layout->addStretch(1);

  QGridLayout *layout = input_page_layout = new QGridLayout();
  layout->addLayout(input_mode_layout, 0, 0);
  layout->addWidget(setup_input_analog_groupbox(), 1, 0);
  layout->addWidget(setup_input_serial_groupbox(), 2, 0);
  layout->addWidget(setup_input_scaling_groupbox(), 1, 1, 2, 1, Qt::AlignTop);
  layout->setRowStretch(3, 1);
  layout->setColumnStretch(2, 1);

  input_page_widget->setLayout(layout);

  input_page_widget->setParent(tab_widget);

  return input_page_widget;
}

static QComboBox * setup_exponent_combobox(int max_exponent)
{
  QComboBox * box = new QComboBox();
  int value = 1;
  for (int i = 0; i <= max_exponent; i++)
  {
    box->addItem(QString::number(value), i);
    value <<= 1;
  }
  return box;
}

QWidget * main_window::setup_input_analog_groupbox()
{
  input_analog_groupbox = new QGroupBox(tr("Analog input on SDA/AN"));
  input_analog_groupbox->setObjectName("input_analog_groupbox");

  input_analog_samples_label = new QLabel(tr("Analog samples:"));
  input_analog_samples_label->setObjectName("input_analog_samples_label");

  input_analog_samples_combobox = setup_exponent_combobox(10);
  input_analog_samples_combobox->setObjectName("input_analog_samples_combobox");

  input_detect_disconnect_checkbox = new QCheckBox(tr("Detect disconnect with power pin (SCL)"));
  input_detect_disconnect_checkbox->setObjectName("input_detect_disconnect_checkbox");

  QHBoxLayout *analog_samples = new QHBoxLayout();
  analog_samples->addWidget(input_analog_samples_label, 0, Qt::AlignLeft);
  analog_samples->addWidget(input_analog_samples_combobox, 0, Qt::AlignLeft);

  QGridLayout *input_analog_layout = new QGridLayout();
  input_analog_layout->addLayout(analog_samples, 0, 0, Qt::AlignLeft);
  input_analog_layout->addWidget(input_detect_disconnect_checkbox, 1, 0, Qt::AlignLeft);

  input_analog_groupbox->setLayout(input_analog_layout);

  return input_analog_groupbox;
}

QWidget * main_window::setup_input_serial_groupbox()
{
  input_serial_groupbox = new QGroupBox(tr("Serial interface"));
  input_serial_groupbox->setObjectName("input_serial_groupbox");

  input_usb_dual_port_radio = new QRadioButton(tr("USB dual port"));
  input_usb_dual_port_radio->setObjectName("input_usb_dual_port_radio");

  input_usb_chained_radio = new QRadioButton(tr("USB chained"));
  input_usb_chained_radio->setObjectName("input_usb_chained_radio");

  input_uart_fixed_baud_radio = new QRadioButton(tr("UART, fixed baud rate: "));
  input_uart_fixed_baud_radio->setObjectName("input_uart_fixed_baud_radio");

  input_uart_fixed_baud_spinbox = new QSpinBox();
  input_uart_fixed_baud_spinbox->setObjectName("input_uart_fixed_baud_spinbox");
  input_uart_fixed_baud_spinbox->setRange(JRK_MIN_ALLOWED_BAUD_RATE, JRK_MAX_ALLOWED_BAUD_RATE);

  input_serial_mode_button_group = new QButtonGroup(this);
  input_serial_mode_button_group->setExclusive(true);
  input_serial_mode_button_group->setObjectName("input_serial_mode_button_group");
  input_serial_mode_button_group->addButton(input_usb_dual_port_radio, JRK_SERIAL_MODE_USB_DUAL_PORT);
  input_serial_mode_button_group->addButton(input_usb_chained_radio, JRK_SERIAL_MODE_USB_CHAINED);
  input_serial_mode_button_group->addButton(input_uart_fixed_baud_radio, JRK_SERIAL_MODE_UART);

  input_enable_crc_checkbox = new QCheckBox(tr("Enable CRC"));
  input_enable_crc_checkbox->setObjectName("input_enable_crc_checkbox");

  input_device_label = new QLabel(tr("Device number:"));
  input_device_label->setObjectName("input_device_label");

  input_device_spinbox = new QSpinBox();
  input_device_spinbox->setObjectName("input_device_spinbox");
  input_device_spinbox->setMaximum(0x3FFF);

  input_device_number_checkbox = new QCheckBox(tr("Enable 14-bit device number"));
  input_device_number_checkbox->setObjectName("input_device_number_checkbox");

  input_timeout_label = new QLabel(tr("Timeout (s):"));
  input_timeout_label->setObjectName("input_timeout_label");

  input_timeout_spinbox = new QDoubleSpinBox();
  input_timeout_spinbox->setObjectName("input_timeout_spinbox");
  input_timeout_spinbox->setSingleStep(0.01);
  input_timeout_spinbox->setDecimals(2);
  input_timeout_spinbox->setRange(0, 655.35);

  input_disable_compact_protocol_checkbox = new QCheckBox(tr("Disable compact protocol"));
  input_disable_compact_protocol_checkbox->setObjectName("input_disable_compact_protocol_checkbox");

  QHBoxLayout *uart_fixed_baud = new QHBoxLayout();
  uart_fixed_baud->addWidget(input_uart_fixed_baud_radio, 0, Qt::AlignLeft);
  uart_fixed_baud->addWidget(input_uart_fixed_baud_spinbox, 0, Qt::AlignLeft);

  QHBoxLayout *device_layout = new QHBoxLayout();
  device_layout->addWidget(input_device_label, 0, Qt::AlignLeft);
  device_layout->addWidget(input_device_spinbox, 0, Qt::AlignLeft);

  QHBoxLayout *timeout_layout = new QHBoxLayout();
  timeout_layout->addWidget(input_timeout_label, 0, Qt::AlignLeft);
  timeout_layout->addWidget(input_timeout_spinbox, 0, Qt::AlignLeft);

  QGridLayout *input_serial_layout = new QGridLayout();
  input_serial_layout->addWidget(input_usb_dual_port_radio, 0, 0, Qt::AlignLeft);
  input_serial_layout->addWidget(input_usb_chained_radio, 1, 0, Qt::AlignLeft);
  input_serial_layout->addLayout(uart_fixed_baud, 2, 0, Qt::AlignLeft);
  input_serial_layout->addItem(setup_vertical_spacer(), 3, 0);
  input_serial_layout->addWidget(input_enable_crc_checkbox, 4, 0, Qt::AlignLeft);
  input_serial_layout->addLayout(device_layout, 5, 0, Qt::AlignLeft);
  input_serial_layout->addWidget(input_device_number_checkbox, 6, 0, Qt::AlignLeft);
  input_serial_layout->addLayout(timeout_layout, 7, 0, Qt::AlignLeft);
  input_serial_layout->addWidget(input_disable_compact_protocol_checkbox, 8, 0, Qt::AlignLeft);
  input_serial_layout->addItem(setup_vertical_spacer(), 9, 0);

  input_serial_groupbox->setLayout(input_serial_layout);

  return input_serial_groupbox;
}

QWidget * main_window::setup_input_scaling_groupbox()
{
  input_scaling_groupbox = new QGroupBox(tr("Scaling (analog and RC mode only)"));
  input_scaling_groupbox->setObjectName("input_scaling_groupbox");

  input_invert_checkbox = new QCheckBox(tr("Invert input direction"));
  input_invert_checkbox->setObjectName("input_invert_checkbox");

  input_error_max_label = new QLabel(tr("Error max:"));
  input_error_max_label->setObjectName("input_error_max_label");

  input_maximum_label = new QLabel(tr("Maximum:"));
  input_maximum_label->setObjectName("input_maximum_label");

  input_neutral_max_label = new QLabel(tr("Neutral max:"));
  input_neutral_max_label->setObjectName("input_neutral_max_label");

  input_neutral_min_label = new QLabel(tr("Neutral min:"));
  input_neutral_min_label->setObjectName("input_neutral_min_label");

  input_minimum_label = new QLabel(tr("Minimum:"));
  input_minimum_label->setObjectName("input_minimum_label");

  input_error_min_label = new QLabel(tr("Error min:"));
  input_error_min_label->setObjectName("input_error_min_label");

  input_degree_label = new QLabel(tr("Degree:"));
  input_degree_label->setObjectName("input_degree_label");

  input_input_label = new QLabel(tr("Input"));
  input_input_label->setObjectName("input_input_label");

  input_error_maximum_spinbox = new QSpinBox();
  input_error_maximum_spinbox->setObjectName("input_error_maximum_spinbox");
  input_error_maximum_spinbox->setRange(0, UINT12_MAX);
  input_error_maximum_spinbox->setValue(4095);

  input_maximum_spinbox = new QSpinBox();
  input_maximum_spinbox->setObjectName("input_maximum_spinbox");
  input_maximum_spinbox->setRange(0, UINT12_MAX);
  input_maximum_spinbox->setValue(4095);

  input_neutral_maximum_spinbox = new QSpinBox();
  input_neutral_maximum_spinbox->setObjectName("input_neutral_maximum_spinbox");
  input_neutral_maximum_spinbox->setRange(0, UINT12_MAX);
  input_neutral_maximum_spinbox->setValue(2048);;

  input_neutral_minimum_spinbox = new QSpinBox();
  input_neutral_minimum_spinbox->setObjectName("input_neutral_minimum_spinbox");
  input_neutral_minimum_spinbox->setRange(0, UINT12_MAX);
  input_neutral_minimum_spinbox->setValue(2048);

  input_minimum_spinbox = new QSpinBox();
  input_minimum_spinbox->setObjectName("input_minimum_spinbox");
  input_minimum_spinbox->setRange(0, UINT12_MAX);
  input_minimum_spinbox->setValue(4);

  input_error_minimum_spinbox = new QSpinBox();
  input_error_minimum_spinbox->setObjectName("input_error_minimum_spinbox");
  input_error_minimum_spinbox->setRange(0, UINT12_MAX);
  input_error_minimum_spinbox->setValue(0);

  input_scaling_degree_combobox = new QComboBox();
  input_scaling_degree_combobox->setObjectName("input_scaling_degree_combobox");
  input_scaling_degree_combobox->addItem("1 - Linear", JRK_SCALING_DEGREE_LINEAR);
  input_scaling_degree_combobox->addItem("2 - Quadratic", JRK_SCALING_DEGREE_QUADRATIC);
  input_scaling_degree_combobox->addItem("3 - Cubic", JRK_SCALING_DEGREE_CUBIC);
  input_scaling_degree_combobox->addItem("4 - Quartic", JRK_SCALING_DEGREE_QUARTIC);
  input_scaling_degree_combobox->addItem("5 - Quintic", JRK_SCALING_DEGREE_QUINTIC);

  input_target_label = new QLabel(tr("Target"));
  input_target_label->setObjectName("input_target_label");

  input_output_maximum_spinbox = new QSpinBox();
  input_output_maximum_spinbox->setObjectName("input_output_maximum_spinbox");
  input_output_maximum_spinbox->setRange(0, UINT12_MAX);
  input_output_maximum_spinbox->setValue(4095);

  input_output_neutral_spinbox = new QSpinBox();
  input_output_neutral_spinbox->setObjectName("input_output_neutral_spinbox");
  input_output_neutral_spinbox->setRange(0, UINT12_MAX);
  input_output_neutral_spinbox->setValue(2048);

  input_output_minimum_spinbox = new QSpinBox();
  input_output_minimum_spinbox->setObjectName("input_output_minimum_spinbox");
  input_output_minimum_spinbox->setRange(0, UINT12_MAX);
  input_output_minimum_spinbox->setValue(0);

  input_learn_button = new QPushButton();
  input_learn_button->setObjectName("input_learn_button");
  input_learn_button->setText(tr("&Learn..."));

  input_reset_range_button = new QPushButton();
  input_reset_range_button->setObjectName("input_reset_range_button");
  input_reset_range_button->setText(tr("R&eset to full range"));

  // used so layout does not change when item is hidden
  QSizePolicy p = sizePolicy();
  p.setRetainSizeWhenHidden(true);

  input_scaling_order_warning_label = new QLabel();
  input_scaling_order_warning_label->setObjectName("input_scaling_order_warning_label");
  input_scaling_order_warning_label->setStyleSheet("color: red;");
  input_scaling_order_warning_label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
  input_scaling_order_warning_label->setSizePolicy(p);
  input_scaling_order_warning_label->setText(tr(
    "Warning: Each input scaling\n"
    "setting should be less than or\n"
    "equal to the setting above it."));

  QGridLayout *input_scaling_layout = new QGridLayout();
  input_scaling_layout->addWidget(input_invert_checkbox, 0, 0, 1, 3, Qt::AlignLeft);
  input_scaling_layout->addWidget(input_input_label, 1, 1, Qt::AlignLeft);
  input_scaling_layout->addWidget(input_target_label, 1, 2, Qt::AlignLeft);
  input_scaling_layout->addWidget(input_error_max_label, 2, 0, Qt::AlignLeft);
  input_scaling_layout->addWidget(input_error_maximum_spinbox, 2, 1, Qt::AlignLeft);
  input_scaling_layout->addWidget(input_maximum_label, 3, 0, Qt::AlignLeft);
  input_scaling_layout->addWidget(input_maximum_spinbox, 3, 1, Qt::AlignLeft);
  input_scaling_layout->addWidget(input_output_maximum_spinbox, 3, 2, Qt::AlignLeft);
  input_scaling_layout->addWidget(input_neutral_max_label, 4, 0, Qt::AlignLeft);
  input_scaling_layout->addWidget(input_neutral_maximum_spinbox, 4, 1, Qt::AlignLeft);
  input_scaling_layout->addWidget(input_output_neutral_spinbox, 4, 2, 2, 1, Qt::AlignLeft);
  input_scaling_layout->addWidget(input_neutral_min_label, 5, 0, Qt::AlignLeft);
  input_scaling_layout->addWidget(input_neutral_minimum_spinbox, 5, 1, Qt::AlignLeft);
  input_scaling_layout->addWidget(input_minimum_label, 6, 0, Qt::AlignLeft);
  input_scaling_layout->addWidget(input_minimum_spinbox, 6, 1, Qt::AlignLeft);
  input_scaling_layout->addWidget(input_output_minimum_spinbox, 6, 2, Qt::AlignLeft);
  input_scaling_layout->addWidget(input_error_min_label, 7, 0, Qt::AlignLeft);
  input_scaling_layout->addWidget(input_error_minimum_spinbox, 7, 1, Qt::AlignLeft);
  input_scaling_layout->addItem(setup_vertical_spacer(), 8, 0);
  input_scaling_layout->addWidget(input_degree_label, 9, 0, Qt::AlignLeft);
  input_scaling_layout->addWidget(input_scaling_degree_combobox, 9, 1, 1, 2, Qt::AlignLeft);
  input_scaling_layout->addWidget(input_learn_button, 0, 3, Qt::AlignRight);
  input_scaling_layout->addWidget(input_reset_range_button, 1, 3, Qt::AlignRight);
  input_scaling_layout->addWidget(input_scaling_order_warning_label, 2, 3, 6, 1);

  input_scaling_groupbox->setLayout(input_scaling_layout);

  return input_scaling_groupbox;
}

QWidget * main_window::setup_feedback_tab()
{
  feedback_page_widget = new QWidget();

  feedback_mode_label = new QLabel(tr("Feedback mode:"));
  feedback_mode_label->setObjectName("feedback_mode_label");

  feedback_mode_combobox = new QComboBox();
  feedback_mode_combobox->setObjectName("feedback_mode_combobox");
  feedback_mode_combobox->addItem("None", JRK_FEEDBACK_MODE_NONE);
  feedback_mode_combobox->addItem("Analog voltage", JRK_FEEDBACK_MODE_ANALOG);
  feedback_mode_combobox->addItem("Frequency (speed control)",
    JRK_FEEDBACK_MODE_FREQUENCY);

  QHBoxLayout *feedback_mode_layout = new QHBoxLayout();
  feedback_mode_layout->addWidget(feedback_mode_label);
  feedback_mode_layout->addWidget(feedback_mode_combobox);

  QGridLayout *layout = feedback_page_layout = new QGridLayout();
  layout->setSizeConstraint(QLayout::SetFixedSize);
  layout->addLayout(feedback_mode_layout, 0, 0, Qt::AlignLeft);
  layout->addWidget(setup_feedback_scaling_groupbox(), 1, 0);
  layout->addWidget(setup_feedback_analog_groupbox(), 2, 0);
  layout->addWidget(setup_feedback_fbt_groupbox(), 1, 1);
  layout->setRowStretch(3, 1);

  feedback_page_widget->setLayout(layout);
  return feedback_page_widget;
}

QWidget * main_window::setup_feedback_scaling_groupbox()
{
  QSizePolicy p = sizePolicy();
  p.setRetainSizeWhenHidden(true);

  feedback_scaling_groupbox = new QGroupBox(tr("Scaling"));
  feedback_scaling_groupbox->setObjectName("feedback_scaling_groupbox");

  feedback_invert_checkbox = new QCheckBox(tr("Invert feedback direction"));
  feedback_invert_checkbox->setObjectName("feedback_invert_checkbox");

  feedback_error_max_label = new QLabel(tr("Error max:"));
  feedback_error_max_label->setObjectName("feedback_error_max_label");

  feedback_maximum_label = new QLabel(tr("Maximum:"));
  feedback_maximum_label->setObjectName("feedback_maximum_label");

  feedback_minimum_label = new QLabel(tr("Minimum:"));
  feedback_minimum_label->setObjectName("feedback_minimum_label");

  feedback_error_min_label = new QLabel(tr("Error min:"));
  feedback_error_min_label->setObjectName("feedback_error_min_label");

  feedback_calibration_label = new QLabel(tr("Calibration"));
  feedback_calibration_label->setObjectName("feedback_calibration_label");

  feedback_scaling_order_warning_label = new QLabel();
  feedback_scaling_order_warning_label->setObjectName("feedback_scaling_order_warning_label");
  feedback_scaling_order_warning_label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
  feedback_scaling_order_warning_label->setSizePolicy(p);
  feedback_scaling_order_warning_label->setStyleSheet("color: red;");
  feedback_scaling_order_warning_label->setText(tr(
    "Warning: Each feedback scaling\n"
    "setting should be less than or\n"
    "equal to the setting above it."));

  feedback_error_maximum_spinbox = new QSpinBox();
  feedback_error_maximum_spinbox->setObjectName("feedback_error_maximum_spinbox");
  feedback_error_maximum_spinbox->setRange(0, UINT12_MAX);

  feedback_maximum_spinbox = new QSpinBox();
  feedback_maximum_spinbox->setObjectName("feedback_maximum_spinbox");
  feedback_maximum_spinbox->setRange(0, UINT12_MAX);

  feedback_minimum_spinbox = new QSpinBox();
  feedback_minimum_spinbox->setObjectName("feedback_minimum_spinbox");
  feedback_minimum_spinbox->setRange(0, UINT12_MAX);

  feedback_error_minimum_spinbox = new QSpinBox();
  feedback_error_minimum_spinbox->setObjectName("feedback_error_minimum_spinbox");
  feedback_error_minimum_spinbox->setRange(0, UINT12_MAX);

  feedback_learn_button = new QPushButton(tr("&Learn..."));
  feedback_learn_button->setObjectName("feedback_learn_button");

  feedback_reset_range_button = new QPushButton(tr("R&eset to full range"));
  feedback_reset_range_button->setObjectName("feedback_reset_range_button");

  QGridLayout *feedback_scaling_layout = new QGridLayout();
  feedback_scaling_layout->addWidget(feedback_invert_checkbox, 0, 0, 1, 3, Qt::AlignLeft);
  feedback_scaling_layout->addWidget(feedback_calibration_label, 1, 1, Qt::AlignLeft);
  feedback_scaling_layout->addWidget(feedback_error_max_label, 2, 0, Qt::AlignLeft);
  feedback_scaling_layout->addWidget(feedback_error_maximum_spinbox, 2, 1, Qt::AlignLeft);
  feedback_scaling_layout->addWidget(feedback_maximum_label, 3, 0, Qt::AlignLeft);
  feedback_scaling_layout->addWidget(feedback_maximum_spinbox, 3, 1, Qt::AlignLeft);
  feedback_scaling_layout->addWidget(feedback_minimum_label, 4, 0, Qt::AlignLeft);
  feedback_scaling_layout->addWidget(feedback_minimum_spinbox, 4, 1, Qt::AlignLeft);
  feedback_scaling_layout->addWidget(feedback_error_min_label, 5, 0, Qt::AlignLeft);
  feedback_scaling_layout->addWidget(feedback_error_minimum_spinbox, 5, 1, Qt::AlignLeft);
  feedback_scaling_layout->addWidget(feedback_learn_button, 0, 3, Qt::AlignRight);
  feedback_scaling_layout->addWidget(feedback_reset_range_button, 1, 3, Qt::AlignRight);
  feedback_scaling_layout->addWidget(feedback_scaling_order_warning_label, 2, 2, 4, 2, Qt::AlignCenter);

  feedback_scaling_groupbox->setLayout(feedback_scaling_layout);

  return feedback_scaling_groupbox;
}

QWidget * main_window::setup_feedback_analog_groupbox()
{
  feedback_analog_groupbox = new QGroupBox(tr("Analog feedback on FBA"));
  feedback_analog_groupbox->setObjectName("feedback_analog_groupbox");

  feedback_analog_samples_label = new QLabel(tr("Analog samples:"));
  feedback_analog_samples_label->setObjectName("feedback_analog_samples_label");

  feedback_analog_samples_combobox = setup_exponent_combobox(10);
  feedback_analog_samples_combobox->setObjectName("feedback_analog_samples_combobox");

  feedback_detect_disconnect_checkbox =
    new QCheckBox(tr("Detect disconnect with power pin (AUX)"));
  feedback_detect_disconnect_checkbox->setObjectName(
    "feedback_detect_disconnect_checkbox");

  QHBoxLayout * analog_samples = new QHBoxLayout();
  analog_samples->addWidget(feedback_analog_samples_label, 0, Qt::AlignLeft);
  analog_samples->addWidget(feedback_analog_samples_combobox, 0, Qt::AlignLeft);

  QGridLayout * layout = new QGridLayout();
  layout->addLayout(analog_samples, 0, 0, Qt::AlignLeft);
  layout->addWidget(feedback_detect_disconnect_checkbox, 1, 0, Qt::AlignLeft);

  feedback_wraparound_checkbox = new QCheckBox(tr("Wraparound"));
  feedback_wraparound_checkbox->setObjectName("feedback_wraparound_checkbox");
  feedback_wraparound_checkbox->setToolTip(tr(
    "A scaled feedback value of 0 is considered to be adjacent to 4095.  "
    "Suitable for systems that rotate over a full circle."));
  layout->addWidget(feedback_wraparound_checkbox, 2, 0, Qt::AlignLeft);

  feedback_analog_groupbox->setLayout(layout);

  return feedback_analog_groupbox;
}

QWidget * main_window::setup_feedback_fbt_groupbox()
{
  QGroupBox * fbt_groupbox = new QGroupBox();
  fbt_groupbox->setObjectName("fbt_groupbox");
  fbt_groupbox->setTitle(tr("Frequency feedback on FBT"));

  QLabel * fbt_method_label = new QLabel();
  fbt_method_label->setObjectName("fbt_method_label");
  fbt_method_label->setText("Measurement method:");

  fbt_method_combobox = new QComboBox();
  fbt_method_combobox->setObjectName("fbt_method_combobox");
  fbt_method_combobox->addItem("Pulse counting", JRK_FBT_METHOD_PULSE_COUNTING);
  fbt_method_combobox->addItem("Pulse timing", JRK_FBT_METHOD_PULSE_TIMING);

  fbt_timing_clock_label = new QLabel();
  fbt_timing_clock_label->setObjectName("fbt_timing_clock_label");
  fbt_timing_clock_label->setText("Pulse timing clock:");

  fbt_timing_clock_combobox = new QComboBox();
  fbt_timing_clock_combobox->setObjectName("fbt_timing_clock_combobox");
  fbt_timing_clock_combobox->addItem("1.5 MHz", JRK_FBT_TIMING_CLOCK_1_5);
  fbt_timing_clock_combobox->addItem("3 MHz", JRK_FBT_TIMING_CLOCK_3);
  fbt_timing_clock_combobox->addItem("6 MHz", JRK_FBT_TIMING_CLOCK_6);
  fbt_timing_clock_combobox->addItem("12 MHz", JRK_FBT_TIMING_CLOCK_12);
  fbt_timing_clock_combobox->addItem("24 MHz", JRK_FBT_TIMING_CLOCK_24);
  fbt_timing_clock_combobox->addItem("48 MHz", JRK_FBT_TIMING_CLOCK_48);

  fbt_timing_polarity_label = new QLabel();
  fbt_timing_polarity_label->setObjectName("fbt_timing_polarity_label");
  fbt_timing_polarity_label->setText("Pulse timing polarity:");

  fbt_timing_polarity_combobox = new QComboBox();
  fbt_timing_polarity_combobox->setObjectName("fbt_timing_polarity_combobox");
  fbt_timing_polarity_combobox->addItem("Active high", 0);
  fbt_timing_polarity_combobox->addItem("Active low", 1);

  fbt_timing_timeout_label = new QLabel();
  fbt_timing_timeout_label->setObjectName("fbt_timing_timeout_label");
  fbt_timing_timeout_label->setText("Pulse timing timeout (ms):");

  fbt_timing_timeout_spinbox = new QSpinBox();
  fbt_timing_timeout_spinbox->setObjectName("fbt_timing_timeout_spinbox");
  fbt_timing_timeout_spinbox->setRange(1, 60000);

  QLabel * fbt_samples_label = new QLabel();
  fbt_samples_label->setObjectName("fbt_samples_label");
  fbt_samples_label->setText("Pulse samples:");

  fbt_samples_spinbox = new QSpinBox();
  fbt_samples_spinbox->setObjectName("fbt_samples_spinbox");
  fbt_samples_spinbox->setRange(1, JRK_MAX_ALLOWED_FBT_SAMPLES);

  fbt_divider_label = new QLabel();
  fbt_divider_label->setObjectName("fbt_divider_label");
  fbt_divider_label->setText("Frequency divider:");

  fbt_divider_combobox = setup_exponent_combobox(15);
  fbt_divider_combobox->setObjectName("fbt_divider_combobox");

  fbt_range_label = new QLabel();
  fbt_range_label->setObjectName("feedback_range_label");
  fbt_range_label->setTextInteractionFlags(Qt::TextSelectableByMouse);
  fbt_range_label->setText(
    "Frequency Measurement Range (at 50% duty cycle): 9.99 MHz to 9.99 MHz  ");
  fbt_range_label->setMinimumWidth(fbt_range_label->sizeHint().width());
  fbt_range_label->setText("");

  QGridLayout * layout = new QGridLayout();
  layout->addWidget(fbt_method_label, 0, 0, Qt::AlignLeft);
  layout->addWidget(fbt_method_combobox, 0, 1, Qt::AlignLeft);
  layout->addWidget(fbt_timing_clock_label, 1, 0, Qt::AlignLeft);
  layout->addWidget(fbt_timing_clock_combobox, 1, 1, Qt::AlignLeft);
  layout->addWidget(fbt_timing_polarity_label, 2, 0, Qt::AlignLeft);
  layout->addWidget(fbt_timing_polarity_combobox, 2, 1, Qt::AlignLeft);
  layout->addWidget(fbt_timing_timeout_label, 3, 0, Qt::AlignLeft);
  layout->addWidget(fbt_timing_timeout_spinbox, 3, 1, Qt::AlignLeft);
  layout->addWidget(fbt_samples_label, 4, 0, Qt::AlignLeft);
  layout->addWidget(fbt_samples_spinbox, 4, 1, Qt::AlignLeft);
  layout->addWidget(fbt_divider_label, 5, 0, Qt::AlignLeft);
  layout->addWidget(fbt_divider_combobox, 5, 1, Qt::AlignLeft);
  layout->addWidget(fbt_range_label, 6, 0, 1, 3, Qt::AlignLeft);
  layout->setColumnStretch(2, 1);

  fbt_groupbox->setLayout(layout);

  return fbt_groupbox;
}

QWidget * main_window::setup_pid_tab()
{
  pid_page_widget = new QWidget();

  pid_proportional_control = new pid_constant_control();
  pid_proportional_control->setObjectName("pid_proportional_control");
  pid_proportional_control->setTitle("Proportional coefficient");

  pid_integral_control = new pid_constant_control();
  pid_integral_control->setObjectName("pid_integral_control");
  pid_integral_control->setTitle("Integral coefficient");

  pid_derivative_control = new pid_constant_control();
  pid_derivative_control->setObjectName("pid_derivative_control");
  pid_derivative_control->setTitle("Derivative coefficient");

  pid_period_label = new QLabel(tr("PID period (ms):"));
  pid_period_label->setObjectName("pid_period_label");

  pid_period_spinbox = new QSpinBox();
  pid_period_spinbox->setObjectName("pid_period_spinbox");
  pid_period_spinbox->setRange(0, 8191);

  integral_limit_label = new QLabel(tr("Integral limit:"));
  integral_limit_label->setObjectName("integral_limit_label");

  integral_limit_spinbox = new QSpinBox();
  integral_limit_spinbox->setObjectName("integral_limit_spinbox");
  integral_limit_spinbox->setRange(0, 0x7FFF);

  integral_divider_label = new QLabel(tr("Integral divider:"));
  integral_divider_label->setObjectName("integral_divider_label");

  integral_divider_combobox = setup_exponent_combobox(15);
  integral_divider_combobox->setObjectName("integral_divider_combobox");

  reset_integral_checkbox = new QCheckBox(
    tr("Reset integral when proportional term exceeds max duty cycle"));
  reset_integral_checkbox->setObjectName("reset_integral_checkbox");

  feedback_dead_zone_label = new QLabel(tr("Feedback dead zone:"));
  feedback_dead_zone_label->setObjectName("feedback_dead_zone_label");

  feedback_dead_zone_spinbox = new QSpinBox();
  feedback_dead_zone_spinbox->setObjectName("feedback_dead_zone_spinbox");
  feedback_dead_zone_spinbox->setRange(0, 255);

  QHBoxLayout * coefficient_layout = new QHBoxLayout();
  coefficient_layout->addWidget(pid_proportional_control);
  coefficient_layout->addWidget(pid_integral_control);
  coefficient_layout->addWidget(pid_derivative_control);
  coefficient_layout->setAlignment(Qt::AlignLeft);
  coefficient_layout->addStretch(1);

  QHBoxLayout * period_layout = new QHBoxLayout();
  period_layout->addWidget(pid_period_label);
  period_layout->addWidget(pid_period_spinbox);
  period_layout->addStretch(1);

  QHBoxLayout * integral_layout = new QHBoxLayout();
  integral_layout->addWidget(integral_limit_label);
  integral_layout->addWidget(integral_limit_spinbox);
  integral_layout->addStretch(1);

  QHBoxLayout * integral_divider_layout = new QHBoxLayout();
  integral_divider_layout->addWidget(integral_divider_label);
  integral_divider_layout->addWidget(integral_divider_combobox);
  integral_divider_layout->addStretch(1);

  QHBoxLayout * deadzone_layout = new QHBoxLayout();
  deadzone_layout->addWidget(feedback_dead_zone_label);
  deadzone_layout->addWidget(feedback_dead_zone_spinbox);
  deadzone_layout->addStretch(1);

  QVBoxLayout * layout = new QVBoxLayout();
  layout->addLayout(coefficient_layout);
  layout->addLayout(period_layout);
  layout->addLayout(integral_layout);
  layout->addLayout(integral_divider_layout);
  layout->addWidget(reset_integral_checkbox);
  layout->addLayout(deadzone_layout);
  layout->addStretch(1);

  pid_page_widget->setLayout(layout);
  return pid_page_widget;
}

QWidget *main_window::setup_motor_tab()
{
  motor_page_widget = new QWidget();

  pwm_frequency_label = new QLabel(tr("PWM frequency:"));
  pwm_frequency_label->setObjectName("pwm_frequency_label");

  pwm_frequency_combobox = new QComboBox();
  pwm_frequency_combobox->setObjectName("pwm_frequency_combobox");
  pwm_frequency_combobox->addItem("20 kHz", JRK_PWM_FREQUENCY_20);
  pwm_frequency_combobox->addItem("5 kHz", JRK_PWM_FREQUENCY_5);

  motor_invert_checkbox = new QCheckBox(tr("Invert motor direction"));
  motor_invert_checkbox->setObjectName("motor_invert_checkbox");

  detect_motor_button = new QPushButton(tr("&Learn..."));
  detect_motor_button->setObjectName("detect_motor_button");
  connect(detect_motor_button, &QPushButton::clicked,
    this, &main_window::on_feedback_learn_button_clicked);

  QGridLayout *motor_controls_layout = new QGridLayout();

  motor_asymmetric_checkbox = new QCheckBox(tr("Asymmetric"));
  motor_asymmetric_checkbox->setObjectName("motor_asymmetric_checkbox");

  motor_forward_label = new QLabel(tr("Forward"));
  motor_forward_label->setObjectName("motor_forward_label");

  motor_reverse_label = new QLabel(tr("Reverse"));
  motor_reverse_label->setObjectName("motor_reverse_label");

  max_duty_cycle_label = new QLabel(tr("Max. duty cycle:"));
  max_duty_cycle_label->setObjectName("max_deceleration_label");

  max_duty_cycle_forward_spinbox = new QSpinBox();
  max_duty_cycle_forward_spinbox->setObjectName("max_duty_cycle_forward_spinbox");
  max_duty_cycle_forward_spinbox->setRange(0, JRK_MAX_ALLOWED_DUTY_CYCLE);

  max_duty_cycle_reverse_spinbox = new QSpinBox();
  max_duty_cycle_reverse_spinbox->setObjectName("max_duty_cycle_reverse_spinbox");
  max_duty_cycle_reverse_spinbox->setRange(0, JRK_MAX_ALLOWED_DUTY_CYCLE);

  max_duty_cycle_means_label = new QLabel(tr("(600 means 100%)"));

  max_acceleration_label = new QLabel(tr("Max. acceleration:"));
  max_acceleration_label->setObjectName("max_acceleration_label");

  max_acceleration_forward_spinbox = new QSpinBox();
  max_acceleration_forward_spinbox->setObjectName("max_acceleration_forward_spinbox");
  max_acceleration_forward_spinbox->setRange(0, JRK_MAX_ALLOWED_DUTY_CYCLE);

  max_acceleration_reverse_spinbox = new QSpinBox();
  max_acceleration_reverse_spinbox->setObjectName("max_acceleration_reverse_spinbox");
  max_acceleration_reverse_spinbox->setRange(0, JRK_MAX_ALLOWED_DUTY_CYCLE);

  max_acceleration_means_label = new QLabel(tr("(600 means no limit)"));

  max_deceleration_label = new QLabel(tr("Max. deceleration:"));
  max_deceleration_label->setObjectName("max_deceleration_label");

  max_deceleration_forward_spinbox = new QSpinBox();
  max_deceleration_forward_spinbox->setObjectName("max_deceleration_forward_spinbox");
  max_deceleration_forward_spinbox->setRange(1, JRK_MAX_ALLOWED_DUTY_CYCLE);

  max_deceleration_reverse_spinbox = new QSpinBox();
  max_deceleration_reverse_spinbox->setObjectName("max_deceleration_reverse_spinbox");
  max_deceleration_reverse_spinbox->setRange(1, JRK_MAX_ALLOWED_DUTY_CYCLE);

  max_deceleration_means_label = new QLabel(tr("(600 means no limit)"));

  brake_duration_label = new QLabel(tr("Brake duration (ms):"));
  brake_duration_label->setObjectName("brake_duration_label");

  brake_duration_forward_spinbox = new QSpinBox();
  brake_duration_forward_spinbox->setObjectName("brake_duration_forward_spinbox");
  brake_duration_forward_spinbox->setRange(0, JRK_MAX_ALLOWED_BRAKE_DURATION);
  brake_duration_forward_spinbox->setSingleStep(JRK_BRAKE_DURATION_UNITS);

  brake_duration_reverse_spinbox = new QSpinBox();
  brake_duration_reverse_spinbox->setObjectName("brake_duration_reverse_spinbox");
  brake_duration_reverse_spinbox->setRange(0, JRK_MAX_ALLOWED_BRAKE_DURATION);
  brake_duration_reverse_spinbox->setSingleStep(JRK_BRAKE_DURATION_UNITS);

  // TODO: change the variable names to hard_current_limit too
  current_limit_label = new QLabel(tr("Hard current limit (A):"));
  current_limit_label->setObjectName("current_limit_label");

  size_t minimum_current_box_width;
  {
    QSpinBox tmp_box;
    tmp_box.setSpecialValueText("999.99 A");
    minimum_current_box_width = tmp_box.sizeHint().width();
  }

  current_limit_forward_spinbox = new nice_spin_box();
  current_limit_forward_spinbox->setObjectName("current_limit_forward_spinbox");
  current_limit_forward_spinbox->set_decimals(2);
  current_limit_forward_spinbox->setMinimumWidth(minimum_current_box_width);

  current_limit_reverse_spinbox = new nice_spin_box();
  current_limit_reverse_spinbox->setObjectName("current_limit_reverse_spinbox");
  current_limit_reverse_spinbox->set_decimals(2);
  current_limit_reverse_spinbox->setMinimumWidth(minimum_current_box_width);

  // TODO: change the variable names to soft_current_limit too
  max_current_label = new QLabel(tr("Soft current limit (A):"));
  max_current_label->setObjectName("max_current_label");

  max_current_forward_spinbox = new nice_spin_box();
  max_current_forward_spinbox->setObjectName("max_current_forward_spinbox");
  max_current_forward_spinbox->set_decimals(3);
  max_current_forward_spinbox->setRange(0, 65535);
  max_current_forward_spinbox->setMinimumWidth(minimum_current_box_width);

  max_current_reverse_spinbox = new nice_spin_box();
  max_current_reverse_spinbox->setObjectName("max_current_reverse_spinbox");
  max_current_reverse_spinbox->set_decimals(3);
  max_current_reverse_spinbox->setRange(0, 65535);
  max_current_reverse_spinbox->setMinimumWidth(minimum_current_box_width);

  max_current_means_label = new QLabel(tr("(0 means no limit)"));
  max_current_means_label->setObjectName("max_current_means_label");

  current_offset_calibration_label = new QLabel(tr("Current offset calibration:"));
  current_offset_calibration_label->setObjectName("current_offset_calibration_label");

  current_offset_calibration_spinbox = new QSpinBox();
  current_offset_calibration_spinbox->setObjectName("current_offset_calibration_spinbox");
  current_offset_calibration_spinbox->setRange(-800, 800);

  current_scale_calibration_label = new QLabel(tr("Current scale calibration:"));
  current_scale_calibration_label->setObjectName("current_scale_calibration_label");

  current_scale_calibration_spinbox = new QSpinBox();
  current_scale_calibration_spinbox->setObjectName("current_scale_calibration_spinbox");
  current_scale_calibration_spinbox->setRange(-1875, 1875);

  current_samples_label = new QLabel(tr("Current samples:"));
  current_samples_label->setObjectName("current_samples_label");

  current_samples_combobox = setup_exponent_combobox(10);
  current_samples_combobox->setObjectName("current_samples_combobox");

  overcurrent_threshold_label = new QLabel(tr("Hard overcurrent threshold:"));
  overcurrent_threshold_label->setObjectName("overcurrent_threshold_label");

  overcurrent_threshold_spinbox = new QSpinBox();
  overcurrent_threshold_spinbox->setObjectName("overcurrent_threshold_spinbox");
  overcurrent_threshold_spinbox->setRange(1, 255);

  int row = 0;
  motor_controls_layout->addWidget(motor_asymmetric_checkbox, row, 2, 1, 2);
  motor_controls_layout->addWidget(motor_forward_label, ++row, 1);
  motor_controls_layout->addWidget(motor_reverse_label, row, 2);
  motor_controls_layout->addWidget(max_duty_cycle_label, ++row, 0);
  motor_controls_layout->addWidget(max_duty_cycle_forward_spinbox, row, 1);
  motor_controls_layout->addWidget(max_duty_cycle_reverse_spinbox, row, 2);
  motor_controls_layout->addWidget(max_duty_cycle_means_label, row, 3);
  motor_controls_layout->addWidget(max_acceleration_label, ++row, 0);
  motor_controls_layout->addWidget(max_acceleration_forward_spinbox, row, 1);
  motor_controls_layout->addWidget(max_acceleration_reverse_spinbox, row, 2);
  motor_controls_layout->addWidget(max_acceleration_means_label, row, 3);
  motor_controls_layout->addWidget(max_deceleration_label, ++row, 0);
  motor_controls_layout->addWidget(max_deceleration_forward_spinbox, row, 1);
  motor_controls_layout->addWidget(max_deceleration_reverse_spinbox, row, 2);
  motor_controls_layout->addWidget(max_deceleration_means_label, row, 3);
  motor_controls_layout->addWidget(brake_duration_label, ++row, 0);
  motor_controls_layout->addWidget(brake_duration_forward_spinbox, row, 1);
  motor_controls_layout->addWidget(brake_duration_reverse_spinbox, row, 2);
  motor_controls_layout->addWidget(current_limit_label, ++row, 0);
  motor_controls_layout->addWidget(current_limit_forward_spinbox, row, 1);
  motor_controls_layout->addWidget(current_limit_reverse_spinbox, row, 2);
  motor_controls_layout->addWidget(max_current_label, ++row, 0);
  motor_controls_layout->addWidget(max_current_forward_spinbox, row, 1);
  motor_controls_layout->addWidget(max_current_reverse_spinbox, row, 2);
  motor_controls_layout->addWidget(max_current_means_label, row, 3);
  motor_controls_layout->addItem(setup_vertical_spacer(), ++row, 0);
  motor_controls_layout->addWidget(current_offset_calibration_label, ++row, 0);
  motor_controls_layout->addWidget(current_offset_calibration_spinbox, row, 1);
  motor_controls_layout->addWidget(current_scale_calibration_label, ++row, 0);
  motor_controls_layout->addWidget(current_scale_calibration_spinbox, row, 1);
  motor_controls_layout->addWidget(current_samples_label, ++row, 0);
  motor_controls_layout->addWidget(current_samples_combobox, row, 1);
  motor_controls_layout->addWidget(overcurrent_threshold_label, ++row, 0);
  motor_controls_layout->addWidget(overcurrent_threshold_spinbox, row, 1);

  motor_controls_layout->setAlignment(Qt::AlignLeft);

  max_duty_cycle_while_feedback_out_of_range_label =
    new QLabel(tr("Max. duty cycle while feedback is out of range:"));
  max_duty_cycle_while_feedback_out_of_range_label->setObjectName(
    "max_duty_cycle_while_feedback_out_of_range_label");

  max_duty_cycle_while_feedback_out_of_range_spinbox = new QSpinBox();
  max_duty_cycle_while_feedback_out_of_range_spinbox->setObjectName(
    "max_duty_cycle_while_feedback_out_of_range_spinbox");
  max_duty_cycle_while_feedback_out_of_range_spinbox->setRange(1, 600);

  max_duty_cycle_while_feedback_out_of_range_means_label =
    new QLabel(tr("(600 means 100%)"));

  motor_off_label = new QLabel(tr("When motor is off:"));
  motor_off_label->setObjectName("motor_off_label");

  motor_brake_radio = new QRadioButton(tr("Brake"));
  motor_brake_radio->setObjectName("motor_brake_radio");
  motor_brake_radio->setChecked(true);

  motor_coast_radio = new QRadioButton(tr("Coast"));
  motor_coast_radio->setObjectName("motor_coast_radio");
  motor_coast_radio->setChecked(false);

  coast_when_off_button_group = new QButtonGroup(this);
  coast_when_off_button_group->setExclusive(true);
  coast_when_off_button_group->setObjectName("coast_when_off_button_group");
  coast_when_off_button_group->addButton(motor_brake_radio, 0);
  coast_when_off_button_group->addButton(motor_coast_radio, 1);

  QHBoxLayout *frequency_layout = new QHBoxLayout();
  frequency_layout->addWidget(pwm_frequency_label);
  frequency_layout->addWidget(pwm_frequency_combobox);
  frequency_layout->addStretch(1);;

  QHBoxLayout *invert_layout = new QHBoxLayout();
  invert_layout->addWidget(motor_invert_checkbox);
  invert_layout->addWidget(detect_motor_button);
  invert_layout->addStretch(1);

  QHBoxLayout *deceleration_layout = new QHBoxLayout();
  deceleration_layout->addWidget(
    max_duty_cycle_while_feedback_out_of_range_label);
  deceleration_layout->addWidget(
    max_duty_cycle_while_feedback_out_of_range_spinbox);
  deceleration_layout->addWidget(
    max_duty_cycle_while_feedback_out_of_range_means_label);
  deceleration_layout->addStretch(1);

  QGridLayout *motor_off_layout = new QGridLayout();
  motor_off_layout->addWidget(motor_off_label,0,0);
  motor_off_layout->addWidget(motor_brake_radio,0,1);
  motor_off_layout->addWidget(motor_coast_radio,1,1);
  motor_off_layout->setColumnStretch(2, 1);

  QVBoxLayout *layout = motor_page_layout = new QVBoxLayout();
  layout->setSizeConstraint(QLayout::SetFixedSize);
  layout->addLayout(frequency_layout);
  layout->addLayout(invert_layout);
  layout->addItem(setup_vertical_spacer());
  layout->addLayout(motor_controls_layout);
  layout->addLayout(deceleration_layout);
  layout->addLayout(motor_off_layout);

  motor_page_widget->setLayout(layout);

  return motor_page_widget;
}

QWidget *main_window::setup_errors_tab()
{
  errors_page_widget = new QWidget();

  QFont font;
  font.setBold(true);

  // Note: We are setting a bunch of hardcoded margins/spacings here.
  // It would probably be better to calculate that somehow.

  errors_bit_mask_label = new QLabel(tr("Bit mask"));
  errors_bit_mask_label->setObjectName("errors_bit_mask_label");
  errors_bit_mask_label->setFont(font);
  errors_bit_mask_label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  errors_bit_mask_label->setContentsMargins(5, 0, 5, 0);

  errors_error_label = new QLabel(tr("Error"));
  errors_error_label->setObjectName("errors_error_label");
  errors_error_label->setFont(font);
  errors_error_label->setAlignment(Qt::AlignCenter);
  errors_error_label->setContentsMargins(5, 0, 5, 0);

  errors_setting_label = new QLabel(tr("Setting"));
  errors_setting_label->setObjectName("errors_setting_label");
  errors_setting_label->setFont(font);
  errors_setting_label->setAlignment(Qt::AlignCenter);

  errors_hard_label = new QLabel(tr("Hard\nstop?"));
  errors_hard_label->setObjectName("errors_hard_label");
  errors_hard_label->setFont(font);
  errors_hard_label->setAlignment(Qt::AlignCenter);

  errors_stopping_motor_label = new QLabel(tr("Currently\nstopping motor?"));
  errors_stopping_motor_label->setObjectName("errors_stopping_motor_label");
  errors_stopping_motor_label->setAlignment(Qt::AlignCenter);
  errors_stopping_motor_label->setFont(font);
  errors_stopping_motor_label->setContentsMargins(5, 0, 5, 0);

  errors_occurrence_count_label = new QLabel(tr("Occurrence\ncount"));
  errors_occurrence_count_label->setObjectName("errors_occurrence_count_label");
  errors_occurrence_count_label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  errors_occurrence_count_label->setFont(font);
  errors_occurrence_count_label->setContentsMargins(5, 0, 5, 0);

  errors_clear_errors = new QPushButton(tr("&Clear errors"));
  errors_clear_errors->setObjectName("errors_clear_errors");

  errors_reset_counts = new QPushButton(tr("Reset c&ounts"));
  errors_reset_counts->setObjectName("errors_reset_counts");

  QGridLayout * layout = errors_page_layout = new QGridLayout();
  layout->setSizeConstraint(QLayout::SetFixedSize);
  layout->setVerticalSpacing(2);
  layout->addWidget(errors_bit_mask_label, 0, 0);
  layout->addWidget(errors_error_label, 0, 1);
  layout->addWidget(errors_setting_label, 0, 2, 1, 3);
  layout->addWidget(errors_hard_label, 0, 5);
  layout->addWidget(errors_stopping_motor_label, 0, 6);
  layout->addWidget(errors_occurrence_count_label, 0, 7);

  // Note: We have to do this before calling setup_error_row or else Qt shows
  // small spurious windows before the main window opens.
  errors_page_widget->setLayout(layout);

  setup_error_row(JRK_ERROR_AWAITING_COMMAND, true, true, false);
  setup_error_row(JRK_ERROR_NO_POWER, true, false, true);
  setup_error_row(JRK_ERROR_MOTOR_DRIVER, true, false, true);
  setup_error_row(JRK_ERROR_INPUT_INVALID, true, false, false);
  setup_error_row(JRK_ERROR_INPUT_DISCONNECT, false, false, false);
  setup_error_row(JRK_ERROR_FEEDBACK_DISCONNECT, false, false, false);
  setup_error_row(JRK_ERROR_SOFT_OVERCURRENT, false, false, false);
  setup_error_row(JRK_ERROR_SERIAL_SIGNAL, false, true, false);
  setup_error_row(JRK_ERROR_SERIAL_OVERRUN, false, true, false);
  setup_error_row(JRK_ERROR_SERIAL_BUFFER_FULL, false, true, false);
  setup_error_row(JRK_ERROR_SERIAL_CRC, false, true, false);
  setup_error_row(JRK_ERROR_SERIAL_PROTOCOL, false, true, false);
  setup_error_row(JRK_ERROR_SERIAL_TIMEOUT, false, true, false);
  setup_error_row(JRK_ERROR_HARD_OVERCURRENT, false, false, false);

  int last_row = layout->rowCount();
  layout->addWidget(errors_clear_errors, last_row, 6, 1, 1, Qt::AlignCenter);
  layout->addWidget(errors_reset_counts, last_row, 7, 1, 1, Qt::AlignLeft);

  layout->setRowStretch(last_row + 1, 1);

  return errors_page_widget;
}

static void copy_margins(QWidget * dest, const QWidget * src)
{
  int left, top, right, bottom;
  src->getContentsMargins(&left, &top, &right, &bottom);
  dest->setContentsMargins(left, top, right, bottom);
}

void main_window::setup_error_row(int error_number,
  bool always_enabled, bool always_latched, bool always_hard)
{
  error_rows.append(error_row());
  error_row & row = error_rows.last();

  row.error_number = error_number;
  row.always_enabled = always_enabled;
  row.always_latched = always_latched;
  row.always_hard = always_hard;

  uint16_t error_mask = 1 << error_number;

  // The frame widget is necessary for setting the background color of the row.
  row.frame = new QWidget();

  if (error_number % 2)
  {
    row.frame->setStyleSheet("background-color: rgb(230,229,229);");
  }

  row.bit_mask_label = new QLabel();
  row.bit_mask_label->setObjectName("bit_mask_label");
  row.bit_mask_label->setAlignment(Qt::AlignLeft);
  row.bit_mask_label->setText(QString::fromStdString(
    convert_error_flags_to_hex_string(error_mask)));
  copy_margins(row.bit_mask_label, errors_bit_mask_label);

  row.error_label = new QLabel();
  row.error_label->setObjectName("error_label");
  row.error_label->setAlignment(Qt::AlignLeft);
  row.error_label->setText(jrk_look_up_error_name_ui(error_mask));
  copy_margins(row.error_label, errors_error_label);

  row.disabled_radio = new QRadioButton(tr("Disabled"));
  row.disabled_radio->setObjectName("disabled_radio");

  row.enabled_radio = new QRadioButton(tr("Enabled"));
  row.enabled_radio->setObjectName("enabled_radio");

  row.latched_radio = new QRadioButton(tr("Enabled and latched"));
  row.latched_radio->setObjectName("latched_radio");

  row.error_enable_group = new QButtonGroup(this);
  row.error_enable_group->setObjectName("error_enable_group");
  row.error_enable_group->setExclusive(true);
  row.error_enable_group->addButton(row.disabled_radio, 0);
  row.error_enable_group->addButton(row.enabled_radio, 1);
  row.error_enable_group->addButton(row.latched_radio, 2);

  connect(row.error_enable_group,
    static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
    [=] (int id) { error_enable_group_buttonToggled(id, row.error_number); });

  row.error_hard = new QCheckBox();
  row.error_hard->setObjectName("error_hard");
  if (always_hard)
  {
    row.error_hard->setEnabled(false);
  }

  connect(row.error_hard, &QCheckBox::stateChanged,
    [=] (int state) { error_hard_stateChanged(state, row.error_number); });

  row.stopping_value = new QLabel(tr("No"));
  row.stopping_value->setObjectName("stopping_value");
  row.stopping_value->setAlignment(Qt::AlignCenter);
  copy_margins(row.stopping_value, errors_stopping_motor_label);

  row.count_value = new QLabel("-");
  row.count_value->setObjectName("count_value");
  row.count_value->setAlignment(Qt::AlignLeft);
  copy_margins(row.count_value, errors_occurrence_count_label);

  int r = errors_page_layout->rowCount();
  errors_page_layout->addWidget(row.frame, r, 0, 3, 8);
  errors_page_layout->addWidget(row.bit_mask_label, r + 1, 0);
  errors_page_layout->addWidget(row.error_label, r + 1, 1);
  errors_page_layout->addWidget(row.disabled_radio, r + 1, 2);
  errors_page_layout->addWidget(row.enabled_radio, r + 1, 3);
  errors_page_layout->addWidget(row.latched_radio, r + 1, 4);
  errors_page_layout->addWidget(row.error_hard, r + 1, 5, Qt::AlignCenter);

  errors_page_layout->addWidget(row.stopping_value, r + 1, 6);
  errors_page_layout->addWidget(row.count_value, r + 1, 7);

  // Note: We have to do this after adding the radio buttons to the layout or
  // else Qt shows small spurious windows before the main window opens.
  row.disabled_radio->setVisible(!always_enabled);
  row.enabled_radio->setVisible(!always_latched);
}

QWidget * main_window::setup_advanced_tab()
{
  advanced_page_widget = new QWidget();

  QGridLayout * layout = new QGridLayout();

  layout->addWidget(setup_pin_configuration_groupbox(), 0, 0);
  layout->addWidget(setup_advanced_miscellaneous_groupbox(), 1, 0);

  layout->setColumnStretch(1, 1);
  layout->setRowStretch(2, 1);

  advanced_page_widget->setLayout(layout);

  return advanced_page_widget;
}

QWidget * main_window::setup_pin_configuration_groupbox()
{
  pin_configuration_groupbox = new QGroupBox(tr("Pin configuration"));
  pin_configuration_groupbox->setObjectName("pin_configuration_groupbox");

  disable_i2c_pullups = new QCheckBox(tr("Disable I\u00B2C pull-ups"));
  disable_i2c_pullups->setObjectName("disable_i2c_pullups");

  analog_sda_pullup = new QCheckBox(tr("Enable pull-up for analog input on SDA/AN"));
  analog_sda_pullup->setObjectName("analog_sda_pullup");

  always_analog_sda = new QCheckBox(tr("Always configure SDA/AN for analog input"));
  always_analog_sda->setObjectName("always_analog_sda");
  always_analog_sda->setToolTip(
    tr("This option causes the jrk to perform analog measurements\n"
      "on the SDA/AN pin and configure SCL as a potentiometer\n"
      "power pin even if the \"Input mode\" setting is not \"Analog\"."));

  always_analog_fba = new QCheckBox(tr("Always configure FBA for analog input"));
  always_analog_fba->setObjectName("always_analog_fba");
  always_analog_fba->setToolTip(
    tr("This option causes the jrk to perform analog measurements on the\n"
      "FBA pin even if the \"Feedback mode\" setting is not \"Analog\"."));

  QVBoxLayout * layout = new QVBoxLayout();
  layout->addWidget(disable_i2c_pullups);
  layout->addWidget(analog_sda_pullup);
  layout->addWidget(always_analog_sda);
  layout->addWidget(always_analog_fba);
  layout->addStretch();

  pin_configuration_groupbox->setLayout(layout);

  return pin_configuration_groupbox;
}

QWidget * main_window::setup_advanced_miscellaneous_groupbox()
{
  advanced_miscellaneous_groupbox = new QGroupBox(tr("Miscellaneous"));
  advanced_miscellaneous_groupbox->setObjectName("advanced_miscellaneous_groupbox");

  never_sleep_checkbox = new QCheckBox(tr("Never sleep (ignore USB suspend)"));
  never_sleep_checkbox->setObjectName("never_sleep_checkbox");

  QLabel * vin_calibration_label = new QLabel(tr("VIN measurement calibration:"));

  vin_calibration_value = new QSpinBox();
  vin_calibration_value->setObjectName("vin_calibration_value");
  vin_calibration_value->setRange(-500, 500);

  QHBoxLayout * vin_calibration_layout = new QHBoxLayout();
  vin_calibration_layout->addWidget(vin_calibration_label);
  vin_calibration_layout->addWidget(vin_calibration_value);
  vin_calibration_layout->addStretch();

  QVBoxLayout * layout = new QVBoxLayout();
  layout->addWidget(never_sleep_checkbox, Qt::AlignLeft);
  layout->addLayout(vin_calibration_layout, Qt::AlignLeft);

  advanced_miscellaneous_groupbox->setLayout(layout);

  return advanced_miscellaneous_groupbox;
}
