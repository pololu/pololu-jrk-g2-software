#include "nice_spin_box.h"

nice_spin_box::nice_spin_box(bool display_in_milli, QWidget* parent)
  : display_in_milli(display_in_milli), QSpinBox(parent)
{
  // Since the range of the nice_spin_box can be changed based on the mapped
  // values, the minimum size is based on the default size of a QSpinBox
  // with the range 0-99. This is used to set the minimum size of the
  // nice_spin_box to fit entered values.
  {
    QAbstractSpinBox temp_box;
    temp_box.setSpecialValueText("0000000");
    this->setMinimumSize(temp_box.sizeHint());
  }

  connect(this, QOverload<int>::of(&QSpinBox::valueChanged),
    this, &set_code_from_value);
}


void nice_spin_box::set_code_from_value(int value)
{
  if (value > 0)
  {
    code = value;
  }
}

// The main_window uses this public function to send the updated map to the
// nice_spin_box and set the range of the QSpinBox.
void nice_spin_box::set_mapping(QMultiMap<int, int>& sent_map)
{
  mapping.clear();
  mapping = sent_map;

  // The "key" values can be within any range so the range of the
  // spin box must dynamic based on the key values mapped to the control.
  setRange(mapping.firstKey(), mapping.lastKey());
}

// Reimplemented from QSpinBox to use the iterator for steps since the values
// are not separated in a set step.
void nice_spin_box::stepBy(int step_value)
{
  QMultiMap<int, int>::const_iterator it;

  it = mapping.find(code);

  while (it.value() == mapping.value(value()))
    it += step_value;

  code = it.key();

  setValue(code);
  selectAll();
}

// Necessary for use with stepBy function.
QDoubleSpinBox::StepEnabled nice_spin_box::stepEnabled()
{
  return StepUpEnabled | StepDownEnabled;
}

// Evaluates text entered into the nice_spin_box and returns a value without
// a suffix which can be used in other functions and comparisons.
int nice_spin_box::valueFromText(const QString& text) const
{
  QString copy = text.toUpper();

  bool value_in_milli = copy.contains("M") || (copy.contains("A") && !copy.contains("M"));
  double entered_value = copy.remove(QRegExp("[^(0-9|.)]")).toDouble();

  if ((!value_in_milli && !display_in_milli)
    || (value_in_milli && display_in_milli))
  {
    entered_value *= 1000;
  }

  double return_value;

  for (int j = 0; j < mapping.size(); ++j)
  {
    int temp_value = mapping.values().at(j);

    // Used to compare the entered_value to the hundredth times 1000 to an int entered_value
    // ex.  does (4.92 * 1000) == 4923 (would be false without calculation below)
    // with formula below becomes does [(4.92 * 1000) == (4923 - (4923 % 10))]
    // = [(4920) == (4923 - 3)] = [4920 == 4920] (returns true)
    temp_value = temp_value - (temp_value % 10);

    if (entered_value >= temp_value)
    {
      return_value = mapping.keys().at(j);
    }
  }

  return return_value;
}

// Creates the string which is set in the nice_spin_box.
QString nice_spin_box::textFromValue(int val) const
{
  if (display_in_milli)
  {
    return QString::number(mapping.value(val));
  }
  else
    return QString::number(mapping.value(val)/1000.0, 'f', 2);
}

// Changes the validator which is native to QDoubleSpinBox. This validator
// allows user to input letters as well as digits, but restricts the number
// of digits which can be entered and the allowed letters.
QValidator::State nice_spin_box::validate(QString& input, int& pos) const
{
  QRegExp r = QRegExp("(\\d{0,6})(\\.\\d{0,2})?(\\s*)(([m|M][a|A]?)|[a|A])?");

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
