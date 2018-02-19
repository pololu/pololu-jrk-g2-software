#pragma once

#include <QObject>
#include <QValidator>
#include <QGroupBox>
#include <QLineEdit>
#include <QLabel>
#include <QSpinBox>
#include <QApplication>
#include <QGridLayout>
#include "main_controller.h"

class pid_constant_control : public QObject
{
  Q_OBJECT

public:
  pid_constant_control(int index, QGroupBox * groupbox);

  void set_controller(main_controller * controller = NULL);

  void set_multiplier_spinbox(uint16_t value);
  void set_exponent_spinbox(uint16_t value);
  void set_constant(double value);

private:
  main_controller * controller;

  QFrame *pid_control_frame;
  QFrame *pid_proportion_frame;
  QLineEdit *pid_constant_lineedit;
  QLabel *pid_equal_label;
  QSpinBox *pid_multiplier_spinbox;
  QLabel *pid_base_label;
  QSpinBox *pid_exponent_spinbox;

  bool suppress_events = false;

  int index;

private slots:
  void pid_multiplier_spinbox_valueChanged(int value);
  void pid_exponent_spinbox_valueChanged(int value);
  void pid_constant_lineedit_textEdited(const QString&);
  void pid_constant_lineedit_editingFinished();
};

class pid_constant_validator : public QDoubleValidator
{
public:
  pid_constant_validator(double bottom, double top, int decimals,
    QObject * parent)
    : QDoubleValidator(bottom, top, decimals, parent)
  {}

  QValidator::State validate(QString &s, int &i) const
  {
    if (s.isEmpty())
    {
      return QValidator::Intermediate;
    }

    if (s == "-")
    {
      return QValidator::Invalid;
    }

    QChar decimalPoint = locale().decimalPoint();

    if(s.indexOf(decimalPoint) != -1)
    {
      int charsAfterPoint = s.length() - s.indexOf(decimalPoint) - 1;

      if (charsAfterPoint > decimals())
      {
        return QValidator::Invalid;
      }
    }

    bool ok;
    double d = locale().toDouble(s, &ok);
    if (ok && d >= bottom() && d <= top())
    {
      return QValidator::Acceptable;
    }
    else
    {
      return QValidator::Invalid;
    }
  }
};
