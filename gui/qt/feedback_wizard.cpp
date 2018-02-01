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
      "This wizard helps you set the feedback scaling parameters for the "
      "analog feedback.  "
      "Please change the feedback mode to analog, then try again.");
    return;
  }

  feedback_wizard wizard(window, controller);

  QObject::connect(window, &main_window::feedback_changed,
    &wizard, &feedback_wizard::set_feedback);

  if (wizard.exec() != QDialog::Accepted) { return; }

  // TODO: controller->handle_motor_invert_input(wizard.result.motor_invert);
  controller->handle_feedback_invert_input(wizard.result.invert);
  controller->handle_feedback_absolute_minimum_input(wizard.result.absolute_minimum);
  controller->handle_feedback_absolute_maximum_input(wizard.result.absolute_maximum);
  controller->handle_feedback_minimum_input(wizard.result.minimum);
  controller->handle_feedback_maximum_input(wizard.result.maximum);
}

feedback_wizard::feedback_wizard(QWidget * parent, main_controller * controller)
  : QWizard(parent), controller(controller)
{
  feedback_mode = controller->cached_settings.get_feedback_mode();

  setWindowTitle("Feedback setup wizard");
  setWindowIcon(QIcon(":app_icon")); //TODO: make sure this works
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  setPage(INTRO, setup_intro_page());
  setPage(LEARN, setup_learn_page());
  setPage(CONCLUSION, setup_conclusion_page());

  setFixedSize(sizeHint());

  // Handle the next and back buttons with custom slots.
  disconnect(button(NextButton), &QAbstractButton::clicked, 0, 0);
  connect(button(NextButton), &QAbstractButton::clicked, this, &handle_next);
  disconnect(button(BackButton), &QAbstractButton::clicked, 0, 0);
  connect(button(BackButton), &QAbstractButton::clicked, this, &handle_back);
}

void feedback_wizard::handle_next()
{
#ifdef _WIN32
  // In Windows, there is a special back button that we need to find and fix.
  // It is a QVistaBackButton, which is a subclass of QAbstractButton, but it is
  // NOT a Q_OBJECT.
  //
  // It is the only QAbstractButton child with an empty object name (for now),
  // so that is how we find it.  This is fragile; it would be much better
  // if Qt assigned an object name to it.
  for (QAbstractButton * button : findChildren<QAbstractButton *>())
  {
    if (button->objectName().isEmpty() &&
      QString("QAbstractButton") == button->metaObject()->className())
    {
      button->disconnect(SIGNAL(clicked()));
      connect(button, &QAbstractButton::clicked, this, &handle_back);
    }
  }
#endif

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

  set_next_button_enabled(!sampling);
  set_progress_visible(sampling);
  update_learn_text();
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

  set_next_button_enabled(!sampling);
  set_progress_visible(sampling);
  update_learn_text();
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
}

bool feedback_wizard::handle_next_on_intro_page()
{
  // TODO: force duty cycle target to 0, clear latched errors
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

void feedback_wizard::update_learn_text()
{
  switch (learn_step)
  {
  case MAX:
    learn_page->setTitle(tr("Step 1 of 2: Maximum"));
    instruction_label->setText(tr(
      "Move the output to its maximum (full forward) position."));
    break;

  case MIN:
    learn_page->setTitle(tr("Step 2 of 2: Minimum"));
    instruction_label->setText(tr(
      "Move the output to its minimum (full reverse) position."));
    break;
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
    "This wizard will help you quickly set up the feedback scaling parameters."));
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

nice_wizard_page * feedback_wizard::setup_learn_page()
{
  nice_wizard_page * page = learn_page = new nice_wizard_page();
  QVBoxLayout * layout = new QVBoxLayout();

  instruction_label = new QLabel();
  instruction_label->setWordWrap(true);
  instruction_label->setAlignment(Qt::AlignTop);
  instruction_label->setMinimumHeight(fontMetrics().lineSpacing() * 2);
  layout->addWidget(instruction_label);
  layout->addSpacing(fontMetrics().height());

  layout->addLayout(setup_feedback_layout());
  layout->addSpacing(fontMetrics().height());

  QLabel * next_label = new QLabel(
    tr("When you click ") + NEXT_BUTTON_TEXT +
    tr(", this wizard will sample the feedback values for one second.  "
    "Please do not move the output while it is being sampled."));
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

QLayout * feedback_wizard::setup_feedback_layout()
{
  QHBoxLayout * layout = new QHBoxLayout();

  QLabel * feedback_label = new QLabel(tr("Feedback:"));
  layout->addWidget(feedback_label);

  feedback_value = new QLabel();
  layout->addWidget(feedback_value);

  feedback_pretty = new QLabel();
  layout->addWidget(feedback_pretty);

  layout->addStretch(1);

  // Set a fixed size for performance.
  feedback_value->setText(QString::number(4095) + " ");
  feedback_value->setFixedSize(feedback_value->sizeHint());
  feedback_value->setText("");

  feedback_pretty->setText("(" +
    QString::fromStdString(convert_analog_12bit_to_v_string(4095) + ") "));
  feedback_pretty->setFixedSize(feedback_pretty->sizeHint());
  feedback_pretty->setText("");

  return layout;
}

nice_wizard_page * feedback_wizard::setup_conclusion_page()
{
  nice_wizard_page * page = new nice_wizard_page();
  QVBoxLayout * layout = new QVBoxLayout();

  page->setTitle(tr("Feedback setup finished"));

  QLabel * completed_label = new QLabel(
    tr("You have successfully completed this wizard.  You can see your new "
    "settings in the \"Scaling\" box after you click ") +
    FINISH_BUTTON_TEXT + tr(".  "
    "To use the new settings, you must first apply them to the device."));
  completed_label->setWordWrap(true);
  layout->addWidget(completed_label);

  layout->addStretch(1);

  page->setLayout(layout);
  return page;
}
