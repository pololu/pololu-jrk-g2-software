#include "nice_spin_box.h"

nice_spin_box::nice_spin_box(QWidget* parent)
  : QDoubleSpinBox(parent)
{
  // Since the range of the nice_spin_box can be changed based on the mapped
  // values, the minimum size is based on the default size of a QDoubleSpinBox
  // with the range 0.0-99.99. The suffix is not set using the setSuffix()
  // function so that is not included in the sizeHint() of the nice_spin_box.
  // This is used to set the minimum size of the nice_spin_box to fit entered
  // values.
  {
    QAbstractSpinBox temp_box;
    temp_box.setSpecialValueText("000000.000 XX");
    this->setMinimumSize(temp_box.sizeHint());
  }

  connect(this, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
    this, &set_code_from_value);
  connect(this, &QDoubleSpinBox::editingFinished, this, &set_value_from_code);

  set_suffix = " A";

  setFocusPolicy(Qt::StrongFocus);
}

// This slot which connected in this class sends the "code" value to the
// main_window. This function is necessary to send the new code value when the
// up/down buttons are used in the spinbox, but if the user is manually entering
// a digits, the code will not change so the original code will be sent to
// main_window.
void nice_spin_box::set_code_from_value()
{
  emit send_code(code);
}

// When enter/return is pressed, or when the control loses focus, the value
// entered is used to determine the closest value in the map that is not greater
// than the entered value. The determined value is then set in the nice_spin_box
// and the code mapped to the value is sent to the main_window.
void nice_spin_box::set_value_from_code()
{
  double entered_value = value();

  for (int j = 0; j < mapping.size(); ++j)
  {
    int temp_value = mapping.values().at(j);

    // Used to compare the value to the hundredth times 1000 to an int value
    // ex.  does (4.92 * 1000) == 4923 (would be false without calculation below)
    // with formula below becomes does [(4.92 * 1000) == (4923 - (4923 % 10))]
    // = [(4920) == (4923 - 3)] = [4920 == 4920] (returns true)
    temp_value = temp_value - (temp_value % 10);

    if (entered_value >= temp_value)
    {
      code = mapping.keys().at(j);
    }
  }

  setValue(mapping.value(code));
  emit send_code(code);
}

// The main_window uses this public function to sent the updated map to the
// nice_spin_box the recent code is sent using the value parameter and used to
// determine whether to change the value in the control. The conditional in the
// function is used to prevent the value from being set while the user is entering
// manually.
void nice_spin_box::set_mapping(QMultiMap<int, int>& sent_map, uint16_t value)
{
  mapping.clear();
  mapping = sent_map;

  setRange(0, mapping.last());

  code = value;


  // Prevents the control from updating itself when the user is entering a value
  if (!this->hasFocus())
  {
    if (!mapping.contains(value))
      code = mapping.firstKey();

    setValue(mapping.value(code));
  }
}

// Reimplemented from QSpinBox to use the iterator for steps since the values
// are not separated in a set step.
void nice_spin_box::stepBy(int step_value)
{
  QMultiMap<int, int>::const_iterator it;

  it = mapping.find(code);
  it += step_value;

  code = it.key();

  setValue(mapping.value(code));
  selectAll();
}

// Necessary for use with stepBy function.
QDoubleSpinBox::StepEnabled nice_spin_box::stepEnabled()
{
  StepEnabled enabled  = StepUpEnabled | StepDownEnabled;

  return  enabled;
}

// Evaluates text entered into the nice_spin_box and returns a value without
// a suffix which can be used in other functions and comparisons.
double nice_spin_box::valueFromText(const QString& text) const
{
  QString copy = text.toUpper();

  double temp_num;
  if (copy.contains("M"))
  {
    copy.remove(QRegExp("[^0-9]"));
    temp_num = copy.toDouble();
    return temp_num;
  }
  else
  {
    copy.remove(QRegExp("[^(0-9|.)]"));
    temp_num = copy.toDouble();
    return temp_num * 1000;
  }
}

// Creates the string which is set in the nice_spin_box.
QString nice_spin_box::textFromValue(double val) const
{
  return QString::number(val/1000, 'f', 2) + set_suffix;
}

// Changes the validator which is native to QDoubleSpinBox. This validator
// allows user to input letters as well as digits, but restricts the number
// of digits which can be entered and the allowed letters.
QValidator::State nice_spin_box::validate(QString& input, int& pos) const
{
  QRegExp r = QRegExp("(\\d{0,6})(\\.\\d{0,2})?(\\s*)(m|M|ma|Ma|mA|MA|a|A)?");

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
