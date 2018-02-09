#include "feedback_wizard.h"
#include "message_box.h"
#include "nice_wizard_page.h"

#include <to_string.h>
#include <jrk_protocol.h>

#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QRadioButton>
#include <QVBoxLayout>

#include <algorithm>
#include <cassert>

// TODO: Move to the most forward position, which will be the minimum feedback
// value because you.

// TODO: learn steps: blank text box, force them to enter something
// between Next button gets enabled.
// "Determine the minimum feedback value at the forward extreme"

// TODO: let people manually type max feedback, or click sample button

// TODO: Learn raw feedback polarity based on first time they drive the buttons
// Based on that, flip the raw value on step 2.  Step 2 has raw feedback value
// and radio buttons for inverting it or not.  Default is based on last
// behavior in step 1.
// Say drive forward should correspond to number going up, if not, switch feedback
// polarity.  Step 2 is learn feedback polarity.  Step 3 is maximum.  Step 4 is minimum.

// TODO: Click Finish to Apply settings, allow users to edit the
// settings before applying

// TODO: Custom button for "Back"

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
  QObject::connect(window, &main_window::controller_updated,
    &wizard, &feedback_wizard::controller_updated);

  int result = wizard.exec();

  // Put the jrk back into a more normal state instead of the force duty cycle
  // state.  Also, this is important to do because the window could conceivably
  // be closed while the duty cycle is being forced to some non-zero value.
  controller->stop_motor();

  if (result != QDialog::Accepted) { return; }

  // TODO: really should validate these setting and apply them before leaving
  // the wizard
  controller->handle_motor_invert_input(wizard.result.motor_invert);
  controller->handle_feedback_invert_input(wizard.result.invert);
  controller->handle_feedback_error_minimum_input(wizard.result.error_minimum);
  controller->handle_feedback_error_maximum_input(wizard.result.error_maximum);
  controller->handle_feedback_minimum_input(wizard.result.minimum);
  controller->handle_feedback_maximum_input(wizard.result.maximum);
  controller->apply_settings();
}

feedback_wizard::feedback_wizard(QWidget * parent, main_controller * controller)
  : QWizard(parent), controller(controller)
{
  const jrk::settings & settings = controller->cached_settings;
  feedback_mode = settings.get_feedback_mode();
  result.motor_invert = original_motor_invert = settings.get_motor_invert();
  result.invert = settings.get_feedback_invert();

  max_duty_cycle_percent = std::min(
    settings.get_max_duty_cycle_forward(),
    settings.get_max_duty_cycle_reverse()) / DUTY_CYCLE_FACTOR;

  setWindowTitle("Feedback setup wizard");
  setWindowIcon(QIcon(":app_icon")); //TODO: make sure this works
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
  connect(button(NextButton), &QAbstractButton::clicked, this, &handle_next);
  disconnect(button(BackButton), &QAbstractButton::clicked, 0, 0);
  connect(button(BackButton), &QAbstractButton::clicked, this, &handle_back);

  connect(qApp, &QApplication::focusChanged,
    this, &focus_changed);

  connect(this, &QDialog::finished, this, &copy_form_into_result);
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

void feedback_wizard::focus_changed()
{
  // If the user presses "Enter" when the focus is in the duty cycle input box,
  // we don't want to trigger the "Next" button.  So whenever the focus changes,
  // we use 'setDefault' to change whether the "Next" button is the default button
  // (i.e. the one that 'Enter' acts on).
  QAbstractButton * next_button = button(NextButton);
  if (next_button->isVisible())
  {
    bool next_is_default = !duty_cycle_input->hasFocus();
    if (next_button->inherits("QPushButton"))
    {
      qobject_cast<QPushButton *>(next_button)->setDefault(next_is_default);
    }
    else
    {
      assert(0);
    }
  }
}

void feedback_wizard::handle_next()
{
  if (disconnected_error()) { return; }

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

      // We don't want a checkbox to get visibly highlighted on the final
      // screen, it puts too much emphasis on something that is optional.
      focusWidget()->clearFocus();
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
  duty_cycle_value->setText(QString::fromStdString(
      convert_duty_cycle_to_percent_string(value)));
}

void feedback_wizard::controller_updated()
{
  std::string status;
  bool use_red;

  if (controller->connected())
  {
    status = jrk::diagnose(controller->cached_settings,
      controller->variables, JRK_DIAGNOSE_FLAG_FEEDBACK_WIZARD);
    uint16_t errors = controller->variables.get_error_flags_halting();
    use_red = (errors & ~(1 << JRK_ERROR_AWAITING_COMMAND)) ? 1 : 0;
  }
  else
  {
    status = "Device disconnected.";
    use_red = true;
  }

  bool styled = !motor_status_value->styleSheet().isEmpty();
  if (!styled && use_red)
  {
    motor_status_value->setStyleSheet("color: red;");
  }
  else if (styled && !use_red)
  {
    motor_status_value->setStyleSheet("");
  }
  motor_status_value->setText(QString::fromStdString(status));
}

void feedback_wizard::motor_invert_changed()
{
  result.motor_invert = motor_invert_radio_true->isChecked();
}

void feedback_wizard::feedback_invert_changed()
{
  result.invert = feedback_invert_radio_true->isChecked();
}

void feedback_wizard::learn_fwd_button_pressed()
{
  start_sampling(learn_fwd_value);
}

void feedback_wizard::learn_rev_button_pressed()
{
  start_sampling(learn_rev_value);
}

void feedback_wizard::duty_cycle_input_changed()
{
  if (duty_cycle_input->value() == max_duty_cycle_percent)
  {
    max_duty_cycle_note->setText(tr(
      "The selected speed limit is the maximum speed allowed by your settings."));
  }
  else
  {
    max_duty_cycle_note->setText("");
  }
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

bool feedback_wizard::handle_next_on_intro_page()
{
  try
  {
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
  sampling = false;

  // Go back to the previous step or page.
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

bool feedback_wizard::handle_next_on_learn_page()
{
  sampling = false;

  // Go to the next step or page.
  if (learn_step == LAST_STEP)
  {
    if (!determine_settings())
    {
      return false;
    }
    copy_result_into_form();
    return true;
  }
  else
  {
    learn_step++;
    return false;
  }
}

void feedback_wizard::start_sampling(QLineEdit * input)
{
  if (disconnected_error()) { return; }

  sampling = true;
  sampling_input = input;
  samples.clear();
  sampling_progress->setValue(0);
  update_learn_page_for_sampling();
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
  update_learn_page_for_sampling();

  uint16_range range = uint16_range::from_samples(samples);
  if (!check_range_not_too_big(range)) { return; }

  sampling_input->setText(QString::number(range.average));
  sampling_input->setFocus();
}

bool feedback_wizard::determine_settings()
{
  const std::string try_again = "\n\n"
   "Please verify that your feedback is connected properly by moving "
   "the output while looking at the feedback value and try again.";

  bool fwd_ok, rev_ok;
  int feedback_fwd = learn_fwd_value->text().toInt(&fwd_ok);
  int feedback_rev = learn_rev_value->text().toInt(&rev_ok);

  if (learn_fwd_value->text().isEmpty() || !fwd_ok ||
    learn_rev_value->text().isEmpty() || !rev_ok ||
    feedback_fwd < 0 || feedback_fwd > 4095 ||
    feedback_rev < 0 || feedback_rev > 4095)
  {
    // This shouldn't happen because of our QIntValidator and disabling
    // the 'Next' button when the inputs are empty.
    show_error_message("Both feedback values must be between 0 and 4095.", this);
    return false;
  }

  int distance = abs(feedback_fwd - feedback_rev);
  if (distance == 0)
  {
    show_error_message("The two feedback values are the same." + try_again,
      this);
    return false;
  }
  // TODO: check this threshold with other people, make sure it is OK
  if (distance < 32)
  {
    show_error_message("The two feedback values are too close." + try_again,
      this);
    return false;
  }

  result.maximum = feedback_fwd;
  result.minimum = feedback_rev;
  if (result.invert) { std::swap(result.maximum, result.minimum); }

  if (result.maximum < result.minimum)
  {
    if (result.invert)
    {
      show_error_message("Because the feedback direction is inverted, the "
        "forward extreme (minimum) feedback value needs to be less than the "
        "reverse extreme (maximum) feedback value.", this);
    }
    else
    {
      show_error_message("The forward extreme (maximum) feedback value needs "
        "to be less than the reverse extreme (minimum) feedback value.", this);
    }
    return false;
  }

  result.error_minimum = result.minimum / 2;
  result.error_maximum = 4095 - (4095 - result.maximum) / 2;

  return true;
}

bool feedback_wizard::check_range_not_too_big(const uint16_range & range)
{
  // We consider 7.5% of the standard full range to be too much variation.
  if (range.range() > (full_range * 3 + 20) / 40)
  {
    show_error_message(
      "The feedback value varied too widely (" + range.min_max_string() + ") "
      "during the sampling time.  "
      "Please hold the output still and try again.", this);
    return false;
  }
  return true;
}

void feedback_wizard::update_learn_page()
{
  update_learn_page_for_sampling();
  update_learn_page_completeness();

  motor_invert_widget->setVisible(learn_step == MOTOR_DIR);
  feedback_invert_widget->setVisible(learn_step == FEEDBACK_DIR);
  maxmin_widget->setVisible(learn_step == MAXMIN);
  feedback_widget->setVisible(learn_step != MOTOR_DIR);

  switch (learn_step)
  {
  case MOTOR_DIR:
    learn_page->setTitle(tr("Step 1 of 3: Motor direction"));
    top_instruction_label->setText(tr("Choose the motor direction:"));
    motor_instruction_label->setText(tr(
      "You can click and hold the buttons below to "
      "drive the motor and test its direction.  "
      "If the motor moves in the wrong direction, change your choice "
      "of direction above and try again.  "
      "If the motor does not move at all, try increasing the speed limit, "
      "fixing any errors, and checking the motor wiring."));
    break;

  case FEEDBACK_DIR:
    learn_page->setTitle(tr("Step 2 of 3: Feeedback direction"));
    top_instruction_label->setText("Choose the feedback direction:");
    motor_instruction_label->setText(tr(
      "You can click and hold the buttons below to see how the feedback "
      "reading responds when the motor moves.  If the feedback reading "
      "decreases while driving forward, you should select "
      "\"Inverted feedback direction\"."));
    break;

  case MAXMIN:
    learn_page->setTitle(tr("Step 3 of 3: Feedback range"));
    if (result.invert)
    {
      top_instruction_label->setText(tr(
        "Move the system to its most forward position and click the \"Sample\" "
        "button next to the \"Forward extreme\" box to record the feedback reading. "
        "Since feedback is inverted, the most forward position has the "
        "lowest feedback value. "
        "Then move the system to the other extreme and click the \"Sample\" "
        "button for the \"Reverse extreme\"."
      ));
      learn_fwd_label->setText(tr("Forward extreme (minimum value):"));
      learn_rev_label->setText(tr("Reverse extreme (maximum value):"));
    }
    else
    {
      top_instruction_label->setText(tr(
        "Move the system to its most forward position and click the \"Sample\" "
        "button next to the \"Forward extreme\" box to record the feedback reading. "
        "The most forward position has the highest feedback value. "
        "Then move the system to the other extreme and click the \"Sample\" "
        "button for the \"Reverse extreme\"."
      ));
      learn_fwd_label->setText(tr("Forward extreme (maximum value):"));
      learn_rev_label->setText(tr("Reverse extreme (minimum value):"));
    }
    motor_instruction_label->setText(tr(
      "You can click and hold the buttons below to drive the motor."));
    break;
  }
}

void feedback_wizard::update_learn_page_for_sampling()
{
  sampling_label->setVisible(sampling);
  sampling_progress->setVisible(sampling);

  motor_control_box->setVisible(!sampling);

  learn_fwd_value->setEnabled(!sampling);
  learn_rev_value->setEnabled(!sampling);

  learn_fwd_button->setEnabled(!sampling);
  learn_rev_button->setEnabled(!sampling);
}

// Makes sure that the 'Next' button is grayed out if there are empty required
// inputs on the page.
void feedback_wizard::update_learn_page_completeness()
{
  if (learn_step == MAXMIN)
  {
    if (learn_fwd_value->text().isEmpty() || learn_rev_value->text().isEmpty())
    {
      learn_page->setComplete(false);
    }
    else
    {
      learn_page->setComplete(true);
    }
  }
  else
  {
    learn_page->setComplete(true);
  }
}

// Warn if the scaling values on the last screen are out of order.
void feedback_wizard::update_order_warning()
{
  bool show_warning = false;

  if (final_max_spinbox->value() > final_error_max_spinbox->value())
  {
    show_warning = true;
  }

  if (final_min_spinbox->value() > final_max_spinbox->value())
  {
    show_warning = true;
  }

  if (final_error_min_spinbox->value() > final_min_spinbox->value())
  {
    show_warning = true;
  }

  order_warning_label->setVisible(show_warning);
}

// This is called when we are about to show the conclusion page.  It copies the
// settings in the result struct into the form on that page so the user can see
// them and edit them.
void feedback_wizard::copy_result_into_form()
{
  final_motor_invert_checkbox->setChecked(result.motor_invert);
  final_invert_checkbox->setChecked(result.invert);
  final_error_max_spinbox->setValue(result.error_maximum);
  final_max_spinbox->setValue(result.maximum);
  final_min_spinbox->setValue(result.minimum);
  final_error_min_spinbox->setValue(result.error_minimum);
}

// This is called when the user is done with the wizard.  In case they
// successfully completed the conclusion page, we want to copy the settings they
// chose from the form back into the result struct.
void feedback_wizard::copy_form_into_result()
{
  result.motor_invert = final_motor_invert_checkbox->isChecked();
  result.invert = final_invert_checkbox->isChecked();
  result.error_maximum = final_error_max_spinbox->value();
  result.maximum = final_max_spinbox->value();
  result.minimum = final_min_spinbox->value();
  result.error_minimum = final_error_min_spinbox->value();
}

int feedback_wizard::forward_duty_cycle()
{
  int duty_cycle = duty_cycle_input->value() * DUTY_CYCLE_FACTOR;

  if (result.motor_invert != original_motor_invert)
  {
    duty_cycle = -duty_cycle;
  }

  return duty_cycle;
}

bool feedback_wizard::disconnected_error()
{
  if (controller->connected()) { return false; }

  show_error_message(
    "The jrk is disconnected.  Please exit this wizard and reconnect.", this);
  return true;
}

nice_wizard_page * feedback_wizard::setup_intro_page()
{
  nice_wizard_page * page = intro_page = new nice_wizard_page();
  page->setObjectName("intro_page");

  QVBoxLayout * layout = new QVBoxLayout();

  page->setTitle("Welcome to the feedback setup wizard");

  QLabel * intro_label = new QLabel();
  intro_label->setAlignment(Qt::AlignTop | Qt::AlignJustify);
  intro_label->setWordWrap(true);
  intro_label->setText(tr(
    "This wizard will help you quickly detect your motor direction "
    "and set up the feedback scaling parameters."));
  layout->addWidget(intro_label);

  QLabel * stop_label = new QLabel();
  stop_label->setAlignment(Qt::AlignTop | Qt::AlignJustify);
  stop_label->setWordWrap(true);
  stop_label->setText(tr(
    "NOTE: When you click Next, this wizard will stop the motor "
    "and clear any latched errors.  To restart the motor later, "
    "you can click the \"Run motor\" button (after fixing any errors)."));
  layout->addWidget(stop_label);

  layout->addStretch(1);

  page->setLayout(layout);
  return page;
}

nice_wizard_page * feedback_wizard::setup_learn_page()
{
  int_validator = new QIntValidator(this);
  int_validator->setRange(0, 4095);

  top_instruction_label = new QLabel();
  top_instruction_label->setAlignment(Qt::AlignTop | Qt::AlignJustify);
  top_instruction_label->setWordWrap(true);

  sampling_label = new QLabel(tr("Sampling..."));

  sampling_progress = new QProgressBar();
  sampling_progress->setMaximum(SAMPLE_COUNT);
  sampling_progress->setTextVisible(false);

  QVBoxLayout * layout = new QVBoxLayout();
  layout->addWidget(top_instruction_label);
  layout->addWidget(setup_motor_invert_widget());
  layout->addWidget(setup_feedback_invert_widget());
  layout->addWidget(setup_maxmin_widget());
  layout->addWidget(setup_feedback_widget());
  layout->addSpacing(fontMetrics().height());
  layout->addWidget(setup_motor_control_box());
  layout->addSpacing(fontMetrics().height());
  layout->addWidget(sampling_label);
  layout->addWidget(sampling_progress);
  layout->addStretch(1);

  nice_wizard_page * page = learn_page = new nice_wizard_page();
  page->setLayout(layout);
  return page;
}

QWidget * feedback_wizard::setup_motor_invert_widget()
{
  motor_invert_radio_false = new QRadioButton();
  motor_invert_radio_false->setText(tr(
    "Standard motor direction (\"forward\" means OUTA positive, OUTB negative)"));

  motor_invert_radio_true = new QRadioButton();
  motor_invert_radio_true->setText(tr(
    "Inverted motor direction (\"forward\" means OUTA negative, OUTB positive)"));
  connect(motor_invert_radio_true, &QRadioButton::toggled,
    this, &motor_invert_changed);

  QVBoxLayout * motor_invert_layout = new QVBoxLayout();
  motor_invert_layout->addWidget(motor_invert_radio_false);
  motor_invert_layout->addWidget(motor_invert_radio_true);

  // This must be done after the radio buttons are added to their parent,
  // because they become unchecked at that time.
  if (result.motor_invert)
  {
    motor_invert_radio_true->setChecked(true);
  }
  else
  {
    motor_invert_radio_false->setChecked(true);
  }

  motor_invert_widget = new QWidget();
  motor_invert_widget->setLayout(motor_invert_layout);
  return motor_invert_widget;
}

QWidget * feedback_wizard::setup_feedback_invert_widget()
{
  feedback_invert_radio_false = new QRadioButton();
  feedback_invert_radio_false->setText(tr(
    "Standard feedback direction (\"forward\" means increasing reading)"));

  feedback_invert_radio_true = new QRadioButton();
  feedback_invert_radio_true->setText(tr(
    "Inverted feedback direction (\"forward\" means decreasing reading)"));
  connect(feedback_invert_radio_true, &QRadioButton::toggled,
    this, &feedback_invert_changed);

  QVBoxLayout * feedback_invert_layout = new QVBoxLayout();
  feedback_invert_layout->addWidget(feedback_invert_radio_false);
  feedback_invert_layout->addWidget(feedback_invert_radio_true);

  // This must be done after the radio buttons are added to their parent,
  // because they become unchecked at that time.
  if (result.invert)
  {
    feedback_invert_radio_true->setChecked(true);
  }
  else
  {
    feedback_invert_radio_false->setChecked(true);
  }

  feedback_invert_widget = new QWidget();
  feedback_invert_widget->setLayout(feedback_invert_layout);
  return feedback_invert_widget;
}

QWidget * feedback_wizard::setup_maxmin_widget()
{
  QGridLayout * layout = new QGridLayout();

  learn_fwd_label = new QLabel();

  learn_fwd_value = new QLineEdit();
  learn_fwd_value->setValidator(int_validator);
  connect(learn_fwd_value, &QLineEdit::textChanged,
    this, &update_learn_page_completeness);

  learn_fwd_button = new QPushButton(tr("Sample"));
  connect(learn_fwd_button, &QAbstractButton::clicked,
    this, &learn_fwd_button_pressed);

  learn_rev_label = new QLabel();

  learn_rev_value = new QLineEdit();
  learn_rev_value->setValidator(int_validator);
  connect(learn_rev_value, &QLineEdit::textChanged,
    this, &update_learn_page_completeness);

  learn_rev_button = new QPushButton(tr("Sample"));
  connect(learn_rev_button, &QAbstractButton::clicked,
    this, &learn_rev_button_pressed);

  {
    int width = fontMetrics().width("99999999");
    learn_fwd_value->setFixedWidth(width);
    learn_rev_value->setFixedWidth(width);
  }

  layout->addWidget(learn_fwd_label, 0, 0);
  layout->addWidget(learn_fwd_value, 0, 1);
  layout->addWidget(learn_fwd_button, 0, 2);
  layout->addWidget(learn_rev_label, 1, 0);
  layout->addWidget(learn_rev_value, 1, 1);
  layout->addWidget(learn_rev_button, 1, 2);
  layout->setColumnStretch(3, 1);
  layout->setMargin(0);

  maxmin_widget = new QWidget();
  maxmin_widget->setLayout(layout);
  return maxmin_widget;
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

QGroupBox * feedback_wizard::setup_motor_control_box()
{
  motor_instruction_label = new QLabel();
  motor_instruction_label->setAlignment(Qt::AlignTop | Qt::AlignJustify);
  motor_instruction_label->setWordWrap(true);

  reverse_button = new QPushButton(tr("Drive reverse"));
  connect(reverse_button, &QAbstractButton::pressed, this, &reverse_button_pressed);
  connect(reverse_button, &QAbstractButton::released, this, &drive_button_released);

  forward_button = new QPushButton(tr("Drive forward"));
  connect(forward_button, &QAbstractButton::pressed, this, &forward_button_pressed);
  connect(forward_button, &QAbstractButton::released, this, &drive_button_released);

  QLabel * duty_cycle_input_label = new QLabel(tr("  Speed limit:"));

  duty_cycle_input = new QSpinBox();
  // TODO: when they reach the max duty cycle setting, show a message:
  //  (Jan wants)
  duty_cycle_input->setRange(0, max_duty_cycle_percent);
  duty_cycle_input->setValue(max_duty_cycle_percent / 4);
  duty_cycle_input->setSingleStep(1);
  duty_cycle_input->setSuffix("%");

  max_duty_cycle_note = new QLabel();

  connect(duty_cycle_input, QOverload<int>::of(&QSpinBox::valueChanged),
    this, &duty_cycle_input_changed);

  // Just in case the input is already at its maximum value.
  duty_cycle_input_changed();

  QGridLayout * button_layout = new QGridLayout();
  button_layout->addWidget(reverse_button, 0, 0);
  button_layout->addWidget(forward_button, 0, 1);
  button_layout->addWidget(duty_cycle_input_label, 0, 2);
  button_layout->addWidget(duty_cycle_input, 0, 3);
  button_layout->addWidget(max_duty_cycle_note, 1, 0, 1, 5);
  button_layout->setColumnStretch(4, 1);
  button_layout->setMargin(0);

  motor_status_value = new QLabel();

  QLabel * duty_cycle_label = new QLabel(tr("Duty cycle percent:"));

  duty_cycle_value = new QLabel();
  duty_cycle_value->setText("-100% ");
  duty_cycle_value->setFixedSize(duty_cycle_value->sizeHint());
  duty_cycle_value->setText("");

  QHBoxLayout * vars_layout = new QHBoxLayout();
  vars_layout->addWidget(duty_cycle_label);
  vars_layout->addWidget(duty_cycle_value);
  vars_layout->addStretch(1);
  vars_layout->setMargin(0);

  QVBoxLayout * layout = new QVBoxLayout();
  layout->addWidget(motor_instruction_label);
  layout->addLayout(button_layout);
  layout->addLayout(vars_layout);
  layout->addWidget(motor_status_value);

  motor_control_box = new QGroupBox();
  // TODO: this title is only appropriate on step 1, right?
  motor_control_box->setTitle(tr("Test motor direction (optional)"));
  motor_control_box->setLayout(layout);

  return motor_control_box;
}

nice_wizard_page * feedback_wizard::setup_conclusion_page()
{
  conclusion_page = new nice_wizard_page();
  conclusion_page->setTitle(tr("Review new settings"));

  QLabel * completed_label = new QLabel();
  completed_label->setAlignment(Qt::AlignTop | Qt::AlignJustify);
  completed_label->setWordWrap(true);
  completed_label->setText(tr(
    "You have successfully completed this wizard.  "
    "You can review and edit your new settings below.  "
    "Click \"Finsh and apply settings\" to close this wizard and apply "
    "these new settings to the device."
  ));

  final_motor_invert_checkbox = new QCheckBox();
  final_motor_invert_checkbox->setText(tr("Invert motor direction"));

  final_invert_checkbox = new QCheckBox();
  final_invert_checkbox->setText(tr("Invert feedback direction"));

  QLabel * final_error_max_label = new QLabel();
  final_error_max_label->setText(tr("Error max:"));

  final_error_max_spinbox = new QSpinBox();
  final_error_max_spinbox->setRange(0, 4095);

  QLabel * final_max_label = new QLabel();
  final_max_label->setText(tr("Maximum:"));

  final_max_spinbox = new QSpinBox();
  final_max_spinbox->setRange(0, 4095);

  QLabel * final_min_label = new QLabel();
  final_min_label->setText(tr("Minimum:"));

  final_min_spinbox = new QSpinBox();
  final_min_spinbox->setRange(0, 4095);

  QLabel * final_error_min_label = new QLabel();
  final_error_min_label->setText(tr("Error min:"));

  final_error_min_spinbox = new QSpinBox();
  final_error_min_spinbox->setRange(0, 4095);

  QGridLayout * scaling_layout = new QGridLayout();
  scaling_layout->addWidget(final_error_max_label, 0, 0);
  scaling_layout->addWidget(final_error_max_spinbox, 0, 1);
  scaling_layout->addWidget(final_max_label, 1, 0);
  scaling_layout->addWidget(final_max_spinbox, 1, 1);
  scaling_layout->addWidget(final_min_label, 2, 0);
  scaling_layout->addWidget(final_min_spinbox, 2, 1);
  scaling_layout->addWidget(final_error_min_label, 3, 0);
  scaling_layout->addWidget(final_error_min_spinbox, 3, 1);
  scaling_layout->setColumnStretch(2, 1);

  order_warning_label = new QLabel();
  order_warning_label->setAlignment(Qt::AlignTop | Qt::AlignJustify);
  order_warning_label->setWordWrap(true);
  order_warning_label->setStyleSheet("color: red;");
  order_warning_label->setText("Warning: Each feedback scaling parameter "
    "should be less than or equal to the parameter above it.");

  connect(final_error_max_spinbox, QOverload<int>::of(&QSpinBox::valueChanged),
    this, &update_order_warning);
  connect(final_max_spinbox, QOverload<int>::of(&QSpinBox::valueChanged),
    this, &update_order_warning);
  connect(final_min_spinbox, QOverload<int>::of(&QSpinBox::valueChanged),
    this, &update_order_warning);
  connect(final_error_min_spinbox, QOverload<int>::of(&QSpinBox::valueChanged),
    this, &update_order_warning);

  QVBoxLayout * layout = new QVBoxLayout();
  layout->addWidget(completed_label);
  layout->addItem(new QSpacerItem(1, fontMetrics().height()));
  layout->addWidget(final_motor_invert_checkbox);
  layout->addWidget(final_invert_checkbox);
  layout->addLayout(scaling_layout);
  layout->addWidget(order_warning_label);
  layout->addStretch(1);

  conclusion_page->setLayout(layout);
  return conclusion_page;
}
