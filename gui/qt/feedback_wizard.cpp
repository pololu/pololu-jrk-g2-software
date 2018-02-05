#include "feedback_wizard.h"
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

void run_feedback_wizard(main_window * window)
{
  main_controller * controller = window->controller;

  if (!controller->connected()) { return; }
  if (!controller->check_settings_applied_before_wizard()) { return; }

  jrk::settings * settings = &controller->cached_settings;
  if (settings->get_feedback_mode() != JRK_FEEDBACK_MODE_ANALOG)
  {
    // Should not happen because the button is disabled.
    window->show_info_message(
      "This wizard helps you detect the motor direction and set the "
      "analog feedback scaling parameters.  "
      "Please change the feedback mode to analog, then try again.");
    return;
  }

  feedback_wizard wizard(window, controller);

  QObject::connect(window, &main_window::feedback_changed,
    &wizard, &feedback_wizard::set_feedback);
  QObject::connect(window, &main_window::duty_cycle_changed,
    &wizard, &feedback_wizard::set_duty_cycle);
  QObject::connect(window, &main_window::motor_status_changed,
    &wizard, &feedback_wizard::set_motor_status);

  int result = wizard.exec();

  // Put the jrk back into a more normal state instead of the force duty cycle
  // state.  Also, this is important to do because the window could conceivably
  // be closed while the duty cycle is being forced to some non-zero value.
  controller->stop_motor();

  if (result != QDialog::Accepted) { return; }

  controller->handle_motor_invert_input(wizard.result.motor_invert);
  controller->handle_feedback_invert_input(wizard.result.invert);
  controller->handle_feedback_absolute_minimum_input(wizard.result.absolute_minimum);
  controller->handle_feedback_absolute_maximum_input(wizard.result.absolute_maximum);
  controller->handle_feedback_minimum_input(wizard.result.minimum);
  controller->handle_feedback_maximum_input(wizard.result.maximum);
}

feedback_wizard::feedback_wizard(QWidget * parent, main_controller * controller)
  : QWizard(parent), controller(controller)
{
  const jrk::settings & settings = controller->cached_settings;
  feedback_mode = settings.get_feedback_mode();
  result.motor_invert = original_motor_invert = settings.get_motor_invert();

  setWindowTitle("Feedback setup wizard");
  setWindowIcon(QIcon(":app_icon")); //TODO: make sure this works
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  setPage(INTRO, setup_intro_page());
  setPage(LEARN, setup_learn_page());
  setPage(CONCLUSION, setup_conclusion_page());

  QSize size = sizeHint();
  size.setHeight(size.height() * 4 / 3);
  setFixedSize(size);

  // Handle the next and back buttons with custom slots.
  disconnect(button(NextButton), &QAbstractButton::clicked, 0, 0);
  connect(button(NextButton), &QAbstractButton::clicked, this, &handle_next);
  disconnect(button(BackButton), &QAbstractButton::clicked, 0, 0);
  connect(button(BackButton), &QAbstractButton::clicked, this, &handle_back);
}

void feedback_wizard::showEvent(QShowEvent * event)
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
#endif
}

void feedback_wizard::handle_next()
{
  if (currentId() == INTRO)
  {
    if (handle_next_on_intro_page())
    {
      next();
    }
  }
  else if (currentId() == LEARN)
  {
    if (handle_next_on_learn_page())
    {
      next();
    }
  }
  else
  {
    next();
  }

  update_learn_page();
}

void feedback_wizard::handle_back()
{
  if (currentId() == LEARN)
  {
    if (handle_back_on_learn_page())
    {
      back();
    }
  }
  else
  {
    back();
  }

  update_learn_page();
}

void feedback_wizard::set_feedback(uint16_t value)
{
  feedback = value;

  feedback_value->setText(QString::number(value));

  if (feedback_mode == JRK_FEEDBACK_MODE_ANALOG)
  {
    feedback_pretty->setText("(" +
      QString::fromStdString(convert_analog_12bit_to_v_string(feedback)) + ")");
  }

  handle_new_sample();
}

void feedback_wizard::set_duty_cycle(int16_t value)
{
  duty_cycle_value->setText(QString::number(value));

  duty_cycle_percent->setText("(" +
    QString::fromStdString(convert_duty_cycle_to_percent_string(value)) + ")");
}

void feedback_wizard::set_motor_status(QString status, bool error)
{
  bool styled = !motor_status_value->styleSheet().isEmpty();

  if (!styled && error)
  {
    motor_status_value->setStyleSheet("color: red;");
  }
  else if (styled && !error)
  {
    motor_status_value->setStyleSheet("");
  }

  motor_status_value->setText(status);
}

void feedback_wizard::motor_invert_changed(bool value)
{
  result.motor_invert = value;
}

void feedback_wizard::reverse_button_pressed()
{
  try
  {
    controller->force_duty_cycle_target_nocatch(-forward_duty_cycle());
  }
  catch (const std::exception & e)
  {
    show_exception(e, this);
  }
}

void feedback_wizard::forward_button_pressed()
{
  try
  {
    controller->force_duty_cycle_target_nocatch(forward_duty_cycle());
  }
  catch (const std::exception & e)
  {
    show_exception(e, this);
  }
}

void feedback_wizard::drive_button_released()
{
  try
  {
    controller->force_duty_cycle_target_nocatch(0);
  }
  catch (const std::exception & e)
  {
    show_exception(e, this);
  }
}

void feedback_wizard::set_next_button_enabled(bool enabled)
{
  // We only care about setting the next button to be enabled when we are on the
  // learn page; it is always enabled on the other pages.
  learn_page->setComplete(enabled);
}

void feedback_wizard::set_progress_visible(bool visible)
{
  sampling_label->setVisible(visible);
  sampling_progress->setVisible(visible);

  motor_control_widget->setVisible(!visible);
  bottom_instruction_label->setVisible(!visible);
}

bool feedback_wizard::handle_next_on_intro_page()
{
  try
  {
    throw_if_not_connected();

    // Force the duty cycle target to 0 to make this wizard safer and because
    // that should be the default state when we drive the motor around manually.
    controller->force_duty_cycle_target_nocatch(0);

    // Clear latched errors so we are more likely to be able to run the motor.
    // This should be done second because if we do it first, it might cause the
    // motor to run.
    controller->clear_errors_nocatch();
  }
  catch (const std::exception & e)
  {
    show_exception(e, this);
    return false;
  }

  // This shouldn't be necessary, but if there is ever a bug in the "Back"
  // button and we go back to the intro page prematurely, it will help.
  learn_step = FIRST_STEP;
  sampling = false;

  return true;
}

bool feedback_wizard::handle_back_on_learn_page()
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

bool feedback_wizard::handle_next_on_learn_page()
{
  if (learn_step_succeeded || learn_step == MOTOR_DIR)
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
    // Start sampling the feedback.
    sampling = true;
    samples.clear();
    sampling_progress->setValue(0);
    return false;
  }
}

void feedback_wizard::handle_new_sample()
{
  if (!sampling) { return; }

  samples.push_back(feedback);
  sampling_progress->setValue(samples.size());

  if (samples.size() == SAMPLE_COUNT)
  {
    handle_sampling_complete();
  }
}

void feedback_wizard::handle_sampling_complete()
{
  sampling = false;
  set_next_button_enabled(true);
  set_progress_visible(false);

  switch (learn_step)
  {
  case MAX:
    learn_step_succeeded = learn_max();
    break;

  case MIN:
    learn_step_succeeded = learn_min();
    break;
  }

  if (learn_step_succeeded)
  {
    // Go to the next step or page.
    handle_next();
  }
}

bool feedback_wizard::learn_max()
{
  learned_max = uint16_range::from_samples(samples);
  if (!check_range_not_too_big(learned_max)) { return false; }

  return true;
}

bool feedback_wizard::learn_min()
{
  std::string const try_again = "\n\n"
   "Please verify that your feedback is connected properly by moving "
   "the output while looking at the feedback value and try again.";

  learned_min = uint16_range::from_samples(samples);
  if (!check_range_not_too_big(learned_min)) { return false; }

  if (learned_min.intersects(learned_max))
  {
    show_error_message(
      "The values sampled for the minimum feedback (" +
      learned_min.min_max_string() + ") intersect the values sampled for "
      "the maximum feedback (" + learned_max.min_max_string() + ")." +
      try_again, this);
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

  result.maximum = real_max->average;
  result.minimum = real_min->average;

  // Set the absolute range: when the feedback is outside of this range, the jrk
  // considers it to be an error.
  result.absolute_minimum = result.minimum / 2;
  result.absolute_maximum = 4095 - (4095 - result.maximum) / 2;

  return true;
}

bool feedback_wizard::check_range_not_too_big(const uint16_range & range)
{
  // We consider 7.5% of the standard full range to be too much variation.
  if (range.range() > (full_range * 3 + 20) / 40)
  {
    show_error_message(
      "The output value varied too widely (" + range.min_max_string() + ") "
      "during the sampling time.  "
      "Please hold the feedback still and try again.", this);
    return false;
  }
  return true;
}

void feedback_wizard::update_learn_page()
{
  set_next_button_enabled(!sampling);
  set_progress_visible(sampling);

  feedback_widget->setVisible(learn_step != MOTOR_DIR);
  motor_invert_checkbox->setVisible(learn_step == MOTOR_DIR);

  switch (learn_step)
  {
  case MOTOR_DIR:
    learn_page->setTitle(tr("Step 1 of 3: Motor direction"));
    instruction_label->setText(tr(
      "Click and hold the buttons below to drive the motor.  "
      "If the motor moves in the "
      "wrong direction, toggle the \"Invert motor direction\" checkbox to fix "
      "it, and try again.  If the motor does not move at all, "
      "try increasing the duty cycle target, "
      "fixing any errors that are occurring, "
      "and checking the motor wiring. "
      "You can skip this step by clicking " NEXT_BUTTON_TEXT "."));
    bottom_instruction_label->setText("");
    break;

  case MAX:
    learn_page->setTitle(tr("Step 2 of 3: Maximum"));
    instruction_label->setText(tr(
      "Use the \"Drive forward\" button or move the output manually to get it "
      "to its maximum (full forward) position.  "
      "If the \"Drive forward\" button drives in the wrong direction, go back "
      "and toggle the \"Invert motor direction\" checkbox to fix it."));
    bottom_instruction_label->setText(tr(
      "When you click " NEXT_BUTTON_TEXT ", this wizard will sample the "
      "feedback values for one second.  Please do not move the output "
      "while it is being sampled."));
    break;

  case MIN:
    learn_page->setTitle(tr("Step 3 of 3: Minimum"));
    instruction_label->setText(tr(
      "Use the \"Drive reverse\" button or move the output manually to get it "
      "to its minimum (full reverse) position."));
    bottom_instruction_label->setText(tr(
      "When you click " NEXT_BUTTON_TEXT ", this wizard will sample the "
      "feedback values for one second.  Please do not move the output "
      "while it is being sampled."));
    break;
  }
}

int feedback_wizard::forward_duty_cycle()
{
  int duty_cycle = duty_cycle_input->value();

  if (result.motor_invert != original_motor_invert)
  {
    duty_cycle = -duty_cycle;
  }

  return duty_cycle;
}

void feedback_wizard::throw_if_not_connected()
{
  if (!controller->connected())
  {
    throw std::runtime_error(
      "The connection to the device was lost.  "
      "Please close this wizard, reconnect, and try again.");
  }
}

nice_wizard_page * feedback_wizard::setup_intro_page()
{
  nice_wizard_page * page = new nice_wizard_page();
  QVBoxLayout * layout = new QVBoxLayout();

  page->setTitle("Welcome to the feedback setup wizard");

  QLabel * intro_label = new QLabel();
  intro_label->setWordWrap(true);
  intro_label->setText(tr(
    "This wizard will help you quickly detect your motor direction "
    "and set up the feedback scaling parameters."));
  layout->addWidget(intro_label);

  QLabel * stopped_label = new QLabel();
  stopped_label->setWordWrap(true);
  stopped_label->setText(tr(
    "NOTE: When you click " NEXT_BUTTON_TEXT ", this wizard will stop the motor "
    "and clear any latched errors.  To restart the motor later, "
    "you can click the \"Run motor\" button (after fixing any errors)."));
  layout->addWidget(stopped_label);

  layout->addStretch(1);

  page->setLayout(layout);
  return page;
}

nice_wizard_page * feedback_wizard::setup_learn_page()
{
  nice_wizard_page * page = learn_page = new nice_wizard_page();
  QVBoxLayout * layout = new QVBoxLayout();

  instruction_label = new QLabel();
  instruction_label->setWordWrap(true);
  instruction_label->setAlignment(Qt::AlignTop);
  instruction_label->setMinimumHeight(fontMetrics().lineSpacing() * 4);
  layout->addWidget(instruction_label);
  layout->addSpacing(fontMetrics().height());

  motor_invert_checkbox = new QCheckBox();
  motor_invert_checkbox->setText(tr("Invert motor direction"));
  motor_invert_checkbox->setChecked(original_motor_invert);
  connect(motor_invert_checkbox, &QCheckBox::stateChanged,
    this, &motor_invert_changed);
  layout->addWidget(motor_invert_checkbox);

  layout->addWidget(setup_feedback_widget());
  layout->addSpacing(fontMetrics().height());

  layout->addWidget(setup_motor_control_widget());
  layout->addSpacing(fontMetrics().height());

  bottom_instruction_label = new QLabel();
  bottom_instruction_label->setWordWrap(true);
  layout->addWidget(bottom_instruction_label);
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

QWidget * feedback_wizard::setup_feedback_widget()
{
  QLabel * feedback_label = new QLabel(tr("Feedback:"));

  feedback_value = new QLabel();
  feedback_value->setText("4095 ");
  feedback_value->setFixedSize(feedback_value->sizeHint());
  feedback_value->setText("");

  feedback_pretty = new QLabel();
  feedback_pretty->setText("(" +
    QString::fromStdString(convert_analog_12bit_to_v_string(4095) + ") "));
  feedback_pretty->setFixedSize(feedback_pretty->sizeHint());
  feedback_pretty->setText("");

  QHBoxLayout * layout = new QHBoxLayout();
  layout->addWidget(feedback_label);
  layout->addWidget(feedback_value);
  layout->addWidget(feedback_pretty);
  layout->addStretch(1);
  layout->setMargin(0);

  feedback_widget = new QWidget();
  feedback_widget->setLayout(layout);

  return feedback_widget;
}

QWidget * feedback_wizard::setup_motor_control_widget()
{
  reverse_button = new QPushButton(tr("Drive reverse"));
  connect(reverse_button, &QAbstractButton::pressed, this, &reverse_button_pressed);
  connect(reverse_button, &QAbstractButton::released, this, &drive_button_released);

  forward_button = new QPushButton(tr("Drive forward"));
  connect(forward_button, &QAbstractButton::pressed, this, &forward_button_pressed);
  connect(forward_button, &QAbstractButton::released, this, &drive_button_released);

  // TODO: make clickable
  QLabel * duty_cycle_input_label = new QLabel(tr("  Speed limit:"));

  // TODO: display as a percentage
  duty_cycle_input = new QSpinBox();
  // TODO: max value should be max duty cycle setting
  // TODO: when they reach the max duty cycle setting, show a message:
  //  (Jan wants)
  duty_cycle_input->setRange(0, 600);
  // TODO: default value is 25% of the max duty cycle (lower of fwd and reverse)
  duty_cycle_input->setValue(60);
  duty_cycle_input->setSingleStep(60);  // TODO: Jan said maybe 5%
  duty_cycle_input->setToolTip("For full speed, enter 600.");

  QHBoxLayout * button_layout = new QHBoxLayout();
  button_layout->addWidget(reverse_button);
  button_layout->addWidget(forward_button);
  button_layout->addWidget(duty_cycle_input_label);
  button_layout->addWidget(duty_cycle_input);
  button_layout->addStretch(1);
  button_layout->setMargin(0);

  // TODO: duty cycle target is too much detail
  // just say "Click and hold one of the buttons above to drive the motor."
  // don't say "force" or "target"
  motor_status_value = new QLabel();

  // TODO: say 'Duty cycle percent'
  QLabel * duty_cycle_label = new QLabel(tr("Duty cycle:"));

  // TODO: don't show this, only show duty cycle as a percentage
  duty_cycle_value = new QLabel();
  duty_cycle_value->setText("-600 ");
  duty_cycle_value->setFixedSize(duty_cycle_value->sizeHint());
  duty_cycle_value->setText("");

  duty_cycle_percent = new QLabel();
  duty_cycle_percent->setText("(-100%) ");
  duty_cycle_percent->setFixedSize(duty_cycle_percent->sizeHint());
  duty_cycle_percent->setText("");

  QHBoxLayout * vars_layout = new QHBoxLayout();
  vars_layout->addWidget(duty_cycle_label);
  vars_layout->addWidget(duty_cycle_value);
  vars_layout->addWidget(duty_cycle_percent);
  vars_layout->addStretch(1);
  vars_layout->setMargin(0);

  QVBoxLayout * layout = new QVBoxLayout();
  layout->addLayout(button_layout);
  layout->addSpacing(fontMetrics().height());
  layout->addLayout(vars_layout);
  layout->addWidget(motor_status_value);
  layout->setMargin(0);

  motor_control_widget = new QWidget();
  motor_control_widget->setLayout(layout);

  return motor_control_widget;
}

nice_wizard_page * feedback_wizard::setup_conclusion_page()
{
  nice_wizard_page * page = new nice_wizard_page();
  QVBoxLayout * layout = new QVBoxLayout();

  page->setTitle(tr("Feedback setup finished"));

  QLabel * completed_label = new QLabel(
    "You have successfully completed this wizard.  You can see your new "
    "settings in the \"Scaling\" box and the \"Invert motor direction\" "
    "checkbox after you click " FINISH_BUTTON_TEXT ".  "
    "To use the new settings, you must first apply them to the device.");
  completed_label->setWordWrap(true);
  layout->addWidget(completed_label);

  layout->addStretch(1);

  page->setLayout(layout);
  return page;
}
