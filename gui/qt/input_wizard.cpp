#include "input_wizard.h"
#include "message_box.h"
#include "nice_wizard_page.h"

#include <jrk_protocol.h>

#include <QIcon>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>

#include <cassert>

#ifdef __APPLE__
#define NEXT_BUTTON_TEXT tr("Continue")
#define FINISH_BUTTON_TEXT tr("Done")
#else
#define NEXT_BUTTON_TEXT tr("Next")
#define FINISH_BUTTON_TEXT tr("Finish")
#endif

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

  handle_new_sample();
}

void input_wizard::set_next_button_enabled(bool enabled)
{
  // We only care about setting the next button to be enabled when we are on the
  // learn page, it is always enabled on the other pages.
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
  set_next_button_enabled(false);
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
  // TODO
  return true;
}

bool input_wizard::learn_max()
{
  // TODO
  return true;
}

bool input_wizard::learn_min()
{
  assert(MIN == LAST_STEP);
  // TODO
  return true;
}

void input_wizard::update_learn_text()
{
  switch (learn_step)
  {
  case NEUTRAL:
    learn_page->setTitle(tr("Step 1 of 3: Neutral"));

    if (input_mode == JRK_INPUT_MODE_PULSE_WIDTH)
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

  // TODO: display the human-readable input units too
  // (after we add it to the main window)

  layout->addStretch(1);

  // Set a fixed size for performance.
  input_value->setText(QString::number(4095));
  input_value->setFixedSize(input_value->sizeHint());

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
