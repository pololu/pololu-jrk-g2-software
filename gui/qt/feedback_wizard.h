#pragma once

#include <uint16_range.h>
#include <stdint.h>
#include <QWizard>

class uint16_range;
class nice_wizard_page;
class QLabel;
class QProgressBar;

class feedback_wizard : public QWizard
{
  Q_OBJECT

  // 20 samples, one every 50 ms.  Total time is 1000 ms.
  static const uint32_t SAMPLE_COUNT = 20;

  enum page { INTRO, LEARN, CONCLUSION };
  enum learn_step { MAX, MIN };
  const int FIRST_STEP = MAX;
  const int LAST_STEP = MIN;

public:
  feedback_wizard(QWidget * parent, uint8_t feedback_mode);

  struct result
  {
    bool invert = false;
    uint16_t absolute_minimum = 0;
    uint16_t absolute_maximum = 4095;
    uint16_t minimum = 0;
    uint16_t maximum = 4095;
  };

  result result;

public slots:
  void handle_next_or_back(int index);
  void set_feedback(uint16_t);

private:
  void set_next_button_enabled(bool enabled);
  void set_progress_visible(bool visible);

  bool handle_back_on_learn_page();
  bool handle_next_on_learn_page();
  void handle_new_sample();
  void handle_sampling_complete();
  bool learn_max();
  bool learn_min();
  bool check_range_not_too_big(const uint16_range &);
  const uint16_t full_range = 4095;
  void update_learn_text();

  nice_wizard_page * setup_intro_page();
  nice_wizard_page * setup_learn_page();
  QLayout * setup_feedback_layout();
  nice_wizard_page * setup_conclusion_page();

  // Controls on the 'Learn' page
  nice_wizard_page * learn_page;
  QLabel * instruction_label;
  QLabel * sampling_label;
  QProgressBar * sampling_progress;
  QLabel * feedback_value;
  QLabel * feedback_pretty;

  // Input mode setting (RC or analog).
  uint8_t feedback_mode;

  // Input variable from the device.
  uint16_t feedback;

  // Current state of the wizard.
  int page = INTRO;
  int learn_step = FIRST_STEP;
  bool learn_step_succeeded = false;
  bool sampling = false;
  std::vector<uint16_t> samples;
  uint16_range learned_max;
  uint16_range learned_min;
};
