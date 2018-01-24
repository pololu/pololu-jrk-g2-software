#include "input_wizard.h"

input_wizard::input_wizard(QWidget * parent)
  : QWizard(parent)
{
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

  // tmphax
  std::cout << "wizard.input_value = " << value << std::endl;
}
