#pragma once

#include <QDoubleSpinBox>
#include <QMapIterator>

#include "main_controller.h"


class nice_spin_box : public QDoubleSpinBox
{
  Q_OBJECT

public:
  nice_spin_box(QWidget* parent = Q_NULLPTR);

  void set_possible_values(uint16_t value);

  QMultiMap<int, double> mapping;

signals:
  void send_code(uint16_t index);

private slots:
  void editing_finished(double entered_value);
  void set_display_value();

private:
  QMultiMap<int, double>::iterator i;

  bool suppress_events = false;
  double entered_value = -1;

protected:
  // Reimplemented QDoubleSpinBox functions
  virtual void stepBy(int step_value);
  virtual StepEnabled stepEnabled();
  double valueFromText(const QString& text) const;
  QString textFromValue(double val) const;
  QValidator::State validate(QString& input, int& pos) const;
};

