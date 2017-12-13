#pragma once

#include <QMainWindow>
#include <QList>
#include <QAction>
#include <QMenu>
#include <QContextMenuEvent>
#include "qcustomplot.h"
#include "graph_widget.h"
#include "graph_window.h"

class pid_constant_control;
class errors_control;

class main_window : public QMainWindow
{
	Q_OBJECT

public:
	main_window(QWidget * parent = 0);
	~main_window();
	
signals:
	void pass_widget(graph_widget *widget);

private slots:
	void receive_widget(graph_widget *widget);
	void on_launchGraph_clicked(QMouseEvent*);

	

protected:
	void context_menu_event(QContextMenuEvent *event);


private:
	QFont font;

	QWidget *central_widget;
	QGridLayout *grid_layout;
	QHBoxLayout *horizontal_layout;
	graph_widget *preview_window;
	AltWindow *altw;
	QWidget *preview_plot;
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

	// feedback tab

	QWidget *feedback_page_widget;
	QGridLayout *feedback_page_layout;
	QLabel *feedback_mode_label;
	QComboBox *feedback_mode_combobox;

	// feedback tab "Scaling (Analog and Tachometer mode only)" groupbox

	QGroupBox *feedback_scaling_groupbox;
	QCheckBox *feedback_invert_feedback_checkbox;
	QLabel *feedback_absolute_max_label;
	QLabel *feedback_maximum_label;
	QLabel *feedback_minimum_label;
	QLabel *feedback_absolute_min_label;
	QLabel *feedback_calibration_label;
	QLabel *feedback_scaling_order_warning_label;
	QDoubleSpinBox *feedback_absolute_max_spinbox;
	QDoubleSpinBox *feedback_maximum_spinbox;
	QDoubleSpinBox *feedback_minimum_spinbox;
	QDoubleSpinBox *feedback_absolute_min_spinbox;
	QPushButton *feedback_learn_button;
	QPushButton *feedback_reset_range_button;

	// feedback tab "Analog to digital conversion" groupbox

	QGroupBox *feedback_analog_groupbox;
	QLabel *feedback_analog_samples_label;
	QComboBox *feedback_analog_samples_combobox;
	QCheckBox *feedback_disconnect_with_aux_checkbox;

	// pid tab

	QWidget *pid_page_widget;
	QGridLayout *pid_page_layout;
	QLabel *pid_period_label;
	QDoubleSpinBox *pid_period_spinbox;
	QLabel *pid_integral_limit_label;
	QDoubleSpinBox *pid_integral_limit_spinbox;
	QCheckBox *pid_reset_integral_checkbox;
	QLabel *pid_deadzone_label;
	QDoubleSpinBox *pid_deadzone_spinbox;

	// pid tab constant controls

	pid_constant_control *pid_proportional_coefficient;
	pid_constant_control *pid_integral_coefficient;
	pid_constant_control *pid_derivative_coefficient;
	
	// motor tab

	QWidget *motor_page_widget;
	QGridLayout *motor_page_layout;
	QLabel *motor_frequency_label;
	QComboBox *motor_frequency_combobox;
	QCheckBox *motor_invert_checkbox;
	QPushButton *motor_detect_motor_button;
	QCheckBox *motor_asymmetric_checkbox;
	QLabel *motor_forward_label;
	QLabel *motor_reverse_label;
	QLabel *motor_duty_label;
	QDoubleSpinBox *motor_duty_forward_spinbox;
	QDoubleSpinBox *motor_duty_reverse_spinbox;
	QLabel *motor_duty_means_label;
	QLabel *motor_acceleration_label;
	QDoubleSpinBox *motor_acceleration_forward_spinbox;
	QDoubleSpinBox *motor_acceleration_reverse_spinbox;
	QLabel *motor_acceleration_means_label;
	QLabel *motor_duration_label;
	QDoubleSpinBox *motor_duration_forward_spinbox;
	QDoubleSpinBox *motor_duration_reverse_spinbox;
	QLabel *motor_current_label;
	QDoubleSpinBox *motor_current_forward_spinbox;
	QDoubleSpinBox *motor_current_reverse_spinbox;
	QLabel *motor_current_means_label;
	QLabel *motor_calibration_label;
	QDoubleSpinBox *motor_calibration_forward_spinbox;
	QDoubleSpinBox *motor_calibration_reverse_spinbox;
	QLabel *motor_outofrange_label;
	QDoubleSpinBox *motor_outofrange_spinbox;
	QLabel *motor_outofrange_means_label;
	QLabel *motor_off_label;
	QRadioButton *motor_brake_radio;
	QRadioButton *motor_coast_radio;

	QWidget *errors_page_widget;
	QGridLayout *errors_page_layout;
	QLabel *errors_bit_mask_label;
	QLabel *errors_error_label;
	QLabel *errors_setting_label;
	QLabel *errors_stopping_motor_label;
	QLabel *errors_occurence_count_label;
	errors_control *awaiting_command;
	errors_control *no_power;
	errors_control *motor_driven_error;
	errors_control *input_invalid;
	errors_control *input_disconnect;
	errors_control *feedback_disconnect;
	errors_control *max_current_exceeded;
	errors_control *serial_signal_error;
	errors_control *serial_overrun;
	errors_control *serial_rx_buffer_full;
	errors_control *serial_crc_error;
	errors_control *serial_protocol_error;
	errors_control *serial_timeout_error;
	QPushButton *errors_clear_errors;
	QPushButton *errors_reset_counts;


	QHBoxLayout *footer_layout;
	QPushButton *motorOffButton;
	QPushButton *motorOnButton;
	QPushButton *apply_settings;


	QAction *sepAct;
	bool widgetAtHome;
	
	QWidget * setup_status_tab();
    QWidget * setup_input_tab();
	QWidget * setup_feedback_tab();
	QWidget * setup_pid_tab();
	QWidget * setup_motor_tab();
	QWidget * setup_errors_tab();
	void setup_ui(QMainWindow *main_window);
	void retranslate_ui(QMainWindow *main_window);

	
};

class pid_constant_control : public QGroupBox
{
	Q_OBJECT
public:
	pid_constant_control(const QString& group_box_title, const QString& object_name, QWidget *parent = 0);
	~pid_constant_control();

private:
	QWidget *central_widget;
	QFrame *pid_control_frame;
	QFrame *pid_proportion_frame;
	QLineEdit *pid_constant_control_textbox;
	QLabel *pid_equal_label;
	QSpinBox *pid_multiplier_spinbox;
	QLabel *pid_base_label;
	QSpinBox *pid_exponent_spinbox;	
};

class errors_control : public QWidget
{
	Q_OBJECT
public:
  errors_control(int row_number, const QString& object_name, const QString& bit_mask_text, 
      const QString& error_label_text, const bool& disabled_visible, 
      const bool& enabled_visible, QWidget *parent = 0);
	~errors_control();
	
	QGridLayout *errors_central;
	QWidget *errors_frame;
	QLabel *bit_mask_label;
	QLabel *error_label;
	QRadioButton *disabled_radio;
	QRadioButton *enabled_radio;
	QRadioButton *latched_radio;
	QLabel *stopping_motor_label;
	QLabel *count_label;
	QLabel *errors_disabled_spacer;
	QLabel *errors_enabled_spacer;
};