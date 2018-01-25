#include "input_wizard.h"

#include <QIcon>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>

#ifdef __APPLE__
#define NEXT_BUTTON_TEXT tr("Continue")
#define FINISH_BUTTON_TEXT tr("Done")
#else
#define NEXT_BUTTON_TEXT tr("Next")
#define FINISH_BUTTON_TEXT tr("Finish")
#endif

input_wizard::input_wizard(QWidget * parent)
  : QWizard(parent)
{
  setPage(INTRO, setup_intro_page());
  setPage(LEARN, setup_learn_page());

  setWindowTitle(tr("Input Setup Wizard"));
  setWindowIcon(QIcon(":app_icon"));  // TODO: make sure this works
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  setFixedSize(sizeHint());
}

void input_wizard::set_input(uint16_t value)
{
  input = value;

  input_value->setText(QString::number(value));
}

void input_wizard::set_progress_visible(bool visible)
{
  sampling_label->setVisible(visible);
  sampling_progress->setVisible(visible);
}

QWizardPage * input_wizard::setup_intro_page()
{
  QWizardPage * page = new QWizardPage();
  QVBoxLayout * layout = new QVBoxLayout();

  page->setTitle(windowTitle());

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
  QWizardPage * page = new QWizardPage();
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
