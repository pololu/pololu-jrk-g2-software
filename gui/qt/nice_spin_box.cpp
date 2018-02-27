#include "nice_spin_box.h"
#include "main_controller.h"

#include <QLineEdit>
#include <QSpinBox>

#include <iostream> //tmphax

nice_spin_box::nice_spin_box(QWidget* parent)
  : QDoubleSpinBox(parent)
{
  connect(this, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
    this, &set_code);
  connect(this, &QDoubleSpinBox::editingFinished, this, &editing_finished);

  setKeyboardTracking(false);
  setSuffix(" A");
  setDecimals(3);
}

void nice_spin_box::set_controller(main_controller * controller)
{
  this->controller = controller;
}

void nice_spin_box::editing_finished()
{
  for(int i = 0; i < mapping.size() - 1; i++)
  {
    if (value() > mapping.at(i).second && value() < mapping.at(i + 1).second)
    {
      setValue(mapping.at(i).second);
    }
    else if (value() == mapping.at(i).second)
      setValue(mapping.at(i).second);
    current_index = i;
  }

  if (value() > mapping.last().second)
  {
    setValue(mapping.last().second);
    current_index = mapping.size() - 1;
  }
}

void nice_spin_box::set_display_value()
{
  setValue(mapping.at(current_index).second);
}

void nice_spin_box::set_possible_values(uint16_t value)
{
  mapping.clear();
  mapping.append(qMakePair(0, (controller->get_current_limit_value(0)/1000)));

  for (int i = 1; i < 96; i++)
  {
    double display_value = (controller->get_current_limit_value(i)/1000);

    // if (display_value >= mapping.last().second)
    // {
      mapping.append(qMakePair(i, display_value));
    // }
  }

  current_index = value;

  setValue(mapping.at(current_index).second);
  // set_display_value();

}

void nice_spin_box::stepBy(int step_value)
{
  if (mapping.at(qBound(0, (current_index + step_value),
    mapping.size() - 1)).second == value())
  {
    current_index += (step_value * 2);
  }
  else
    current_index += step_value;

  current_index = qBound(0, current_index, mapping.size() - 1);

  setValue(mapping.at(current_index).second);
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

void nice_spin_box::set_code(int index)
{
  std::cout << mapping.at(current_index).first << " set code " << mapping.at(current_index).second << std::endl; //tmphax

  controller->handle_current_limit_forward_input(current_index);
}
