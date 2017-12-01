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
    MainWindow->resize(818,547);
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
    // number of the Tic.
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
    // input_page_widget = new QWidget();
    feedback_page_widget = new QWidget();
    pid_page_widget = new QWidget();
    motor_page_widget = new QWidget();
    errors_page_widget = new QWidget();



    tab_widget->addTab(status_page_widget, tr("Status"));
    tab_widget->addTab(setup_input_tab(), tr("Input"));
    tab_widget->addTab(feedback_page_widget, tr("Feedback"));
    tab_widget->addTab(pid_page_widget, tr("PID"));
    tab_widget->addTab(motor_page_widget, tr("Motor"));
    tab_widget->addTab(errors_page_widget, tr("Errors"));

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
    
    QHBoxLayout *scaling_spinboxes = new QHBoxLayout();
    QVBoxLayout *scaling_column1_labels = new QVBoxLayout();
    QLabel *blankLabel = new QLabel(tr(" "));
    scaling_column1_labels->addWidget(blankLabel);
    input_absolute_max_label = new QLabel(tr("Absolute max:"));
    input_absolute_max_label->setObjectName("input_absolute_max_label");
    scaling_column1_labels->addWidget(input_absolute_max_label);
    input_maximum_label = new QLabel(tr("Maximum:"));
    input_maximum_label->setObjectName("input_maximum_label");
    scaling_column1_labels->addWidget(input_maximum_label);
    input_neutral_max_label = new QLabel(tr("Neutral max:"));
    input_neutral_max_label->setObjectName("input_neutral_max_label");
    scaling_column1_labels->addWidget(input_neutral_max_label);
    input_neutral_min_label = new QLabel(tr("Neutral min:"));
    input_neutral_min_label->setObjectName("input_neutral_min_label");
    scaling_column1_labels->addWidget(input_neutral_min_label);
    input_minimum_label = new QLabel(tr("Minimum:"));
    input_minimum_label->setObjectName("input_minimum_label");
    scaling_column1_labels->addWidget(input_minimum_label);
    input_absolute_min_label = new QLabel(tr("Absolute min:"));
    input_absolute_min_label->setObjectName("input_absolute_min_label");
    scaling_column1_labels->addWidget(input_absolute_min_label);
    input_degree_label = new QLabel(tr("Degree:"));
    input_degree_label->setObjectName("input_degree_label");
    scaling_column1_labels->addWidget(input_degree_label);

    QVBoxLayout *scaling_input_spinboxes = new QVBoxLayout();
    input_input_label = new QLabel(tr("Input"));
    input_input_label->setObjectName("input_input_label");
    scaling_input_spinboxes->addWidget(input_input_label);
    input_absolute_max_spinbox = new QDoubleSpinBox();
    input_absolute_max_spinbox->setObjectName("input_absolute_max_spinbox");
    scaling_input_spinboxes->addWidget(input_absolute_max_spinbox);
    input_maximum_spinbox = new QDoubleSpinBox();
    input_maximum_spinbox->setObjectName("input_maximum_spinbox");
    scaling_input_spinboxes->addWidget(input_maximum_spinbox);
    input_neutral_max_spinbox = new QDoubleSpinBox();
    input_neutral_max_spinbox->setObjectName("input_neutral_max_spinbox");
    scaling_input_spinboxes->addWidget(input_neutral_max_spinbox);
    input_neutral_min_spinbox = new QDoubleSpinBox();
    input_neutral_min_spinbox->setObjectName("input_neutral_min_spinbox");
    scaling_input_spinboxes->addWidget(input_neutral_min_spinbox);
    input_minimum_spinbox = new QDoubleSpinBox();
    input_minimum_spinbox->setObjectName("input_minimum_spinbox");
    scaling_input_spinboxes->addWidget(input_minimum_spinbox);    
    input_absolute_min_spinbox = new QDoubleSpinBox();
    input_absolute_min_spinbox->setObjectName("input_absolute_min_spinbox");
    scaling_input_spinboxes->addWidget(input_absolute_min_spinbox);    
    input_degree_combobox = new QComboBox();
    input_degree_combobox->setObjectName("input_degree_combobox");
    scaling_input_spinboxes->addWidget(input_degree_combobox); 

    QVBoxLayout *scaling_target_column = new QVBoxLayout();
    input_target_label = new QLabel(tr("Target"));
    input_target_label->setObjectName("input_target_label");
    scaling_target_column->addWidget(input_target_label);
    input_output_maximum_spinbox = new QDoubleSpinBox();
    input_output_maximum_spinbox->setObjectName("input_output_maximum_spinbox");
    scaling_target_column->addWidget(input_output_maximum_spinbox);
    input_output_neutral_spinbox = new QDoubleSpinBox();
    input_output_neutral_spinbox->setObjectName("input_output_neutral_spinbox");
    scaling_target_column->addWidget(input_output_neutral_spinbox);
    input_output_minimum_spinbox = new QDoubleSpinBox();
    input_output_minimum_spinbox->setObjectName("input_output_minimum_spinbox");
    scaling_target_column->addWidget(input_output_minimum_spinbox);

    scaling_spinboxes->addLayout(scaling_column1_labels);
    scaling_spinboxes->addLayout(scaling_input_spinboxes);
    scaling_spinboxes->addLayout(scaling_target_column);

    scaling_column1->addLayout(scaling_spinboxes);

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
    column2->addWidget(input_scaling_groupbox);

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
