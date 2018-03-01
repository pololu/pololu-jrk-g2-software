#include "nice_spin_box.h"
#include "main_controller.h"

#include <QLineEdit>
#include <QSpinBox>
#include <QtMath>
#include <QRegExpValidator>
#include <QDebug>
#include <iostream> //tmphax

nice_spin_box::nice_spin_box(int index, QWidget* parent)
  : index(index), QDoubleSpinBox(parent)
{
  connect(this, QOverload<const QString&>::of(&QDoubleSpinBox::valueChanged), this, &editing_finished);

  setRange(-1, 10000);
  setKeyboardTracking(false);
  setDecimals(2);
}

void nice_spin_box::set_controller(main_controller * controller)
{
  this->controller = controller;
}

void nice_spin_box::editing_finished(const QString& text)
{
  if (suppress_events) { return; }

  qDebug() << text;

  entered_value = text.toDouble();

  double smallest_diff = qFabs(entered_value - mapping.values().at(0));

  for(int i = 1; i < mapping.size() - 1; i++)
  {
    double current_diff = qFabs(entered_value - mapping.values().at(i));
    if (current_diff < smallest_diff)
    {
      smallest_diff = current_diff;
      int closest = i;
      current_index = closest;
    }
  }

  if (entered_value > mapping.values().last())
  {
    current_index = mapping.size() - 1;
  }

  set_code();
}

void nice_spin_box::set_display_value()
{
  suppress_events = true;
  setValue(mapping[current_index]);
  suppress_events = false;
}

void nice_spin_box::set_possible_values(uint16_t value)
{
  mapping.clear();
  mapping.insert(0, (controller->get_current_limit_value(0)/1000));

  for (int i = 1; i < 96; i++)
  {
    double display_value = (controller->get_current_limit_value(i)/1000);

    mapping.insert(i, display_value);
  }

  current_index = value;

  set_display_value();
}

void nice_spin_box::stepBy(int step_value)
{
  if (mapping.values().at(qBound(0, (current_index + step_value),
    mapping.size() - 1)) == value())
  {
    current_index += (step_value * 2);
  }
  else
    current_index += step_value;

  current_index = qBound(0, current_index, mapping.size() - 1);

  setValue(mapping[current_index]);
}

QDoubleSpinBox::StepEnabled nice_spin_box::stepEnabled()
{
  StepEnabled enabled = StepUpEnabled | StepDownEnabled;
  if (qBound(0, current_index, mapping.size() - 1) == 0)
  {
    enabled ^= StepDownEnabled;
  }
  if (qBound(0, current_index, mapping.size() - 1) == mapping.size() - 1)
  {
    enabled ^= StepUpEnabled;
  }
  return enabled;
}

void nice_spin_box::set_code()
{
  if (suppress_events) { return; }
  controller->handle_current_limit_amps_spinbox_input(index, current_index);
}
