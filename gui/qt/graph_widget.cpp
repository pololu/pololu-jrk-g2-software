#include "graph_widget.h"
#include "main_window.h"
#include "qcustomplot.h"

#include <QApplication>
#include <QCheckBox>
#include <QCloseEvent>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QSpinBox>
#include <QVector>
#include <QWidget>

#include <array>
#include <ctime>


graph_widget::graph_widget(QWidget * parent)
{
  setup_ui();

  connect(max_y, SIGNAL(valueChanged(double)),
    this, SLOT(change_ranges()));

  connect(min_y, SIGNAL(valueChanged(double)),
    this, SLOT(change_ranges()));

  connect(domain, SIGNAL(valueChanged(int)),
    this, SLOT(remove_data_to_scroll()));
}

void graph_widget::setup_ui()
// sets the gui objects in the graph window
{
  pause_run_button = new QPushButton(this);
  pause_run_button->setObjectName("pause_run_button");
  pause_run_button->setCheckable(true);
  pause_run_button->setChecked(false);
  pause_run_button->setText(tr("&Pause"));

  label1 = new QLabel();
  label1->setText(tr("Range (%):"));

  custom_plot = new QCustomPlot();

  label2 = new QLabel();
  label2->setText(tr("Time (s):"));

  label3 = new QLabel();
  label3->setAlignment(Qt::AlignCenter);
  label3->setText(tr("-"));

  min_y = new QDoubleSpinBox();
  min_y->setRange(-100,0);
  min_y->setDecimals(0);
  min_y->setSingleStep(1.0);
  min_y->setValue(-100);

  max_y = new QDoubleSpinBox();
  max_y->setRange(0,100);
  max_y->setDecimals(0);
  max_y->setSingleStep(1.0);
  max_y->setValue(100);

  domain = new QSpinBox();
  domain->setValue(10); // initialized the graph to show 10 seconds of data

  setup_plot(input, "Input", "#00ffff", "0-", 4095);

  setup_plot(target, "Target", "#0000ff", "0-", 4095, true);

  setup_plot(feedback, "Feedback", "#ffc0cb", "0-", 4095);

  setup_plot(scaled_feedback, "Scaled feedback", "#ff0000", "0-", 4095, true);

  setup_plot(error, "Error", "#9400d3", "\u00B1", 4095);

  setup_plot(integral, "Integral", "#ff8c00", "\u00B1", 1000);

  setup_plot(duty_cycle_target, "Duty cycle target", "#32cd32", "\u00B1", 600);

  setup_plot(duty_cycle, "Duty cycle", "#006400", "\u00B1", 600);

  setup_plot(raw_current, "Raw current", "#b8860b", "0\u2013", 4095);

  setup_plot(current, "Current (mA)", "#0000ff", "\u00B1", 100000);

  setup_plot(current_chopping_log, "Current chopping log", "#ff00ff", "0\u2013", 1);

  custom_plot->xAxis->QCPAxis::setRangeReversed(true);
  custom_plot->yAxis->setRange(-100,100);
  custom_plot->xAxis->setTickLabelType(QCPAxis::ltNumber);
  custom_plot->xAxis->setAutoTickStep(false);
  custom_plot->xAxis->setTickStep(1000);

  // this is used to see the x-axis to see accurate time.
  // custom_plot->xAxis2->setVisible(true);
  // custom_plot->xAxis2->setTickLabelType(QCPAxis::ltNumber);
  // custom_plot->xAxis2->setAutoTickStep(true);
  // custom_plot->xAxis2->setTickStep(2);

  QMetaObject::connectSlotsByName(this);
}

void graph_widget::setup_plot(plot& x, QString display_text, QString color,
  QString range_label, double range, bool default_check)
{
  x.color = color;
  x.range = new QDoubleSpinBox();
  x.display = new QCheckBox();
  x.display->setText(display_text);
  x.display->setStyleSheet("border: 5px solid "+ color + ";"
    "background-color: white;");
  x.default_check = default_check;
  x.range_label = new QLabel();
  x.range_label->setText(range_label);
  x.graph_data_selection_bar = new QHBoxLayout();
  x.range_value = range;

  all_plots.append(&x);

  x.range->setDecimals(0);
  x.range->setSingleStep(1.0);

  x.axis = custom_plot->axisRect(0)->addAxis(QCPAxis::atRight);

  x.axis->setVisible(false);

  // x.range_label->setStyleSheet(QStringLiteral("font: 14px"));

  x.axis->setRange(-x.range_value, x.range_value);

  x.range->setRange(0, x.range_value);
  x.range->setValue(x.range_value);
  // Set the size of the labels and buttons for the errors tab in
  // a way that can change from OS to OS.
  {
    QLabel tmp_label2;
    tmp_label2.setText("xxxxxxxxxxxxx");
    QLabel tmp_label3;
    tmp_label3.setText("xxxxxxxxxxxxxxxxxxxxxxxxxx");
    x.range->setFixedWidth(tmp_label2.sizeHint().width());
    x.display->setFixedWidth(tmp_label3.sizeHint().width());
  }

  x.display->setCheckable(true);
  x.display->setChecked(true);

  x.graph_data_selection_bar->setMargin(0);;
  x.graph_data_selection_bar->addWidget(x.display);
  x.graph_data_selection_bar->addWidget(x.range_label);
  x.graph_data_selection_bar->addWidget(x.range);

  x.graph = new QCPGraph(custom_plot->xAxis2,x.axis);

  x.graph->setPen(QPen(x.color));

  connect(x.range, SIGNAL(valueChanged(double)),
    this, SLOT(change_ranges()));

  connect(x.display, SIGNAL(clicked()), this, SLOT(set_line_visible()));
}

void graph_widget::change_ranges()
{
  for (auto plot : all_plots)
  {
    plot->axis->setRangeUpper((plot->range->value()) * ((max_y->value())/100));
    plot->axis->setRangeLower((plot->range->value()) * ((min_y->value())/100));
  }
  custom_plot->yAxis->setRange(min_y->value(), max_y->value());
  custom_plot->replot();
}

void graph_widget::on_pause_run_button_clicked()
{
  pause_run_button->setText(pause_run_button->isChecked() ? "R&un" : "&Pause");
  custom_plot->replot();
}

void graph_widget::set_line_visible()
{
  for (auto plot : all_plots)
  {
    plot->graph->setVisible(plot->display->isChecked());
    custom_plot->replot();
  }
}

void graph_widget::remove_data_to_scroll()
// modifies the x-axis based on the domain value
// and removes data outside of visible range
{
  custom_plot->xAxis->setRange(0, domain->value()*1000);

  custom_plot->xAxis2->setRange(key, domain->value(), Qt::AlignRight);

  custom_plot->replot();
}

// plots data on graph (TODO: better function name)
void graph_widget::realtime_data_slot()
{
  key += (refreshTimer/1000);

  for (auto plot : all_plots)
  {
    plot->graph->addData(key, plot->plot_value);
    plot->graph->removeDataBefore(key - domain->value());
  }

  remove_data_to_scroll();
}
