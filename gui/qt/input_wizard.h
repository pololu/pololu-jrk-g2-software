#pragma once

#include <QWizard>
#include <stdint.h>

class nice_wizard_page;
class QLabel;
class QProgressBar;

class input_wizard : public QWizard
{
  Q_OBJECT

  // 20 samples, one every 50 ms.  Total time is 1000 ms.
  static const uint32_t SAMPLE_COUNT = 20;

  enum page_number { INTRO, LEARN, CONCLUSION };
  enum learn_step { NEUTRAL, MAX, MIN };

public:
  input_wizard(QWidget * parent, uint8_t input_mode);

  struct result
  {
    bool invert = false;
    uint16_t absolute_minimum = 0;
    uint16_t absolute_maximum = 4095;
    uint16_t minimum = 0;
    uint16_t maximum = 4095;
    uint16_t neutral_minimum = 2048;
    uint16_t neutral_maximum = 2048;
  };

  result result;

public slots:
  void handle_next_or_back(int index);
  void set_input(uint16_t);

private:
  void set_next_button_enabled(bool enabled);
  void set_progress_visible(bool visible);

  bool handle_back_on_learn_page();
  bool handle_next_on_learn_page();
  void handle_new_sample();
  void handle_sampling_complete();
  void update_learn_text();

  nice_wizard_page * setup_intro_page();
  nice_wizard_page * setup_learn_page();
  QLayout * setup_input_layout();
  nice_wizard_page * setup_conclusion_page();

  // Controls on the 'Learn' page
  nice_wizard_page * learn_page;
  QLabel * instruction_label;
  QLabel * sampling_label;
  QProgressBar * sampling_progress;
  QLabel * input_value;

  // Input mode setting (RC or analog).
  uint8_t input_mode;

  // Input variable from the device.
  uint16_t input;

  // Current state of the wizard.
  int current_page = INTRO;
  int learn_step = NEUTRAL;
  bool sampling = false;
  std::vector<uint16_t> samples;
};
