#include "nice_spin_box.h"

#include <iostream> //tmphax

nice_spin_box::nice_spin_box(QWidget* parent)
  : QDoubleSpinBox(parent)
{
  connect(this, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &set_code_from_value);

  setKeyboardTracking(false);
  setDecimals(3);
}

void nice_spin_box::set_code_from_value(double entered_value)
{
  for (int j = 0; j < mapping.size(); ++j)
  {
    if (entered_value == 0)
    {
      code = 0;
    }
    else if (entered_value >= mapping.values().at(j))
    {
      code = mapping.keys().at(j);
    }
  }

  emit send_code(code);
}

void nice_spin_box::editing_finished()
{
  entered_value = value();
  std::cout << entered_value << std::endl;
  for (int j = 0; j < mapping.size() - 1; ++j)
  {
    if (entered_value >= mapping.values().at(j))
    {
      code = mapping.keys().at(j);
    }
  }

  emit send_code(code);
}

void nice_spin_box::set_display_value()
{
  setValue(mapping.value(code, 0));
}

void nice_spin_box::set_possible_values(QMultiMap<int, int>& sent_maps, uint16_t value)
{
  mapping.clear();
  mapping = sent_maps;
  setRange(0, mapping.last());

  code = value;

  set_display_value();
}

void nice_spin_box::stepBy(int step_value)
{
  QMultiMap<int, int>::const_iterator it;
  it = mapping.find(code);
  it += step_value;
  code = it.key();
  set_display_value();
  selectAll();
}

QDoubleSpinBox::StepEnabled nice_spin_box::stepEnabled()
{
  return  StepUpEnabled | StepDownEnabled;
}

double nice_spin_box::valueFromText(const QString& text) const
{
  QString copy = text.toUpper();
  double temp_num = copy.toDouble();
  if (copy.contains("M"))
  {
    copy.remove(QRegExp("(M|MA)"));
    temp_num = copy.toDouble();
    return temp_num;
  }
  else
    return temp_num * 1000;
}

QString nice_spin_box::textFromValue(double val) const
{
  return QString::number(val/1000, 'f', 3);
}

QValidator::State nice_spin_box::validate(QString& input, int& pos) const
{
  QRegExp r = QRegExp("(\\d{0,6})(\\.\\d{0,4})?(\\s*)(m|ma|Ma|mA|MA|a|A)?");

  if (r.exactMatch(input))
  {
    return QValidator::Acceptable;
  }
  else
  {
    return QValidator::Invalid;
  }
}
