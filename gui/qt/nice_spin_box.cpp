#include "nice_spin_box.h"
#include "main_controller.h"

#include <QLineEdit>
#include <QSpinBox>
#include <QtMath>
#include <QRegExpValidator>
#include <QDebug>
#include <iostream> //tmphax

nice_spin_box::nice_spin_box(QWidget* parent)
  : QDoubleSpinBox(parent)
{
  connect(this, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &editing_finished);

  setKeyboardTracking(false);
  setDecimals(3);
}

void nice_spin_box::editing_finished(double entered_value)
{
  if (suppress_events) { return; }
    for (int j = 0; j < mapping.size(); ++j)
    {
      if (entered_value >= mapping.values().at(j))
      {
        int new_key = mapping.keys().at(j);
        i = mapping.find(new_key);
      }
    }

  emit send_code(i.key());
}

void nice_spin_box::set_display_value()
{
  suppress_events = true;
  setValue(i.value());
  suppress_events = false;
}

void nice_spin_box::set_possible_values(uint16_t value)
{
  setRange(mapping.first(), mapping.last());

  i = mapping.find(value);

  set_display_value();
}

void nice_spin_box::stepBy(int step_value)
{
  i += step_value;

  setValue(i.value());
}

QDoubleSpinBox::StepEnabled nice_spin_box::stepEnabled()
{
  return  StepUpEnabled | StepDownEnabled;
}

double nice_spin_box::valueFromText(const QString& text) const
{
  QString copy = text.toUpper();
  if (copy.contains("M"))
  {
    copy.remove(QRegExp("(M|MA)"));
    double temp_num = copy.toDouble();
    temp_num /=1000;
    return temp_num;
  }
  return copy.toDouble();
}

QString nice_spin_box::textFromValue(double val) const
{
  return QString::number(val, 'f', 2);
}

QValidator::State nice_spin_box::validate(QString& input, int& pos) const
{
  QRegExp r = QRegExp("(\\d{0,6})(\\.\\d{0,4})?(\\s*)(m|ma|Ma|mA|MA)?");

  if (r.exactMatch(input))
  {
    return QValidator::Acceptable;
  }
  else
  {
    return QValidator::Invalid;
  }
}
