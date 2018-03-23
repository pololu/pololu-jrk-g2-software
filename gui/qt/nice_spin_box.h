#pragma once

#include <QSpinBox>

class nice_spin_box : public QSpinBox
{
  Q_OBJECT

public:
  // The "display_in_milli" variable is so the user can set the QSpinBox to
  // display milli-units and use it with the same functionality.
  nice_spin_box(bool display_in_milli = false, QWidget* parent = Q_NULLPTR);

  void set_mapping(QMultiMap<int, int>&);

private slots:
  void set_code_from_value(int value);

private:
  QMultiMap<int, int> mapping;
  int code = -1;
  bool display_in_milli;

protected:
  // Reimplemented QSpinBox functions
  virtual void stepBy(int step_value);
  virtual StepEnabled stepEnabled();
  int valueFromText(const QString& text) const;
  QString textFromValue(int val) const;
  QValidator::State validate(QString& input, int& pos) const;
};

