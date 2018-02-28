#pragma once

#include <QDoubleSpinBox>

#include "main_controller.h"


class nice_spin_box : public QDoubleSpinBox
{
  Q_OBJECT

public:
  nice_spin_box(QWidget* parent = Q_NULLPTR);

  void set_controller(main_controller * controller = NULL);

  void set_possible_values(uint16_t value);

private slots:
  void editing_finished();
  void set_display_value();
  void set_code();

private:
  main_controller * controller;
  QList<QPair<int, double>> mapping;
  int current_index = 0;

  bool suppress_events = false;

  // Reimplemented QDoubleSpinBox functions
  virtual void stepBy(int step_value);
  virtual StepEnabled stepEnabled();
};
