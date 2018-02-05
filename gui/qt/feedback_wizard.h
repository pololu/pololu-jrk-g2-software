#pragma once

#include <uint16_range.h>
#include <stdint.h>
#include <QWizard>
#include "main_window.h"
#include "main_controller.h"

class uint16_range;
class nice_wizard_page;
class QLabel;
class QProgressBar;

void run_feedback_wizard(main_window *);

class feedback_wizard : public QWizard
{
  Q_OBJECT

  // 20 samples, one every 50 ms.  Total time is 1000 ms.
  static const uint32_t SAMPLE_COUNT = 20;

  // For converting between native jrk duty cycles and percentages.
  static const int DUTY_CYCLE_FACTOR = JRK_MAX_ALLOWED_DUTY_CYCLE / 100;

  enum page { INTRO, LEARN, CONCLUSION };
  enum learn_step { MOTOR_DIR, MAX, MIN };
  const int FIRST_STEP = MOTOR_DIR;
  const int LAST_STEP = MIN;

public:
  feedback_wizard(QWidget * parent, main_controller *);

  struct result
  {
    bool motor_invert = false;
    bool invert = false;
    uint16_t absolute_minimum = 0;
    uint16_t absolute_maximum = 4095;
    uint16_t minimum = 0;
    uint16_t maximum = 4095;
  };

  result result;

  virtual void showEvent(QShowEvent *);

public slots:
  void handle_next();
  void handle_back();
  void set_feedback(uint16_t);
  void set_duty_cycle(int16_t);
  void set_motor_status(QString status, bool error);
  void motor_invert_changed(bool);
  void duty_cycle_input_changed();
  void reverse_button_pressed();
  void forward_button_pressed();
  void drive_button_released();

private:
  void set_next_button_enabled(bool enabled);
  void set_progress_visible(bool visible);

  bool handle_next_on_intro_page();
  bool handle_back_on_learn_page();
  bool handle_next_on_learn_page();
  void handle_new_sample();
  void handle_sampling_complete();
  bool learn_max();
  bool learn_min();
  bool check_range_not_too_big(const uint16_range &);

  const uint16_t full_range = 4095;

  void update_learn_page();
  int forward_duty_cycle();
  void throw_if_not_connected();

  // This class needs to hold a pointer to the main_controller since there are a
  // variety of commands it needs to send to the jrk to control the motor, which
  // means it has access to a lot of things that it should not use.
  main_controller * controller;

  nice_wizard_page * setup_intro_page();
  nice_wizard_page * setup_learn_page();
  QWidget * setup_feedback_widget();
  QWidget * setup_motor_control_widget();
  nice_wizard_page * setup_conclusion_page();

  // Controls on the 'Learn' page
  nice_wizard_page * learn_page;
  QLabel * instruction_label;
  QLabel * sampling_label;
  QProgressBar * sampling_progress;
  QCheckBox * motor_invert_checkbox;
  QWidget * feedback_widget;
  QLabel * feedback_value;
  QLabel * feedback_pretty;
  QWidget * motor_control_widget;
  QPushButton * reverse_button;
  QPushButton * forward_button;
  QSpinBox * duty_cycle_input;
  QLabel * max_duty_cycle_note;
  QLabel * duty_cycle_value;
  QLabel * motor_status_value;
  QLabel * bottom_instruction_label;

  // The jrk's current settings.
  uint8_t feedback_mode;
  bool original_motor_invert;
  uint8_t max_duty_cycle_percent;

  // Input variable from the device.
  uint16_t feedback;

  // Current state of the wizard.
  int learn_step = FIRST_STEP;
  bool learn_step_succeeded = false;
  bool sampling = false;
  std::vector<uint16_t> samples;
  uint16_range learned_max;
  uint16_range learned_min;
};
