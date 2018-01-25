#include "input_wizard.h"

#include <QIcon>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include "jrk_protocol.h"

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
// of the page they are trying to go to.  It figures out what happened and then
// calls a handler functions to make higher-level decisions.  The handler should
// true if it is OK with changing to the new page.
void input_wizard::handle_next_or_back(int id)
{
  // Look at current_page to figure out what whether the user is trying to move
  // forward or backward or neither.
  if (current_page == id)
  {
    // We are already on the expected page so don't do anything.  This can
    // happen if the user tried to move and we rejected it.
    return;
  }

  if (current_page == LEARN)
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

  current_page = currentId();

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
  // TODO
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
    if (learn_step == NEUTRAL)
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
  if (sampling)
  {
    // We will advance to the next step/page when the sampling is finished.
    return false;
  }

  // Start sampling the input.
  sampling = true;
  samples.clear();
  sampling_progress->setValue(0);
  return false;
}

void input_wizard::handle_new_sample()
{
  if (!sampling) { return; }

  samples.push_back(input);
  sampling_progress->setValue(samples.size());

  if (samples.size() == SAMPLE_COUNT)
  {
    sampling = false;
    set_progress_visible(false);
    set_next_button_enabled(true);
    handle_sampling_complete();
  }
}

void input_wizard::handle_sampling_complete()
{
  // TODO
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

QWizardPage * input_wizard::setup_intro_page()
{
  QWizardPage * page = new QWizardPage();
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

QWizardPage * input_wizard::setup_learn_page()
{
  QWizardPage * page = learn_page = new QWizardPage();
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

QWizardPage * input_wizard::setup_conclusion_page()
{
  QWizardPage * page = new QWizardPage();
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
