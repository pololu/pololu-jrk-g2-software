#include "main_window.h"
#include "main_controller.h"
#include "graph_window.h"
#include "bootloader_window.h"

#include "to_string.h"

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
#include <string>
#include <QString>

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
  altw = 0;

  widgetAtHome = true;
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
  preview_window->refreshTimer = static_cast<double>(interval_ms);
}

void main_window::start_update_timer()
{
  assert(update_timer);
  update_timer->start();
}

bool main_window::confirm(std::string const & question)
{
  QMessageBox mbox(QMessageBox::Question, windowTitle(),
    QString::fromStdString(question), QMessageBox::Ok | QMessageBox::Cancel, this);
  int button = mbox.exec();
  return button == QMessageBox::Ok;
}

void main_window::show_error_message(std::string const & message)
{
  QMessageBox mbox(QMessageBox::Critical, windowTitle(),
    QString::fromStdString(message), QMessageBox::NoButton, this);
  mbox.exec();
}

void main_window::show_info_message(std::string const & message)
{
  QMessageBox mbox(QMessageBox::Information, windowTitle(),
    QString::fromStdString(message), QMessageBox::NoButton, this);
  mbox.exec();
}

void main_window::show_warning_message(std::string const & message)
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

void main_window::set_device_name(std::string const & name, bool link_enabled)
{
  return; //TODO: fix function
  QString text = QString::fromStdString(name);
  if (link_enabled)
  {
    text = "<a href=\"#doc\">" + text + "</a>";
  }
  device_name_value->setText(text);
}

void main_window::set_serial_number(std::string const & serial_number)
{
  return; //TODO: fix function
  serial_number_value->setText(QString::fromStdString(serial_number));
}

void main_window::set_firmware_version(std::string const & firmware_version)
{
  firmware_version_value->setText(QString::fromStdString(firmware_version));
}

void main_window::set_device_reset(std::string const & device_reset)
{
  device_reset_value->setText(QString::fromStdString(device_reset));
}

void main_window::set_up_time(uint32_t up_time)
{
  up_time_value->setText(QString::fromStdString(
    convert_up_time_to_hms_string(up_time)));
}

void main_window::set_duty_cycle(int16_t duty_cycle)
{
  preview_window->duty_cycle.plot_value = duty_cycle;
  duty_cycle_value->setText(QString::number(duty_cycle));
}

void main_window::set_raw_current_mv(uint16_t current)
{
  preview_window->raw_current.plot_value = current;
  raw_current_value->setText(QString::number(current) + " mV");
}

void main_window::set_scaled_current_mv(int32_t current)
{
  preview_window->scaled_current.plot_value = current;
  scaled_current_value->setText(QString::number(current) + " mV");
}

void main_window::set_current_chopping_log(uint16_t log)
{
  preview_window->current_chopping_log.plot_value = log;
  current_chopping_log_value->setText(QString::number(log));
}

void main_window::set_vin_voltage(uint16_t vin_voltage)
{
  vin_voltage_value->setText(QString::fromStdString(
    convert_mv_to_v_string(vin_voltage)));
}

void main_window::set_error_flags_halting(uint16_t error_flags_halting)
{
  error_flags_halting_value->setText(QString::fromStdString(
    convert_error_flags_to_hex_string(error_flags_halting)));

  for (int i = 0; i < 16; i++)
  {
    if (error_rows[i].stopping_value == NULL) { continue; }

    // setStyleSheet() is expensive, so only call it if something actually
    // changed. Check if there's currently a stylesheet applied and decide
    // whether we need to do anything based on that.
    bool styled = !error_rows[i].stopping_value->styleSheet().isEmpty();

    if (error_flags_halting & (1 << i))
    {
      error_rows[i].stopping_value->setText(tr("Yes"));
      if (!styled)
      {
        error_rows[i].stopping_value->setStyleSheet(
          ":enabled { background-color: red; color: white; }");
      }
    }
    else
    {
      error_rows[i].stopping_value->setText(tr("No"));
      if (styled)
      {
        error_rows[i].stopping_value->setStyleSheet("");
      }
    }
  }
}

void main_window::increment_errors_occurred(uint16_t errors_occurred)
{
  for (int i = 0; i < 32; i++)
  {
    if (error_rows[i].count_value == NULL) { continue; }

    if (errors_occurred & (1 << i))
    {
      error_rows[i].count++;
      error_rows[i].count_value->setText(QString::number(error_rows[i].count));
    }
  }
}

void main_window::reset_error_counts()
{
  for (int i = 0; i < 32; i++)
  {
    if (error_rows[i].count_value == NULL) { continue; }

    error_rows[i].count = 0;
    error_rows[i].count_value->setText(tr("-"));
  }
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
  // TODO: manual_target_widget->setEnabled(enabled);
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

void main_window::setup_ui()
{
  setObjectName(QStringLiteral("main_window"));
  setWindowTitle("Pololu Jrk G2 Configuration Utility");

  update_timer = new QTimer(this);
  update_timer->setObjectName("update_timer");

  central_widget = new QWidget();
  central_widget->setObjectName(QStringLiteral("central_widget"));
  this->setCentralWidget(central_widget);

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
  stop_motor_action->setText(tr("Stop motor")); // TODO: shortcut key with &

  run_motor_action = new QAction(this);
  run_motor_action->setObjectName("run_motor_action");
  run_motor_action->setText(tr("Run motor")); // TODO: shortcut key with &

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
  device_menu->addSeparator();
  device_menu->addAction(reload_settings_action);
  device_menu->addAction(restore_defaults_action);
  device_menu->addAction(apply_settings_action);
  device_menu->addSeparator();
  device_menu->addAction(upgrade_firmware_action);

  help_menu->addAction(documentation_action);
  help_menu->addAction(about_action);

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
  tmp_box.addItem("TXXXXX: #1234567890123456");
  device_list_value->setMinimumWidth(tmp_box.sizeHint().width() * 105 / 100);
  }

  grid_layout = new QGridLayout();
  grid_layout->setObjectName(QStringLiteral("grid_layout"));

  horizontal_layout = new QHBoxLayout();
  horizontal_layout->setSpacing(6);
  horizontal_layout->setObjectName(QStringLiteral("horizontal_layout"));

  preview_window = new graph_widget();
  preview_window->setObjectName(QStringLiteral("preview_window"));
  preview_window->custom_plot->xAxis->setTicks(false);
  preview_window->custom_plot->yAxis->setTicks(false);

  QWidget *preview_plot = preview_window->custom_plot;
  preview_plot->setCursor(Qt::PointingHandCursor);
  preview_plot->setToolTip("Click on preview to view full plot");
  preview_plot->setFixedSize(150,150);

  stop_motor = new QCheckBox(tr("Stop motor"));
  stop_motor->setEnabled(true);  // TODO: doesn't work yet

  tab_widget = new QTabWidget();
  tab_widget->addTab(setup_status_tab(), tr("Status"));
  tab_widget->addTab(setup_input_tab(), tr("Input"));
  tab_widget->addTab(setup_feedback_tab(), tr("Feedback"));
  tab_widget->addTab(setup_pid_tab(), tr("PID"));
  tab_widget->addTab(setup_motor_tab(), tr("Motor"));
  tab_widget->addTab(setup_errors_tab(), tr("Errors"));

  stop_motor_button = new QPushButton();
  stop_motor_button->setObjectName("stop_motor_button");
  stop_motor_button->setText(tr("&Stop Motor"));
  stop_motor_button->setStyleSheet("background-color:red");
  stop_motor_button->setFixedSize(stop_motor_button->sizeHint());

  run_motor_button = new QPushButton();
  run_motor_button->setObjectName("run_motor_button");
  run_motor_button->setText(tr("&Run Motor"));
  run_motor_button->setFixedSize(run_motor_button->sizeHint());

  apply_settings_button = new QPushButton();
  apply_settings_button->setObjectName("apply_settings");
  apply_settings_button->setText(tr("&Apply settings"));
  apply_settings_button->setFixedSize(apply_settings_button->sizeHint());

  QHBoxLayout *stop_and_run_buttons = new QHBoxLayout();
  stop_and_run_buttons->addWidget(stop_motor_button, Qt::AlignLeft);
  stop_and_run_buttons->addWidget(run_motor_button, Qt::AlignLeft);

  grid_layout->addWidget(device_list_label, 0, 0, Qt::AlignRight);
  grid_layout->addWidget(device_list_value, 0, 1, Qt::AlignLeft);
  grid_layout->addItem(new QSpacerItem(device_list_value->sizeHint().width(), 0), 0, 2);
  grid_layout->addWidget(connection_status_value, 0, 3);
  grid_layout->addWidget(stop_motor, 0, 4, Qt::AlignRight);
  grid_layout->addWidget(preview_plot, 0, 5, Qt::AlignLeft);
  grid_layout->addWidget(tab_widget, 1, 0, 1, 6);
  grid_layout->addLayout(stop_and_run_buttons, 2, 0, 1, 3, Qt::AlignLeft);
  grid_layout->addWidget(apply_settings_button, 2, 5, Qt::AlignRight);

  connect(preview_plot, SIGNAL(mousePress(QMouseEvent*)), this,
    SLOT(on_launchGraph_clicked(QMouseEvent*)));

  connect(update_timer, SIGNAL(timeout()), preview_window, SLOT(realtime_data_slot()));

  connect(stop_motor_button, SIGNAL(clicked()),
    stop_motor_action, SLOT(trigger()));

  connect(run_motor_button, SIGNAL(clicked()),
    run_motor_action, SLOT(trigger()));

  connect(apply_settings_button, SIGNAL(clicked()),
    apply_settings_action, SLOT(trigger()));

  connect(
    preview_window->pauseRunButton, &QPushButton::clicked,
    [=](const bool& d) {
      preview_window->pauseRunButton->isChecked() ?
      disconnect(update_timer, SIGNAL(timeout()), preview_window, SLOT(realtime_data_slot())) :
      connect(update_timer, SIGNAL(timeout()), preview_window, SLOT(realtime_data_slot()));
      preview_window->custom_plot->replot();
    });

  central_widget->setLayout(grid_layout);

  QMetaObject::connectSlotsByName(this);
}

void main_window::on_launchGraph_clicked(QMouseEvent *event)
{
  graph_widget *red = preview_window;
  horizontal_layout->removeWidget(red);
  if(altw == 0)
  {
    altw = new graph_window(this);
    connect(altw, SIGNAL(pass_widget(graph_widget*)), this,
    SLOT(receive_widget(graph_widget*)));
  }

  altw->receive_widget(red);
  altw->show();
  widgetAtHome = false;
}

void main_window::receive_widget(graph_widget *widget)
{
  widget->custom_plot->setFixedSize(150,150);
  widget->custom_plot->xAxis->setTicks(false);
  widget->custom_plot->yAxis->setTicks(false);
  widget->custom_plot->setCursor(Qt::PointingHandCursor);
  widget->custom_plot->setToolTip("Click on preview to view full plot");
  grid_layout->addWidget(widget->custom_plot, 0, 5, Qt::AlignLeft);
  // header_layout->addWidget(widget->custom_plot);
  widgetAtHome = true;
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

  layout->addWidget(setup_variables_box(), 0, 0, 1, 1);
  layout->addWidget(setup_manual_target_box(), 1, 0, 1, 1);

  layout->setRowStretch(2, 1);

  status_page_widget->setLayout(layout);
  return status_page_widget;
}

QWidget * main_window::setup_variables_box()
{
  variables_box = new QGroupBox();
  variables_box->setTitle(tr("Variables"));  // TODO: better name?

  QGridLayout * layout = new QGridLayout();

  int row = 0;

  setup_read_only_text_field(layout, row++, &firmware_version_label,
    &firmware_version_value);
  firmware_version_label->setText(tr("Firmware version:"));

  setup_read_only_text_field(layout, row++,
    &device_reset_label, &device_reset_value);
  device_reset_label->setText(tr("Last reset:"));

  setup_read_only_text_field(layout, row++, &up_time_label, &up_time_value);
  up_time_label->setText(tr("Up time:"));

  setup_read_only_text_field(layout, row++, &duty_cycle_label, &duty_cycle_value);
  duty_cycle_label->setText(tr("Duty cycle:"));

  setup_read_only_text_field(layout, row++, &raw_current_label,
    &raw_current_value);
  raw_current_label->setText(tr("Raw current:"));

  current_offset_label = new QLabel();
  current_offset_label->setText(tr("Current offset:"));
  layout->addWidget(current_offset_label, row, 0, Qt::AlignLeft);

  current_offset_value = new QSpinBox();
  current_offset_value->setObjectName("current_offset_value");
  current_offset_value->setRange(0, 5000);
  current_offset_value->setValue(50);
  current_offset_value->setSuffix(" mV");
  layout->addWidget(current_offset_value, row, 1, Qt::AlignLeft);
  row++;

  setup_read_only_text_field(layout, row++, &scaled_current_label,
    &scaled_current_value);
  scaled_current_label->setText(tr("Scaled current:"));

  setup_read_only_text_field(layout, row++,
    &current_chopping_log_label, &current_chopping_log_value);
  current_chopping_log_label->setText(tr("Current chopping log:"));

  setup_read_only_text_field(layout, row++,
    &vin_voltage_label, &vin_voltage_value);
  vin_voltage_label->setText(tr("VIN voltage:"));

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

  manual_target_entry_value = new QSpinBox();
  manual_target_entry_value->setObjectName("manual_target_entry_value");
  // Don't emit valueChanged events while user is typing (e.g. if the user
  // enters 500, we don't want to set targets of 5, 50, and 500).
  manual_target_entry_value->setKeyboardTracking(false);
  manual_target_entry_value->setRange(0, 4095);

  set_target_button = new QPushButton();
  set_target_button->setObjectName("set_target_button");
  set_target_button->setText(tr("Set &target"));

  manual_target_scroll_bar = new QScrollBar(Qt::Horizontal);
  manual_target_scroll_bar->setObjectName("manual_target_scroll_bar");

  // TODO: scroll bar range should be based on the feedback mode in the cached
  // settings, and set with a function named set_manual_target_range that is
  // called from the controller, instead of just hardcoded here
  manual_target_scroll_bar->setMinimum(2048 - 600 - 20);
  manual_target_scroll_bar->setMaximum(2048 + 600 + 20);

  manual_target_scroll_bar->setValue(2048);

  layout->addWidget(manual_target_entry_value, 0, 0);
  layout->addWidget(set_target_button, 0, 1);
  layout->addWidget(manual_target_scroll_bar, 0, 2);

  layout->setRowStretch(1, 1);
  layout->setColumnStretch(2, 1);

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
  layout->addWidget(setup_input_scaling_groupbox(), 0, 1, 3, 1, Qt::AlignTop);

  input_page_widget->setLayout(layout);

  input_page_widget->setParent(tab_widget);

  return input_page_widget;
}

QWidget * main_window::setup_input_analog_groupbox()
{
  input_analog_groupbox = new QGroupBox(tr("Analog to digital conversion"));
  input_analog_groupbox->setObjectName("input_analog_groupbox");

  input_analog_samples_label = new QLabel(tr("Analog samples:"));
  input_analog_samples_label->setObjectName("input_analog_samples_label");

  input_analog_samples_combobox = new QComboBox();
  input_analog_samples_combobox->setObjectName("input_analog_samples_combobox");
  input_analog_samples_combobox->addItem("4", 0);
  input_analog_samples_combobox->addItem("8", 1);
  input_analog_samples_combobox->addItem("16", 2);
  input_analog_samples_combobox->addItem("32", 3);
  input_analog_samples_combobox->addItem("64", 4);
  input_analog_samples_combobox->addItem("128", 5);
  input_analog_samples_combobox->addItem("256", 6);
  input_analog_samples_combobox->addItem("512", 7);
  input_analog_samples_combobox->addItem("1024", 8);

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

  input_usb_dual_port_radio = new QRadioButton(tr("USB Dual Port"));
  input_usb_dual_port_radio->setObjectName("input_usb_dual_port_radio");

  input_usb_chained_radio = new QRadioButton(tr("USB Chained"));
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
  input_serial_layout->addItem(new QSpacerItem(1, fontMetrics().height()), 3, 0);
  input_serial_layout->addWidget(input_enable_crc_checkbox, 4, 0, Qt::AlignLeft);
  input_serial_layout->addLayout(device_layout, 5, 0, Qt::AlignLeft);
  input_serial_layout->addWidget(input_device_number_checkbox, 6, 0, Qt::AlignLeft);
  input_serial_layout->addLayout(timeout_layout, 7, 0, Qt::AlignLeft);
  input_serial_layout->addWidget(input_disable_compact_protocol_checkbox, 8, 0, Qt::AlignLeft);
  input_serial_layout->addItem(new QSpacerItem(1, fontMetrics().height()), 9, 0);
  input_serial_layout->addWidget(input_never_sleep_checkbox, 10, 0, Qt::AlignLeft);

  input_serial_groupbox->setLayout(input_serial_layout);

  return input_serial_groupbox;
}

QWidget * main_window::setup_input_scaling_groupbox()
{
  input_scaling_groupbox = new QGroupBox(tr("Scaling (Analog and Pulse Width mode only)"));
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
  input_scaling_order_warning_label->setStyleSheet(QStringLiteral("color:red"));

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
  input_scaling_layout->addItem(new QSpacerItem(1, fontMetrics().height()), 8, 0);
  input_scaling_layout->addWidget(input_degree_label, 9, 0, Qt::AlignLeft);
  input_scaling_layout->addWidget(input_scaling_degree_combobox, 9, 1, 1, 2, Qt::AlignLeft);
  input_scaling_layout->addItem(new QSpacerItem(input_scaling_degree_combobox->sizeHint().width(), 0), 0, 3);
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
  layout->addWidget(setup_feedback_scaling_groupbox(), 1, 0, Qt::AlignLeft);
  layout->addWidget(setup_feedback_analog_groupbox(), 2, 0,Qt::AlignLeft);

  feedback_page_widget->setLayout(layout);
  return feedback_page_widget;
}

QWidget * main_window::setup_feedback_scaling_groupbox()
{
  QSizePolicy p = this->sizePolicy();
  p.setRetainSizeWhenHidden(true);

  feedback_scaling_groupbox = new QGroupBox(tr("Scaling (Analog and Tachometer mode only)"));
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
  feedback_scaling_order_warning_label->setStyleSheet(QStringLiteral("color:red"));
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
  feedback_scaling_layout->addWidget(feedback_scaling_order_warning_label,2,2,4,2, Qt::AlignCenter);

  feedback_scaling_groupbox->setLayout(feedback_scaling_layout);

  return feedback_scaling_groupbox;
}

QWidget * main_window::setup_feedback_analog_groupbox()
{
  feedback_analog_groupbox = new QGroupBox(tr("Analog to digital conversion"));
  feedback_analog_groupbox->setObjectName("feedback_analog_groupbox");

  feedback_analog_samples_label = new QLabel(tr("Analog samples:"));
  feedback_analog_samples_label->setObjectName("feedback_analog_samples_label");

  feedback_analog_samples_combobox = new QComboBox();
  feedback_analog_samples_combobox->setObjectName("feedback_analog_samples_combobox");
  feedback_analog_samples_combobox->addItem("4", 0);
  feedback_analog_samples_combobox->addItem("8", 1);
  feedback_analog_samples_combobox->addItem("16", 2);
  feedback_analog_samples_combobox->addItem("32", 3);
  feedback_analog_samples_combobox->addItem("64", 4);
  feedback_analog_samples_combobox->addItem("128", 5);
  feedback_analog_samples_combobox->addItem("256", 6);
  feedback_analog_samples_combobox->addItem("512", 7);
  feedback_analog_samples_combobox->addItem("1024", 8);

  feedback_detect_disconnect_checkbox = new QCheckBox(tr("Detect disconnect with power pin"));
  feedback_detect_disconnect_checkbox->setObjectName("feedback_detect_disconnect_checkbox");

  QGridLayout *feedback_analog_layout = new QGridLayout();
  feedback_analog_layout->addWidget(feedback_analog_samples_label,0,0);
  feedback_analog_layout->addWidget(feedback_analog_samples_combobox,0,1,Qt::AlignLeft);
  feedback_analog_layout->addWidget(feedback_detect_disconnect_checkbox,1,0,1,2);

  feedback_analog_groupbox->setLayout(feedback_analog_layout);

  return feedback_analog_groupbox;
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

  pid_integral_limit_label = new QLabel(tr("Integral limit:"));
  pid_integral_limit_label->setObjectName("pid_integral_limit_label");

  pid_integral_limit_spinbox = new QSpinBox();
  pid_integral_limit_spinbox->setObjectName("pid_integral_limit_spinbox");

  pid_reset_integral_checkbox = new QCheckBox(
    tr("Reset integral when proportional term exceeds max duty cycle"));
  pid_reset_integral_checkbox->setObjectName("pid_reset_integral_checkbox");

  pid_feedback_dead_zone_label = new QLabel(tr("Feedback dead zone:"));
  pid_feedback_dead_zone_label->setObjectName("pid_feedback_dead_zone_label");

  pid_feedback_dead_zone_spinbox = new QSpinBox();
  pid_feedback_dead_zone_spinbox->setObjectName("pid_feedback_dead_zone_spinbox");

  QGridLayout *group_box_row = new QGridLayout();
  group_box_row->addWidget(pid_proportional_coefficient_groupbox, 0, 0, Qt::AlignCenter);
  group_box_row->addWidget(pid_integral_coefficient_groupbox, 0, 1, Qt::AlignCenter);
  group_box_row->addWidget(pid_derivative_coefficient_groupbox, 0, 2, Qt::AlignCenter);

  QHBoxLayout *period_row_layout = new QHBoxLayout();
  period_row_layout->addWidget(pid_period_label);
  period_row_layout->addWidget(pid_period_spinbox);

  QHBoxLayout *integral_row_layout = new QHBoxLayout();
  integral_row_layout->addWidget(pid_integral_limit_label);
  integral_row_layout->addWidget(pid_integral_limit_spinbox);

  QHBoxLayout *deadzone_row_layout = new QHBoxLayout();
  deadzone_row_layout->addWidget(pid_feedback_dead_zone_label);
  deadzone_row_layout->addWidget(pid_feedback_dead_zone_spinbox);

  QGridLayout *layout = pid_page_layout = new QGridLayout();
  layout->setSizeConstraint(QLayout::SetFixedSize);
  layout->addLayout(group_box_row,0,0);
  layout->addLayout(period_row_layout,1,0,1,1,Qt::AlignLeft);
  layout->addLayout(integral_row_layout,2,0,1,1,Qt::AlignLeft);
  layout->addWidget(pid_reset_integral_checkbox,3,0,1,2,Qt::AlignLeft);
  layout->addLayout(deadzone_row_layout,4,0,1,1,Qt::AlignLeft);

  pid_page_widget->setLayout(layout);
  return pid_page_widget;
}

QWidget *main_window::setup_motor_tab()
{
  motor_page_widget = new QWidget();

  motor_frequency_label = new QLabel(tr("PVM frequency:"));
  motor_frequency_label->setObjectName("motor_frequency_label");

  motor_pwm_frequency_combobox = new QComboBox();
  motor_pwm_frequency_combobox->setObjectName("motor_pwm_frequency_combobox");
  motor_pwm_frequency_combobox->addItem("20 kHz", JRK_MOTOR_PWM_FREQUENCY_20);
  motor_pwm_frequency_combobox->addItem("5 kHz", JRK_MOTOR_PWM_FREQUENCY_5);

  motor_invert_checkbox = new QCheckBox(tr("Invert motor direction"));
  motor_invert_checkbox->setObjectName("motor_invert_checkbox");

  motor_detect_motor_button = new QPushButton(tr("Detect Motor Direction"));
  motor_detect_motor_button->setObjectName("motor_detect_motor_button");
  motor_detect_motor_button->setEnabled(false); //tmphax: not ready to use

  QGridLayout *motor_controls_layout = new QGridLayout();

  motor_asymmetric_checkbox = new QCheckBox(tr("Asymmetric"));
  motor_asymmetric_checkbox->setObjectName("motor_asymmetric_checkbox");

  motor_forward_label = new QLabel(tr("Forward"));
  motor_forward_label->setObjectName("motor_forward_label");

  motor_reverse_label = new QLabel(tr("Reverse"));
  motor_reverse_label->setObjectName("motor_reverse_label");

  motor_max_duty_cycle_label = new QLabel(tr("Max. duty cycle:"));
  motor_max_duty_cycle_label->setObjectName("motor_max_deceleration_label");

  motor_max_duty_cycle_forward_spinbox = new QSpinBox();
  motor_max_duty_cycle_forward_spinbox->setObjectName("motor_max_duty_cycle_forward_spinbox");
  motor_max_duty_cycle_forward_spinbox->setRange(0, JRK_MAX_ALLOWED_DUTY_CYCLE);

  motor_max_duty_cycle_reverse_spinbox = new QSpinBox();
  motor_max_duty_cycle_reverse_spinbox->setObjectName("motor_max_duty_cycle_reverse_spinbox");
  motor_max_duty_cycle_reverse_spinbox->setRange(0, JRK_MAX_ALLOWED_DUTY_CYCLE);

  motor_max_duty_cycle_means_label = new QLabel(tr("(600 means 100%)"));

  motor_max_acceleration_label = new QLabel(tr("Max. acceleration:"));
  motor_max_acceleration_label->setObjectName("motor_max_acceleration_label");

  motor_max_acceleration_forward_spinbox = new QSpinBox();
  motor_max_acceleration_forward_spinbox->setObjectName("motor_max_acceleration_forward_spinbox");
  motor_max_acceleration_forward_spinbox->setRange(0, JRK_MAX_ALLOWED_DUTY_CYCLE);

  motor_max_acceleration_reverse_spinbox = new QSpinBox();
  motor_max_acceleration_reverse_spinbox->setObjectName("motor_max_acceleration_reverse_spinbox");
  motor_max_acceleration_reverse_spinbox->setRange(0, JRK_MAX_ALLOWED_DUTY_CYCLE);

  motor_max_acceleration_means_label = new QLabel(tr("(600 means no limit)"));

  motor_max_deceleration_label = new QLabel(tr("Max. deceleration:"));
  motor_max_deceleration_label->setObjectName("motor_max_deceleration_label");

  motor_max_deceleration_forward_spinbox = new QSpinBox();
  motor_max_deceleration_forward_spinbox->setObjectName("motor_max_deceleration_forward_spinbox");
  motor_max_deceleration_forward_spinbox->setRange(0, JRK_MAX_ALLOWED_DUTY_CYCLE);

  motor_max_deceleration_reverse_spinbox = new QSpinBox();
  motor_max_deceleration_reverse_spinbox->setObjectName("motor_max_deceleration_reverse_spinbox");
  motor_max_deceleration_reverse_spinbox->setRange(0, JRK_MAX_ALLOWED_DUTY_CYCLE);

  motor_max_deceleration_means_label = new QLabel(tr("(600 means no limit)"));

  motor_brake_duration_label = new QLabel(tr("Brake duration (ms):"));
  motor_brake_duration_label->setObjectName("motor_brake_duration_label");

  motor_brake_duration_forward_spinbox = new QSpinBox();
  motor_brake_duration_forward_spinbox->setObjectName("motor_brake_duration_forward_spinbox");
  motor_brake_duration_forward_spinbox->setRange(0, JRK_MAX_ALLOWED_BRAKE_DURATION);
  motor_brake_duration_forward_spinbox->setSingleStep(JRK_BRAKE_DURATION_UNITS);

  motor_brake_duration_reverse_spinbox = new QSpinBox();
  motor_brake_duration_reverse_spinbox->setObjectName("motor_brake_duration_reverse_spinbox");
  motor_brake_duration_reverse_spinbox->setRange(0, JRK_MAX_ALLOWED_BRAKE_DURATION);
  motor_brake_duration_reverse_spinbox->setSingleStep(JRK_BRAKE_DURATION_UNITS);

  motor_max_current_label = new QLabel(tr("Max. current:"));
  motor_max_current_label->setObjectName("motor_max_current_label");

  motor_max_current_forward_spinbox = new QSpinBox();
  motor_max_current_forward_spinbox->setObjectName("motor_max_current_forward_spinbox");
  motor_max_current_forward_spinbox->setRange(0, 31); // TODO: add macro for max

  motor_max_current_reverse_spinbox = new QSpinBox();
  motor_max_current_reverse_spinbox->setObjectName("motor_max_current_reverse_spinbox");
  motor_max_current_reverse_spinbox->setRange(0, 31); // TODO: add macro for max

  motor_max_current_means_label = new QLabel(tr("(0 to 31)"));
  motor_max_current_means_label->setObjectName("motor_max_current_means_label");

  motor_calibration_label = new QLabel(tr("Current calibration:"));
  motor_calibration_label->setObjectName("motor_calibration_label");

  motor_calibration_forward_spinbox = new QSpinBox();
  motor_calibration_forward_spinbox->setObjectName("motor_calibration_forward_spinbox");
  motor_calibration_forward_spinbox->setRange(1, 255);

  motor_calibration_reverse_spinbox = new QSpinBox();
  motor_calibration_reverse_spinbox->setObjectName("motor_calibration_reverse_spinbox");
  motor_calibration_reverse_spinbox->setRange(1, 255);

  motor_controls_layout->addWidget(motor_asymmetric_checkbox,0,2,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_forward_label,1,1,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_reverse_label,1,2,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_max_duty_cycle_label,2,0,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_max_duty_cycle_forward_spinbox, 2, 1, Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_max_duty_cycle_reverse_spinbox,2,2,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_max_duty_cycle_means_label,2,3,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_max_acceleration_label,3,0,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_max_acceleration_forward_spinbox,3,1,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_max_acceleration_reverse_spinbox,3,2,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_max_acceleration_means_label,3,3,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_max_deceleration_label,4,0,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_max_deceleration_forward_spinbox,4,1,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_max_deceleration_reverse_spinbox,4,2,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_max_deceleration_means_label,4,3,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_brake_duration_label,5,0,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_brake_duration_forward_spinbox,5,1,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_brake_duration_reverse_spinbox,5,2,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_max_current_label,6,0,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_max_current_forward_spinbox,6,1,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_max_current_reverse_spinbox,6,2,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_max_current_means_label,6,3,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_calibration_label,7,0,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_calibration_forward_spinbox,7,1,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_calibration_reverse_spinbox,7,2,Qt::AlignLeft);

  motor_out_of_range_label = new QLabel(tr("Max. duty cycle while feedback is out of range:"));
  motor_out_of_range_label->setObjectName("motor_out_of_range_label");

  motor_out_of_range_spinbox = new QSpinBox();
  motor_out_of_range_spinbox->setObjectName("motor_out_of_range_spinbox");

  motor_outofrange_means_label = new QLabel(tr("(600 means 100%)"));

  motor_off_label = new QLabel(tr("When motor is off:"));
  motor_off_label->setObjectName("motor_off_label");

  motor_brake_radio = new QRadioButton(tr("Brake"));
  motor_brake_radio->setObjectName("motor_brake_radio");
  motor_brake_radio->setChecked(true);

  motor_coast_radio = new QRadioButton(tr("Coast"));
  motor_coast_radio->setObjectName("motor_coast_radio");
  motor_coast_radio->setChecked(false);

  motor_coast_when_off_button_group = new QButtonGroup(this);
  motor_coast_when_off_button_group->setExclusive(true);
  motor_coast_when_off_button_group->setObjectName("motor_coast_when_off_button_group");
  motor_coast_when_off_button_group->addButton(motor_brake_radio, 0);
  motor_coast_when_off_button_group->addButton(motor_coast_radio, 1);

  QHBoxLayout *frequency_layout = new QHBoxLayout();
  frequency_layout->addWidget(motor_frequency_label);
  frequency_layout->addWidget(motor_pwm_frequency_combobox);

  QHBoxLayout *invert_layout = new QHBoxLayout();
  invert_layout->addWidget(motor_invert_checkbox);
  invert_layout->addWidget(motor_detect_motor_button);

  QHBoxLayout *deceleration_layout = new QHBoxLayout();
  deceleration_layout->addWidget(motor_out_of_range_label, -1, Qt::AlignLeft);
  deceleration_layout->addWidget(motor_out_of_range_spinbox, -1, Qt::AlignLeft);
  deceleration_layout->addWidget(motor_outofrange_means_label, -1, Qt::AlignLeft);

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
  QGridLayout *layout = errors_page_layout = new QGridLayout();
  layout->setSizeConstraint(QLayout::SetFixedSize);
  layout->setVerticalSpacing(0);
  QFont font;
  font.setBold(true);
  font.setWeight(75);
  errors_bit_mask_label = new QLabel(tr("Bit mask\n"));
  errors_bit_mask_label->setObjectName("errors_bit_mask_label");
  errors_bit_mask_label->setFont(font);
  errors_error_label = new QLabel(tr("Error\n"));
  errors_error_label->setObjectName("errors_error_label");
  errors_error_label->setFont(font);
  errors_setting_label = new QLabel(tr("Setting\n"));
  errors_setting_label->setObjectName("errors_setting_label");
  errors_setting_label->setFont(font);
  errors_stopping_motor_label = new QLabel(tr("Currently\nstopping motor?\n"));
  errors_stopping_motor_label->setObjectName("errors_stopping_motor_label");
  errors_stopping_motor_label->setAlignment(Qt::AlignCenter);
  errors_stopping_motor_label->setFont(font);
  errors_occurence_count_label = new QLabel(tr("Occurence\ncount\n"));
  errors_occurence_count_label->setObjectName("errors_occurence_count_label");
  errors_occurence_count_label->setAlignment(Qt::AlignCenter);
  errors_occurence_count_label->setFont(font);

  QGridLayout *errors_column_labels = new QGridLayout();
  QWidget *errors_column_labels_frame = new QWidget();

  int row_number = 0;

  QHBoxLayout *errors_bottom_buttons = new QHBoxLayout();
  errors_clear_errors = new QPushButton(tr("&Clear Errors"));
  errors_clear_errors->setObjectName("errors_clear_errors");
  errors_reset_counts = new QPushButton(tr("Reset c&ounts"));
  errors_reset_counts->setObjectName("errors_reset_counts");
  errors_bottom_buttons->addWidget(errors_clear_errors,-1,Qt::AlignRight);
  errors_bottom_buttons->addWidget(errors_reset_counts,-1,Qt::AlignLeft);

  layout->addWidget(errors_bit_mask_label,0,0,Qt::AlignLeft);
  layout->addWidget(errors_error_label,0,1,Qt::AlignCenter);
  layout->addWidget(errors_setting_label,0,2,1,3,Qt::AlignCenter);
  layout->addWidget(errors_stopping_motor_label,0,5,1,2,Qt::AlignRight);
  layout->addWidget(errors_occurence_count_label,0,7,Qt::AlignLeft);
  layout->addWidget(new errors_control(++row_number,
    "awaiting_command", "0x0001", "Awaiting command", false, false, error_rows[JRK_ERROR_AWAITING_COMMAND])
    ,row_number,0,1,8,Qt::AlignCenter);
  layout->addWidget(new errors_control(++row_number,
    "no_power", "0x0002", "No power", false, true, error_rows[JRK_ERROR_NO_POWER])
    ,row_number,0,1,8,Qt::AlignCenter);
  layout->addWidget(new errors_control(++row_number,
    "motor_driver_error", "0x0004", "Motor driver error", false, true, error_rows[JRK_ERROR_MOTOR_DRIVER])
    ,row_number,0,1,8,Qt::AlignCenter);
  layout->addWidget(new errors_control(++row_number,
    "input_invalid", "0x0008", "Input invalid", false, true, error_rows[JRK_ERROR_INPUT_INVALID])
    ,row_number,0,1,8,Qt::AlignCenter);
  layout->addWidget(new errors_control(++row_number,
    "input_disconnect", "0x0010", "Input disconnect", true, true, error_rows[JRK_ERROR_INPUT_DISCONNECT])
    ,row_number,0,1,8,Qt::AlignCenter);
  layout->addWidget(new errors_control(++row_number,
    "feedback_disconnect", "0x0020", "Feedback disconnect", true, true, error_rows[JRK_ERROR_FEEDBACK_DISCONNECT])
    ,row_number,0,1,8,Qt::AlignCenter);
  layout->addWidget(new errors_control(++row_number,
    "max_current_exceeded", "0x0040", "Max. current exceeded", true, true, error_rows[JRK_ERROR_MAXIMUM_CURRENT_EXCEEDED])
    ,row_number,0,1,8,Qt::AlignCenter);
  layout->addWidget(new errors_control(++row_number,
    "serial_signal_error", "0x0080", "Serial signal error", true, false, error_rows[JRK_ERROR_SERIAL_SIGNAL])
    ,row_number,0,1,8,Qt::AlignCenter);
  layout->addWidget(new errors_control(++row_number,
    "serial_overrun", "0x0100", "Serial overrun", true, false, error_rows[JRK_ERROR_SERIAL_OVERRUN])
    ,row_number,0,1,8,Qt::AlignCenter);
  layout->addWidget(new errors_control(++row_number,
    "serial_rx_buffer_full", "0x0200", "Serial RX buffer full", true, false, error_rows[JRK_ERROR_SERIAL_BUFFER_FULL])
    ,row_number,0,1,8,Qt::AlignCenter);
  layout->addWidget(new errors_control(++row_number,
    "serial_crc_error", "0x0400", "Serial CRC error", true, false, error_rows[JRK_ERROR_SERIAL_CRC])
    ,row_number,0,1,8,Qt::AlignCenter);
  layout->addWidget(new errors_control(++row_number,
    "serial_protocol_error", "0x0800", "Serial protocol error", true, false, error_rows[JRK_ERROR_SERIAL_PROTOCOL])
    ,row_number,0,1,8,Qt::AlignCenter);
  layout->addWidget(new errors_control(++row_number,
    "serial_timeout_error", "0x1000", "Serial timeout error", true, false, error_rows[JRK_ERROR_SERIAL_TIMEOUT])
    ,row_number,0,1,8,Qt::AlignCenter);
  layout->addLayout(errors_bottom_buttons,16,0,4,8,Qt::AlignCenter);

  layout->setMargin(0);

  QHBoxLayout *page_layout = new QHBoxLayout();
  page_layout->setSizeConstraint(QLayout::SetFixedSize);
  page_layout->addLayout(layout,Qt::AlignCenter);
  errors_page_widget->setLayout(page_layout);

  return errors_page_widget;
}

void main_window::center_at_startup_if_needed()
{
  // Center the window.  This fixes a strange bug on the Raspbian Jessie where
  // the window would appear in the upper left with its title bar off the
  // screen.  On other platforms, the default window position did not make much
  // sense, so it is nice to center it.
  //
  // In case this causes problems, you can set the TICGUI_CENTER environment
  // variable to "N".
  //
  // NOTE: This position issue on Raspbian is a bug in Qt that should be fixed.
  // Another workaround for it was to uncomment the lines in retranslate() that
  // set up errors_stopping_header_label, error_rows[*].name_label, and
  // manual_target_velocity_mode_radio, but then the Window would strangely
  // start in the lower right.
  auto env = QProcessEnvironment::systemEnvironment();
  if (env.value("TICGUI_CENTER") != "N")
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
  center_at_startup_if_needed();

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

  // TODO: this logic should be in controller, and depend on cached settings
  // feedback mode.
  manual_target_scroll_bar->setValue(2048);
  manual_target_entry_value->setValue(2048);
}

void main_window::on_set_target_button_clicked()
{
  // TODO: this logic should be in controller?
  suppress_events = true;
  manual_target_scroll_bar->setValue(manual_target_entry_value->value());
  suppress_events = false;

  controller->set_target(manual_target_entry_value->value());
}

void main_window::on_manual_target_scroll_bar_valueChanged(int value)
{
  if (suppress_events) { return; }
  manual_target_entry_value->setValue(value);
  on_set_target_button_clicked();  // TODO: remove when we have the auto checkbox
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
  uint8_t input_analog_samples = input_analog_samples_combobox->itemData(index).toUInt();
  controller->handle_input_analog_samples_input(input_analog_samples);
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
  uint8_t feedback_analog_samples = feedback_analog_samples_combobox->itemData(index).toUInt();
  controller->handle_feedback_analog_samples_input(feedback_analog_samples);
}

void main_window::on_feedback_detect_disconnect_checkbox_stateChanged(int state)
{
  if (suppress_events) { return; }
  controller->handle_feedback_detect_disconnect_input(state == Qt::Checked);
}

void main_window::on_pid_period_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_pid_period_input(value);
}

void main_window::on_pid_integral_limit_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_pid_integral_limit_input(value);
}

void main_window::on_pid_reset_integral_checkbox_stateChanged(int state)
{
  if (suppress_events) { return; }
  controller->handle_pid_reset_integral_input(state == Qt::Checked);
}

void main_window::on_pid_feedback_dead_zone_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_pid_feedback_dead_zone_input(value);
}

void main_window::on_motor_pwm_frequency_combobox_currentIndexChanged(int index)
{
  if (suppress_events) { return; }
  uint8_t motor_pwm_frequency = motor_pwm_frequency_combobox->itemData(index).toUInt();
  controller->handle_motor_pwm_frequency_input(motor_pwm_frequency);
}

void main_window::on_motor_invert_checkbox_stateChanged(int state)
{
  if (suppress_events) { return; }
  controller->handle_motor_invert_input(state == Qt::Checked);
}

void main_window::on_motor_detect_motor_button_clicked()
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

void main_window::on_motor_max_duty_cycle_forward_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_motor_max_duty_cycle_forward_input(value);
}

void main_window::on_motor_max_duty_cycle_reverse_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_motor_max_duty_cycle_reverse_input(value);
}

void main_window::on_motor_max_acceleration_forward_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_motor_max_acceleration_forward_input(value);
}

void main_window::on_motor_max_acceleration_reverse_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_motor_max_acceleration_reverse_input(value);
}

void main_window::on_motor_max_deceleration_forward_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_motor_max_deceleration_forward_input(value);
}

void main_window::on_motor_max_deceleration_reverse_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_motor_max_deceleration_reverse_input(value);
}

void main_window::on_motor_brake_duration_forward_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_motor_brake_duration_forward_input(value);
}

void main_window::on_motor_brake_duration_reverse_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_motor_brake_duration_reverse_input(value);
}

void main_window::on_motor_max_current_forward_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_motor_max_current_forward_input(value);
}

void main_window::on_motor_max_current_reverse_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_motor_max_current_reverse_input(value);
}

void main_window::on_motor_calibration_forward_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_motor_current_calibration_forward_input(value);
}

void main_window::on_motor_calibration_reverse_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_motor_current_calibration_reverse_input(value);
}

void main_window::on_motor_out_of_range_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }
  controller->handle_motor_out_range_input(value);
}

void main_window::on_motor_coast_when_off_button_group_buttonToggled(int id, bool checked)
{
  if (suppress_events) { return; }
  if (checked) { controller->handle_motor_coast_when_off_input(id); }
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

static bool ordered(QList<int> p)
{
  for (int i = 0; i < p.size()-1; i++)
  {
    if (p[i] > p[i + 1])
      return false;
  }
  return true;
}

void main_window::set_tab_pages_enabled(bool enabled)
{
  for (int i = 0; i < tab_widget->count(); i++)
  {
    tab_widget->widget(i)->setEnabled(enabled);
  }

  // tmphax, disable tabs not implemented yet
  // tab_widget->widget(1)->setEnabled(false);
  // tab_widget->widget(2)->setEnabled(false);
  // tab_widget->widget(3)->setEnabled(false);
  tab_widget->widget(5)->setEnabled(false);
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

void main_window::set_motor_status_message(std::string const & message, bool stopped)
{
  // // setStyleSheet() is expensive, so only call it if something actually
  // // changed. Check if there's currently a stylesheet applied and decide
  // // whether we need to do anything based on that.
  // bool styled = !motor_status_value->styleSheet().isEmpty();

  // if (!styled && stopped)
  // {
  //   motor_status_value->setStyleSheet("color: red;");
  // }
  // else if (styled && !stopped)
  // {
  //   motor_status_value->setStyleSheet("");
  // }
  // motor_status_value->setText(QString::fromStdString(message));
}

void main_window::set_serial_baud_rate(uint32_t serial_baud_rate)
{
  set_spin_box(input_uart_fixed_baud_spinbox, serial_baud_rate);
}

void main_window::set_serial_device_number(uint8_t serial_device_number)
{
  set_spin_box(input_device_spinbox, serial_device_number);
}

void main_window::set_never_sleep(bool never_sleep)
{
  set_check_box(input_never_sleep_checkbox, never_sleep);
}

void main_window::set_input_mode(uint8_t input_mode)
{
  set_u8_combobox(input_mode_combobox, input_mode);
  switch (input_mode)
  {
    case 0:
      input_analog_groupbox->setEnabled(false);
      input_serial_groupbox->setEnabled(true);
      input_scaling_groupbox->setEnabled(false);
      break;
    case 1:
      input_analog_groupbox->setEnabled(true);
      input_serial_groupbox->setEnabled(true);
      input_scaling_groupbox->setEnabled(true);
      break;
    case 2:
      input_analog_groupbox->setEnabled(false);
      input_serial_groupbox->setEnabled(true);
      input_scaling_groupbox->setEnabled(true);
      break;
    default:
      return;
  }
}

void main_window::set_input_analog_samples_exponent(uint8_t input_analog_samples)
{
  set_u8_combobox(input_analog_samples_combobox, input_analog_samples);
}

void main_window::set_input_invert(bool input_invert)
{
  set_check_box(input_invert_checkbox, input_invert);
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
  switch (feedback_mode)
  {
    case 0:
      feedback_analog_groupbox->setEnabled(false);
      feedback_scaling_groupbox->setEnabled(false);
      break;
    case 1:
      feedback_analog_groupbox->setEnabled(true);
      feedback_scaling_groupbox->setEnabled(true);
      break;
    case 2:
      feedback_analog_groupbox->setEnabled(false);
      feedback_scaling_groupbox->setEnabled(true);
      break;
    default:
      return;
  }
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

void main_window::set_feedback_detect_disconnect(bool feedback_detect_disconnect)
{
  set_check_box(feedback_detect_disconnect_checkbox, feedback_detect_disconnect);
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
  QDoubleSpinBox *spin = pid_constant_controls[index]->pid_constant_spinbox;
  set_double_spin_box(spin, value);
}

void main_window::set_pid_period(uint16_t value)
{
  set_spin_box(pid_period_spinbox, value);
}

void main_window::set_pid_integral_limit(uint16_t value)
{
  set_spin_box(pid_integral_limit_spinbox, value);
}

void main_window::set_pid_reset_integral(bool enabled)
{
  set_check_box(pid_reset_integral_checkbox, enabled);
}

void main_window::set_feedback_dead_zone(uint8_t value)
{
  set_spin_box(pid_feedback_dead_zone_spinbox, value);
}

void main_window::set_motor_pwm_frequency(uint8_t pwm_frequency)
{
  set_u8_combobox(motor_pwm_frequency_combobox, pwm_frequency);
}

void main_window::set_motor_invert(bool enabled)
{
  set_check_box(motor_invert_checkbox, enabled);
}

void main_window::set_motor_asymmetric(bool checked)
{
  set_check_box(motor_asymmetric_checkbox, checked);
  motor_max_duty_cycle_reverse_spinbox->setEnabled(checked);
  motor_max_acceleration_reverse_spinbox->setEnabled(checked);
  motor_max_deceleration_reverse_spinbox->setEnabled(checked);
  motor_brake_duration_reverse_spinbox->setEnabled(checked);
  motor_max_current_reverse_spinbox->setEnabled(checked);
  motor_calibration_reverse_spinbox->setEnabled(checked);

}

void main_window::set_motor_max_duty_cycle_forward(uint16_t duty_cycle)
{
  set_spin_box(motor_max_duty_cycle_forward_spinbox, duty_cycle);
}

void main_window::set_motor_max_duty_cycle_reverse(uint16_t duty_cycle)
{
  set_spin_box(motor_max_duty_cycle_reverse_spinbox, duty_cycle);
}

void main_window::set_motor_max_acceleration_forward(uint16_t acceleration)
{
  set_spin_box(motor_max_acceleration_forward_spinbox, acceleration);
}

void main_window::set_motor_max_acceleration_reverse(uint16_t acceleration)
{
  set_spin_box(motor_max_acceleration_reverse_spinbox, acceleration);
}

void main_window::set_motor_max_deceleration_forward(uint16_t acceleration)
{
  set_spin_box(motor_max_deceleration_forward_spinbox, acceleration);
}

void main_window::set_motor_max_deceleration_reverse(uint16_t acceleration)
{
  set_spin_box(motor_max_deceleration_reverse_spinbox, acceleration);
}

void main_window::set_motor_brake_duration_forward(uint32_t brake_duration)
{
  set_spin_box(motor_brake_duration_forward_spinbox, brake_duration);
}

void main_window::set_motor_brake_duration_reverse(uint32_t brake_duration)
{
  set_spin_box(motor_brake_duration_reverse_spinbox, brake_duration);
}

void main_window::set_motor_max_current_forward(uint16_t current)
{
  set_spin_box(motor_max_current_forward_spinbox, current);
}

void main_window::set_motor_max_current_reverse(uint16_t current)
{
  set_spin_box(motor_max_current_reverse_spinbox, current);
}

void main_window::set_motor_current_calibration_forward(uint16_t current)
{
  set_spin_box(motor_calibration_forward_spinbox, current);
}

void main_window::set_motor_current_calibration_reverse(uint16_t current)
{
  set_spin_box(motor_calibration_reverse_spinbox, current);
}

void main_window::set_motor_max_duty_cycle_out_of_range(uint16_t value)
{
  set_spin_box(motor_out_of_range_spinbox, value);
}

void main_window::set_motor_coast_when_off(uint8_t value)
{
  suppress_events = true;
  QAbstractButton * radio = motor_coast_when_off_button_group->button(value);
  if (radio)
  {
    radio->setChecked(true);
  }
  else
  {
    // The value doesn't correspond with any of the radio buttons, so clear
    // the currently selected button, if any.
    QAbstractButton * checked = motor_coast_when_off_button_group->checkedButton();
    if (checked)
    {
      motor_coast_when_off_button_group->setExclusive(false);
      checked->setChecked(false);
      motor_coast_when_off_button_group->setExclusive(true);
    }
  }
  suppress_events = false;
}

void main_window::on_update_timer_timeout()
{
  controller->update();
}

bool main_window::motor_asymmetric_checked()
{
  return motor_asymmetric_checkbox->isChecked();
}

int32_t main_window::get_current_offset_mv()
{
  return current_offset_value->value();
}

void pid_constant_control::setup(QGroupBox * groupbox)
{
  QFont font;
  font.setFamily(QStringLiteral("MS Shell Dlg 2"));
  font.setPointSize(16);
  font.setBold(true);
  font.setWeight(75);

  QFont font1;
  font1.setPointSize(12);
  font1.setBold(true);
  font1.setWeight(75);

  pid_base_label = new QLabel();
  pid_base_label->setObjectName("pid_base_label");
  pid_base_label->setFont(font);
  pid_base_label->setLayoutDirection(Qt::LeftToRight);
  pid_base_label->setAlignment(Qt::AlignCenter);
  pid_base_label->setText(tr("2"));

  pid_control_frame = new QFrame();
  pid_control_frame->setObjectName(QStringLiteral("pid_control_frame"));
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
  pid_exponent_spinbox->setRange(0, 15);
  connect(pid_exponent_spinbox, SIGNAL(valueChanged(int)), this,
    SLOT(on_pid_exponent_spinbox_valueChanged(int)));

  pid_equal_label = new QLabel();
  pid_equal_label->setObjectName("pid_equal_label");
  pid_equal_label->setText(tr("="));

  pid_equal_label->setFont(font1);
  pid_equal_label->setAlignment(Qt::AlignCenter);

  pid_constant_spinbox = new QDoubleSpinBox();
  pid_constant_spinbox->setObjectName("pid_constant_spinbox");
  pid_constant_spinbox->setDecimals(5);
  pid_constant_spinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
  pid_constant_spinbox->setRange(0, 1023);
  connect(pid_constant_spinbox, SIGNAL(valueChanged(double)), this,
    SLOT(on_pid_constant_spinbox_valueChanged(double)));

  QGridLayout *group_box_layout = new QGridLayout();
  group_box_layout->addWidget(pid_base_label,3,1,3,2);
  group_box_layout->addWidget(pid_control_frame,2,1,1,5);
  group_box_layout->addWidget(pid_multiplier_spinbox,1,2,1,3);
  group_box_layout->addWidget(pid_exponent_spinbox,3,3,1,3);
  group_box_layout->addWidget(pid_equal_label,2,7,1,2);
  group_box_layout->addWidget(pid_constant_spinbox,1,9,3,1,Qt::AlignCenter);
  group_box_layout->setSizeConstraint(QLayout::SetFixedSize);

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

void pid_constant_control::on_pid_constant_spinbox_valueChanged(double value)
{
  if (window_suppress_events()) { return; }
  window_controller()->handle_pid_constant_control_constant(index, value);
}

errors_control::errors_control
(int row_number, const QString& object_name, const QString& bit_mask_text,
    const QString& error_label_text, const bool& disabled_visible,
      const bool& enabled_visible, error_row &er, QWidget *parent)
{
  QSizePolicy p = this->sizePolicy();
  p.setRetainSizeWhenHidden(true);

  er.errors_frame = new QWidget();

  if(row_number % 2 != 0)
  {
    er.errors_frame->setStyleSheet(QStringLiteral("background-color:rgb(230,229,229)"));
  }

  er.count = 0;

  errors_central = new QGridLayout(this);

  er.bit_mask_label = new QLabel();
  er.bit_mask_label->setObjectName("bit_mask_label");

  er.error_label = new QLabel();
  er.error_label->setObjectName("error_label");

  er.disabled_radio = new QRadioButton(tr("Disabled"));
  er.disabled_radio->setObjectName("disabled_radio");
  er.disabled_radio->setSizePolicy(p);

  er.enabled_radio = new QRadioButton(tr("Enabled"));
  er.enabled_radio->setObjectName("enabled_radio");
  er.enabled_radio->setSizePolicy(p);

  er.latched_radio = new QRadioButton(tr("Enabled and latched"));
  er.latched_radio->setObjectName("latched_radio");

  er.stopping_value = new QLabel(tr("No"));
  er.stopping_value->setObjectName("stopping_value");

  er.count_value = new QLabel(tr("0"));
  er.count_value->setObjectName("count_value");

  setObjectName(object_name);
  er.bit_mask_label->setText(bit_mask_text);
  er.error_label->setText(error_label_text);

  // Set the size of the labels and buttons for the errors tab in
  // a way that can change from OS to OS.
  {
    QRadioButton tmp_button;
    tmp_button.setText("xxxxxxxxxxxxxxxxxxxxxxxxxxx");
    QRadioButton tmp_button_2;
    tmp_button_2.setText("xxxxxxxxx");
    QLabel tmp_label;
    tmp_label.setText("xxxxxxxxxxxxxxxxxxxxxxxx");
    QLabel tmp_label2;
    tmp_label2.setText("xxxxxxxxxx");
    er.bit_mask_label->setFixedWidth(tmp_label2.sizeHint().width());
    er.error_label->setFixedWidth(tmp_label.sizeHint().width());
    er.disabled_radio->setFixedWidth(tmp_button_2.sizeHint().width());
    er.enabled_radio->setFixedWidth(tmp_button_2.sizeHint().width());
    er.latched_radio->setFixedWidth(tmp_button.sizeHint().width());
    er.stopping_value->setFixedWidth(tmp_label2.sizeHint().width() * 150/100);
    er.count_value->setFixedWidth(tmp_label2.sizeHint().width());
  }


  errors_central->addWidget(er.errors_frame,0,0,3,9);
  errors_central->addWidget(er.bit_mask_label,1,1,Qt::AlignLeft);
  errors_central->addWidget(er.error_label,1,2,Qt::AlignCenter);
  errors_central->addWidget(er.disabled_radio,1,3,Qt::AlignRight);
  errors_central->addWidget(er.enabled_radio,1,4,Qt::AlignCenter);
  errors_central->addWidget(er.latched_radio,1,5,Qt::AlignLeft);
  errors_central->addWidget(er.stopping_value,1,6,Qt::AlignLeft);
  errors_central->addWidget(er.count_value,1,7,1,2,Qt::AlignRight);
  errors_central->setColumnStretch(6,10);

  er.disabled_radio->setVisible(disabled_visible);
  er.enabled_radio->setVisible(enabled_visible);

  errors_central->setMargin(0);
}
