#include "nice_spin_box.h"

#include <iostream>

nice_spin_box::nice_spin_box(bool display_in_milli, QWidget* parent)
  : display_in_milli(display_in_milli), QSpinBox(parent)
{
  connect(this, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
    this, &nice_spin_box::set_code_from_value);
}

void nice_spin_box::set_code_from_value(int value)
{
  code = value;
}

void nice_spin_box::set_mapping(const QMap<int, int> & map)
{
  if (map != mapping)
  {
    mapping = map;
    setRange(mapping.firstKey(), mapping.lastKey());
    setValue(code);
  }
}

void nice_spin_box::stepBy(int step_value)
{
  // TODO: fix all the code in this function

  if (mapping.empty())
  {
    code = value();
    code += step_value;
    return;
  }

  code = value();

  code += step_value;

  while (!mapping.contains(code))
  {
    code += step_value;
  }

  while ((mapping.value(code) == mapping.value(value())))
  {
    code += step_value;
  }

  if (mapping.value(code) - (mapping.value(code) % 10) == 0)
  {
    code += step_value;
  }

  code = mapping.keys(mapping.value(code)).first();

  setValue(code);
  selectAll();
}

// Necessary for use with stepBy function.  TODO: why?
QDoubleSpinBox::StepEnabled nice_spin_box::stepEnabled()
{
  return StepUpEnabled | StepDownEnabled;
}

// Converts user input into a value (a key in the mapping).
int nice_spin_box::valueFromText(const QString & text) const
{
  QString copy = text.toUpper();

  bool value_in_milli = copy.contains("M");
  bool value_in_units = (copy.contains("A") && !copy.contains("M"));
  double entered_value = copy.remove(QRegExp("[^(0-9|.)]")).toDouble();

  if ((!value_in_milli && !display_in_milli)
    || (value_in_units && display_in_milli))
  {
    entered_value *= 1000;
  }

  if (mapping.empty())
  {
    return entered_value;
  }

  double return_value = 0;

  for (auto value : mapping.values())
  {
    int temp_value = value;

    // Used to compare the entered_value to the hundredth times 1000 to an int entered_value
    // ex.  does (4.92 * 1000) == 4923 (would be false without calculation below)
    // with formula below becomes does [(4.92 * 1000) == (4923 - (4923 % 10))]
    // = [(4920) == (4923 - 3)] = [4920 == 4920] (returns true)
    temp_value = temp_value - (temp_value % 10);

    if (entered_value >= temp_value)
    {
      return_value = mapping.key(value);
    }
  }
  return return_value;
}

// Determines how values are displayed to the user.
QString nice_spin_box::textFromValue(int val) const
{
  if (mapping.empty())
  {
    if (display_in_milli)
    {
      return QString::number(val);
    }
    else
    {
      return QString::number(val/1000.0, 'f', decimals);
    }
  }

  if (display_in_milli)
  {
    return QString::number(mapping.value(val));
  }
  else
  {
    int temp_val = mapping.value(val);
    return QString::number(mapping.value(val)/1000.0, 'f', decimals);
  }
}

// Allow the user to input letters as well as digits, but restrict the number
// of digits which can be entered and the allowed letters.
QValidator::State nice_spin_box::validate(QString & input, int & pos) const
{
  // TODO: use QRegularExpression instead; it is recommended by the docs here:
  // http://doc.qt.io/qt-5/qregexp.html
  // TODO: try to use a case-insensitive regular expression
  QRegExp r = QRegExp("(\\d{0,6})(\\.\\d{0,3})?(\\s*)(([m|M][a|A]?)|[a|A])?");

  if (input.isEmpty())
  {
    return QValidator::Intermediate;
  }

  if (r.exactMatch(input))
  {
    return QValidator::Acceptable;
  }
  else
  {
    return QValidator::Invalid;
  }
}
