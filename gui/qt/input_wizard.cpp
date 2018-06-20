#include "input_wizard.h"
#include "message_box.h"
#include "nice_wizard_page.h"
#include "wizard_button_text.h"

#include <to_string.h>
#include <jrk_protocol.h>

#include <QIcon>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>

#include <algorithm>
#include <cassert>

void run_input_wizard(main_window * window)
{
  main_controller * controller = window->controller;

  if (!controller->check_settings_applied_before_wizard()) { return; }

  jrk::settings * settings = &controller->cached_settings;
  if (settings->get_input_mode() != JRK_INPUT_MODE_ANALOG &&
    settings->get_input_mode() != JRK_INPUT_MODE_RC)
  {
    // Should not happen because the button is disabled.
    window->show_info_message(
      "This wizard helps you set the input scaling parameters for the "
      "pulse width (RC) or analog input.  "
      "Please change the input mode to pulse width or analog, then try again.");
    return;
  }

  // Stop the motor so it isn't moving while people are learning the input.
  controller->stop_motor();  // TODO: don't do that until they get past the first screen

  uint8_t input_mode = settings->get_input_mode();

  input_wizard wizard(window, input_mode);
  QObject::connect(window, &main_window::input_changed,
    &wizard, &input_wizard::set_input);

  if (wizard.exec() != QDialog::Accepted) { return; }

  controller->handle_input_invert_input(wizard.result.invert);
  controller->handle_input_error_minimum_input(wizard.result.error_minimum);
  controller->handle_input_error_maximum_input(wizard.result.error_maximum);
  controller->handle_input_minimum_input(wizard.result.minimum);
  controller->handle_input_maximum_input(wizard.result.maximum);
  controller->handle_input_neutral_minimum_input(wizard.result.neutral_minimum);
  controller->handle_input_neutral_maximum_input(wizard.result.neutral_maximum);
  // TODO: apply settings
}

input_wizard::input_wizard(QWidget * parent, uint8_t input_mode)
  : QWizard(parent), input_mode(input_mode)
{
  setWindowTitle(tr("Input setup wizard"));
  setWindowIcon(QIcon(":app_icon"));  // TODO: make sure this works
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  setPage(INTRO, setup_intro_page());
  setPage(LEARN, setup_learn_page());
  setPage(CONCLUSION, setup_conclusion_page());

  setFixedSize(sizeHint());

  connect(this, &QWizard::currentIdChanged,
    this, &input_wizard::handle_next_or_back);
}

// This gets called when the user pressed next or back, and it receives the ID
// of the page the QWizard class it trying to go to.  It figures out what
// happened and then calls a handler function.  The handler should true if it is
// OK with changing to the new page and take care of any other effects.
void input_wizard::handle_next_or_back(int id)
{
  if (page == id)
  {
    // We are already on the expected page so don't do anything.  This can
    // happen if the user tried to move and we rejected it.
    return;
  }

  if (page == LEARN)
  {
    if (id == INTRO)
    {
      // User clicked back from the learn page.
      if (!handle_back_on_learn_page())
      {
        next();
      }
    }
    else if (id == CONCLUSION)
    {
      if (!handle_next_on_learn_page())
      {
        back();
      }
    }
  }

  page = currentId();

  set_next_button_enabled(!sampling);
  set_progress_visible(sampling);
  update_learn_text();
}

void input_wizard::set_input(uint16_t value)
{
  input = value;

  input_value->setText(QString::number(value));

  if (input_mode == JRK_INPUT_MODE_RC)
  {
    input_pretty->setText("(" +
      QString::fromStdString(convert_rc_12bit_to_us_string(input)) + ")");
  }
  else if (input_mode == JRK_INPUT_MODE_ANALOG)
  {
    input_pretty->setText("(" +
      QString::fromStdString(convert_analog_12bit_to_v_string(input)) + ")");
  }

  handle_new_sample();
}

void input_wizard::set_next_button_enabled(bool enabled)
{
  // We only care about setting the next button to be enabled when we are on the
  // learn page; it is always enabled on the other pages.
  learn_page->setComplete(enabled);
}

void input_wizard::set_progress_visible(bool visible)
{
  sampling_label->setVisible(visible);
  sampling_progress->setVisible(visible);
}

bool input_wizard::handle_back_on_learn_page()
{
  if (sampling)
  {
    // We were in the middle of sampling, so just cancel that.
    sampling = false;
    return false;
  }
  else
  {
    // We were not sampling, so go back to the previous step or page.
    if (learn_step == FIRST_STEP)
    {
      return true;
    }
    else
    {
      learn_step--;
      return false;
    }
  }
}

bool input_wizard::handle_next_on_learn_page()
{
  if (learn_step_succeeded)
  {
    // We completed this step of the learning so go to the next step or page.
    learn_step_succeeded = false;
    if (learn_step == LAST_STEP)
    {
      return true;
    }
    else
    {
      learn_step++;
      return false;
    }
  }
  else if (sampling)
  {
    // We will advance to the next step/page when the sampling is finished.
    // This case shouldn't even happen because we disable the 'Next' button.
    assert(0);
    return false;
  }
  else
  {
    // Start sampling the input.
    sampling = true;
    samples.clear();
    sampling_progress->setValue(0);
    return false;
  }
}

void input_wizard::handle_new_sample()
{
  if (!sampling) { return; }

  samples.push_back(input);
  sampling_progress->setValue(samples.size());

  if (samples.size() == SAMPLE_COUNT)
  {
    handle_sampling_complete();
  }
}

void input_wizard::handle_sampling_complete()
{
  sampling = false;
  set_next_button_enabled(true);
  set_progress_visible(false);

  switch (learn_step)
  {
  case NEUTRAL:
    learn_step_succeeded = learn_neutral();
    break;

  case MAX:
    learn_step_succeeded = learn_max();
    break;

  case MIN:
    learn_step_succeeded = learn_min();
    break;
  }

  if (learn_step_succeeded)
  {
    // Go to the next step or page.  We can't directly control it so just
    // trigger handle_next_or_back(), which will check the learn_step_succeded
    // flag.
    next();
  }
}

bool input_wizard::learn_neutral()
{
  uint16_range r = uint16_range::from_samples(samples);
  if (!check_range_not_too_big(r)) { return false; }

  // Set the deadband region to 5% of the standard full range or 3 times the
  // sampled range, whichever is greater.
  uint16_t deadband = std::max<uint16_t>((full_range() + 10) / 20, r.range());

  learned_neutral = uint16_range::from_center_and_range(r.average, deadband, 4095);

  return true;
}

bool input_wizard::learn_max()
{
  learned_max = uint16_range::from_samples(samples);
  if (!check_range_not_too_big(learned_max)) { return false; }

  if (learned_max.intersects(learned_neutral))
  {
    // The input was indistinguishable from neutral, so warn the user!
    show_warning_message(
      "The values sampled for the maximum input (" +
      learned_max.min_max_string() +
      ") intersect with the neutral deadband (" +
      learned_neutral.min_max_string() + ").\n\n"
      "If you continue, you will only be able to use the input in one " +
      "direction.\n\n"
      "You can go back and try again if this is not the desired behavior.",
      this);
  }

  return true;
}

bool input_wizard::learn_min()
{
  std::string const try_again = "\n\n"
   "Please verify that your input is connected properly by moving it "
   "while looking at the input value and try again.";

  learned_min = uint16_range::from_samples(samples);
  if (!check_range_not_too_big(learned_min)) { return false; }

  if (learned_min.intersects(learned_max))
  {
    show_error_message(
      "The values sampled for the minimum input (" +
      learned_min.min_max_string() + ") intersect the values sampled for "
      "the maximum input (" + learned_max.min_max_string() + ")." +
      try_again, this);
    return false;
  }

  // Make sure at least one input direction is distinguishable from neutral.
  if (learned_min.intersects(learned_neutral) && learned_max.intersects(learned_min))
  {
    show_error_message(
      "The values sampled for the minimum input (" +
      learned_min.min_max_string() + ") and the values sampled for the "
      "maximum input (" + learned_max.min_max_string() +
      ") both intersect the neutral deadband (" +
      learned_neutral.min_max_string() + ")." + try_again, this);
    return false;
  }

  // Invert the channel if necessary.
  uint16_range * real_max = &learned_max;
  uint16_range * real_min = &learned_min;
  if (learned_min.is_entirely_above(learned_max))
  {
    result.invert = true;
    std::swap(real_max, real_min);
  }
  else
  {
    result.invert = false;
  }
  assert(real_max->is_entirely_above(*real_min));

  // Check that the max and min are not both on the same side of the deadband.
  if (real_min->is_entirely_above(learned_neutral))
  {
    show_error_message(
      "The maximum and minimum values measured for the input (" +
      learned_max.min_max_string() + " and " +
      learned_min.min_max_string() +
      ") were both above the neutral deadband (" +
      learned_neutral.min_max_string() + ")." + try_again, this);
    return false;
  }
  if (learned_neutral.is_entirely_above(*real_max))
  {
    show_error_message(
      "The maximum and minimum values measured for the input (" +
      learned_max.min_max_string() + " and " +
      learned_min.min_max_string() +
      ") were both below the neutral deadband (" +
      learned_neutral.min_max_string() + ")." + try_again, this);
    return false;
  }

  if (learned_min.intersects(learned_neutral))
  {
    show_warning_message(
      "The values sampled for the minimum input (" +
      learned_min.min_max_string() +
      ") intersect with the neutral deadband (" +
      learned_neutral.min_max_string() + ").\n\n"
      "If you continue, you will only be able to use the input in one " +
      "direction.\n\n"
      "You can go back and try again if this is not the desired behavior.",
      this);
  }

  // All the checks are done, so figure out the new settings.

  result.neutral_minimum = learned_neutral.min;
  result.neutral_maximum = learned_neutral.max;

  if (real_max->intersects(learned_neutral))
  {
    // real_max intersects the deadband, so that is a direction
    // that the user doesn't want to move their input.
    result.maximum = result.neutral_maximum;
  }
  else
  {
    // Try to set input maximum to a value that is a little bit below
    // real_max->min so that when the user pushes the input all the way in that
    // direction, they can get the full output.
    int margin = std::min(
      (real_max->min - result.neutral_maximum + 15) / 30,
      (full_range() + 50) / 100);
    result.maximum = real_max->min - margin;
  }

  if (real_min->intersects(learned_neutral))
  {
    // real_min intersects the deadband, so that is a direction
    // that the user doesn't want to move their input.
    result.minimum = result.neutral_minimum;
  }
  else
  {
    // Try to set input minimum to a value that is a little bit above
    // real_min->max so that when the user pushes the input all the way in that
    // direction, they can get the full output.
    int margin = std::min(
      (result.neutral_maximum - real_min->max + 15) / 30,
      (full_range() + 50) / 100);
    result.minimum = real_min->max + margin;
  }

  // Set the error range: when the input is outside of this range, the jrk
  // considers it to be an error.
  if (input_mode == JRK_INPUT_MODE_RC)
  {
    // We usually want 500 us to 2500 us to be the error range for RC inputs,
    // but we try to keep a margin of 10 us between the error range and the
    // values people want to use.  Remember the units are 2/3 us.
    result.error_minimum = std::min(750, std::max(result.minimum - 15, 0));
    result.error_maximum = std::max(750, std::min(result.maximum + 15, 4095));
  }
  else
  {
    result.error_minimum = result.minimum / 2;
    result.error_maximum = 4095 - (4095 - result.maximum) / 2;
  }

  return true;
}

bool input_wizard::check_range_not_too_big(const uint16_range & range)
{
  // We consider 7.5% of the standard full range to be too much variation.
  if (range.range() > (full_range() * 3 + 20) / 40)
  {
    show_error_message(
      "The input value varied too widely (" + range.min_max_string() + ") "
      "during the sampling time.  "
      "Please hold the input still and try again.", this);
    return false;
  }
  return true;
}

uint16_t input_wizard::full_range() const
{
  if (input_mode == JRK_INPUT_MODE_RC)
  {
    // Standard RC full range is 1500 to 3000 (units of 2/3 us).
    return 1500;
  }

  return 4095;
}

void input_wizard::update_learn_text()
{
  switch (learn_step)
  {
  case NEUTRAL:
    learn_page->setTitle(tr("Step 1 of 3: Neutral"));

    if (input_mode == JRK_INPUT_MODE_RC)
    {
      instruction_label->setText(tr(
        "Verify that you have connected your pulse width input to the RC pin.  "
        "Next, move the input to its neutral position."));
    }
    else if (input_mode == JRK_INPUT_MODE_ANALOG)
    {
      instruction_label->setText(tr(
        "Verify that you have connected your analog input to the SDA/AN pin.  "
        "Next, move the input to its neutral position."));
    }
    else
    {
      // Should not happen.
      assert(0);
      instruction_label->setText(tr(
        "Move the input to its neutral position."));
    }
    break;

  case MAX:
    learn_page->setTitle(tr("Step 2 of 3: Maximum"));
    instruction_label->setText(tr(
      "Move the input to its maximum (full forward) position."));
    break;

  case MIN:
    learn_page->setTitle(tr("Step 3 of 3: Minimum"));
    instruction_label->setText(tr(
      "Move the input to its minimum (full reverse) position."));
    break;
  }
}

nice_wizard_page * input_wizard::setup_intro_page()
{
  nice_wizard_page * page = new nice_wizard_page();
  QVBoxLayout * layout = new QVBoxLayout();

  page->setTitle("Welcome to the input setup wizard");

  QLabel * intro_label = new QLabel();
  intro_label->setWordWrap(true);
  intro_label->setText(tr(
    "This wizard will help you quickly set up the input scaling parameters."));
  layout->addWidget(intro_label);
  layout->addStretch(1);

  QLabel * stopped_label = new QLabel();
  stopped_label->setWordWrap(true);
  stopped_label->setText(tr(
    "NOTE: Your motor has been automatically stopped so that it does not "
    "cause problems while you are using this wizard.  To restart it manually "
    "later, you can click the \"Run motor\" button (after fixing any errors)."));
  layout->addWidget(stopped_label);

  page->setLayout(layout);
  return page;
}

nice_wizard_page * input_wizard::setup_learn_page()
{
  nice_wizard_page * page = learn_page = new nice_wizard_page();
  QVBoxLayout * layout = new QVBoxLayout();

  instruction_label = new QLabel();
  instruction_label->setWordWrap(true);
  instruction_label->setAlignment(Qt::AlignTop);
  instruction_label->setMinimumHeight(fontMetrics().lineSpacing() * 2);
  layout->addWidget(instruction_label);
  layout->addSpacing(fontMetrics().height());

  layout->addLayout(setup_input_layout());
  layout->addSpacing(fontMetrics().height());

  QLabel * next_label = new QLabel(
    tr("When you click ") + NEXT_BUTTON_TEXT +
    tr(", this wizard will sample the input values for one second.  "
    "Please do not change the input while it is being sampled."));
  next_label->setWordWrap(true);
  layout->addWidget(next_label);
  layout->addSpacing(fontMetrics().height());

  sampling_label = new QLabel(tr("Sampling..."));
  layout->addWidget(sampling_label);

  sampling_progress = new QProgressBar();
  sampling_progress->setMaximum(SAMPLE_COUNT);
  sampling_progress->setTextVisible(false);
  layout->addWidget(sampling_progress);

  set_progress_visible(false);

  layout->addStretch(1);

  page->setLayout(layout);

  return page;
}

QLayout * input_wizard::setup_input_layout()
{
  QHBoxLayout * layout = new QHBoxLayout();

  QLabel * input_label = new QLabel(tr("Input:"));
  layout->addWidget(input_label);

  input_value = new QLabel();
  layout->addWidget(input_value);

  input_pretty = new QLabel();
  layout->addWidget(input_pretty);

  layout->addStretch(1);

  // Set a fixed size for performance.
  input_value->setText(QString::number(4095) + " ");
  input_value->setFixedSize(input_value->sizeHint());
  input_value->setText("");

  input_pretty->setText("(" +
    QString::fromStdString(convert_rc_12bit_to_us_string(4095) + ") "));
  input_pretty->setFixedSize(input_pretty->sizeHint());
  input_pretty->setText("");

  return layout;
}

nice_wizard_page * input_wizard::setup_conclusion_page()
{
  nice_wizard_page * page = new nice_wizard_page();
  QVBoxLayout * layout = new QVBoxLayout();

  page->setTitle(tr("Input setup finished"));

  QLabel * completed_label = new QLabel(
    tr("You have successfully completed this wizard.  You can see your new "
    "settings in the \"Input\" column and \"Invert input direction\" checkbox "
    "after you click ") +
    FINISH_BUTTON_TEXT + tr(".  "
    "To use the new settings, you must first apply them to the device."));
  completed_label->setWordWrap(true);
  layout->addWidget(completed_label);

  layout->addStretch(1);

  page->setLayout(layout);
  return page;
}
