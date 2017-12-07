#include "mainwindow.h"
#include "qcustomplot.h"
#include "altwindow.h"

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

MainWindow::MainWindow(QWidget * parent)
    : QMainWindow(parent)
{
    setupUi(this);
    altw = 0;

    sepAct = new QAction(tr("&Separate"), this);
    sepAct->setStatusTip(tr("Make the red part pop out into another window."));
    connect(sepAct, SIGNAL(triggered()), this, SLOT(on_separateBtn_clicked()));
    widgetAtHome = true;
}

MainWindow::~MainWindow()
{
    if (altw != 0)
    {
        delete altw;
    }
}

void MainWindow::setupUi(QMainWindow *MainWindow)
{
    font.setPointSizeF(8.25);

    MainWindow->setObjectName(QStringLiteral("MainWindow"));
    MainWindow->resize(1000,700);
    MainWindow->setWindowTitle("Pololu Jrk G2 Configuration Utility");

    centralWidget = new QWidget(MainWindow);
    centralWidget->setObjectName(QStringLiteral("centralWidget"));

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

    gridLayout = new QGridLayout(centralWidget);
    gridLayout->setSpacing(6);
    gridLayout->setContentsMargins(11,11,11,11);
    gridLayout->setObjectName(QStringLiteral("gridLayout"));

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing(6);
    horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));

    previewWindow = new GraphWindow();
    previewWindow->setObjectName(QStringLiteral("previewWindow"));
    previewWindow->customPlot->xAxis->setTicks(false);
    previewWindow->customPlot->yAxis->setTicks(false);
    QWidget *previewPlot = previewWindow->customPlot;
    previewPlot->setCursor(Qt::PointingHandCursor);
    previewPlot->setToolTip("Click on preview to view full plot");
    previewPlot->setFixedSize(150,150);

    stop_motor = new QCheckBox(tr("Stop motor"));

    header_layout->addWidget(device_list_label);
    header_layout->addWidget(device_list_value);
    header_layout->addWidget(connection_status_value, 1, Qt::AlignLeft);
    header_layout->addWidget(stop_motor);
    header_layout->addWidget(previewPlot);

    gridLayout->addLayout(header_layout,0,0);

    tab_widget = new QTabWidget();
    status_page_widget = new QWidget();
    input_page_widget = new QWidget();
    feedback_page_widget = new QWidget();
    pid_page_widget = new QWidget();
    motor_page_widget = new QWidget();
    errors_page_widget = new QWidget();


    tab_widget->setFixedSize(1000,700);
    tab_widget->addTab(status_page_widget, tr("Status"));
    tab_widget->addTab(setup_input_tab(), tr("Input"));
    tab_widget->addTab(setup_feedback_tab(), tr("Feedback"));
    tab_widget->addTab(setup_pid_tab(), tr("PID"));
    tab_widget->addTab(setup_motor_tab(), tr("Motor"));
    tab_widget->addTab(setup_errors_tab(), tr("Errors"));

    gridLayout->addWidget(tab_widget,1,0);

    motorOffButton = new QPushButton();
    motorOnButton = new QPushButton();
    apply_settings = new QPushButton();
    footer_layout = new QHBoxLayout();

    motorOffButton->resize(75,23);
    motorOffButton->setObjectName("motorOffButton");
    motorOffButton->setText(tr("&Stop Motor"));
    motorOffButton->setStyleSheet(QStringLiteral("background-color:red"));

    motorOnButton->resize(75,23);
    motorOnButton->setObjectName("motorOnButton");
    motorOnButton->setText(tr("&Run Motor"));

    apply_settings->resize(148,23);
    apply_settings->setObjectName("apply_settings");
    apply_settings->setText(tr("&Apply settings to device"));

    footer_layout->addWidget(motorOffButton);
    footer_layout->addWidget(motorOnButton);
    footer_layout->addSpacing(200);
    footer_layout->addWidget(apply_settings);
    footer_layout->setStretch(2,3);

    gridLayout->addLayout(footer_layout,2,0);
    
    connect(previewPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(on_launchGraph_clicked(QMouseEvent*)));

    MainWindow->setCentralWidget(centralWidget);
    MainWindow->setMenuBar(menu_bar);

    QMetaObject::connectSlotsByName(MainWindow);
  
}

void MainWindow::on_launchGraph_clicked(QMouseEvent *event)
{
    GraphWindow *red = previewWindow;
    horizontalLayout->removeWidget(red);
    if(altw == 0)
    {
        altw = new AltWindow(this);
        connect(altw, SIGNAL(passWidget(GraphWindow*)), this, SLOT(receiveWidget(GraphWindow*)));
    }

    altw->receiveWidget(red);
    altw->show();
    widgetAtHome = false;
}

void MainWindow::receiveWidget(GraphWindow *widget)
{
    widget->customPlot->setFixedSize(150,150);
    widget->customPlot->xAxis->setTicks(false);
    widget->customPlot->yAxis->setTicks(false);
    widget->customPlot->setCursor(Qt::PointingHandCursor);
    widget->customPlot->setToolTip("Click on preview to view full plot");
    header_layout->addWidget(widget->customPlot);
    widgetAtHome = true;
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    if (widgetAtHome)
    {
        QMenu menu(this);
        menu.addAction(sepAct);
        menu.exec(event->globalPos());
    }
}

void MainWindow::retranslateUi(QMainWindow *MainWindow)
{
    MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
}

QWidget * MainWindow::setup_input_tab()
{
    input_page_widget = new QWidget();
    QGridLayout *layout = input_page_layout = new QGridLayout();
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

    input_scaling_order_warning_label = new QLabel(tr("Warning: some of the values\nare not in the correct order."));
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
    return input_page_widget;
}

QWidget * MainWindow::setup_feedback_tab()
{
    QGridLayout *layout = feedback_page_layout = new QGridLayout();

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
    feedback_scaling_order_warning_label = new QLabel(tr("Warning: some of the values\nare not in the correct order"));
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

QWidget * MainWindow::setup_pid_tab()
{
    QGridLayout *layout = pid_page_layout = new QGridLayout();
    pid_proportional_coefficient = new PIDConstantControl();
    pid_proportional_coefficient->setObjectName("pid_proportional_coefficient");
    pid_proportional_coefficient->setTitle(tr("Proportional Coefficient"));
    pid_proportional_coefficient->setFixedSize(200,100);
    pid_integral_coefficient = new PIDConstantControl();
    pid_integral_coefficient->setObjectName("pid_integral_coefficient");
    pid_integral_coefficient->setTitle(tr("Integral Coefficient"));
    pid_integral_coefficient->setFixedSize(200,100);
    pid_derivative_coefficient = new PIDConstantControl();
    pid_derivative_coefficient->setObjectName("pid_derivative_coefficient");
    pid_derivative_coefficient->setTitle(tr("Derivative Coefficient"));
    pid_derivative_coefficient->setFixedSize(200,100);
    QHBoxLayout *group_box_row = new QHBoxLayout();
    group_box_row->addWidget(pid_proportional_coefficient);
    group_box_row->addWidget(pid_integral_coefficient);
    group_box_row->addWidget(pid_derivative_coefficient);

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
    pid_reset_integral_checkbox = new QCheckBox(tr("Reset integral when proportional term exceeds max duty cycle"));
    pid_reset_integral_checkbox->setObjectName("pid_reset_integral_checkbox");
    pid_deadzone_label = new QLabel(tr("Feedback dead zone:"));
    pid_deadzone_label->setObjectName("pid_deadzone_label");
    pid_deadzone_spinbox = new QDoubleSpinBox();
    pid_deadzone_spinbox->setObjectName("pid_deadzone_spinbox");
    QHBoxLayout *deadzone_row_layout = new QHBoxLayout();
    deadzone_row_layout->addWidget(pid_deadzone_label);
    deadzone_row_layout->addWidget(pid_deadzone_spinbox);
    layout->addLayout(group_box_row,0,0);
    layout->addLayout(period_row_layout,1,0,Qt::AlignLeft);
    layout->addLayout(integral_row_layout,2,0,Qt::AlignLeft);
    layout->addWidget(pid_reset_integral_checkbox,3,0,Qt::AlignLeft);
    layout->addLayout(deadzone_row_layout,4,0,Qt::AlignLeft);

    pid_page_widget->setLayout(layout);
    return pid_page_widget;
}

QWidget *MainWindow::setup_motor_tab()
{
    QGridLayout *layout = motor_page_layout = new QGridLayout();
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

QWidget *MainWindow::setup_errors_tab()
{
    QGridLayout *layout = errors_page_layout = new QGridLayout();
    QFont font;
    font.setBold(true);
    font.setWeight(75);
    errors_bit_mask_label = new QLabel(tr("Bit mask"));
    errors_bit_mask_label->setObjectName("errors_bit_mask_label");
    errors_bit_mask_label->setFont(font);
    errors_error_label = new QLabel(tr("Error"));
    errors_error_label->setObjectName("errors_error_label");
    errors_error_label->setFont(font);
    errors_setting_label = new QLabel(tr("Setting"));
    errors_setting_label->setObjectName("errors_setting_label");
    errors_setting_label->setFont(font);
    errors_stopping_motor_label = new QLabel(tr("Currently\nstopping motor?"));
    errors_stopping_motor_label->setObjectName("errors_stopping_motor_label");
    errors_stopping_motor_label->setAlignment(Qt::AlignCenter);
    errors_stopping_motor_label->setFont(font);
    errors_occurence_count_label = new QLabel(tr("Occurence count"));
    errors_occurence_count_label->setObjectName("errors_occurence_count_label");
    errors_occurence_count_label->setFont(font);

    QGridLayout *errors_column_labels = new QGridLayout();
    QWidget *errors_column_labels_frame = new QWidget();
    errors_column_labels_frame->setFixedWidth(1000);
    errors_column_labels->setColumnStretch(0,45);
    errors_column_labels->setColumnStretch(1,125);
    errors_column_labels->setColumnStretch(2,320);
    errors_column_labels->setColumnStretch(3,75);
    errors_column_labels->setColumnStretch(4,93);
    errors_column_labels->addWidget(errors_bit_mask_label,0,0,Qt::AlignCenter);
    errors_column_labels->addWidget(errors_error_label,0,1,Qt::AlignCenter);
    errors_column_labels->addWidget(errors_setting_label,0,2,Qt::AlignCenter);
    errors_column_labels->addWidget(errors_stopping_motor_label,0,3,Qt::AlignCenter);
    errors_column_labels->addWidget(errors_occurence_count_label,0,4,Qt::AlignCenter);
    errors_column_labels_frame->setLayout(errors_column_labels);

    awaiting_command = new ErrorsControl();
    awaiting_command->setObjectName("awaiting_command");
    awaiting_command->bit_mask_label->setText(tr("0x0001"));
    awaiting_command->error_label->setText(tr("Awaiting command"));
    awaiting_command->disabled_radio->setVisible(false);
    awaiting_command->enabled_radio->setVisible(false);
    
    no_power = new ErrorsControl();
    no_power->setObjectName("no_power");
    no_power->bit_mask_label->setText(tr("0x0002"));
    no_power->error_label->setText(tr("No power"));
    no_power->disabled_radio->setVisible(false);
    no_power->errors_frame->setStyleSheet(QStringLiteral("background-color:rgb(230,229,229)"));

    motor_driven_error = new ErrorsControl();
    motor_driven_error->setObjectName("motor_driven_error");
    motor_driven_error->bit_mask_label->setText(tr("0x0004"));
    motor_driven_error->error_label->setText(tr("Motor driven error"));
    motor_driven_error->disabled_radio->setVisible(false);

    input_invalid = new ErrorsControl();
    input_invalid->setObjectName("input_invalid");
    input_invalid->bit_mask_label->setText(tr("0x0008"));
    input_invalid->error_label->setText(tr("Input invalid"));
    input_invalid->disabled_radio->setVisible(false);
    input_invalid->setStyleSheet(QStringLiteral("background-color:rgb(230,229,229)"));

    input_disconnect = new ErrorsControl();
    input_disconnect->setObjectName("input_disconnect");
    input_disconnect->bit_mask_label->setText(tr("0x0010"));
    input_disconnect->error_label->setText(tr("Input disconnect"));

    feedback_disconnect = new ErrorsControl();
    feedback_disconnect->setObjectName("feedback_disconnect");
    feedback_disconnect->bit_mask_label->setText(tr("0x0020"));
    feedback_disconnect->error_label->setText(tr("Feedback disconnect"));
    feedback_disconnect->setStyleSheet(QStringLiteral("background-color:rgb(230,229,229)"));

    max_current_exceeded = new ErrorsControl();
    max_current_exceeded->setObjectName("max_current_exceeded");
    max_current_exceeded->bit_mask_label->setText(tr("0x0040"));
    max_current_exceeded->error_label->setText(tr("Max. current exceeded"));

    serial_signal_error = new ErrorsControl();
    serial_signal_error->setObjectName("serial_signal_error");
    serial_signal_error->bit_mask_label->setText(tr("0x0080"));
    serial_signal_error->error_label->setText(tr("Serial signal error"));
    serial_signal_error->enabled_radio->setVisible(false);
    serial_signal_error->setStyleSheet(QStringLiteral("background-color:rgb(230,229,229)"));

    serial_overrun = new ErrorsControl();
    serial_overrun->setObjectName("serial_overrun");
    serial_overrun->bit_mask_label->setText(tr("0x0100"));
    serial_overrun->error_label->setText(tr("Serial overrun"));
    serial_overrun->enabled_radio->setVisible(false);

    serial_rx_buffer_full = new ErrorsControl();
    serial_rx_buffer_full->setObjectName("serial_rx_buffer_full");
    serial_rx_buffer_full->bit_mask_label->setText(tr("0x0200"));
    serial_rx_buffer_full->error_label->setText(tr("Serial RX buffer full"));
    serial_rx_buffer_full->enabled_radio->setVisible(false);
    serial_rx_buffer_full->setStyleSheet(QStringLiteral("background-color:rgb(230,229,229)"));

    serial_crc_error = new ErrorsControl();
    serial_crc_error->setObjectName("serial_crc_error");
    serial_crc_error->bit_mask_label->setText(tr("0x0400"));
    serial_crc_error->error_label->setText(tr("Serial CRC error"));
    serial_crc_error->enabled_radio->setVisible(false);

    serial_protocol_error = new ErrorsControl();
    serial_protocol_error->setObjectName("serial_protocol_error");
    serial_protocol_error->bit_mask_label->setText(tr("0x0800"));
    serial_protocol_error->error_label->setText(tr("Serial protocol error"));
    serial_protocol_error->enabled_radio->setVisible(false);
    serial_protocol_error->setStyleSheet(QStringLiteral("background-color:rgb(230,229,229)"));

    serial_timeout_error = new ErrorsControl();
    serial_timeout_error->setObjectName("serial_timeout_error");
    serial_timeout_error->bit_mask_label->setText(tr("0x1000"));
    serial_timeout_error->error_label->setText(tr("Serial timeout error"));
    serial_timeout_error->enabled_radio->setVisible(false);

    QHBoxLayout *errors_bottom_buttons = new QHBoxLayout();
    errors_clear_errors = new QPushButton(tr("&Clear Errors"));
    errors_clear_errors->setObjectName("errors_clear_errors");
    errors_clear_errors->setFixedSize(100,40);
    errors_reset_counts = new QPushButton(tr("Reset c&ounts"));
    errors_reset_counts->setObjectName("errors_reset_counts");
    errors_reset_counts->setFixedSize(100,40);
    errors_bottom_buttons->addSpacing(600);
    errors_bottom_buttons->addWidget(errors_clear_errors);
    errors_bottom_buttons->addWidget(errors_reset_counts);

    layout->setVerticalSpacing(0);
    layout->addWidget(errors_column_labels_frame,0,0);
    layout->addWidget(awaiting_command,1,0);
    layout->addWidget(no_power,2,0);
    layout->addWidget(motor_driven_error,3,0);
    layout->addWidget(input_invalid,4,0);
    layout->addWidget(input_disconnect,5,0);
    layout->addWidget(feedback_disconnect,6,0);
    layout->addWidget(max_current_exceeded,7,0);
    layout->addWidget(serial_signal_error,8,0);
    layout->addWidget(serial_overrun,9,0);
    layout->addWidget(serial_rx_buffer_full,10,0);
    layout->addWidget(serial_crc_error,11,0);
    layout->addWidget(serial_protocol_error,12,0);
    layout->addWidget(serial_timeout_error,13,0);
    layout->addLayout(errors_bottom_buttons,14,0);

    errors_page_widget->setLayout(layout);
    return errors_page_widget;
}

PIDConstantControl::PIDConstantControl(QWidget *parent)
    : QGroupBox(parent)
{
    centralWidget = new QWidget(this);
    centralWidget->setObjectName(tr("centralWidget"));
    pid_base_label = new QLabel(centralWidget);
    pid_base_label->setObjectName("pid_base_label");
    pid_base_label->setGeometry(QRect(13,64,16,27));
    QFont font;
    font.setFamily(QStringLiteral("MS Shell Dlg 2"));
    font.setPointSize(16);
    font.setBold(true);
    font.setWeight(75);
    pid_base_label->setFont(font);
    pid_base_label->setLayoutDirection(Qt::LeftToRight);
    pid_base_label->setAlignment(Qt::AlignCenter);
    pid_base_label->setText(tr("2"));
    pid_control_frame = new QFrame(centralWidget);
    pid_control_frame->setObjectName(QStringLiteral("pid_control_frame"));
    pid_control_frame->setGeometry(QRect(11, 36, 65, 27));
    pid_control_frame->setFrameShadow(QFrame::Plain);
    pid_control_frame->setLineWidth(4);
    pid_control_frame->setFrameShape(QFrame::HLine);
    pid_multiplier_spinbox = new QSpinBox(centralWidget);
    pid_multiplier_spinbox->setObjectName("pid_multiplier_spinbox");
    pid_multiplier_spinbox->setGeometry(QRect(14,21,58,20));
    pid_multiplier_spinbox->setAlignment(Qt::AlignCenter);
    pid_exponent_spinbox = new QSpinBox(centralWidget);
    pid_exponent_spinbox->setObjectName("pid_exponent_spinbox");
    pid_exponent_spinbox->setGeometry(QRect(31,56,41,20));
    pid_exponent_spinbox->setAlignment(Qt::AlignCenter);
    pid_equal_label = new QLabel(centralWidget);
    pid_equal_label->setObjectName("pid_equal_label");
    pid_equal_label->setGeometry(QRect(87,32,21,31));
    pid_equal_label->setText(tr("="));
    QFont font1;
    font1.setPointSize(12);
    font1.setBold(true);
    font1.setWeight(75);
    pid_equal_label->setFont(font1);
    pid_equal_label->setAlignment(Qt::AlignCenter);
    pid_constant_control_textbox = new QTextEdit(centralWidget);
    pid_constant_control_textbox->setObjectName("pid_constant_control_textbox");
    pid_constant_control_textbox->setGeometry(QRect(122, 39, 70, 21));
    pid_constant_control_textbox->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QMetaObject::connectSlotsByName(this);
}

PIDConstantControl::~PIDConstantControl()
{

}

ErrorsControl::ErrorsControl(QWidget *parent)
{
   
    this->setFixedHeight(40);
    errors_frame = new QWidget(this);
    errors_frame->setFixedWidth(1000);
    errors_central = new QGridLayout();
    bit_mask_label = new QLabel();
    bit_mask_label->setObjectName("bit_mask_label");
    bit_mask_label->setFixedWidth(55);
    error_label = new QLabel();
    error_label->setObjectName("error_label");
    error_label->setFixedWidth(150);
    disabled_radio = new QRadioButton(tr("Disabled"));
    disabled_radio->setObjectName("disabled_radio");
    disabled_radio->setFixedWidth(100);
    errors_disabled_spacer = new QLabel(tr(""));
    errors_disabled_spacer->setObjectName("errors_disabled_spacer");
    errors_disabled_spacer->setFixedWidth(75);
    errors_enabled_spacer = new QLabel(tr(""));
    errors_enabled_spacer->setObjectName("errors_enabled_spacer");
    errors_enabled_spacer->setFixedWidth(75);
    enabled_radio = new QRadioButton(tr("Enabled"));
    enabled_radio->setObjectName("enabled_radio");
    enabled_radio->setFixedWidth(100);
    latched_radio = new QRadioButton(tr("Enabled and latched"));
    latched_radio->setObjectName("latched_radio");
    latched_radio->setFixedWidth(175);
    stopping_motor_label = new QLabel(tr("No"));
    stopping_motor_label->setObjectName("stopping_motor_label");
    stopping_motor_label->setFixedWidth(75);
    count_label = new QLabel(tr("0"));
    count_label->setObjectName("count_label");
    count_label->setFixedWidth(93);

    errors_central->setColumnStretch(0,55);
    errors_central->setColumnStretch(1,125);
    errors_central->setColumnStretch(2,65);
    errors_central->setColumnStretch(3,65);
    errors_central->setColumnStretch(4,150);
    errors_central->setColumnStretch(5,75);
    errors_central->setColumnStretch(6,93);


    errors_central->addWidget(bit_mask_label,0,0);
    errors_central->addWidget(error_label,0,1,Qt::AlignLeft);
    errors_central->addWidget(disabled_radio,0,2);
    errors_central->addWidget(enabled_radio,0,3);
    errors_central->addWidget(latched_radio,0,4);
    errors_central->addWidget(stopping_motor_label,0,5,Qt::AlignRight);
    errors_central->addWidget(count_label,0,6,Qt::AlignRight);

    errors_frame->setLayout(errors_central);

    QMetaObject::connectSlotsByName(this);
}

ErrorsControl::~ErrorsControl()
{

}