#include "input_wizard.h"

#include <QLabel>
#include <QVBoxLayout>

input_wizard::input_wizard(QWidget * parent)
  : QWizard(parent)
{
  setPage(INTRO, setup_intro_page());

  setWindowTitle(tr("Input Setup Wizard"));
  setWindowIcon(QIcon(":app_icon"));  // TODO: make sure this works
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  // TODO: try using setSizeConstraint(QLayout::SetFixedSize) on the
  // layout object instead
  setFixedSize(sizeHint());
}

void input_wizard::set_input(uint16_t value)
{
  input = value;
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
