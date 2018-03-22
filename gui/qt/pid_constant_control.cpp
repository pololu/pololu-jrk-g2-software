#include "pid_constant_control.h"

#include <cmath>

// TODO: better size for line edits, they are way too wide

pid_constant_control::pid_constant_control(QWidget * parent)
 : QGroupBox(parent)
{
  QFont base_font;
  base_font.setPointSize(16);
  base_font.setBold(true);

  QFont equal_font;
  equal_font.setPointSize(12);
  equal_font.setBold(true);

  QLabel * base_label = new QLabel();
  base_label->setObjectName("base_label");
  base_label->setFont(base_font);
  base_label->setLayoutDirection(Qt::LeftToRight);
  base_label->setAlignment(Qt::AlignCenter);
  base_label->setText("2");

  QFrame * division_frame = new QFrame();
  division_frame->setObjectName("division_frame");
  division_frame->setFrameShadow(QFrame::Plain);
  division_frame->setLineWidth(4);
  division_frame->setFrameShape(QFrame::HLine);

  multiplier_spinbox = new QSpinBox();
  multiplier_spinbox->setObjectName("multiplier_spinbox");
  multiplier_spinbox->setAlignment(Qt::AlignCenter);
  multiplier_spinbox->setRange(0, 1023);

  connect(multiplier_spinbox, QOverload<int>::of(&QSpinBox::valueChanged),
    this, &pid_constant_control::multiplier_or_exponent_changed);

  exponent_spinbox = new QSpinBox();
  exponent_spinbox->setObjectName("exponent_spinbox");
  exponent_spinbox->setAlignment(Qt::AlignCenter);
  exponent_spinbox->setRange(0, 18);

  connect(exponent_spinbox, QOverload<int>::of(&QSpinBox::valueChanged),
    this, &pid_constant_control::multiplier_or_exponent_changed);

  QLabel * equal_label = new QLabel();
  equal_label->setObjectName("equal_label");
  equal_label->setText(tr("="));
  equal_label->setFont(equal_font);
  equal_label->setAlignment(Qt::AlignCenter);

  constant_line_edit = new QLineEdit();
  constant_line_edit->setObjectName("constant_line_edit");

  // This prevents the user from entering invalid characters.
  pid_constant_validator * constant_validator =
    new pid_constant_validator(0, 1023, 7, constant_line_edit);
  constant_line_edit->setValidator(constant_validator);

  connect(constant_line_edit, &QLineEdit::textEdited,
    this, &pid_constant_control::constant_text_edited);

  connect(constant_line_edit, &QLineEdit::editingFinished,
    this, &pid_constant_control::constant_editing_finished);

  QGridLayout * group_box_layout = new QGridLayout();
  group_box_layout->addWidget(base_label, 2, 0, 3, 1, Qt::AlignBottom);
  group_box_layout->addWidget(division_frame, 1, 0, 1, 3);
  group_box_layout->addWidget(multiplier_spinbox, 0, 0, 1, 3, Qt::AlignCenter);
  group_box_layout->addWidget(exponent_spinbox, 2, 1, 1, 1, Qt::AlignCenter);
  group_box_layout->addWidget(equal_label, 0, 4, 3, 1, Qt::AlignCenter);
  group_box_layout->addWidget(constant_line_edit, 0, 5, 3, 1, Qt::AlignVCenter);
  group_box_layout->setColumnStretch(6, 1);

  setLayout(group_box_layout);
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  set_constant_from_values();
}

void pid_constant_control::set_values(int multiplier, int exponent)
{
  bool changed = set_values_core(multiplier, exponent);

  // Only call set_constant_from_values if one of the values actually changed.
  // This prevents the constant from being recalculated while user is typing.
  if (changed)
  {
    set_constant_from_values();
  }
}

void pid_constant_control::constant_text_edited()
{
  set_values_from_constant();
}

void pid_constant_control::constant_editing_finished()
{
  set_constant_from_values();
}

void pid_constant_control::multiplier_or_exponent_changed()
{
  if (suppress_events) { return; }

  set_constant_from_values();

  emit values_changed(multiplier_spinbox->value(), exponent_spinbox->value());
}

// Set the constant based on the multiplier and exponent.
void pid_constant_control::set_constant_from_values()
{
  double constant = multiplier_spinbox->value();
  for (int i = 0; i < exponent_spinbox->value(); i++)
  {
    constant /= 2;
  }

  int precision = (constant < 0.0001 && constant != 0) ? 7 : 5;

  constant_line_edit->setText(QString::number(constant, 'f', precision));
}

// Set the multiplier and exponent based on the constant.
void pid_constant_control::set_values_from_constant()
{
  double input = constant_line_edit->text().toDouble();
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

  set_values_core(multiplier, exponent);
}

// Change the spinboxes to display new values, while making sure to emit the
// value_changed signal at most once.
bool pid_constant_control::set_values_core(int multiplier, int exponent)
{
  bool changed = false;
  suppress_events = true;

  if (multiplier != multiplier_spinbox->value())
  {
    multiplier_spinbox->setValue(multiplier);
    changed = true;
  }
  if (exponent != exponent_spinbox->value())
  {
    exponent_spinbox->setValue(exponent);
    changed = true;
  }
  suppress_events = false;

  if (changed)
  {
    emit values_changed(multiplier_spinbox->value(), exponent_spinbox->value());
  }

  return changed;
}
