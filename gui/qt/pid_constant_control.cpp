#include "pid_constant_control.h"

pid_constant_control::pid_constant_control(int index, QWidget * parent)
 : index(index), QGroupBox(parent)
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

void pid_constant_control::set_controller(main_controller * controller)
{
  this->controller = controller;
}

void pid_constant_control::set_multiplier_spinbox(uint16_t value)
{
  if (pid_multiplier_spinbox->value() != value)
  {
    suppress_events = true;
    pid_multiplier_spinbox->setValue(value);
    suppress_events = false;
  }
}

void pid_constant_control::set_exponent_spinbox(uint16_t value)
{
  if (pid_exponent_spinbox->value() != value)
  {
    suppress_events = true;
    pid_exponent_spinbox->setValue(value);
    suppress_events = false;
  }
}

void pid_constant_control::set_constant(double value)
{
  suppress_events = true;
  if (value < 0.0001 && value != 0)
  {
    pid_constant_lineedit->setText(QString::number(value, 'f', 7));
  }
  else
    pid_constant_lineedit->setText(QString::number(value, 'f', 5));
  suppress_events = false;
}

void pid_constant_control::pid_multiplier_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }

  controller->handle_pid_constant_control_multiplier(index, value);
}

void pid_constant_control::pid_exponent_spinbox_valueChanged(int value)
{
  if (suppress_events) { return; }

  controller->handle_pid_constant_control_exponent(index, value);
}

void pid_constant_control::pid_constant_lineedit_textEdited(const QString& text)
{
  if (suppress_events) { return; }

  double value = pid_constant_lineedit->displayText().toDouble();

  controller->handle_pid_constant_control_constant(index, value);
}

void pid_constant_control::pid_constant_lineedit_editingFinished()
{
  if (suppress_events) { return; }

  controller->recompute_constant(index, pid_multiplier_spinbox->value(),
    pid_exponent_spinbox->value());
}
