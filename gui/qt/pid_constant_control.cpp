#include "pid_constant_control.h"

pid_constant_control::pid_constant_control(int index, QGroupBox * groupbox,
  QObject * parent) : index(index), QObject(parent)
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
  pid_base_label->setText(tr("2"));

  pid_control_frame = new QFrame();
  pid_control_frame->setObjectName("pid_control_frame");
  pid_control_frame->setFrameShadow(QFrame::Plain);
  pid_control_frame->setLineWidth(4);
  pid_control_frame->setFrameShape(QFrame::HLine);

  pid_multiplier_spinbox = new QSpinBox();
  pid_multiplier_spinbox->setObjectName("pid_multiplier_spinbox");
  pid_multiplier_spinbox->setAlignment(Qt::AlignCenter);
  pid_multiplier_spinbox->setRange(0, 1023);

  connect(pid_multiplier_spinbox, SIGNAL(valueChanged(int)), this,
    SLOT(pid_multiplier_spinbox_valueChanged(int)));

  pid_exponent_spinbox = new QSpinBox();
  pid_exponent_spinbox->setObjectName("pid_exponent_spinbox");
  pid_exponent_spinbox->setAlignment(Qt::AlignCenter);
  pid_exponent_spinbox->setRange(0, 18);

  connect(pid_exponent_spinbox, SIGNAL(valueChanged(int)), this,
    SLOT(pid_exponent_spinbox_valueChanged(int)));

  pid_equal_label = new QLabel();
  pid_equal_label->setObjectName("pid_equal_label");
  pid_equal_label->setText(tr("="));

  pid_equal_label->setFont(font1);
  pid_equal_label->setAlignment(Qt::AlignCenter);

  pid_constant_lineedit = new QLineEdit();
  pid_constant_lineedit->setObjectName("pid_constant_lineedit");

  QFontMetrics metrics(QApplication::font());
  pid_constant_lineedit->setFixedWidth(metrics.width("0000.00000000"));

  pid_constant_validator *constant_validator =
    new pid_constant_validator(0, 1023, 7, pid_constant_lineedit);
  pid_constant_lineedit->setValidator(constant_validator);

  connect(pid_constant_lineedit, SIGNAL(textEdited(const QString&)), this,
    SLOT(pid_constant_lineedit_textEdited(const QString&)));

  connect(pid_constant_lineedit, SIGNAL(editingFinished()),
    this, SLOT(pid_constant_lineedit_editingFinished()));

  QGridLayout *group_box_layout = new QGridLayout();
  group_box_layout->addWidget(pid_base_label,3,1,3,2);
  group_box_layout->addWidget(pid_control_frame,2,1,1,5);
  group_box_layout->addWidget(pid_multiplier_spinbox,1,2,1,3);
  group_box_layout->addWidget(pid_exponent_spinbox,3,3,1,3);
  group_box_layout->addWidget(pid_equal_label,2,7,1,2);
  group_box_layout->addWidget(pid_constant_lineedit,1,9,3,1,Qt::AlignCenter);

  groupbox->setLayout(group_box_layout);
}

void pid_constant_control::pid_multiplier_spinbox_valueChanged(int value)
{
  controller->handle_pid_constant_control_multiplier(index, value);
}

void pid_constant_control::pid_exponent_spinbox_valueChanged(int value)
{
  controller->handle_pid_constant_control_exponent(index, value);
}

void pid_constant_control::pid_constant_lineedit_textEdited(const QString& text)
{
  double value = pid_constant_lineedit->displayText().toDouble();

  controller->handle_pid_constant_control_constant(index, value);
}

void pid_constant_control::pid_constant_lineedit_editingFinished()
{
  controller->recompute_constant(index, pid_multiplier_spinbox->value(),
    pid_exponent_spinbox->value());
}
