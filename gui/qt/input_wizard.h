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

  struct result
  {
    uint16_t absolute_minimum = 0;
    uint16_t absolute_maximum = 4095;
    uint16_t minimum = 0;
    uint16_t maximum = 4095;
    uint16_t neutral_minimum = 2048;
    uint16_t neutral_maximum = 2048;
  };

  result result;

public slots:
  void set_input(uint16_t);

private:
  uint16_t input;
};
