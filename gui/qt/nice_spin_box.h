#pragma once

#include <QDoubleSpinBox>

class nice_spin_box : public QDoubleSpinBox
{
  Q_OBJECT

public:
  nice_spin_box(QWidget* parent = Q_NULLPTR);

  void set_mapping(QMultiMap<int, int>&, uint16_t value);

signals:
  void send_code(int value);

private slots:
  void set_value_from_code();
  void set_code_from_value();

private:
  QMultiMap<int, int> mapping;
  QString set_suffix;
  int code = -1;

protected:
  // Reimplemented QSpinBox functions
  virtual void stepBy(int step_value);
  virtual StepEnabled stepEnabled();
  double valueFromText(const QString& text) const;
  QString textFromValue(double val) const;
  QValidator::State validate(QString& input, int& pos) const;
};

