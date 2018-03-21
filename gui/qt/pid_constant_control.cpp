#include "pid_constant_control.h"

#include <cmath>

// TODO: better size for line edits, they are way too twide

pid_constant_control::pid_constant_control(QWidget * parent)
 : QGroupBox(parent)
{
  QFont font;
  font.setPointSize(16);
  font.setBold(true);

  QFont font1;
  font1.setPointSize(12);
  font1.setBold(true);

  pid_base_label = new QLabel();
  pid_base_label->setObjectName("pid_base_label");
  pid_base_label->setFont(font);
  pid_base_label->setLayoutDirection(Qt::LeftToRight);
  pid_base_label->setAlignment(Qt::AlignCenter);
  pid_base_label->setText("2");

  pid_control_frame = new QFrame();
  pid_control_frame->setObjectName("pid_control_frame");
  pid_control_frame->setFrameShadow(QFrame::Plain);
  pid_control_frame->setLineWidth(4);
  pid_control_frame->setFrameShape(QFrame::HLine);

  pid_multiplier_spinbox = new QSpinBox();
  pid_multiplier_spinbox->setObjectName("pid_multiplier_spinbox");
  pid_multiplier_spinbox->setAlignment(Qt::AlignCenter);
  pid_multiplier_spinbox->setRange(0, 1023);

  connect(pid_multiplier_spinbox, QOverload<int>::of(&QSpinBox::valueChanged),
    this, &pid_multiplier_spinbox_valueChanged);

  pid_exponent_spinbox = new QSpinBox();
  pid_exponent_spinbox->setObjectName("pid_exponent_spinbox");
  pid_exponent_spinbox->setAlignment(Qt::AlignCenter);
  pid_exponent_spinbox->setRange(0, 18);

  connect(pid_exponent_spinbox, QOverload<int>::of(&QSpinBox::valueChanged),
    this, &pid_exponent_spinbox_valueChanged);

  pid_equal_label = new QLabel();
  pid_equal_label->setObjectName("pid_equal_label");
  pid_equal_label->setText(tr("="));

  pid_equal_label->setFont(font1);
  pid_equal_label->setAlignment(Qt::AlignCenter);

  pid_constant_lineedit = new QLineEdit();
  pid_constant_lineedit->setObjectName("pid_constant_lineedit");

  // This prevents the user from entering invalid characters.
  pid_constant_validator *constant_validator =
    new pid_constant_validator(0, 1023, 7, pid_constant_lineedit);
  pid_constant_lineedit->setValidator(constant_validator);

  connect(pid_constant_lineedit, &QLineEdit::textEdited,
    this, &pid_constant_lineedit_textEdited);

  connect(pid_constant_lineedit, &QLineEdit::editingFinished,
    this, &pid_constant_lineedit_editingFinished);

  QGridLayout *group_box_layout = new QGridLayout();
  group_box_layout->addWidget(pid_base_label, 2, 0, 3, 1, Qt::AlignBottom);
  group_box_layout->addWidget(pid_control_frame, 1, 0, 1, 3);
  group_box_layout->addWidget(pid_multiplier_spinbox, 0, 0, 1, 3, Qt::AlignCenter);
  group_box_layout->addWidget(pid_exponent_spinbox, 2, 1, 1, 1, Qt::AlignCenter);
  group_box_layout->addWidget(pid_equal_label, 0, 4, 3, 1, Qt::AlignCenter);
  group_box_layout->addWidget(pid_constant_lineedit, 0, 5, 3, 1, Qt::AlignVCenter);
  group_box_layout->setColumnStretch(6, 1);

  setLayout(group_box_layout);
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void pid_constant_control::set_spinboxes(int multiplier, int exponent)
{
  pid_multiplier_spinbox->setValue(multiplier);
  pid_exponent_spinbox->setValue(exponent);

  // Prevents constant from being recalculated while user is entering a value.
  if (!pid_constant_lineedit->hasFocus())
  {
    set_constant();
  }
}

// Calculates value based on multiplier and exponent values.
void pid_constant_control::set_constant()
{
  double constant = static_cast<double> (pid_multiplier_spinbox->value());
  for (int i = 0; i < pid_exponent_spinbox->value(); i++)
  {
    constant /= 2;
  }

  if (constant < 0.0001 && constant != 0)
  {
    pid_constant_lineedit->setText(QString::number(constant, 'f', 7));
  }
  else
    pid_constant_lineedit->setText(QString::number(constant, 'f', 5));
}

void pid_constant_control::pid_multiplier_spinbox_valueChanged(int value)
{
  emit send_new_values(value, pid_exponent_spinbox->value());
}

void pid_constant_control::pid_exponent_spinbox_valueChanged(int value)
{
  emit send_new_values(pid_multiplier_spinbox->value(), value);
}

// Uses the main_controller to calculate optimal exponent and multiplier
// while user is entering a constant into the QLineEdit.
void pid_constant_control::pid_constant_lineedit_textEdited(const QString& text)
{
  QString copy = text;

  double value = 0;

  value = copy.toDouble();

  double input = value;
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

  set_spinboxes(multiplier, exponent);
}

// Uses the main_controller to calculate the exact constant give the desired
// multiplier and exponent.
void pid_constant_control::pid_constant_lineedit_editingFinished()
{
  set_constant();
}
