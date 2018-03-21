#include "pid_constant_control.h"

#include <cmath>

// TODO: better size for line edits, they are way too twide

// Note: This control sometimes sends the values_changed signal twice for one
// keystroke.  For example, when the box says "2.2" and you type a "5" at the
// end.  The first time the signal is sent, only one of the values will be
// correct and the other will be old.  This is not ideal but does not cause any
// problems in our application.

pid_constant_control::pid_constant_control(QWidget * parent)
 : QGroupBox(parent)
{
  QFont font;
  font.setPointSize(16);
  font.setBold(true);

  QFont font1;
  font1.setPointSize(12);
  font1.setBold(true);

  QLabel * base_label = new QLabel();
  base_label->setObjectName("base_label");
  base_label->setFont(font);
  base_label->setLayoutDirection(Qt::LeftToRight);
  base_label->setAlignment(Qt::AlignCenter);
  base_label->setText("2");

  QFrame * division_frame = new QFrame();
  division_frame->setObjectName("pid_control_frame");
  division_frame->setFrameShadow(QFrame::Plain);
  division_frame->setLineWidth(4);
  division_frame->setFrameShape(QFrame::HLine);

  multiplier_spinbox = new QSpinBox();
  multiplier_spinbox->setObjectName("multiplier_spinbox");
  multiplier_spinbox->setAlignment(Qt::AlignCenter);
  multiplier_spinbox->setRange(0, 1023);

  connect(multiplier_spinbox, QOverload<int>::of(&QSpinBox::valueChanged),
    this, &multiplier_spinbox_valueChanged);

  exponent_spinbox = new QSpinBox();
  exponent_spinbox->setObjectName("pid_exponent_spinbox");
  exponent_spinbox->setAlignment(Qt::AlignCenter);
  exponent_spinbox->setRange(0, 18);

  connect(exponent_spinbox, QOverload<int>::of(&QSpinBox::valueChanged),
    this, &exponent_spinbox_valueChanged);

  QLabel * equal_label = new QLabel();
  equal_label->setObjectName("pid_equal_label");
  equal_label->setText(tr("="));
  equal_label->setFont(font1);
  equal_label->setAlignment(Qt::AlignCenter);

  constant_lineedit = new QLineEdit();
  constant_lineedit->setObjectName("pid_constant_lineedit");

  // This prevents the user from entering invalid characters.
  pid_constant_validator * constant_validator =
    new pid_constant_validator(0, 1023, 7, constant_lineedit);
  constant_lineedit->setValidator(constant_validator);

  connect(constant_lineedit, &QLineEdit::textEdited,
    this, &pid_constant_control::constant_lineedit_textEdited);

  connect(constant_lineedit, &QLineEdit::editingFinished,
    this, &pid_constant_control::constant_lineedit_editingFinished);

  QGridLayout * group_box_layout = new QGridLayout();
  group_box_layout->addWidget(base_label, 2, 0, 3, 1, Qt::AlignBottom);
  group_box_layout->addWidget(division_frame, 1, 0, 1, 3);
  group_box_layout->addWidget(multiplier_spinbox, 0, 0, 1, 3, Qt::AlignCenter);
  group_box_layout->addWidget(exponent_spinbox, 2, 1, 1, 1, Qt::AlignCenter);
  group_box_layout->addWidget(equal_label, 0, 4, 3, 1, Qt::AlignCenter);
  group_box_layout->addWidget(constant_lineedit, 0, 5, 3, 1, Qt::AlignVCenter);
  group_box_layout->setColumnStretch(6, 1);

  setLayout(group_box_layout);
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void pid_constant_control::set_values(int multiplier, int exponent)
{
  multiplier_spinbox->setValue(multiplier);
  exponent_spinbox->setValue(exponent);

  // Prevents constant from being recalculated while user is entering a value.
  if (!constant_lineedit->hasFocus())
  {
    set_constant();
  }
}

// Calculates value based on multiplier and exponent values.
void pid_constant_control::set_constant()
{
  double constant = multiplier_spinbox->value();
  for (int i = 0; i < exponent_spinbox->value(); i++)
  {
    constant /= 2;
  }

  if (constant < 0.0001 && constant != 0)
  {
    constant_lineedit->setText(QString::number(constant, 'f', 7));
  }
  else
  {
    constant_lineedit->setText(QString::number(constant, 'f', 5));
  }
}

void pid_constant_control::multiplier_spinbox_valueChanged(int value)
{
  emit values_changed(value, exponent_spinbox->value());
}

void pid_constant_control::exponent_spinbox_valueChanged(int value)
{
  emit values_changed(multiplier_spinbox->value(), value);
}

// Uses the main_controller to calculate optimal exponent and multiplier
// while user is entering a constant into the QLineEdit.
void pid_constant_control::constant_lineedit_textEdited(const QString & text)
{
  double input = text.toDouble();
  int i;
  int largest_divisor = 1;
  for (i = 0; i < 18; i++)
  {
    largest_divisor *= 2;
    if (std::rint(largest_divisor * input) > 1023)
    {
      largest_divisor /= 2;
      break;
    }
  }
  int multiplier = std::rint(largest_divisor * input);
  int exponent = i;

  while (multiplier % 2 == 0 && exponent != 0)
  {
    multiplier /= 2;
    exponent -= 1;
  }

  set_values(multiplier, exponent);
}

// Uses the main_controller to calculate the exact constant give the desired
// multiplier and exponent.
void pid_constant_control::constant_lineedit_editingFinished()
{
  set_constant();
}
