#pragma once

#include <QSpinBox>

// This class if used for QSpinBox in which the value is set based on
// a value that is not the same as the display value.
// ex. value passed into the QSpinBox is 3 but display value is 15.
// Although this class is made with QSpinBox the values can be displayed
// with decimals and digits following the decimal.
class nice_spin_box : public QSpinBox
{
  Q_OBJECT

public:
  // The "display_in_milli" variable is so the user can set the QSpinBox to
  // display milli-units and use it with the same functionality.
  nice_spin_box(bool display_in_milli = false, QWidget* parent = Q_NULLPTR);

  void set_mapping(QMap<int, int>&);
  void set_decimals(int m_decimals = 0) { decimals = m_decimals; }

private slots:
  void set_code_from_value(int value);

private:
  // "mapping" is used within the class to determine the display value
  // based on the value set in the QSpinBox.
  QMap<int, int> mapping;
  int code = -1;
  bool display_in_milli;
  int decimals = 0;

protected:
  // Reimplemented QSpinBox functions
  virtual void stepBy(int step_value);
  virtual StepEnabled stepEnabled();
  int valueFromText(const QString& text) const;
  QString textFromValue(int val) const;
  QValidator::State validate(QString& input, int& pos) const;
};

