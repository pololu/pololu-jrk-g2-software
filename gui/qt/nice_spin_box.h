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

  QMap<int, int> mapping;

  // TODO: do we really need this?
  int code = -1;

  int decimals = 0;

public:
  nice_spin_box(QWidget * parent = NULL);

  // Sets the mapping from encoded current limits which get returned by value(),
  // to actual current limits (in milliamps) which are displayed to the user.
  // If you pass an empty map or don't call this function at all, then value()
  // uses milliamps, and and you should call setRange to set the allowed range.
  void set_mapping(const QMap<int, int> &);

  // Sets the number of digits to show after the decimal point.
  // Must be between 0 and 3.
  void set_decimals(int decimals) { this->decimals = decimals; }

private slots:
  void set_code_from_value(int value);

private:
  QString text_from_ma(int milliamps) const;
  int canonical_key_for_text(const QString & text) const;
  int step_up(int code) const;
  int step_down(int code) const;

protected:
  virtual void stepBy(int step_value);
  virtual StepEnabled stepEnabled();
  int valueFromText(const QString& text) const;
  QString textFromValue(int val) const;
  QValidator::State validate(QString& input, int& pos) const;
};

