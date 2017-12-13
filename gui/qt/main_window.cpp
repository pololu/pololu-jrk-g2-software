#include "main_window.h"
#include "qcustomplot.h"
#include "graph_window.h"

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include <QMainWindow>
#include <QSerialPort>
#include <QVector>
#include <QtCore>
#include <QCloseEvent>

main_window::main_window(QWidget * parent)
  : QMainWindow(parent)
{
  setup_ui(this);
  altw = 0;

  widgetAtHome = true;
}

main_window::~main_window()
{
  if (altw != 0)
  {
    delete altw;
  }
}

void main_window::setup_ui(QMainWindow *main_window)
{
  font.setPointSizeF(8.25);

  main_window->setObjectName(QStringLiteral("main_window"));
  main_window->setWindowTitle("Pololu Jrk G2 Configuration Utility");

  central_widget = new QWidget(main_window);
  central_widget->setObjectName(QStringLiteral("central_widget"));

  menu_bar = new QMenuBar();

  file_menu = menu_bar->addMenu("");
  file_menu->setTitle(tr("&File"));
 
  open_settings_action = new QAction(this);
  open_settings_action->setObjectName("open_settings_action");
  open_settings_action->setText(tr("&Open settings file..."));
  open_settings_action->setShortcut(Qt::CTRL + Qt::Key_O);
  file_menu->addAction(open_settings_action);

  save_settings_action = new QAction(this);
  save_settings_action->setObjectName("save_settings_action");
  save_settings_action->setText(tr("&Save settings file..."));
  save_settings_action->setShortcut(Qt::CTRL + Qt::Key_S);
  file_menu->addAction(save_settings_action);

  file_menu->addSeparator();

  exit_action = new QAction(this);
  exit_action->setObjectName("exit_action");
  exit_action->setText(tr("E&xit"));
  exit_action->setShortcut(QKeySequence::Quit);
  connect(exit_action, SIGNAL(triggered()), this, SLOT(close()));
  file_menu->addAction(exit_action);

  device_menu = menu_bar->addMenu("");
  device_menu->setTitle(tr("&Device"));

  disconnect_action = new QAction(this);
  disconnect_action->setObjectName("disconnect_action");
  disconnect_action->setText(tr("&Disconnect"));
  disconnect_action->setShortcut(Qt::CTRL + Qt::Key_D);
  device_menu->addAction(disconnect_action);

  device_menu->addSeparator();

  reload_settings_action = new QAction(this);
  reload_settings_action->setObjectName("reload_settings_action");
  reload_settings_action->setText(tr("Re&load settings from device"));
  device_menu->addAction(reload_settings_action);

  restore_defaults_action = new QAction(this);
  restore_defaults_action->setObjectName("restore_defaults_action");
  restore_defaults_action->setText(tr("&Restore default settings"));
  device_menu->addAction(restore_defaults_action);

  apply_settings_action = new QAction(this);
  apply_settings_action->setObjectName("apply_settings_action");
  apply_settings_action->setText(tr("&Apply settings"));
  apply_settings_action->setShortcut(Qt::CTRL + Qt::Key_P);
  device_menu->addAction(apply_settings_action);

  upgrade_firmware_action = new QAction(this);
  upgrade_firmware_action->setObjectName("upgrade_firmware_action");
  upgrade_firmware_action->setText(tr("&Upgrade firmware..."));
  device_menu->addAction(upgrade_firmware_action);

  help_menu = menu_bar->addMenu("");
  help_menu->setTitle(tr("&Help"));

  documentation_action = new QAction(this);
  documentation_action->setObjectName("documentation_action");
  documentation_action->setText(tr("&Online documentation..."));
  documentation_action->setShortcut(QKeySequence::HelpContents);
  help_menu->addAction(documentation_action);

  about_action = new QAction(this);
  about_action->setObjectName("about_action");
  about_action->setText(tr("&About..."));
  about_action->setShortcut(QKeySequence::WhatsThis);
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

  grid_layout = new QGridLayout(central_widget);
  grid_layout->setSpacing(6);
  grid_layout->setContentsMargins(11,11,11,11);
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
  
  // tmphax:stop graph 
  connect(
    stop_motor, &QCheckBox::stateChanged,
    [=](const bool& d) {
      stop_motor->isChecked() ? preview_window->data_timer.stop() : 
      preview_window->data_timer.start(preview_window->refreshTimer);
      preview_window->custom_plot->replot();
    });

  header_layout->addWidget(device_list_label);
  header_layout->addWidget(device_list_value);
  header_layout->addWidget(connection_status_value, 1, Qt::AlignLeft);
  header_layout->addWidget(stop_motor);
  header_layout->addWidget(preview_plot);

  grid_layout->addLayout(header_layout,0,0);

  tab_widget = new QTabWidget();

  tab_widget->addTab(setup_status_tab(), tr("Status"));
  tab_widget->addTab(setup_input_tab(), tr("Input"));
  tab_widget->addTab(setup_feedback_tab(), tr("Feedback"));
  tab_widget->addTab(setup_pid_tab(), tr("PID"));
  tab_widget->addTab(setup_motor_tab(), tr("Motor"));
  tab_widget->addTab(setup_errors_tab(), tr("Errors"));

  grid_layout->addWidget(tab_widget,1,0);

  motorOffButton = new QPushButton();
  motorOnButton = new QPushButton();
  apply_settings = new QPushButton();
  footer_layout = new QHBoxLayout();

  // motorOffButton->resize(75,23);
  motorOffButton->setObjectName("motorOffButton");
  motorOffButton->setText(tr("&Stop Motor"));
  motorOffButton->setStyleSheet(QStringLiteral("background-color:red"));
  motorOffButton->setFixedSize(motorOffButton->sizeHint());

  // motorOnButton->resize(75,23);
  motorOnButton->setObjectName("motorOnButton");
  motorOnButton->setText(tr("&Run Motor"));
  motorOnButton->setFixedSize(motorOnButton->sizeHint());

  // apply_settings->resize(148,23);
  apply_settings->setObjectName("apply_settings");
  apply_settings->setText(tr("&Apply settings to device"));
  apply_settings->setFixedSize(apply_settings->sizeHint());

  footer_layout->addWidget(motorOffButton,Qt::AlignRight);
  footer_layout->addWidget(motorOnButton,Qt::AlignLeft);
  footer_layout->addSpacing(200);
  footer_layout->addWidget(apply_settings,Qt::AlignRight);
  footer_layout->setStretch(2,3);

  grid_layout->addLayout(footer_layout,2,0);
  
  connect(preview_plot, SIGNAL(mousePress(QMouseEvent*)), this, 
    SLOT(on_launchGraph_clicked(QMouseEvent*)));

  main_window->setCentralWidget(central_widget);
  main_window->setMenuBar(menu_bar);

  QMetaObject::connectSlotsByName(main_window);

}

void main_window::on_launchGraph_clicked(QMouseEvent *event)
{
  graph_widget *red = preview_window;
  horizontal_layout->removeWidget(red);
  if(altw == 0)
  {
    altw = new AltWindow(this);
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
  header_layout->addWidget(widget->custom_plot);
  widgetAtHome = true;
}

void main_window::context_menu_event(QContextMenuEvent *event)
{
  if (widgetAtHome)
  {
    QMenu menu(this);
    menu.addAction(sepAct);
    menu.exec(event->globalPos());
  }
}

void main_window::retranslate_ui(QMainWindow *main_window)
{
  main_window->setWindowTitle(QApplication::translate("main_window", "main_window", Q_NULLPTR));
}

QWidget * main_window::setup_status_tab()
{
  status_page_widget = new QWidget();
  return status_page_widget;
}

QWidget * main_window::setup_input_tab()
{
  input_page_widget = new QWidget();
  QGridLayout *layout = input_page_layout = new QGridLayout();
  layout->setSizeConstraint(QLayout::SetFixedSize);
  QVBoxLayout *column1 = new QVBoxLayout();
  QVBoxLayout *column2 = new QVBoxLayout();

  QHBoxLayout *input_mode_layout = new QHBoxLayout();
  input_mode_label = new QLabel(tr("Input mode:"));
  input_mode_label->setObjectName("input_mode_label");
  input_mode_label->setFont(font);
  input_mode_layout->addWidget(input_mode_label);
  input_mode_combobox = new QComboBox();
  input_mode_combobox->setObjectName("input_mode_combobox");
  input_mode_layout->addWidget(input_mode_combobox);
  column1->addLayout(input_mode_layout);

  QVBoxLayout *input_analog_layout = new QVBoxLayout();
  input_analog_groupbox = new QGroupBox(tr("Analog to digital conversion"));
  input_analog_groupbox->setObjectName("input_analog_groupbox");
  QHBoxLayout *analog_samples = new QHBoxLayout();
  input_analog_samples_label = new QLabel(tr("Analog samples:"));
  input_analog_samples_label->setObjectName("input_analog_samples_label");
  input_analog_samples_label->setFont(font);
  analog_samples->addWidget(input_analog_samples_label);
  input_analog_samples_combobox = new QComboBox();
  input_analog_samples_combobox->setObjectName("input_analog_samples_combobox");
  analog_samples->addWidget(input_analog_samples_combobox);
  input_analog_layout->addLayout(analog_samples);
  input_disconnect_with_aux_checkbox = new QCheckBox(tr("Detect disconnect with AUX"));
  input_disconnect_with_aux_checkbox->setObjectName("input_disconnect_with_aux_checkbox");
  input_analog_layout->addWidget(input_disconnect_with_aux_checkbox);
  input_analog_groupbox->setLayout(input_analog_layout);
  column1->addWidget(input_analog_groupbox);

  QGridLayout *input_scaling_layout = new QGridLayout();
  input_scaling_groupbox = new QGroupBox(tr("Scaling(Analog and Pulse Width mode only)"));
  input_scaling_groupbox->setObjectName("input_scaling_groupbox");
  QVBoxLayout *scaling_column1 = new QVBoxLayout();
  input_invert_checkbox = new QCheckBox(tr("Invert input direction"));
  input_invert_checkbox->setObjectName("input_invert_checkbox");
  scaling_column1->addWidget(input_invert_checkbox);
  
  QGridLayout *scaling_column1_labels = new QGridLayout();
  
  input_absolute_max_label = new QLabel(tr("Absolute max:"));
  input_absolute_max_label->setObjectName("input_absolute_max_label");
  scaling_column1_labels->addWidget(input_absolute_max_label,1,0);
  input_maximum_label = new QLabel(tr("Maximum:"));
  input_maximum_label->setObjectName("input_maximum_label");
  scaling_column1_labels->addWidget(input_maximum_label,2,0);
  input_neutral_max_label = new QLabel(tr("Neutral max:"));
  input_neutral_max_label->setObjectName("input_neutral_max_label");
  scaling_column1_labels->addWidget(input_neutral_max_label,3,0);
  input_neutral_min_label = new QLabel(tr("Neutral min:"));
  input_neutral_min_label->setObjectName("input_neutral_min_label");
  scaling_column1_labels->addWidget(input_neutral_min_label,4,0);
  input_minimum_label = new QLabel(tr("Minimum:"));
  input_minimum_label->setObjectName("input_minimum_label");
  scaling_column1_labels->addWidget(input_minimum_label,5,0);
  input_absolute_min_label = new QLabel(tr("Absolute min:"));
  input_absolute_min_label->setObjectName("input_absolute_min_label");
  scaling_column1_labels->addWidget(input_absolute_min_label,6,0);
  input_degree_label = new QLabel(tr("Degree:"));
  input_degree_label->setObjectName("input_degree_label");
  scaling_column1_labels->addWidget(input_degree_label,7,0,7,0,Qt::AlignTop);

  input_input_label = new QLabel(tr("Input"));
  input_input_label->setObjectName("input_input_label");
  scaling_column1_labels->addWidget(input_input_label,0,1);
  input_absolute_max_spinbox = new QDoubleSpinBox();
  input_absolute_max_spinbox->setObjectName("input_absolute_max_spinbox");
  scaling_column1_labels->addWidget(input_absolute_max_spinbox,1,1);
  input_maximum_spinbox = new QDoubleSpinBox();
  input_maximum_spinbox->setObjectName("input_maximum_spinbox");
  scaling_column1_labels->addWidget(input_maximum_spinbox,2,1);
  input_neutral_max_spinbox = new QDoubleSpinBox();
  input_neutral_max_spinbox->setObjectName("input_neutral_max_spinbox");
  scaling_column1_labels->addWidget(input_neutral_max_spinbox,3,1);
  input_neutral_min_spinbox = new QDoubleSpinBox();
  input_neutral_min_spinbox->setObjectName("input_neutral_min_spinbox");
  scaling_column1_labels->addWidget(input_neutral_min_spinbox,4,1);
  input_minimum_spinbox = new QDoubleSpinBox();
  input_minimum_spinbox->setObjectName("input_minimum_spinbox");
  scaling_column1_labels->addWidget(input_minimum_spinbox,5,1);    
  input_absolute_min_spinbox = new QDoubleSpinBox();
  input_absolute_min_spinbox->setObjectName("input_absolute_min_spinbox");
  scaling_column1_labels->addWidget(input_absolute_min_spinbox,6,1);    
  input_degree_combobox = new QComboBox();
  input_degree_combobox->setObjectName("input_degree_combobox");
  scaling_column1_labels->addWidget(input_degree_combobox,7,1,7,2,Qt::AlignTop); 

  input_target_label = new QLabel(tr("Target"));
  input_target_label->setObjectName("input_target_label");
  scaling_column1_labels->addWidget(input_target_label,0,2);
  input_output_maximum_spinbox = new QDoubleSpinBox();
  input_output_maximum_spinbox->setObjectName("input_output_maximum_spinbox");
  scaling_column1_labels->addWidget(input_output_maximum_spinbox,2,2);
  input_output_neutral_spinbox = new QDoubleSpinBox();
  input_output_neutral_spinbox->setObjectName("input_output_neutral_spinbox");
  scaling_column1_labels->addWidget(input_output_neutral_spinbox,3,2);
  input_output_minimum_spinbox = new QDoubleSpinBox();
  input_output_minimum_spinbox->setObjectName("input_output_minimum_spinbox");
  scaling_column1_labels->addWidget(input_output_minimum_spinbox,4,2);

  scaling_column1->addLayout(scaling_column1_labels);
  

  input_scaling_layout->addLayout(scaling_column1,0,0);

  QVBoxLayout *scaling_column2 = new QVBoxLayout();
  input_learn_button = new QPushButton();
  input_learn_button->setObjectName("input_learn_button");
  input_learn_button->setText(tr("Learn.."));
  scaling_column2->addWidget(input_learn_button);
  
 
  input_reset_range_button = new QPushButton();
  input_reset_range_button->setObjectName("input_reset_range_button");
  input_reset_range_button->setText(tr("Reset to full range"));
  scaling_column2->addWidget(input_reset_range_button);

  input_scaling_order_warning_label = new QLabel(
    tr("Warning: some of the values\nare not in the correct order."));
  input_scaling_order_warning_label->setObjectName("input_scaling_order_waring_label");
  input_scaling_order_warning_label->setStyleSheet(QStringLiteral("color:red"));
  scaling_column2->addWidget(input_scaling_order_warning_label);  
  input_scaling_layout->addLayout(scaling_column2,0,1);
  input_scaling_groupbox->setLayout(input_scaling_layout);
  column2->addWidget(input_scaling_groupbox,-1,Qt::AlignCenter);

  QVBoxLayout *input_serial_layout = new QVBoxLayout();
  input_serial_groupbox = new QGroupBox(tr("Serial Interface"));
  input_serial_groupbox->setObjectName("input_serial_groupbox");
  input_usb_dual_port_radio = new QRadioButton(tr("USB Dual Port"));
  input_usb_dual_port_radio->setObjectName("input_usb_dual_port_radio");
  input_serial_layout->addWidget(input_usb_dual_port_radio);
  input_usb_chained_radio = new QRadioButton(tr("USB Chained"));
  input_usb_chained_radio->setObjectName("input_usb_chained_radio");
  input_serial_layout->addWidget(input_usb_chained_radio);
  input_uart_detect_baud_radio = new QRadioButton(tr("UART, detect baud rate"));
  input_uart_detect_baud_radio->setObjectName("input_uart_detect_baud_radio");
  input_serial_layout->addWidget(input_uart_detect_baud_radio);
  QHBoxLayout *uart_fixed_baud_rate = new QHBoxLayout();
  input_uart_fixed_baud_radio = new QRadioButton(tr("UART, fixed baud rate: "));
  input_uart_fixed_baud_radio->setObjectName("input_uart_fixed_baud_radio");
  input_uart_fixed_baud_spinbox = new QDoubleSpinBox();
  input_uart_fixed_baud_spinbox->setObjectName("input_uart_fixed_baud_spinbox");
  uart_fixed_baud_rate->addWidget(input_uart_fixed_baud_radio);
  uart_fixed_baud_rate->addWidget(input_uart_fixed_baud_spinbox);
  input_serial_layout->addLayout(uart_fixed_baud_rate);
  input_enable_crc_checkbox = new QCheckBox(tr("Enable CRC"));
  QHBoxLayout *device_number = new QHBoxLayout();
  input_device_label = new QLabel(tr("Device Number:"));
  input_device_label->setObjectName("input_device_label");
  input_device_spinbox = new QDoubleSpinBox();
  input_device_spinbox->setObjectName("input_device_spinbox");
  device_number->addWidget(input_device_label);
  device_number->addWidget(input_device_spinbox);
  input_serial_layout->addLayout(device_number);
  QHBoxLayout *timeout_row = new QHBoxLayout();
  input_timeout_label = new QLabel(tr("Timeout (s):"));
  input_timeout_label->setObjectName("input_timeout_label");
  input_timeout_spinbox = new QDoubleSpinBox();
  input_timeout_spinbox->setObjectName("input_timeout_spinbox");
  timeout_row->addWidget(input_timeout_label);
  timeout_row->addWidget(input_timeout_spinbox);
  input_serial_layout->addLayout(timeout_row);
  input_never_sleep_checkbox = new QCheckBox(tr("Never sleep (ignore USB suspend)"));
  input_never_sleep_checkbox->setObjectName("input_never_sleep_checkbox");
  input_serial_layout->addWidget(input_never_sleep_checkbox);
  input_serial_groupbox->setLayout(input_serial_layout);
  column1->addWidget(input_serial_groupbox);

  layout->addLayout(column1,0,0);
  layout->addLayout(column2,0,1);

  input_page_widget->setLayout(layout);
  input_page_widget->setParent(tab_widget);
  return input_page_widget;
}

QWidget * main_window::setup_feedback_tab()
{
  feedback_page_widget = new QWidget();
  QGridLayout *layout = feedback_page_layout = new QGridLayout();
  layout->setSizeConstraint(QLayout::SetFixedSize);
  QHBoxLayout *feedback_mode_layout = new QHBoxLayout();
  feedback_mode_label = new QLabel(tr("Feedback mode:"));
  feedback_mode_label->setObjectName("feedback_mode_label");
  feedback_mode_layout->addWidget(feedback_mode_label);
  feedback_mode_combobox = new QComboBox();
  feedback_mode_combobox->setObjectName("feedback_mode_combobox");
  feedback_mode_layout->addWidget(feedback_mode_combobox);

  QGridLayout *feedback_scaling_layout = new QGridLayout();
  feedback_scaling_groupbox = new QGroupBox(tr("Scaling (Analog and Tachometer mode only"));
  feedback_scaling_groupbox->setObjectName("feedback_scaling_groupbox");
  feedback_invert_feedback_checkbox = new QCheckBox(tr("Invert feedback direction"));
  feedback_invert_feedback_checkbox->setObjectName("feedback_invert_feedback_checkbox");
  feedback_scaling_layout->addWidget(feedback_invert_feedback_checkbox,0,0,0,3,Qt::AlignTop);
  feedback_absolute_max_label = new QLabel(tr("Asolute max:"));
  feedback_absolute_max_label->setObjectName("feedback_absolute_max_label");
  feedback_scaling_layout->addWidget(feedback_absolute_max_label,2,0);
  feedback_maximum_label = new QLabel(tr("Maximum:"));
  feedback_maximum_label->setObjectName("feedback_maximum_label");
  feedback_scaling_layout->addWidget(feedback_maximum_label,3,0);
  feedback_minimum_label = new QLabel(tr("Minimum:"));
  feedback_minimum_label->setObjectName("feedback_minimum_label");
  feedback_scaling_layout->addWidget(feedback_minimum_label,4,0);
  feedback_absolute_min_label = new QLabel(tr("Absolute min:"));
  feedback_absolute_min_label->setObjectName("feedback_absolute_min_label");
  feedback_scaling_layout->addWidget(feedback_absolute_min_label,5,0);
  feedback_calibration_label = new QLabel(tr("Calibration"));
  feedback_calibration_label->setObjectName("feedback_calibration_label");
  feedback_scaling_layout->addWidget(feedback_calibration_label,1,1);
  feedback_scaling_order_warning_label = new QLabel(
    tr("Warning: some of the values\nare not in the correct order"));
  feedback_scaling_order_warning_label->setObjectName("feedback_scaling_order_waring_label");
  feedback_scaling_order_warning_label->setStyleSheet(QStringLiteral("color:red"));
  feedback_scaling_layout->addWidget(feedback_scaling_order_warning_label,5,4,5,2);
  feedback_absolute_max_spinbox = new QDoubleSpinBox();
  feedback_absolute_max_spinbox->setObjectName("feedback_absolute_max_spinbox");
  feedback_scaling_layout->addWidget(feedback_absolute_max_spinbox,2,1);
  feedback_maximum_spinbox = new QDoubleSpinBox();
  feedback_maximum_spinbox->setObjectName("feedback_maximum_spinbox");
  feedback_scaling_layout->addWidget(feedback_maximum_spinbox,3,1);
  feedback_minimum_spinbox = new QDoubleSpinBox();
  feedback_minimum_spinbox->setObjectName("feedback_minimum_spinbox");
  feedback_scaling_layout->addWidget(feedback_minimum_spinbox,4,1);
  feedback_absolute_min_spinbox = new QDoubleSpinBox();
  feedback_absolute_min_spinbox->setObjectName("feedback_absolute_min_spinbox");
  feedback_scaling_layout->addWidget(feedback_absolute_min_spinbox,5,1);
  feedback_learn_button = new QPushButton(tr("Learn.."));
  feedback_learn_button->setObjectName("feedback_learn_button");
  feedback_scaling_layout->addWidget(feedback_learn_button,0,5,Qt::AlignRight);
  feedback_reset_range_button = new QPushButton(tr("Reset to full range"));
  feedback_reset_range_button->setObjectName("feedback_reset_range_button");
  feedback_scaling_layout->addWidget(feedback_reset_range_button,1,4,1,2,Qt::AlignRight);
  feedback_scaling_groupbox->setLayout(feedback_scaling_layout);

  QGridLayout *feedback_analog_layout = new QGridLayout();
  feedback_analog_groupbox = new QGroupBox(tr("Analog to digital conversion"));
  feedback_analog_groupbox->setObjectName("feedback_analog_groupbox");
  feedback_analog_samples_label = new QLabel(tr("Analog samples:"));
  feedback_analog_samples_label->setObjectName("feedback_analog_samples_label");
  feedback_analog_layout->addWidget(feedback_analog_samples_label,0,0);
  feedback_analog_samples_combobox = new QComboBox();
  feedback_analog_samples_combobox->setObjectName("feedback_analog_samples_label");
  feedback_analog_layout->addWidget(feedback_analog_samples_combobox,0,1,Qt::AlignLeft);
  feedback_disconnect_with_aux_checkbox = new QCheckBox(tr("Detect disconnect with AUX"));
  feedback_disconnect_with_aux_checkbox->setObjectName("feedback_disconnect_with_aux_checkbox");
  feedback_analog_layout->addWidget(feedback_disconnect_with_aux_checkbox,1,0,1,2);
  feedback_analog_groupbox->setLayout(feedback_analog_layout);

  layout->addLayout(feedback_mode_layout,0,0,Qt::AlignLeft);
  layout->addWidget(feedback_scaling_groupbox,1,0,Qt::AlignLeft);
  layout->addWidget(feedback_analog_groupbox,2,0,Qt::AlignLeft);

  feedback_page_widget->setLayout(layout);
  return feedback_page_widget;
}

QWidget * main_window::setup_pid_tab()
{
  pid_page_widget = new QWidget();
  QGridLayout *layout = pid_page_layout = new QGridLayout(pid_page_widget);
  layout->setSizeConstraint(QLayout::SetFixedSize);
  pid_proportional_coefficient = new pid_constant_control(
    "Proportional Coefficient", "pid_proportional_coefficient");
  pid_integral_coefficient = new pid_constant_control(
    "Integral Coefficient", "pid_integral_coefficient");
  pid_derivative_coefficient = new pid_constant_control(
    "Derivative Coefficient", "pid_derivative_coefficient");
  QGridLayout *group_box_row = new QGridLayout();
  // group_box_row->setRowStretch(0,pid_constant_control()->sizeHint().height());
  group_box_row->addWidget(pid_proportional_coefficient,0,0,Qt::AlignCenter);
  group_box_row->addWidget(pid_integral_coefficient,0,1,Qt::AlignCenter);
  group_box_row->addWidget(pid_derivative_coefficient,0,2,Qt::AlignCenter);

  pid_period_label = new QLabel(tr("PID period (ms):"));
  pid_period_label->setObjectName("pid_period_label");
  pid_period_spinbox = new QDoubleSpinBox();
  pid_period_spinbox->setObjectName("pid_period_spinbox");
  QHBoxLayout *period_row_layout = new QHBoxLayout();
  period_row_layout->addWidget(pid_period_label);
  period_row_layout->addWidget(pid_period_spinbox);
  pid_integral_limit_label = new QLabel(tr("Integral limit:"));
  pid_integral_limit_label->setObjectName("pid_integral_limit_label");
  pid_integral_limit_spinbox = new QDoubleSpinBox();
  pid_integral_limit_spinbox->setObjectName("pid_integral_limit_spinbox");
  QHBoxLayout *integral_row_layout = new QHBoxLayout();
  integral_row_layout->addWidget(pid_integral_limit_label);
  integral_row_layout->addWidget(pid_integral_limit_spinbox);
  pid_reset_integral_checkbox = new QCheckBox(
    tr("Reset integral when proportional term exceeds max duty cycle"));
  pid_reset_integral_checkbox->setObjectName("pid_reset_integral_checkbox");
  pid_deadzone_label = new QLabel(tr("Feedback dead zone:"));
  pid_deadzone_label->setObjectName("pid_deadzone_label");
  pid_deadzone_spinbox = new QDoubleSpinBox();
  pid_deadzone_spinbox->setObjectName("pid_deadzone_spinbox");
  QHBoxLayout *deadzone_row_layout = new QHBoxLayout();
  deadzone_row_layout->addWidget(pid_deadzone_label);
  deadzone_row_layout->addWidget(pid_deadzone_spinbox);
  layout->addLayout(group_box_row,0,0);
  layout->addLayout(period_row_layout,1,0,1,1,Qt::AlignLeft);
  layout->addLayout(integral_row_layout,2,0,1,1,Qt::AlignLeft);
  layout->addWidget(pid_reset_integral_checkbox,3,0,1,2,Qt::AlignLeft);
  layout->addLayout(deadzone_row_layout,4,0,1,1,Qt::AlignLeft);

  // pid_page_widget->setLayout(layout);
  return pid_page_widget;
}

QWidget *main_window::setup_motor_tab()
{
  motor_page_widget = new QWidget();
  QGridLayout *layout = motor_page_layout = new QGridLayout();
  layout->setSizeConstraint(QLayout::SetFixedSize);
  QHBoxLayout *frequency_layout = new QHBoxLayout();
  motor_frequency_label = new QLabel(tr("PVM frequency:"));
  motor_frequency_label->setObjectName("motor_frequency_label");
  motor_frequency_combobox = new QComboBox();
  motor_frequency_combobox->setObjectName("motor_frequency_combobox");
  frequency_layout->addWidget(motor_frequency_label);
  frequency_layout->addWidget(motor_frequency_combobox);
  layout->addLayout(frequency_layout,0,0,Qt::AlignLeft);
  QHBoxLayout *invert_layout = new QHBoxLayout();
  motor_invert_checkbox = new QCheckBox(tr("Invert motor direction"));
  motor_invert_checkbox->setObjectName("motor_invert_checkbox");
  motor_detect_motor_button = new QPushButton(tr("Detect Motor Direction"));
  motor_detect_motor_button->setObjectName("motor_detect_motor_button");
  invert_layout->addWidget(motor_invert_checkbox);
  invert_layout->addWidget(motor_detect_motor_button);
  QGridLayout *motor_controls_layout = new QGridLayout();
  motor_asymmetric_checkbox = new QCheckBox(tr("Asymmetric"));
  motor_asymmetric_checkbox->setObjectName("motor_asymmetric_checkbox");
  motor_forward_label = new QLabel(tr("Forward"));
  motor_forward_label->setObjectName("motor_forward_label");
  motor_reverse_label = new QLabel(tr("Reverse"));
  motor_reverse_label->setObjectName("motor_reverse_label");
  motor_duty_label = new QLabel(tr("Max. duty cycle:"));
  motor_duty_label->setObjectName("motor_duty_label");
  motor_duty_forward_spinbox = new QDoubleSpinBox();
  motor_duty_forward_spinbox->setObjectName("motor_duty_forward_spinbox");
  motor_duty_reverse_spinbox = new QDoubleSpinBox();
  motor_duty_reverse_spinbox->setObjectName("motor_duty_reverse_spinbox");
  motor_duty_means_label = new QLabel(tr("(600 means 100%)"));
  motor_acceleration_label = new QLabel(tr("Max. acceleration:"));
  motor_acceleration_label->setObjectName("motor_acceleration_label");
  motor_acceleration_forward_spinbox = new QDoubleSpinBox();
  motor_acceleration_forward_spinbox->setObjectName("motor_acceleration_forward_spinbox");
  motor_acceleration_reverse_spinbox = new QDoubleSpinBox();
  motor_acceleration_reverse_spinbox->setObjectName("motor_acceleration_reverse_spinbox");
  motor_acceleration_means_label = new QLabel(tr("(600 means no limit)"));
  motor_duration_label = new QLabel(tr("Brake duration (ms):"));
  motor_duration_label->setObjectName("motor_duration_label");
  motor_duration_forward_spinbox = new QDoubleSpinBox();
  motor_duration_forward_spinbox->setObjectName("motor_duration_forward_spinbox");
  motor_duration_reverse_spinbox = new QDoubleSpinBox();
  motor_duration_reverse_spinbox->setObjectName("motor_duration_reverse_spinbox");
  motor_current_label = new QLabel(tr("Max. current (A):"));
  motor_current_label->setObjectName("motor_current_label");
  motor_current_forward_spinbox = new QDoubleSpinBox();
  motor_current_forward_spinbox->setObjectName("motor_current_forward_spinbox");
  motor_current_reverse_spinbox = new QDoubleSpinBox();
  motor_current_reverse_spinbox->setObjectName("motor_current_reverse_spinbox");
  motor_current_means_label = new QLabel(tr("(0.000 means no limit)"));
  motor_current_means_label->setObjectName("motor_current_means_label");
  motor_calibration_label = new QLabel(tr("Current calibration:"));
  motor_calibration_label->setObjectName("motor_calibration_label");
  motor_calibration_forward_spinbox = new QDoubleSpinBox();
  motor_calibration_forward_spinbox->setObjectName("motor_calibration_forward_spinbox");
  motor_calibration_reverse_spinbox = new QDoubleSpinBox();
  motor_calibration_reverse_spinbox->setObjectName("motor_calibration_reverse_spinbox");

  motor_controls_layout->addWidget(motor_asymmetric_checkbox,0,2,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_forward_label,1,1,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_reverse_label,1,2,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_duty_label,2,0,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_duty_forward_spinbox,2,1,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_duty_reverse_spinbox,2,2,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_duty_means_label,2,3,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_acceleration_label,3,0,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_acceleration_forward_spinbox,3,1,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_acceleration_reverse_spinbox,3,2,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_acceleration_means_label,3,3,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_duration_label,4,0,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_duration_forward_spinbox,4,1,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_duration_reverse_spinbox,4,2,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_current_label,5,0,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_current_forward_spinbox,5,1,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_current_reverse_spinbox,5,2,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_current_means_label,5,3,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_calibration_label,6,0,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_calibration_forward_spinbox,6,1,Qt::AlignLeft);
  motor_controls_layout->addWidget(motor_calibration_reverse_spinbox,6,2,Qt::AlignLeft);

  QHBoxLayout *duty_cycle_layout = new QHBoxLayout();
  motor_outofrange_label = new QLabel(tr("Max. duty cycke while feedback is out of range:"));
  motor_outofrange_label->setObjectName("motor_outofrange_label");
  motor_outofrange_spinbox = new QDoubleSpinBox();
  motor_outofrange_spinbox->setObjectName("motor_outofrange_spinbox");
  motor_outofrange_means_label = new QLabel(tr("(600 means 100%)"));

  duty_cycle_layout->addWidget(motor_outofrange_label);
  duty_cycle_layout->addWidget(motor_outofrange_spinbox);
  duty_cycle_layout->addWidget(motor_outofrange_means_label);

  QGridLayout *motor_off_layout = new QGridLayout();
  motor_off_label = new QLabel(tr("When motor is off:"));
  motor_off_label->setObjectName("motor_off_label");
  motor_brake_radio = new QRadioButton(tr("Brake"));
  motor_brake_radio->setObjectName("motor_brake_radio");
  motor_coast_radio = new QRadioButton(tr("Coast"));
  motor_coast_radio->setObjectName("motor_coast_radio");

  motor_off_layout->addWidget(motor_off_label,0,0);
  motor_off_layout->addWidget(motor_brake_radio,0,1);
  motor_off_layout->addWidget(motor_coast_radio,1,1);

  layout->addLayout(invert_layout,1,0,Qt::AlignLeft);
  layout->addLayout(motor_controls_layout,2,0,Qt::AlignLeft);
  layout->addLayout(duty_cycle_layout,3,0,Qt::AlignLeft);
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
    "awaiting_command", "0x0001", "Awaiting command", false, false),row_number,0,1,8,Qt::AlignCenter);
  layout->addWidget(new errors_control(++row_number, 
    "no_power", "0x0002", "No power", false, true),row_number,0,1,8,Qt::AlignCenter);
  layout->addWidget(new errors_control(++row_number, 
    "motor_driven_error", "0x0004", "Motor driver error", false, true),row_number,0,1,8,Qt::AlignCenter);
  layout->addWidget(new errors_control(++row_number, 
    "input_invalid", "0x0008", "Input invalid", false, true),row_number,0,1,8,Qt::AlignCenter);
  layout->addWidget(new errors_control(++row_number, 
    "input_disconnect", "0x0010", "Input disconnect", true, true),row_number,0,1,8,Qt::AlignCenter);
  layout->addWidget(new errors_control(++row_number, 
    "feedback_disconnect", "0x0020", "Feedback disconnect", true, true),row_number,0,1,8,Qt::AlignCenter);
  layout->addWidget(new errors_control(++row_number, 
    "max_current_exceeded", "0x0040", "Max. current exceeded", true, true),row_number,0,1,8,Qt::AlignCenter);
  layout->addWidget(new errors_control(++row_number, 
    "serial_signal_error", "0x0080", "Serial signal error", true, false),row_number,0,1,8,Qt::AlignCenter);
  layout->addWidget(new errors_control(++row_number, 
    "serial_overrun", "0x0100", "Serial overrun", true, false),row_number,0,1,8,Qt::AlignCenter);
  layout->addWidget(new errors_control(++row_number, 
    "serial_rx_buffer_full", "0x0200", "Serial RX buffer full", true, false),row_number,0,1,8,Qt::AlignCenter);
  layout->addWidget(new errors_control(++row_number, 
    "serial_crc_error", "0x0400", "Serial CRC error", true, false),row_number,0,1,8,Qt::AlignCenter);
  layout->addWidget(new errors_control(++row_number, 
    "serial_protocol_error", "0x0800", "Serial protocol error", true, false),row_number,0,1,8,Qt::AlignCenter);
  layout->addWidget(new errors_control(++row_number, 
    "serial_timeout_error", "0x1000", "Serial timeout error", true, false),row_number,0,1,8,Qt::AlignCenter);
  layout->addLayout(errors_bottom_buttons,16,0,4,8,Qt::AlignBottom);

  layout->setMargin(0);

  QHBoxLayout *page_layout = new QHBoxLayout();
  page_layout->setSizeConstraint(QLayout::SetFixedSize);
  page_layout->addLayout(layout,Qt::AlignCenter);
  errors_page_widget->setLayout(page_layout);

  return errors_page_widget;
}

// void main_window::set_device_list_contents(std::vector<jrk::device> const & device_list)
// {
//   suppress_events = true;
//   device_list_value->clear();
//   device_list_value->addItem(tr("Not connected"), QString()); // null value
//   for (jrk::device const & device : device_list)
//   {
//     device_list_value->addItem(
//       QString::fromStdString(
//         " #" + device.get_serial_number()),
//       QString::fromStdString(device.get_os_id()));
//   }
//   suppress_events = false;
// }

// void main_window::set_device_list_selected(jrk::device const & device)
// {
//   // TODO: show an error if we couldn't find the specified device
//   // (findData returned -1)?
//   suppress_events = true;
//   int index = 0;
//   if (device)
//   {
//     index = device_list_value->findData(QString::fromStdString(device.get_os_id()));
//   }
//   device_list_value->setCurrentIndex(index);
//   suppress_events = false;
// }

// void main_window::set_connection_status(std::string const & status, bool error)
// {
//   if (error)
//   {
//     connection_status_value->setStyleSheet("color: red;");
//   }
//   else
//   {
//     connection_status_value->setStyleSheet("");
//   }
//   connection_status_value->setText(QString::fromStdString(status));
// }

pid_constant_control::pid_constant_control(const QString& group_box_title, 
  const QString& object_name, QWidget *parent)
  : QGroupBox(parent)
{
  setObjectName(object_name);
  setTitle(group_box_title);

  pid_base_label = new QLabel();
  pid_base_label->setObjectName("pid_base_label");
  QFont font;
  font.setFamily(QStringLiteral("MS Shell Dlg 2"));
  font.setPointSize(16);
  font.setBold(true);
  font.setWeight(75);
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
  pid_exponent_spinbox = new QSpinBox();
  pid_exponent_spinbox->setObjectName("pid_exponent_spinbox");
  pid_exponent_spinbox->setAlignment(Qt::AlignCenter);
  pid_equal_label = new QLabel();
  pid_equal_label->setObjectName("pid_equal_label");
  pid_equal_label->setText(tr("="));
  QFont font1;
  font1.setPointSize(12);
  font1.setBold(true);
  font1.setWeight(75);
  pid_equal_label->setFont(font1);
  pid_equal_label->setAlignment(Qt::AlignCenter);
  pid_constant_control_textbox = new QLineEdit();
  pid_constant_control_textbox->setObjectName("pid_constant_control_textbox");

  QGridLayout *group_box_layout = new QGridLayout();
  group_box_layout->addWidget(pid_base_label,3,1,3,2);
  group_box_layout->addWidget(pid_control_frame,2,1,1,5);
  group_box_layout->addWidget(pid_multiplier_spinbox,1,2,1,3);
  group_box_layout->addWidget(pid_exponent_spinbox,3,3,1,3);
  group_box_layout->addWidget(pid_equal_label,2,7,1,2);
  group_box_layout->addWidget(pid_constant_control_textbox,1,9,3,1,Qt::AlignCenter);
  group_box_layout->setSizeConstraint(QLayout::SetFixedSize);

  setLayout(group_box_layout);

  QMetaObject::connectSlotsByName(this);
}

pid_constant_control::~pid_constant_control()
{

}

errors_control::errors_control
(int row_number, const QString& object_name, const QString& bit_mask_text, 
    const QString& error_label_text, const bool& disabled_visible, 
      const bool& enabled_visible, QWidget *parent)
{ 
  QSizePolicy p = this->sizePolicy();
  p.setRetainSizeWhenHidden(true);

  errors_frame = new QWidget();

  if(row_number % 2 != 0)
  {
    errors_frame->setStyleSheet(QStringLiteral("background-color:rgb(230,229,229)"));
  }
  errors_central = new QGridLayout(this);

  bit_mask_label = new QLabel();
  bit_mask_label->setObjectName("bit_mask_label");
  
  error_label = new QLabel();
  error_label->setObjectName("error_label"); 
  
  disabled_radio = new QRadioButton(tr("Disabled"));
  disabled_radio->setObjectName("disabled_radio");
  disabled_radio->setSizePolicy(p);
  
  enabled_radio = new QRadioButton(tr("Enabled"));
  enabled_radio->setObjectName("enabled_radio");
  enabled_radio->setSizePolicy(p);

  latched_radio = new QRadioButton(tr("Enabled and latched"));
  latched_radio->setObjectName("latched_radio");
  
  stopping_motor_label = new QLabel(tr("No"));
  stopping_motor_label->setObjectName("stopping_motor_label");
  
  count_label = new QLabel(tr("0"));
  count_label->setObjectName("count_label");

  setObjectName(object_name);
  bit_mask_label->setText(bit_mask_text);
  error_label->setText(error_label_text);
  disabled_radio->setVisible(disabled_visible);
  enabled_radio->setVisible(enabled_visible);  

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
    bit_mask_label->setFixedWidth(tmp_label2.sizeHint().width());
    error_label->setFixedWidth(tmp_label.sizeHint().width());
    disabled_radio->setFixedWidth(tmp_button_2.sizeHint().width());
    enabled_radio->setFixedWidth(tmp_button_2.sizeHint().width());
    latched_radio->setFixedWidth(tmp_button.sizeHint().width());
    stopping_motor_label->setFixedWidth(tmp_label2.sizeHint().width() * 150/100);
    count_label->setFixedWidth(tmp_label2.sizeHint().width());
  }


  errors_central->addWidget(errors_frame,0,0,3,9);
  errors_central->addWidget(bit_mask_label,1,1,Qt::AlignLeft);
  errors_central->addWidget(error_label,1,2,Qt::AlignCenter);
  errors_central->addWidget(disabled_radio,1,3,Qt::AlignRight);
  errors_central->addWidget(enabled_radio,1,4,Qt::AlignCenter);
  errors_central->addWidget(latched_radio,1,5,Qt::AlignLeft);
  errors_central->addWidget(stopping_motor_label,1,6,Qt::AlignLeft);
  errors_central->addWidget(count_label,1,7,1,2,Qt::AlignRight);
  errors_central->setColumnStretch(6,10);

  errors_central->setMargin(0);
  QMetaObject::connectSlotsByName(this);
}

errors_control::~errors_control()
{

}