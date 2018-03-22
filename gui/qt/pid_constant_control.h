#pragma once

#include <QObject>
#include <QValidator>
#include <QGroupBox>
#include <QLineEdit>
#include <QLabel>
#include <QSpinBox>
#include <QApplication>
#include <QGridLayout>

class pid_constant_control : public QGroupBox
{
  Q_OBJECT

public:
  pid_constant_control(QWidget * parent = Q_NULLPTR);

  void set_values(int multiplier, int exponent);

private slots:
  void constant_text_edited();
  void constant_editing_finished();
  void multiplier_or_exponent_changed();

private:
  bool suppress_events = false;
  QSpinBox * multiplier_spinbox;
  QSpinBox * exponent_spinbox;
  QLineEdit * constant_line_edit;

  void set_constant_from_values();
  void set_values_from_constant();
  bool set_values_core(int multiplier, int exponent);

signals:
  void values_changed(int multiplier, int exponent);
};

class pid_constant_validator : public QDoubleValidator
{
public:
  pid_constant_validator(double bottom, double top, int decimals,
    QObject * parent)
    : QDoubleValidator(bottom, top, decimals, parent)
  {
    setNotation(QDoubleValidator::StandardNotation);
  }

  // QDoubleValidator::validate returns "Intermediate" for numbers that are
  // outside the allowed range.  We want to return "Invalid" instead so that
  // people cannot type those numbers, even temporarily.
  QValidator::State validate(QString & input, int & pos) const
  {
    QValidator::State state = QDoubleValidator::validate(input, pos);

    if (state == QValidator::Intermediate)
    {
      bool ok;
      double d = locale().toDouble(input, &ok);
      if (ok && (d < bottom() || d > top()))
      {
        state = QValidator::Invalid;
      }
    }

    return state;
  }

  // QDoubleValidator::fixup does nothing.  If we don't override it, then the
  // user can change focus out of the lineedit while the lineedit is empty, and
  // the editingFinished signal does not fire, so our code does not fix the
  // lineedit to display a value.
  //
  // Because of the validator above, I think the only intermediate states we
  // could be in are "." and "".
  void fixup(QString & input) const
  {
    input = "0";
  }
};
