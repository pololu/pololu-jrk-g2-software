#pragma once

#include <QWizardPage>

// nice_wizard_page is a subclass of QWizardPage that adds the setComplete
// method, which allows you to control whether the 'Next' button is enabled or
// disabled for this page.

class nice_wizard_page : public QWizardPage
{
public:
  virtual bool isComplete() const
  {
    return complete;
  }

  bool setComplete(bool value)
  {
    complete = value;
    emit completeChanged();
  }

private:
  bool complete = true;
};
