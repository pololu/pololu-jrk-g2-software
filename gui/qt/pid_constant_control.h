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

private:
  void set_constant();

  QSpinBox * multiplier_spinbox;
  QSpinBox * exponent_spinbox;
  QLineEdit * constant_lineedit;

signals:
  void values_changed(int multiplier, int exponent);

private slots:
  void multiplier_spinbox_valueChanged(int value);
  void exponent_spinbox_valueChanged(int value);
  void constant_lineedit_textEdited(const QString&);
  void constant_lineedit_editingFinished();
};

// QDoubleValidator::validate returns "Intermediate" for numbers that are
// outside the allowed range.  We want to return "Invalid" instead so that
// people cannot type those numbers, even temporarily.
class pid_constant_validator : public QDoubleValidator
{
public:
  pid_constant_validator(double bottom, double top, int decimals,
    QObject * parent)
    : QDoubleValidator(bottom, top, decimals, parent)
  {
    setNotation(QDoubleValidator::StandardNotation);
  }

  QValidator::State validate(QString & s, int & i) const
  {
    QValidator::State state = QDoubleValidator::validate(s, i);

    if (state == QValidator::Intermediate)
    {
      bool ok;
      double d = locale().toDouble(s, &ok);
      if (ok && (d < bottom() || d > top()))
      {
        state = QValidator::Invalid;
      }
    }

    return state;
  }
};
