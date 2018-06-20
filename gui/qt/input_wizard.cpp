#include "input_wizard.h"
#include "message_box.h"
#include "nice_wizard_page.h"
#include "util.h"

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

  uint8_t input_mode = settings->get_input_mode();

  input_wizard wizard(window, input_mode, controller);
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
  controller->apply_settings();
}

input_wizard::input_wizard(QWidget * parent, uint8_t input_mode,
  main_controller * controller)
  : QWizard(parent), input_mode(input_mode), controller(controller)
{
  setWindowTitle(tr("Input setup wizard"));
  setWindowIcon(QIcon(":app_icon"));
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  setPage(INTRO, setup_intro_page());
  setPage(LEARN, setup_learn_page());
  setPage(CONCLUSION, setup_conclusion_page());

  QSize size = sizeHint();
  size.setHeight(size.height() * 4 / 3);
  setFixedSize(size);

  // Custom text for the buttons, so that it doesn't change when they are on
  // macOS and so we can make it clear that the 'Finish' button also applies
  // settings.
  setButtonText(NextButton, tr("Next"));
  setButtonText(FinishButton, tr("Finish and apply settings"));

  // Handle the next and back buttons with custom slots.
  disconnect(button(NextButton), &QAbstractButton::clicked, 0, 0);
  connect(button(NextButton), &QAbstractButton::clicked,
    this, &input_wizard::handle_next);
  disconnect(button(BackButton), &QAbstractButton::clicked, 0, 0);
  connect(button(BackButton), &QAbstractButton::clicked,
    this, &input_wizard::handle_back);

  connect(this, &QDialog::finished, this, &input_wizard::copy_form_into_result);
}

void input_wizard::showEvent(QShowEvent * event)
{
#ifdef _WIN32
  // In Windows, there is a special back button that we need to find and fix.
  // It doesn't have a good object name or class name, so we just identify it
  // using "disconnect".
  //
  // The back button is set up in QWizard::event when it receives a show event.
  // It then calls QDialog::event, which eventually calls QWidget::event, which
  // calls showEvent, so showEvent is a good place to fix the button.
  for (QAbstractButton * button : findChildren<QAbstractButton *>())
  {
    if (disconnect(button, SIGNAL(clicked()), this, SLOT(back())))
    {
      connect(button, SIGNAL(clicked()), this, SLOT(handle_back()));
    }
  }

  // Make the margins symmetric instead of having the left margin be larger than
  // the right margin.  In qwizard.cpp, there is a hardcoded margin of 18 for
  // the QFrame object.  It's hard to override the margins for the QFrame
  // because it gets set at different times, so just add a margin to our pages.
  QWidget * frame = qobject_cast<QWidget *>(learn_page->parent());
  if (frame)
  {
    int left, top, right, bottom;
    frame->getContentsMargins(&left, &top, &right, &bottom);
    int fudge = left - right;
    intro_page->setContentsMargins(0, 0, fudge, 0);
    learn_page->setContentsMargins(0, 0, fudge, 0);
    conclusion_page->setContentsMargins(0, 0, fudge, 0);
  }
#endif
}

void input_wizard::handle_next()
{
  if (disconnected_error()) { return; }

  if (currentId() == INTRO)
  {
    handle_next_on_intro_page();
  }
  else if (currentId() == LEARN)
  {
    handle_next_on_learn_page();
  }
  else
  {
    next();
  }
}

void input_wizard::handle_back()
{
  if (currentId() == LEARN)
  {
    handle_back_on_learn_page();
  }
  else
  {
    back();
  }
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

void input_wizard::handle_next_on_intro_page()
{
  // Stop the motor so it isn't moving during this wizard.
  try
  {
    controller->stop_motor_nocatch();
  }
  catch (const std::exception & e)
  {
    show_exception(e, this);
    return;
  }

  // This shouldn't be necessary, but if there is ever a bug in the "Back"
  // button and we go back to the intro page prematurely, it will help.
  learn_step = FIRST_STEP;
  sampling = false;
  update_learn_page();
  next();
}

void input_wizard::handle_back_on_learn_page()
{
  if (sampling)
  {
    // We were in the middle of sampling, so just cancel that.
    sampling = false;
    update_learn_page();
  }
  else
  {
    // We were not sampling, so go back to the previous step or page.
    if (learn_step == FIRST_STEP)
    {
      back();
    }
    else
    {
      learn_step--;
      update_learn_page();
    }
  }
}

void input_wizard::handle_next_on_learn_page()
{
  if (sampling)
  {
    // We will advance to the next step/page when the sampling is finished.
    // This case shouldn't even happen because we disable the 'Next' button.
    assert(0);
  }
  else
  {
    // Start sampling the input.
    sampling = true;
    samples.clear();
    sampling_progress->setValue(0);
    update_learn_page();
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

  bool success = false;
  switch (learn_step)
  {
  case NEUTRAL:
    success = learn_neutral();
    break;

  case MAX:
    success = learn_max();
    break;

  case MIN:
    success = learn_min();
    break;
  }

  if (success)
  {
    if (learn_step == LAST_STEP)
    {
      copy_result_into_form();

      // Go to the conclusion page.
      next();

      // We don't want a checkbox to get visibly highlighted on the final
      // screen, it puts too much emphasis on something that is optional.
      focusWidget()->clearFocus();
    }
    else
    {
      learn_step++;
    }
  }
  update_learn_page();
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

void input_wizard::update_learn_page()
{
  set_next_button_enabled(!sampling);
  set_progress_visible(sampling);
  update_learn_text();
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
      assert(0);  // Should not happen.
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

void input_wizard::update_order_warning()
{
  bool warn = !ordered({
    final_error_min_spinbox->value(),
    final_min_spinbox->value(),
    final_neutral_min_spinbox->value(),
    final_neutral_max_spinbox->value(),
    final_max_spinbox->value(),
    final_error_max_spinbox->value()});
  order_warning_label->setVisible(warn);
}

void input_wizard::copy_result_into_form()
{
  final_invert_checkbox->setChecked(result.invert);
  final_error_max_spinbox->setValue(result.error_maximum);
  final_max_spinbox->setValue(result.maximum);
  final_neutral_max_spinbox->setValue(result.neutral_maximum);
  final_neutral_min_spinbox->setValue(result.neutral_minimum);
  final_min_spinbox->setValue(result.minimum);
  final_error_min_spinbox->setValue(result.error_minimum);
}

void input_wizard::copy_form_into_result()
{
  result.invert = final_invert_checkbox->isChecked();
  result.error_maximum = final_error_max_spinbox->value();
  result.maximum = final_max_spinbox->value();
  result.neutral_maximum = final_neutral_max_spinbox->value();
  result.neutral_minimum = final_neutral_min_spinbox->value();
  result.minimum = final_min_spinbox->value();
  result.error_minimum = final_error_min_spinbox->value();
}

bool input_wizard::disconnected_error()
{
  if (controller->connected()) { return false; }
  show_error_message(
    "The jrk is disconnected.  Please exit this wizard and reconnect.", this);
  return true;
}

nice_wizard_page * input_wizard::setup_intro_page()
{
  nice_wizard_page * page = intro_page = new nice_wizard_page();
  QVBoxLayout * layout = new QVBoxLayout();

  page->setTitle("Welcome to the input setup wizard");

  QLabel * intro_label = new QLabel();
  intro_label->setWordWrap(true);
  if (input_mode == JRK_INPUT_MODE_RC)
  {
    intro_label->setText(tr(
      "This wizard will help you quickly set up the scaling parameters "
      "for your RC input."));
  }
  else if (input_mode == JRK_INPUT_MODE_ANALOG)
  {
    intro_label->setText(tr(
      "This wizard will help you quickly set up the scaling parameters "
      "for your analog input."));
  }
  else
  {
    assert(0);  // Should not happen.
    intro_label->setText(tr(
      "This wizard will help you quickly set up the scaling parameters "
      "for your input."));
  }
  layout->addWidget(intro_label);

  QLabel * stop_label = new QLabel();
  stop_label->setAlignment(Qt::AlignTop | Qt::AlignJustify);
  stop_label->setWordWrap(true);
  stop_label->setText(tr(
    "NOTE: When you click Next, this wizard will stop the motor.  "
    "To restart the motor later, you can click the \"Run motor\" button "
    "(after fixing any errors)."));
  layout->addWidget(stop_label);

  layout->addStretch(1);

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

  QLabel * next_label = new QLabel();
  next_label->setWordWrap(true);
  next_label->setText(tr(
    "When you click Next, this wizard will sample the input values for one "
    "second.  Please do not change the input while it is being sampled."));
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
  nice_wizard_page * page = conclusion_page = new nice_wizard_page();
  page->setTitle(tr("Review new settings"));

  QLabel * completed_label = new QLabel();
  completed_label->setAlignment(Qt::AlignTop | Qt::AlignJustify);
  completed_label->setWordWrap(true);
  completed_label->setText(tr(
    "You have successfully completed this wizard.  "
    "You can review and edit your new settings below.  "
    "Click \"Finsh and apply settings\" to close this wizard and apply "
    "these new settings to the device."
  ));

  final_invert_checkbox = new QCheckBox();
  final_invert_checkbox->setText(tr("Invert input direction"));

  QLabel * final_error_max_label = new QLabel();
  final_error_max_label->setText(tr("Error max:"));

  final_error_max_spinbox = new QSpinBox();
  final_error_max_spinbox->setRange(0, 4095);

  QLabel * final_max_label = new QLabel();
  final_max_label->setText(tr("Maximum:"));

  final_max_spinbox = new QSpinBox();
  final_max_spinbox->setRange(0, 4095);

  QLabel * final_neutral_max_label = new QLabel();
  final_neutral_max_label->setText(tr("Neutral max:"));

  final_neutral_max_spinbox = new QSpinBox();
  final_neutral_max_spinbox->setRange(0, 4095);

  QLabel * final_neutral_min_label = new QLabel();
  final_neutral_min_label->setText(tr("Neutral min:"));

  final_neutral_min_spinbox = new QSpinBox();
  final_neutral_min_spinbox->setRange(0, 4095);

  QLabel * final_min_label = new QLabel();
  final_min_label->setText(tr("Minimum:"));

  final_min_spinbox = new QSpinBox();
  final_min_spinbox->setRange(0, 4095);

  QLabel * final_error_min_label = new QLabel();
  final_error_min_label->setText(tr("Error min:"));

  final_error_min_spinbox = new QSpinBox();
  final_error_min_spinbox->setRange(0, 4095);

  order_warning_label = new QLabel();
  order_warning_label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
  order_warning_label->setStyleSheet("color: red;");
  order_warning_label->setVisible(false);
  order_warning_label->setText(tr(
    "Warning: Each input scaling\n"
    "setting should be less than or\n"
    "equal to the setting above it."));

  QGridLayout * scaling_layout = new QGridLayout();
  scaling_layout->addWidget(final_error_max_label, 0, 0);
  scaling_layout->addWidget(final_error_max_spinbox, 0, 1);
  scaling_layout->addWidget(final_max_label, 1, 0);
  scaling_layout->addWidget(final_max_spinbox, 1, 1);
  scaling_layout->addWidget(final_neutral_max_label, 2, 0);
  scaling_layout->addWidget(final_neutral_max_spinbox, 2, 1);
  scaling_layout->addWidget(final_neutral_min_label, 3, 0);
  scaling_layout->addWidget(final_neutral_min_spinbox, 3, 1);
  scaling_layout->addWidget(final_min_label, 4, 0);
  scaling_layout->addWidget(final_min_spinbox, 4, 1);
  scaling_layout->addWidget(final_error_min_label, 5, 0);
  scaling_layout->addWidget(final_error_min_spinbox, 5, 1);
  scaling_layout->addWidget(order_warning_label, 0, 2, 6, 1);
  scaling_layout->setColumnStretch(2, 1);

  auto valueChanged = static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged);
  connect(final_error_max_spinbox, valueChanged,
    this, &input_wizard::update_order_warning);
  connect(final_max_spinbox, valueChanged,
    this, &input_wizard::update_order_warning);
  connect(final_neutral_max_spinbox, valueChanged,
    this, &input_wizard::update_order_warning);
  connect(final_neutral_min_spinbox, valueChanged,
    this, &input_wizard::update_order_warning);
  connect(final_min_spinbox, valueChanged,
    this, &input_wizard::update_order_warning);
  connect(final_error_min_spinbox, valueChanged,
    this, &input_wizard::update_order_warning);

  QVBoxLayout * layout = new QVBoxLayout();
  layout->addWidget(completed_label);
  layout->addItem(new QSpacerItem(1, fontMetrics().height()));
  layout->addWidget(final_invert_checkbox);
  layout->addLayout(scaling_layout);
  layout->addStretch(1);

  page->setLayout(layout);
  return page;
}
