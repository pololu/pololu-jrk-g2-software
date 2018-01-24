#include "config.h"
#include "main_window.h"
#include "main_controller.h"
#include "graph_window.h"
#include "bootloader_window.h"

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

#include <cassert>
#include <cmath>

#ifdef QT_STATIC
#include <QtPlugin>
#ifdef _WIN32
Q_IMPORT_PLUGIN (QWindowsIntegrationPlugin);
#endif
#ifdef __linux__
Q_IMPORT_PLUGIN (QLinuxFbIntegrationPlugin);
Q_IMPORT_PLUGIN (QXcbIntegrationPlugin);
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
  QMessageBox mbox(QMessageBox::Question, windowTitle(),
    QString::fromStdString(question), QMessageBox::Ok | QMessageBox::Cancel, this);
  int button = mbox.exec();
  return button == QMessageBox::Ok;
}

void main_window::show_error_message(const std::string & message)
{
  QMessageBox mbox(QMessageBox::Critical, windowTitle(),
    QString::fromStdString(message), QMessageBox::NoButton, this);
  mbox.exec();
}

void main_window::show_info_message(const std::string & message)
{
  QMessageBox mbox(QMessageBox::Information, windowTitle(),
    QString::fromStdString(message), QMessageBox::NoButton, this);
  mbox.exec();
}

void main_window::show_warning_message(const std::string & message)
{
  QMessageBox mbox(QMessageBox::Information, windowTitle(),
    QString::fromStdString(message), QMessageBox::NoButton, this);
  mbox.exec();
}

void main_window::open_bootloader_window()
{
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

void main_window::set_input(uint16_t input)
{
  graph->input.plot_value = input;
  input_value->setText(QString::number(input));
}

void main_window::set_target(uint16_t target)
{
  graph->target.plot_value = target;
  target_value->setText(QString::number(target));
}

void main_window::set_feedback(uint16_t feedback)
{
  graph->feedback.plot_value = feedback;
  feedback_value->setText(QString::number(feedback));
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
  duty_cycle_value->setText(QString::number(duty_cycle));
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
  if (device)
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

void main_window::set_apply_settings_enabled(bool enabled)
{
  apply_settings_button->setEnabled(enabled);
  apply_settings_action->setEnabled(enabled);
}

void main_window::set_vin_calibration(int16_t vin_calibration)
{
  // TODO: set_spin_box(vin_calibration_value, vin_calibration);
}

void main_window::set_never_sleep(bool never_sleep)
{
  set_check_box(input_never_sleep_checkbox, never_sleep);
}

void main_window::set_input_mode(uint8_t input_mode)
{
  set_u8_combobox(input_mode_combobox, input_mode);
  input_scaling_groupbox->setEnabled(input_mode != JRK_INPUT_MODE_SERIAL);
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

void main_window::set_input_serial_timeout(uint16_t value)
{
  set_double_spin_box(input_timeout_spinbox, value);
}

void main_window::set_input_compact_protocol(bool enabled)
{
  set_check_box(input_disable_compact_protocol_checkbox, enabled);
}

void main_window::set_input_never_sleep(bool enabled)
{
  set_check_box(input_never_sleep_checkbox, enabled);
}

void main_window::set_input_absolute_minimum(uint16_t input_absolute_minimum)
{
  set_spin_box(input_absolute_minimum_spinbox, input_absolute_minimum);
}

void main_window::set_input_absolute_maximum(uint16_t input_absolute_maximum)
{
  set_spin_box(input_absolute_maximum_spinbox, input_absolute_maximum);
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
  bool enabled =
  !ordered({input_absolute_minimum_spinbox->value(), input_minimum_spinbox->value(),
    input_neutral_minimum_spinbox->value(), input_neutral_maximum_spinbox->value(),
    input_maximum_spinbox->value(), input_absolute_maximum_spinbox->value()});
  input_scaling_order_warning_label->setVisible(enabled);
}

void main_window::set_feedback_mode(uint8_t feedback_mode)
{
  set_u8_combobox(feedback_mode_combobox, feedback_mode);

  feedback_scaling_groupbox->setEnabled(
    feedback_mode != JRK_FEEDBACK_MODE_NONE);
}

void main_window::set_feedback_invert(bool feedback_invert)
{
  set_check_box(feedback_invert_checkbox, feedback_invert);
}

void main_window::set_feedback_absolute_minimum(uint16_t value)
{
  set_spin_box(feedback_absolute_minimum_spinbox, value);
}

void main_window::set_feedback_absolute_maximum(uint16_t value)
{
  set_spin_box(feedback_absolute_maximum_spinbox, value);
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
  !ordered({feedback_absolute_minimum_spinbox->value(), feedback_minimum_spinbox->value(),
    feedback_maximum_spinbox->value(), feedback_absolute_maximum_spinbox->value()});
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

void main_window::set_pid_multiplier(int index, uint16_t value)
{
  QSpinBox *spin = pid_constant_controls[index]->pid_multiplier_spinbox;
  set_spin_box(spin, value);
}

void main_window::set_pid_exponent(int index, uint16_t value)
{
  QSpinBox *spin = pid_constant_controls[index]->pid_exponent_spinbox;
  set_spin_box(spin, value);
}

void main_window::set_pid_constant(int index, double value)
{
  suppress_events = true;
  QLineEdit *spin = pid_constant_controls[index]->pid_constant_lineedit;

  if (value < 0.0001 && value != 0)
  {
    spin->setText(QString::number(value, 'f', 7));
  }
  else
    spin->setText(QString::number(value, 'f', 5));
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

void main_window::set_current_limit_code_forward(uint16_t current)
{
  set_spin_box(current_limit_forward_spinbox, current);
}

void main_window::set_current_limit_code_reverse(uint16_t current)
{
  set_spin_box(current_limit_reverse_spinbox, current);
}

void main_window::set_current_limit_meaning(const char * str)
{
  current_limit_means_label->setText(str);
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
  for (int i = 0; i < tab_widget->count(); i++)
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

void main_window::showEvent(QShowEvent * event)
{
  if (!start_event_reported)
  {
    start_event_reported = true;
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
}

void main_window::receive_widget(graph_widget *widget)
{
  widget->set_preview_mode(true);

  horizontal_layout->addWidget(widget->custom_plot, 0);
  preview_frame->setStyleSheet("border: 1px solid black");
}

void main_window::on_launchGraph_clicked(QMouseEvent *event)
{
  graph_widget *red = graph;
  horizontal_layout->removeWidget(red);
  preview_frame->setStyleSheet("border: 1px solid white");
  if(popout_graph_window == 0)
  {
    popout_graph_window = new graph_window(this);
    connect(popout_graph_window, SIGNAL(pass_widget(graph_widget*)), this,
    SLOT(receive_widget(graph_widget*)));
  }

  popout_graph_window->receive_widget(red);
  popout_graph_window->show();
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

void main_window::on_input_timeout_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_input_timeout_input(value);
}

void main_window::on_input_disable_compact_protocol_checkbox_stateChanged(int state)
{
  if (suppress_events) { return; }
  controller->handle_input_disable_compact_protocol_input(state == Qt::Checked);
}

void main_window::on_input_never_sleep_checkbox_stateChanged(int state)
{
  if (suppress_events) { return; }
  controller->handle_input_never_sleep_input(state == Qt::Checked);
}

void main_window::on_input_absolute_minimum_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_input_absolute_minimum_input(value);
}

void main_window::on_input_absolute_maximum_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_input_absolute_maximum_input(value);
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
  if (suppress_events) { return; }
  controller->handle_input_absolute_minimum_input(0);
  controller->handle_input_absolute_maximum_input(4095);
  controller->handle_input_minimum_input(0);
  controller->handle_input_maximum_input(4095);
  controller->handle_input_neutral_minimum_input(2048);
  controller->handle_input_neutral_maximum_input(2048);
  controller->handle_output_minimum_input(0);
  controller->handle_output_neutral_input(2048);
  controller->handle_output_maximum_input(4095);
  controller->handle_input_invert_input(false);
  input_scaling_order_warning_label->setVisible(false);
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

void main_window::on_feedback_absolute_minimum_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_feedback_absolute_minimum_input(value);
}

void main_window::on_feedback_absolute_maximum_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_feedback_absolute_maximum_input(value);
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
  if (suppress_events) { return; }
  controller->handle_feedback_absolute_maximum_input(4095);
  controller->handle_feedback_maximum_input(4095);
  controller->handle_feedback_minimum_input(0);
  controller->handle_feedback_absolute_minimum_input(0);
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

void main_window::on_detect_motor_button_clicked()
{
  if (suppress_events) { return; }
  QMessageBox mbox(QMessageBox::Question, "Detect motor direction confirmation",
    "Really detect motor direction?  This will drive the motor!\n"
    "Feedback must be configured and tested before proceeding.\n"
    "It is also recommended that you set Max. duty cycle, Max. current,\n"
    "and enable the \"Max. current exceeded\" error for this test.", QMessageBox::Ok | QMessageBox::Cancel, this);
  int button = mbox.exec();
  if (!(button == QMessageBox::Ok))
    return;
  else
    controller->handle_motor_detect_direction_button_clicked();
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

void main_window::on_error_enable_group_buttonToggled(int id, int index)
{
  if (suppress_events) { return; }
  controller->handle_error_enable_input(index, id);
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
  this->setCentralWidget(central_widget);

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

  grid_layout = new QGridLayout();
  grid_layout->setObjectName("grid_layout");

  tab_widget = new QTabWidget();
  tab_widget->addTab(setup_status_tab(), tr("Status"));
  tab_widget->addTab(setup_input_tab(), tr("Input"));
  tab_widget->addTab(setup_feedback_tab(), tr("Feedback"));
  tab_widget->addTab(setup_pid_tab(), tr("PID"));
  tab_widget->addTab(setup_motor_tab(), tr("Motor"));
  tab_widget->addTab(setup_errors_tab(), tr("Errors"));

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

  QHBoxLayout *stop_and_run_buttons = new QHBoxLayout();
  stop_and_run_buttons->addWidget(stop_motor_button, 0, Qt::AlignLeft);
  stop_and_run_buttons->addWidget(run_motor_button, 0, Qt::AlignLeft);

  apply_settings_button = new QPushButton();
  apply_settings_button->setObjectName("apply_settings");
  apply_settings_button->setText(tr("&Apply settings"));

  QHBoxLayout *header_layout = new QHBoxLayout();
  header_layout->addWidget(device_list_label, 0);
  header_layout->addWidget(device_list_value, 0);
  header_layout->addWidget(connection_status_value, 0);

  grid_layout->addLayout(header_layout, 0, 0, 1, 0, Qt::AlignLeft);
  grid_layout->addWidget(tab_widget, 1, 0, 1, 5);
  grid_layout->addLayout(stop_and_run_buttons, 2, 0, 1, 3, Qt::AlignLeft);
  grid_layout->addWidget(apply_settings_button, 2, 4, Qt::AlignRight);


  connect(stop_motor_button, SIGNAL(clicked()),
    stop_motor_action, SLOT(trigger()));

  connect(run_motor_button, SIGNAL(clicked()),
    run_motor_action, SLOT(trigger()));

  connect(apply_settings_button, SIGNAL(clicked()),
    apply_settings_action, SLOT(trigger()));

  central_widget->setLayout(grid_layout);

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

  setStyleSheet(stylesheet);
}

void main_window::setup_menu_bar()
{
  menu_bar = new QMenuBar();
  this->setMenuBar(menu_bar);

  file_menu = menu_bar->addMenu("");
  file_menu->setTitle(tr("&File"));

  device_menu = menu_bar->addMenu("");
  device_menu->setTitle(tr("&Device"));

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

  help_menu->addAction(documentation_action);
  help_menu->addAction(about_action);
}

QSpacerItem * main_window::setup_vertical_spacer()
{
  return new QSpacerItem(1, fontMetrics().height());
}

static void setup_read_only_text_field(QGridLayout * layout,
  int row, int col, int value_col_span, QLabel ** label, QLabel ** value)
{
  QLabel * new_value = new QLabel();
  new_value->setTextInteractionFlags(Qt::TextSelectableByMouse);

  QLabel * new_label = new QLabel();
  new_label->setBuddy(new_value);

  layout->addWidget(new_label, row, col, Qt::AlignLeft);
  layout->addWidget(new_value, row, col + 1, 1, value_col_span, Qt::AlignLeft);

  if (label) { *label = new_label; }
  if (value) { *value = new_value; }
}

static void setup_read_only_text_field(QGridLayout * layout,
  int row, int col, QLabel ** label, QLabel ** value)
{
  setup_read_only_text_field(layout, row, col, 1, label, value);
}

static void setup_read_only_text_field(QGridLayout * layout,
  int row, QLabel ** label, QLabel ** value)
{
  setup_read_only_text_field(layout, row, 0, 1, label, value);
}

QWidget * main_window::setup_status_tab()
{
  status_page_widget = new QWidget();
  QGridLayout * layout = new QGridLayout();

  layout->addWidget(setup_variables_box(), 0, 0, Qt::AlignLeft);
  layout->addWidget(setup_preview_plot(), 0, 1, Qt::AlignCenter);

  layout->addWidget(setup_manual_target_box(), 1, 0, 1, 3);

  layout->setRowStretch(2, 1);

  status_page_widget->setLayout(layout);
  return status_page_widget;
}

QWidget * main_window::setup_preview_plot()
{
  graph = new graph_widget();
  graph->setObjectName(QStringLiteral("graph"));
  graph->set_preview_mode(true);

  preview_frame = new QFrame();
  preview_frame->setStyleSheet("border: 1px solid black");

  preview_plot = new QWidget();
  preview_plot = graph->custom_plot;

  horizontal_layout = new QHBoxLayout();
  horizontal_layout->addWidget(preview_plot);

  connect(preview_plot, SIGNAL(mousePress(QMouseEvent*)), this,
    SLOT(on_launchGraph_clicked(QMouseEvent*)));

  preview_frame->setLayout(horizontal_layout);

  return preview_frame;
}

QWidget * main_window::setup_variables_box()
{
  variables_box = new QGroupBox();
  variables_box->setTitle(tr("Variables"));  // TODO: better name?

  QGridLayout * layout = new QGridLayout();

  int row = 0;

  setup_read_only_text_field(layout, row++, &device_name_label,
    &device_name_value);
  device_name_value->setObjectName("device_name_value");
  device_name_value->setTextInteractionFlags(Qt::TextBrowserInteraction);
  device_name_label->setText(tr("Name:"));

  setup_read_only_text_field(layout, row++, &serial_number_label,
    &serial_number_value);
  serial_number_label->setText(tr("Serial number:"));

  setup_read_only_text_field(layout, row++, &firmware_version_label,
    &firmware_version_value);
  firmware_version_label->setText(tr("Firmware version:"));

  setup_read_only_text_field(layout, row++, &cmd_port_label,
    &cmd_port_value);
  cmd_port_label->setText(tr("Command port:"));

  setup_read_only_text_field(layout, row++, &ttl_port_label,
    &ttl_port_value);
  ttl_port_label->setText(tr("TTL port:"));

  setup_read_only_text_field(layout, row++,
    &device_reset_label, &device_reset_value);
  device_reset_label->setText(tr("Last reset:"));

  {
    QLabel tmp(tr("Software reset (bootloader)"));
    device_reset_value->setMinimumWidth(tmp.sizeHint().width());
  }

  setup_read_only_text_field(layout, row++, &up_time_label, &up_time_value);
  up_time_label->setText(tr("Up time:"));

  setup_read_only_text_field(layout, row++, &input_label, &input_value);
  input_label->setText(tr("Input:"));

  setup_read_only_text_field(layout, row++, &target_label, &target_value);
  target_label->setText(tr("Target:"));

  setup_read_only_text_field(layout, row++, &feedback_label, &feedback_value);
  feedback_label->setText(tr("Feedback:"));

  setup_read_only_text_field(layout, row++, &scaled_feedback_label,
    &scaled_feedback_value);
  scaled_feedback_label->setText(tr("Scaled feedback:"));

  setup_read_only_text_field(layout, row++, &error_label, &error_value);
  error_label->setText(tr("Error:"));

  setup_read_only_text_field(layout, row++, &integral_label, &integral_value);
  integral_label->setText(tr("Integral:"));

  setup_read_only_text_field(layout, row++, &duty_cycle_target_label,
    &duty_cycle_target_value);
  duty_cycle_target_label->setText(tr("Duty cycle target:"));

  setup_read_only_text_field(layout, row++, &duty_cycle_label, &duty_cycle_value);
  duty_cycle_label->setText(tr("Duty cycle:"));

  setup_read_only_text_field(layout, row++, &raw_current_label,
    &raw_current_value);
  raw_current_label->setText(tr("Raw current:"));

  setup_read_only_text_field(layout, row++, &current_label,
    &current_value);
  current_label->setText(tr("Current:"));

  // TODO: what kind of current chopping thing do we want to show here?
  // it's not really a log any more
  setup_read_only_text_field(layout, row++, &current_chopping_count_label,
    &current_chopping_count_value);
  current_chopping_count_label->setText(tr("Current chopping count:"));

  setup_read_only_text_field(layout, row++, &vin_voltage_label,
    &vin_voltage_value);
  vin_voltage_label->setText(tr("VIN voltage:"));

  setup_read_only_text_field(layout, row++, &pid_period_count_label,
    &pid_period_count_value);
  pid_period_count_label->setText(tr("PID period count:"));

  setup_read_only_text_field(layout, row++, &pid_period_exceeded_label,
    &pid_period_exceeded_value);
  pid_period_exceeded_label->setText(tr("PID period exceeded:"));

  setup_read_only_text_field(layout, row++,
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
  input_mode_combobox->addItem("Serial", JRK_INPUT_MODE_SERIAL);
  input_mode_combobox->addItem("Analog", JRK_INPUT_MODE_ANALOG);
  input_mode_combobox->addItem("Pulse width", JRK_INPUT_MODE_PULSE_WIDTH);

  QHBoxLayout *input_mode_layout = new QHBoxLayout();
  input_mode_layout->addWidget(input_mode_label, 0, Qt::AlignLeft);
  input_mode_layout->addWidget(input_mode_combobox, 0, Qt::AlignLeft);

  QGridLayout *layout = input_page_layout = new QGridLayout();
  layout->setSizeConstraint(QLayout::SetFixedSize);
  layout->addLayout(input_mode_layout, 0, 0, Qt::AlignLeft);
  layout->addWidget(setup_input_analog_groupbox(), 1, 0);
  layout->addWidget(setup_input_serial_groupbox(), 2, 0);
  layout->addWidget(setup_input_scaling_groupbox(), 1, 1, 2, 1, Qt::AlignTop);

  input_page_widget->setLayout(layout);

  input_page_widget->setParent(tab_widget);

  return input_page_widget;
}

static QComboBox * setup_analog_samples_exponent_combobox()
{
  QComboBox * box = new QComboBox();
  box->addItem("1", 0);
  box->addItem("2", 1);
  box->addItem("4", 2);
  box->addItem("8", 3);
  box->addItem("16", 4);
  box->addItem("32", 5);
  box->addItem("64", 6);
  box->addItem("128", 7);
  box->addItem("256", 8);
  box->addItem("512", 9);
  box->addItem("1024", 10);
  return box;
}

QWidget * main_window::setup_input_analog_groupbox()
{
  input_analog_groupbox = new QGroupBox(tr("Analog to digital conversion"));
  input_analog_groupbox->setObjectName("input_analog_groupbox");

  input_analog_samples_label = new QLabel(tr("Analog samples:"));
  input_analog_samples_label->setObjectName("input_analog_samples_label");

  input_analog_samples_combobox = setup_analog_samples_exponent_combobox();
  input_analog_samples_combobox->setObjectName("input_analog_samples_combobox");

  input_detect_disconnect_checkbox = new QCheckBox(tr("Detect disconnect with power pin"));
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
  input_serial_groupbox = new QGroupBox(tr("Serial Interface"));
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

  input_device_label = new QLabel(tr("Device Number:"));
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
  input_timeout_spinbox->setSingleStep(JRK_SERIAL_TIMEOUT_UNITS);
  input_timeout_spinbox->setDecimals(2);
  input_timeout_spinbox->setRange(0, JRK_MAX_ALLOWED_SERIAL_TIMEOUT);

  input_disable_compact_protocol_checkbox = new QCheckBox(tr("Disable compact protocol"));
  input_disable_compact_protocol_checkbox->setObjectName("input_disable_compact_protocol_checkbox");

  input_never_sleep_checkbox = new QCheckBox(tr("Never sleep (ignore USB suspend)"));
  input_never_sleep_checkbox->setObjectName("input_never_sleep_checkbox");

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
  input_serial_layout->addWidget(input_never_sleep_checkbox, 10, 0, Qt::AlignLeft);

  input_serial_groupbox->setLayout(input_serial_layout);

  return input_serial_groupbox;
}

QWidget * main_window::setup_input_scaling_groupbox()
{
  input_scaling_groupbox = new QGroupBox(tr("Scaling (analog and pulse width mode only)"));
  input_scaling_groupbox->setObjectName("input_scaling_groupbox");

  input_invert_checkbox = new QCheckBox(tr("Invert input direction"));
  input_invert_checkbox->setObjectName("input_invert_checkbox");

  input_absolute_max_label = new QLabel(tr("Absolute max:"));
  input_absolute_max_label->setObjectName("input_absolute_max_label");

  input_maximum_label = new QLabel(tr("Maximum:"));
  input_maximum_label->setObjectName("input_maximum_label");

  input_neutral_max_label = new QLabel(tr("Neutral max:"));
  input_neutral_max_label->setObjectName("input_neutral_max_label");

  input_neutral_min_label = new QLabel(tr("Neutral min:"));
  input_neutral_min_label->setObjectName("input_neutral_min_label");

  input_minimum_label = new QLabel(tr("Minimum:"));
  input_minimum_label->setObjectName("input_minimum_label");

  input_absolute_min_label = new QLabel(tr("Absolute min:"));
  input_absolute_min_label->setObjectName("input_absolute_min_label");

  input_degree_label = new QLabel(tr("Degree:"));
  input_degree_label->setObjectName("input_degree_label");

  input_input_label = new QLabel(tr("Input"));
  input_input_label->setObjectName("input_input_label");

  input_absolute_maximum_spinbox = new QSpinBox();
  input_absolute_maximum_spinbox->setObjectName("input_absolute_maximum_spinbox");
  input_absolute_maximum_spinbox->setRange(0, UINT12_MAX);
  input_absolute_maximum_spinbox->setValue(4095);

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

  input_absolute_minimum_spinbox = new QSpinBox();
  input_absolute_minimum_spinbox->setObjectName("input_absolute_minimum_spinbox");
  input_absolute_minimum_spinbox->setRange(0, UINT12_MAX);
  input_absolute_minimum_spinbox->setValue(0);

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
  input_learn_button->setText(tr("Learn..."));

  input_reset_range_button = new QPushButton();
  input_reset_range_button->setObjectName("input_reset_range_button");
  input_reset_range_button->setText(tr("Reset to full range"));

  input_scaling_order_warning_label = new QLabel(
    tr("Warning: some of the values\nare not in the correct order."));
  input_scaling_order_warning_label->setObjectName("input_scaling_order_warning_label");
  input_scaling_order_warning_label->setStyleSheet("color: red;");

  QGridLayout *input_scaling_layout = new QGridLayout();
  input_scaling_layout->addWidget(input_invert_checkbox, 0, 0, 1, 2, Qt::AlignLeft);
  input_scaling_layout->addWidget(input_input_label, 1, 1, Qt::AlignLeft);
  input_scaling_layout->addWidget(input_target_label, 1, 2, Qt::AlignLeft);
  input_scaling_layout->addWidget(input_absolute_max_label, 2, 0, Qt::AlignLeft);
  input_scaling_layout->addWidget(input_absolute_maximum_spinbox, 2, 1, Qt::AlignLeft);
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
  input_scaling_layout->addWidget(input_absolute_min_label, 7, 0, Qt::AlignLeft);
  input_scaling_layout->addWidget(input_absolute_minimum_spinbox, 7, 1, Qt::AlignLeft);
  input_scaling_layout->addItem(setup_vertical_spacer(), 8, 0);
  input_scaling_layout->addWidget(input_degree_label, 9, 0, Qt::AlignLeft);
  input_scaling_layout->addWidget(input_scaling_degree_combobox, 9, 1, 1, 2, Qt::AlignLeft);
  input_scaling_layout->addWidget(input_learn_button, 0, 4, Qt::AlignRight);
  input_scaling_layout->addWidget(input_reset_range_button, 1, 4, Qt::AlignRight);
  input_scaling_layout->addWidget(input_scaling_order_warning_label, 2, 3, 7, 2, Qt::AlignCenter);

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
  feedback_mode_combobox->addItem("Analog", JRK_FEEDBACK_MODE_ANALOG);
  feedback_mode_combobox->addItem("Frequency (digital)", JRK_FEEDBACK_MODE_FREQUENCY);

  QHBoxLayout *feedback_mode_layout = new QHBoxLayout();
  feedback_mode_layout->addWidget(feedback_mode_label);
  feedback_mode_layout->addWidget(feedback_mode_combobox);

  QGridLayout *layout = feedback_page_layout = new QGridLayout();
  layout->setSizeConstraint(QLayout::SetFixedSize);
  layout->addLayout(feedback_mode_layout, 0, 0, Qt::AlignLeft);
  layout->addWidget(setup_feedback_scaling_groupbox(), 1, 0);
  layout->addWidget(setup_feedback_analog_groupbox(), 2, 0);
  layout->addWidget(setup_feedback_options_groupbox(), 3, 0);

  feedback_page_widget->setLayout(layout);
  return feedback_page_widget;
}

QWidget * main_window::setup_feedback_scaling_groupbox()
{
  QSizePolicy p = this->sizePolicy();
  p.setRetainSizeWhenHidden(true);

  feedback_scaling_groupbox = new QGroupBox(tr("Scaling (analog and tachometer mode only)"));
  feedback_scaling_groupbox->setObjectName("feedback_scaling_groupbox");

  feedback_invert_checkbox = new QCheckBox(tr("Invert feedback direction"));
  feedback_invert_checkbox->setObjectName("feedback_invert_checkbox");

  feedback_absolute_max_label = new QLabel(tr("Absolute max:"));
  feedback_absolute_max_label->setObjectName("feedback_absolute_max_label");

  feedback_maximum_label = new QLabel(tr("Maximum:"));
  feedback_maximum_label->setObjectName("feedback_maximum_label");

  feedback_minimum_label = new QLabel(tr("Minimum:"));
  feedback_minimum_label->setObjectName("feedback_minimum_label");

  feedback_absolute_min_label = new QLabel(tr("Absolute min:"));
  feedback_absolute_min_label->setObjectName("feedback_absolute_min_label");

  feedback_calibration_label = new QLabel(tr("Calibration"));
  feedback_calibration_label->setObjectName("feedback_calibration_label");

  feedback_scaling_order_warning_label = new QLabel(
    tr("Warning: some of the values\nare not in the correct order"));
  feedback_scaling_order_warning_label->setObjectName("feedback_scaling_order_warning_label");
  feedback_scaling_order_warning_label->setStyleSheet("color: red;");
  feedback_scaling_order_warning_label->setSizePolicy(p);

  feedback_absolute_maximum_spinbox = new QSpinBox();
  feedback_absolute_maximum_spinbox->setObjectName("feedback_absolute_maximum_spinbox");
  feedback_absolute_maximum_spinbox->setRange(0, UINT12_MAX);

  feedback_maximum_spinbox = new QSpinBox();
  feedback_maximum_spinbox->setObjectName("feedback_maximum_spinbox");
  feedback_maximum_spinbox->setRange(0, UINT12_MAX);

  feedback_minimum_spinbox = new QSpinBox();
  feedback_minimum_spinbox->setObjectName("feedback_minimum_spinbox");
  feedback_minimum_spinbox->setRange(0, UINT12_MAX);

  feedback_absolute_minimum_spinbox = new QSpinBox();
  feedback_absolute_minimum_spinbox->setObjectName("feedback_absolute_minimum_spinbox");
  feedback_absolute_minimum_spinbox->setRange(0, UINT12_MAX);

  feedback_learn_button = new QPushButton(tr("Learn..."));
  feedback_learn_button->setObjectName("feedback_learn_button");

  feedback_reset_range_button = new QPushButton(tr("Reset to full range"));
  feedback_reset_range_button->setObjectName("feedback_reset_range_button");

  QGridLayout *feedback_scaling_layout = new QGridLayout();
  feedback_scaling_layout->addWidget(feedback_invert_checkbox, 0, 0, 1, 2, Qt::AlignLeft);
  feedback_scaling_layout->addWidget(feedback_calibration_label, 1, 1, Qt::AlignLeft);
  feedback_scaling_layout->addWidget(feedback_absolute_max_label, 2, 0, Qt::AlignLeft);
  feedback_scaling_layout->addWidget(feedback_absolute_maximum_spinbox, 2, 1, Qt::AlignLeft);
  feedback_scaling_layout->addWidget(feedback_maximum_label, 3, 0, Qt::AlignLeft);
  feedback_scaling_layout->addWidget(feedback_maximum_spinbox, 3, 1, Qt::AlignLeft);
  feedback_scaling_layout->addWidget(feedback_minimum_label, 4, 0, Qt::AlignLeft);
  feedback_scaling_layout->addWidget(feedback_minimum_spinbox, 4, 1, Qt::AlignLeft);
  feedback_scaling_layout->addWidget(feedback_absolute_min_label, 5, 0, Qt::AlignLeft);
  feedback_scaling_layout->addWidget(feedback_absolute_minimum_spinbox, 5, 1, Qt::AlignLeft);
  feedback_scaling_layout->addWidget(feedback_learn_button, 0, 3, Qt::AlignRight);
  feedback_scaling_layout->addWidget(feedback_reset_range_button, 1, 3, Qt::AlignRight);
  feedback_scaling_layout->addWidget(feedback_scaling_order_warning_label, 2, 2, 4, 2, Qt::AlignCenter);

  feedback_scaling_groupbox->setLayout(feedback_scaling_layout);

  return feedback_scaling_groupbox;
}

QWidget * main_window::setup_feedback_analog_groupbox()
{
  feedback_analog_groupbox = new QGroupBox(tr("Analog to digital conversion"));
  feedback_analog_groupbox->setObjectName("feedback_analog_groupbox");

  feedback_analog_samples_label = new QLabel(tr("Analog samples:"));
  feedback_analog_samples_label->setObjectName("feedback_analog_samples_label");

  feedback_analog_samples_combobox = setup_analog_samples_exponent_combobox();
  feedback_analog_samples_combobox->setObjectName("feedback_analog_samples_combobox");

  feedback_detect_disconnect_checkbox =
    new QCheckBox(tr("Detect disconnect with power pin"));
  feedback_detect_disconnect_checkbox->setObjectName(
    "feedback_detect_disconnect_checkbox");

  QHBoxLayout * analog_samples = new QHBoxLayout();
  analog_samples->addWidget(feedback_analog_samples_label, 0, Qt::AlignLeft);
  analog_samples->addWidget(feedback_analog_samples_combobox, 0, Qt::AlignLeft);

  QGridLayout * layout = new QGridLayout();
  layout->addLayout(analog_samples, 0, 0, Qt::AlignLeft);
  layout->addWidget(feedback_detect_disconnect_checkbox, 1, 0, Qt::AlignLeft);

  feedback_analog_groupbox->setLayout(layout);

  return feedback_analog_groupbox;
}

QWidget * main_window::setup_feedback_options_groupbox()
{
  feedback_options_groupbox = new QGroupBox();
  feedback_options_groupbox->setObjectName("feedback_options_groupbox");
  feedback_options_groupbox->setTitle(tr("Feedback options"));

  feedback_wraparound_checkbox = new QCheckBox(tr("Wraparound"));
  feedback_wraparound_checkbox->setObjectName("feedback_wraparound_checkbox");
  feedback_wraparound_checkbox->setToolTip(
    "A scaled feedback value of 0 is considered to be adjacent to 4095.  "
    "Suitable for systems that rotate over a full circle.");

  QGridLayout * layout = new QGridLayout();
  layout->addWidget(feedback_wraparound_checkbox, 0, 0, Qt::AlignLeft);

  feedback_options_groupbox->setLayout(layout);

  return feedback_options_groupbox;
}

QWidget * main_window::setup_pid_tab()
{
  pid_page_widget = new QWidget();

  pid_proportional_coefficient_groupbox = new QGroupBox();
  pid_proportional_coefficient_groupbox->setTitle("Proportional coefficient");
  pid_constant_controls[0] = new pid_constant_control(0, this);
  pid_constant_controls[0]->setup(pid_proportional_coefficient_groupbox);

  pid_integral_coefficient_groupbox = new QGroupBox();
  pid_integral_coefficient_groupbox->setTitle("Integral coefficient");
  pid_constant_controls[1] = new pid_constant_control(1, this);
  pid_constant_controls[1]->setup(pid_integral_coefficient_groupbox);

  pid_derivative_coefficient_groupbox = new QGroupBox();
  pid_derivative_coefficient_groupbox->setTitle("Derivative coefficient");
  pid_constant_controls[2] = new pid_constant_control(2, this);
  pid_constant_controls[2]->setup(pid_derivative_coefficient_groupbox);

  pid_period_label = new QLabel(tr("PID period (ms):"));
  pid_period_label->setObjectName("pid_period_label");

  pid_period_spinbox = new QSpinBox();
  pid_period_spinbox->setObjectName("pid_period_spinbox");

  integral_limit_label = new QLabel(tr("Integral limit:"));
  integral_limit_label->setObjectName("integral_limit_label");

  integral_limit_spinbox = new QSpinBox();
  integral_limit_spinbox->setObjectName("integral_limit_spinbox");
  integral_limit_spinbox->setRange(0, 0x7FFF);

  reset_integral_checkbox = new QCheckBox(
    tr("Reset integral when proportional term exceeds max duty cycle"));
  reset_integral_checkbox->setObjectName("reset_integral_checkbox");

  feedback_dead_zone_label = new QLabel(tr("Feedback dead zone:"));
  feedback_dead_zone_label->setObjectName("feedback_dead_zone_label");

  feedback_dead_zone_spinbox = new QSpinBox();
  feedback_dead_zone_spinbox->setObjectName("feedback_dead_zone_spinbox");

  QGridLayout *group_box_row = new QGridLayout();
  group_box_row->addWidget(pid_proportional_coefficient_groupbox, 0, 0);
  group_box_row->addWidget(pid_integral_coefficient_groupbox, 0, 1);
  group_box_row->addWidget(pid_derivative_coefficient_groupbox, 0, 2);

  QHBoxLayout *period_row_layout = new QHBoxLayout();
  period_row_layout->addWidget(pid_period_label);
  period_row_layout->addWidget(pid_period_spinbox);

  QHBoxLayout *integral_row_layout = new QHBoxLayout();
  integral_row_layout->addWidget(integral_limit_label);
  integral_row_layout->addWidget(integral_limit_spinbox);

  QHBoxLayout *deadzone_row_layout = new QHBoxLayout();
  deadzone_row_layout->addWidget(feedback_dead_zone_label);
  deadzone_row_layout->addWidget(feedback_dead_zone_spinbox);

  QGridLayout *layout = pid_page_layout = new QGridLayout();
  layout->setSizeConstraint(QLayout::SetFixedSize);
  layout->addLayout(group_box_row, 0, 0, 1, 3);
  layout->addLayout(period_row_layout, 1, 0, 1, 1, Qt::AlignLeft);
  layout->addLayout(integral_row_layout, 2, 0, 1, 1, Qt::AlignLeft);
  layout->addWidget(reset_integral_checkbox, 3, 0, 1, 1, Qt::AlignLeft);
  layout->addLayout(deadzone_row_layout, 4, 0, 1, 1, Qt::AlignLeft);

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

  detect_motor_button = new QPushButton(tr("Detect Motor Direction"));
  detect_motor_button->setObjectName("detect_motor_button");
  detect_motor_button->setEnabled(true); //tmphax: not ready to use

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
  max_deceleration_forward_spinbox->setRange(0, JRK_MAX_ALLOWED_DUTY_CYCLE);

  max_deceleration_reverse_spinbox = new QSpinBox();
  max_deceleration_reverse_spinbox->setObjectName("max_deceleration_reverse_spinbox");
  max_deceleration_reverse_spinbox->setRange(0, JRK_MAX_ALLOWED_DUTY_CYCLE);

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

  // TODO: let people enter current limits in A instead of with codes
  current_limit_label = new QLabel(tr("Current limit code:"));
  current_limit_label->setObjectName("current_limit_label");

  current_limit_forward_spinbox = new QSpinBox();
  current_limit_forward_spinbox->setObjectName("current_limit_forward_spinbox");
  current_limit_forward_spinbox->setRange(0, 95);

  current_limit_reverse_spinbox = new QSpinBox();
  current_limit_reverse_spinbox->setObjectName("current_limit_reverse_spinbox");
  current_limit_reverse_spinbox->setRange(0, 95);

  current_limit_means_label = new QLabel(tr("(0 to 95)"));
  current_limit_means_label->setObjectName("current_limit_means_label");

  // TODO: let people enter max currents in A instead of mA
  max_current_label = new QLabel(tr("Max current (mA):"));
  max_current_label->setObjectName("max_current_label");

  max_current_forward_spinbox = new QSpinBox();
  max_current_forward_spinbox->setObjectName("max_current_forward_spinbox");
  max_current_forward_spinbox->setRange(0, 65535);

  max_current_reverse_spinbox = new QSpinBox();
  max_current_reverse_spinbox->setObjectName("max_current_reverse_spinbox");
  max_current_reverse_spinbox->setRange(0, 65535);

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

  current_samples_combobox = setup_analog_samples_exponent_combobox();
  current_samples_combobox->setObjectName("current_samples_combobox");

  overcurrent_threshold_label = new QLabel(tr("Overcurrent threshold:"));
  overcurrent_threshold_label->setObjectName("overcurrent_threshold_label");

  overcurrent_threshold_spinbox = new QSpinBox();
  overcurrent_threshold_spinbox->setObjectName("overcurrent_threshold_spinbox");
  overcurrent_threshold_spinbox->setRange(1, 255);

  int row = 0;
  motor_controls_layout->addWidget(motor_asymmetric_checkbox, row, 2, Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_forward_label, ++row, 1, Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_reverse_label, row, 2, Qt::AlignLeft);
  motor_controls_layout->addWidget(max_duty_cycle_label, ++row, 0, Qt::AlignLeft);
  motor_controls_layout->addWidget(max_duty_cycle_forward_spinbox, row, 1, Qt::AlignLeft);
  motor_controls_layout->addWidget(max_duty_cycle_reverse_spinbox, row, 2, Qt::AlignLeft);
  motor_controls_layout->addWidget(max_duty_cycle_means_label, row, 3, Qt::AlignLeft);
  motor_controls_layout->addWidget(max_acceleration_label, ++row, 0, Qt::AlignLeft);
  motor_controls_layout->addWidget(max_acceleration_forward_spinbox, row, 1, Qt::AlignLeft);
  motor_controls_layout->addWidget(max_acceleration_reverse_spinbox, row, 2, Qt::AlignLeft);
  motor_controls_layout->addWidget(max_acceleration_means_label, row, 3, Qt::AlignLeft);
  motor_controls_layout->addWidget(max_deceleration_label, ++row, 0, Qt::AlignLeft);
  motor_controls_layout->addWidget(max_deceleration_forward_spinbox, row, 1, Qt::AlignLeft);
  motor_controls_layout->addWidget(max_deceleration_reverse_spinbox, row, 2, Qt::AlignLeft);
  motor_controls_layout->addWidget(max_deceleration_means_label, row, 3, Qt::AlignLeft);
  motor_controls_layout->addWidget(brake_duration_label, ++row, 0, Qt::AlignLeft);
  motor_controls_layout->addWidget(brake_duration_forward_spinbox, row, 1, Qt::AlignLeft);
  motor_controls_layout->addWidget(brake_duration_reverse_spinbox, row, 2, Qt::AlignLeft);
  motor_controls_layout->addWidget(current_limit_label, ++row, 0, Qt::AlignLeft);
  motor_controls_layout->addWidget(current_limit_forward_spinbox, row, 1, Qt::AlignLeft);
  motor_controls_layout->addWidget(current_limit_reverse_spinbox, row, 2, Qt::AlignLeft);
  motor_controls_layout->addWidget(current_limit_means_label, row, 3, Qt::AlignLeft);
  motor_controls_layout->addWidget(max_current_label, ++row, 0, Qt::AlignLeft);
  motor_controls_layout->addWidget(max_current_forward_spinbox, row, 1, Qt::AlignLeft);
  motor_controls_layout->addWidget(max_current_reverse_spinbox, row, 2, Qt::AlignLeft);
  motor_controls_layout->addWidget(max_current_means_label, row, 3, Qt::AlignLeft);
  motor_controls_layout->addItem(setup_vertical_spacer(), ++row, 0, Qt::AlignLeft);
  motor_controls_layout->addWidget(current_offset_calibration_label, ++row, 0, Qt::AlignLeft);
  motor_controls_layout->addWidget(current_offset_calibration_spinbox, row, 1, Qt::AlignLeft);
  motor_controls_layout->addWidget(current_scale_calibration_label, ++row, 0, Qt::AlignLeft);
  motor_controls_layout->addWidget(current_scale_calibration_spinbox, row, 1, Qt::AlignLeft);
  motor_controls_layout->addWidget(current_samples_label, ++row, 0, Qt::AlignLeft);
  motor_controls_layout->addWidget(current_samples_combobox, row, 1, Qt::AlignLeft);
  motor_controls_layout->addWidget(overcurrent_threshold_label, ++row, 0, Qt::AlignLeft);
  motor_controls_layout->addWidget(overcurrent_threshold_spinbox, row, 1, Qt::AlignLeft);

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

  QHBoxLayout *invert_layout = new QHBoxLayout();
  invert_layout->addWidget(motor_invert_checkbox);
  invert_layout->addWidget(detect_motor_button);

  QHBoxLayout *deceleration_layout = new QHBoxLayout();
  deceleration_layout->addWidget(
    max_duty_cycle_while_feedback_out_of_range_label, -1, Qt::AlignLeft);
  deceleration_layout->addWidget(
    max_duty_cycle_while_feedback_out_of_range_spinbox, -1, Qt::AlignLeft);
  deceleration_layout->addWidget(
    max_duty_cycle_while_feedback_out_of_range_means_label, -1, Qt::AlignLeft);

  QGridLayout *motor_off_layout = new QGridLayout();
  motor_off_layout->addWidget(motor_off_label,0,0);
  motor_off_layout->addWidget(motor_brake_radio,0,1);
  motor_off_layout->addWidget(motor_coast_radio,1,1);

  QGridLayout *layout = motor_page_layout = new QGridLayout();
  layout->setSizeConstraint(QLayout::SetFixedSize);
  layout->addLayout(frequency_layout,0,0,Qt::AlignLeft);
  layout->addLayout(invert_layout,1,0,Qt::AlignLeft);
  layout->addLayout(motor_controls_layout,2,0,Qt::AlignLeft);
  layout->addLayout(deceleration_layout,3,0,Qt::AlignLeft);
  layout->addLayout(motor_off_layout,4,0,Qt::AlignLeft);

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
  layout->addWidget(errors_stopping_motor_label, 0, 5);
  layout->addWidget(errors_occurrence_count_label, 0, 6);

  // Note: We have to do this before calling setup_error_row or else Qt shows
  // small spurious windows before the main window opens.
  errors_page_widget->setLayout(layout);

  setup_error_row(JRK_ERROR_AWAITING_COMMAND, true, true);
  setup_error_row(JRK_ERROR_NO_POWER, true, false);
  setup_error_row(JRK_ERROR_MOTOR_DRIVER, true, false);
  setup_error_row(JRK_ERROR_INPUT_INVALID, true, false);
  setup_error_row(JRK_ERROR_INPUT_DISCONNECT, true, false);
  setup_error_row(JRK_ERROR_FEEDBACK_DISCONNECT, false, false);
  setup_error_row(JRK_ERROR_MAX_CURRENT_EXCEEDED, false, false);
  setup_error_row(JRK_ERROR_SERIAL_SIGNAL, false, true);
  setup_error_row(JRK_ERROR_SERIAL_OVERRUN, false, true);
  setup_error_row(JRK_ERROR_SERIAL_BUFFER_FULL, false, true);
  setup_error_row(JRK_ERROR_SERIAL_CRC, false, true);
  setup_error_row(JRK_ERROR_SERIAL_PROTOCOL, false, true);
  setup_error_row(JRK_ERROR_SERIAL_TIMEOUT, false, true);
  setup_error_row(JRK_ERROR_OVERCURRENT, false, false);

  int last_row = layout->rowCount();
  layout->addWidget(errors_clear_errors, last_row, 5, 1, 1, Qt::AlignCenter);
  layout->addWidget(errors_reset_counts, last_row, 6, 1, 1, Qt::AlignLeft);

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
  bool always_enabled, bool always_latched)
{
  error_rows.append(error_row());
  error_row & row = error_rows.last();

  row.error_number = error_number;
  row.always_enabled = always_enabled;
  row.always_latched = always_latched;

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

  // TODO: cleaner syntax for this?
  connect(row.error_enable_group,
    static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
    [=] (int id)
    {
      on_error_enable_group_buttonToggled(id, row.error_number);
    });

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
  errors_page_layout->addWidget(row.stopping_value, r + 1, 5);
  errors_page_layout->addWidget(row.count_value, r + 1, 6);

  // Note: We have to do this after adding the radio buttons to the layout or
  // else Qt shows small spurious windows before the main window opens.
  row.disabled_radio->setVisible(!always_enabled);
  row.enabled_radio->setVisible(!always_latched);
}

void pid_constant_control::setup(QGroupBox * groupbox)
{
  QFont font;
  font.setPointSize(16);
  font.setBold(true);

  QFont font1;
  font1.setPointSize(12);
  font1.setBold(true);

  pid_base_label = new QLabel();
  pid_base_label->setObjectName("pid_base_label");
  pid_base_label->setFont(font);
  pid_base_label->setLayoutDirection(Qt::LeftToRight);
  pid_base_label->setAlignment(Qt::AlignCenter);
  pid_base_label->setText(tr("2"));

  pid_control_frame = new QFrame();
  pid_control_frame->setObjectName("pid_control_frame");
  pid_control_frame->setFrameShadow(QFrame::Plain);
  pid_control_frame->setLineWidth(4);
  pid_control_frame->setFrameShape(QFrame::HLine);

  pid_multiplier_spinbox = new QSpinBox();
  pid_multiplier_spinbox->setObjectName("pid_multiplier_spinbox");
  pid_multiplier_spinbox->setAlignment(Qt::AlignCenter);
  pid_multiplier_spinbox->setRange(0, 1023);

  connect(pid_multiplier_spinbox, SIGNAL(valueChanged(int)), this,
    SLOT(on_pid_multiplier_spinbox_valueChanged(int)));

  pid_exponent_spinbox = new QSpinBox();
  pid_exponent_spinbox->setObjectName("pid_exponent_spinbox");
  pid_exponent_spinbox->setAlignment(Qt::AlignCenter);
  pid_exponent_spinbox->setRange(0, 18);

  connect(pid_exponent_spinbox, SIGNAL(valueChanged(int)), this,
    SLOT(on_pid_exponent_spinbox_valueChanged(int)));

  pid_equal_label = new QLabel();
  pid_equal_label->setObjectName("pid_equal_label");
  pid_equal_label->setText(tr("="));

  pid_equal_label->setFont(font1);
  pid_equal_label->setAlignment(Qt::AlignCenter);

  pid_constant_lineedit = new QLineEdit();
  pid_constant_lineedit->setObjectName("pid_constant_lineedit");

  QFontMetrics metrics(QApplication::font());
  pid_constant_lineedit->setFixedWidth(metrics.width("0000.00000000"));

  pid_constant_validator *constant_validator =
    new pid_constant_validator(0, 1023, 7, pid_constant_lineedit);
  pid_constant_lineedit->setValidator(constant_validator);

  connect(pid_constant_lineedit, SIGNAL(textEdited(const QString&)), this,
    SLOT(on_pid_constant_lineedit_textEdited(const QString&)));

  connect(pid_constant_lineedit, SIGNAL(editingFinished()),
    this, SLOT(on_pid_constant_lineedit_editingFinished()));

  QGridLayout *group_box_layout = new QGridLayout();
  group_box_layout->addWidget(pid_base_label,3,1,3,2);
  group_box_layout->addWidget(pid_control_frame,2,1,1,5);
  group_box_layout->addWidget(pid_multiplier_spinbox,1,2,1,3);
  group_box_layout->addWidget(pid_exponent_spinbox,3,3,1,3);
  group_box_layout->addWidget(pid_equal_label,2,7,1,2);
  group_box_layout->addWidget(pid_constant_lineedit,1,9,3,1,Qt::AlignCenter);

  groupbox->setLayout(group_box_layout);

  QMetaObject::connectSlotsByName(this);
}

bool pid_constant_control::window_suppress_events() const
{
  return ((main_window *)parent())->suppress_events;
}

void pid_constant_control::set_window_suppress_events(bool suppress_events)
{
  ((main_window *)parent())->suppress_events = suppress_events;
}

main_controller * pid_constant_control::window_controller() const
{
  return ((main_window *)parent())->controller;
}

void pid_constant_control::on_pid_multiplier_spinbox_valueChanged(int value)
{
  if (window_suppress_events()) { return; }
  window_controller()->handle_pid_constant_control_multiplier(index, value);
}

void pid_constant_control::on_pid_exponent_spinbox_valueChanged(int value)
{
  if (window_suppress_events()) { return; }
  window_controller()->handle_pid_constant_control_exponent(index, value);
}

void pid_constant_control::on_pid_constant_lineedit_textEdited(const QString& text)
{
  if (window_suppress_events()) { return; }
  double value = pid_constant_lineedit->displayText().toDouble();

  window_controller()->handle_pid_constant_control_constant(index, value);
}

void pid_constant_control::on_pid_constant_lineedit_editingFinished()
{
  if (window_suppress_events()) { return; }
  window_controller()->recompute_constant(index, pid_multiplier_spinbox->value(), pid_exponent_spinbox->value());
}

