#pragma once

#include <QObject>
#include <QIcon>
#include <QWidget>
#include <QWizard>
#include <stdint.h>

#include <iostream> // tmphax

class input_wizard : public QWizard
{
  Q_OBJECT

public:
  input_wizard(QWidget * parent = 0);

  // tmphax
  ~input_wizard() {
    std::cout << "Input wizard destructor" << std::endl;
  }

public slots:
  void set_input(uint16_t);

private:
  uint16_t input;
};
