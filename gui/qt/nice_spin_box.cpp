#include "nice_spin_box.h"

nice_spin_box::nice_spin_box(QWidget* parent) : QSpinBox(parent)
{
  connect(this, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
    this, &nice_spin_box::set_code_from_value);
}

void nice_spin_box::set_code_from_value(int value)
{
  code = value;
}

void nice_spin_box::set_mapping(const QMap<int, int> & new_mapping)
{
  if (mapping == new_mapping) { return; }
  mapping = new_mapping;
  if (!mapping.empty())
  {
    setRange(mapping.firstKey(), mapping.lastKey());
  }
  setValue(code);
}

QString nice_spin_box::text_from_ma(int milliamps) const
{
  return QString::number(milliamps / 1000.0, 'f', decimals);
}

#define LOWER (!found_key || i.value() < best_value || \
  (i.value() == best_value && i.key() < best_key))

// Tries to select a key that:
// a) has a milliamp value greater than the specified key's
// b) has a text different from the specified key's
// while optimizing for:
// c) the lowest possible milliamp value
// d) the lowest possible key (to break ties)
//
// If it cannot find something satisfying a && b, it means we are near the top
// of the mapping, so instead it selects the key corresponding to the text
// of the maximum value, while optimizing for the same things.
int nice_spin_box::step_up(int key) const
{
  if (mapping.empty()) { return key + 1; }

  int value = mapping.value(key, 0);
  QString value_text = textFromValue(key);

  bool found_key = false;
  int best_key = 0;
  int best_value = 0;
  int max_value = std::numeric_limits<int>::min();
  for (auto i = mapping.constBegin(); i != mapping.constEnd(); ++i)
  {
    if (i.value() > value && value_text != text_from_ma(i.value()) && LOWER)
    {
      found_key = true;
      best_key = i.key();
      best_value = i.value();
    }

    if (i.value() > max_value) { max_value = i.value(); }
  }
  if (found_key) { return best_key; }

  QString max_value_text = text_from_ma(max_value);
  for (auto i = mapping.constBegin(); i != mapping.constEnd(); ++i)
  {
    if (text_from_ma(i.value()) == max_value_text && LOWER)
    {
      found_key = true;
      best_key = i.key();
      best_value = i.value();
    }
  }
  return best_key;
}


int nice_spin_box::step_down(int key) const
{
  return key; // TODO
}

void nice_spin_box::stepBy(int step_value)
{
  code = value();

  while (step_value > 0)
  {
    code = step_up(code);
    step_value--;
  }

  while (step_value < 0)
  {
    code = step_down(code);
    step_value++;
  }

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
  double entered_value = copy.remove(QRegExp("[^(0-9|.)]")).toDouble();

  if (value_in_milli)
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
QString nice_spin_box::textFromValue(int value) const
{
  if (mapping.empty())
  {
    return text_from_ma(value);
  }
  return text_from_ma(mapping.value(value));
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
