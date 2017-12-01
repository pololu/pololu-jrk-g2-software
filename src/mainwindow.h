#pragma once

#include <QMainWindow>
#include <QList>
#include <QAction>
#include <QMenu>
#include <QContextMenuEvent>
#include "qcustomplot.h"
#include "graphwindow.h"
#include "altwindow.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget * parent = 0);
	~MainWindow();
	
signals:
	void passWidget(GraphWindow *widget);

private slots:
	void receiveWidget(GraphWindow *widget);
	void on_launchGraph_clicked(QMouseEvent*);

	

protected:
	void contextMenuEvent(QContextMenuEvent *event);


private:
	QFont font;

	QWidget *centralWidget;
	QGridLayout *gridLayout;
	QHBoxLayout *horizontalLayout;
	GraphWindow *previewWindow;
	QPushButton *separateBtn;
	AltWindow *altw;
	QWidget *previewPlot;
	QMenuBar * menu_bar;
	QMenu * file_menu;
	QAction * open_settings_action;
	QAction * save_settings_action;
	QAction * exit_action;
	QMenu * device_menu;
	QAction * disconnect_action;
	QAction * reload_settings_action;
	QAction * restore_defaults_action;
	QAction * apply_settings_action;
	QAction * upgrade_firmware_action;
	QMenu * help_menu;
	QAction * documentation_action;
	QAction * about_action;

	QHBoxLayout * header_layout;
	QLabel * device_list_label;
	QComboBox * device_list_value;
	QLabel * connection_status_value;
	QCheckBox *stop_motor;

	QTabWidget *tab_widget;
	QWidget *status_page_widget;

	// input tab

	QWidget *input_page_widget;
	QGridLayout *input_page_layout;
	QLabel *input_mode_label;
	QComboBox *input_mode_combobox;

	// input tab "Analog to digital conversion" groupbox

	QGroupBox *input_analog_groupbox;
	QLabel *input_analog_samples_label;
	QCheckBox *input_disconnect_with_aux_checkbox;
	QComboBox *input_analog_samples_combobox;

	// input tab "Scaling (Analog and Pulse Width mode only)" groupbox

	QGroupBox *input_scaling_groupbox;
	QLabel *input_scaling_order_warning_label;
	QLabel *input_absolute_max_label;
	QDoubleSpinBox *input_absolute_max_spinbox;
	QLabel *input_maximum_label;
	QDoubleSpinBox *input_maximum_spinbox;
	QLabel *input_neutral_max_label;
	QDoubleSpinBox *input_neutral_max_spinbox;
	QLabel *input_neutral_min_label;
	QDoubleSpinBox *input_neutral_min_spinbox;
	QLabel *input_minimum_label;
	QDoubleSpinBox *input_minimum_spinbox;
	QLabel *input_absolute_min_label;
	QDoubleSpinBox *input_absolute_min_spinbox;
	QLabel *input_degree_label;
	QComboBox *input_degree_combobox;
	QCheckBox *input_invert_checkbox;
	QPushButton *input_learn_button;
	QPushButton *input_reset_range_button;
	QLabel *input_input_label;
	QLabel *input_target_label;
	QDoubleSpinBox *input_output_maximum_spinbox;
	QDoubleSpinBox *input_output_neutral_spinbox;
	QDoubleSpinBox *input_output_minimum_spinbox;

	// input tab "Serial interface" groupbox

	QGroupBox *input_serial_groupbox;
	QCheckBox *input_never_sleep_checkbox;
	QRadioButton *input_usb_dual_port_radio;
	QRadioButton *input_usb_chained_radio;
	QLabel *input_device_label;
	QDoubleSpinBox *input_device_spinbox;
	QDoubleSpinBox *input_uart_fixed_baud_spinbox;
	QRadioButton *input_uart_detect_baud_radio;
	QCheckBox *input_enable_crc_checkbox;
	QLabel *input_timeout_label;
	QRadioButton *input_uart_fixed_baud_radio;
	QDoubleSpinBox *input_timeout_spinbox;





	QWidget *feedback_page_widget;
	QWidget *pid_page_widget;
	QWidget *motor_page_widget;
	QWidget *errors_page_widget;

	QHBoxLayout *footer_layout;
	QPushButton *motorOffButton;
	QPushButton *motorOnButton;
	QPushButton *apply_settings;


	QAction *sepAct;
	bool widgetAtHome;
	
	QWidget * setup_input_tab();
	void setupUi(QMainWindow *MainWindow);
	void retranslateUi(QMainWindow *MainWindow);

	
};