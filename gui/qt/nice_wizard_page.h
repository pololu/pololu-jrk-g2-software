#pragma once

#include <QWizardPage>

// nice_wizard_page is a subclass of QWizardPage that adds the setComplete
// method, which allows you to control whether the 'Next' button is enabled or
// disabled for this page.
//
// We can't just do button(NextButton)->setEnabled(false) in the QWizard class
// because that doesn't take care of the focus properly.  The 'Cancel' button
// will end up getting focussed while the Next button is disabled, and then if
// the user presses Enter, the window will close, which is bad.
class nice_wizard_page : public QWizardPage
{
public:
  virtual bool isComplete() const
  {
    return complete;
  }

  void setComplete(bool value)
  {
    complete = value;
    emit completeChanged();
  }

private:
  bool complete = true;
};
