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

// double refreshTimer = 25; // used as a constant for both places the refresh rate is used

graph_widget::graph_widget(QWidget * parent)
{
  setup_ui();
  setup_plots();

  connect(max_y, SIGNAL(valueChanged(double)), this, SLOT(change_ranges()));
  connect(min_y, SIGNAL(valueChanged(double)), this, SLOT(change_ranges()));
  connect(domain, SIGNAL(valueChanged(int)), this, SLOT(remove_data_to_scroll()));

  // connect the signal from the dataTime to call realtime_data_slot() using the refresh interval "refreshTimer"
  // set as a constant to be used throughout the class
  // connect(&data_timer, SIGNAL(timeout()), this, SLOT(realtime_data_slot()));
  // data_timer.start(refreshTimer); // Interval 0 means to refresh as fast as possible
}

void graph_widget::setup_ui()
// sets the gui objects in the graph window
{
  this->setObjectName(tr("graph_widget"));

  central_widget = new QWidget(this);
  central_widget->setObjectName(tr("central_widget"));

  pauseRunButton = new QPushButton();
  pauseRunButton->setObjectName("pauseRunButton");
  pauseRunButton->setCheckable(true);
  pauseRunButton->setChecked(false);
  pauseRunButton->setText(tr("&Pause"));

  connect(pauseRunButton, SIGNAL(clicked()), this, SLOT(on_pauseRunButton_clicked()));

  QFont font;
  font.setPointSize(8);

  label1 = new QLabel();
  label1->setFont(font);
  label1->setText(tr("Range (%)"));

  custom_plot = new QCustomPlot();

  label2 = new QLabel();
  label2->setFont(font);
  label2->setText(tr("Time (s)"));

  label3 = new QLabel();
  label3->setFont(font);
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

  input.plot_range = new QDoubleSpinBox();

  target.plot_range = new QDoubleSpinBox();

  feedback.plot_range = new QDoubleSpinBox();

  scaled_feedback.plot_range = new QDoubleSpinBox();

  error.plot_range = new QDoubleSpinBox();

  integral.plot_range = new QDoubleSpinBox();

  duty_cycle_target.plot_range = new QDoubleSpinBox();

  duty_cycle.plot_range = new QDoubleSpinBox();

  current.plot_range = new QDoubleSpinBox();

  input.plot_display = new QCheckBox("Input");
  input.plot_display->setStyleSheet(QStringLiteral("background-color:#00ffff"));

  target.plot_display = new QCheckBox("Target");
  target.plot_display->setStyleSheet(QStringLiteral("background-color:#0000ff"));

  feedback.plot_display = new QCheckBox("Feedback");
  feedback.plot_display->setStyleSheet(QStringLiteral("background-color:#ffc0cb"));

  scaled_feedback.plot_display = new QCheckBox("Scaled Feedback");
  scaled_feedback.plot_display->setStyleSheet(QStringLiteral("background-color:#ff0000"));

  error.plot_display = new QCheckBox("Error");
  error.plot_display->setStyleSheet(QStringLiteral("background-color:#9400d3"));

  integral.plot_display = new QCheckBox("Integral");
  integral.plot_display->setStyleSheet(QStringLiteral("background-color:#ff8c00"));

  duty_cycle_target.plot_display = new QCheckBox("Duty cycle target");
  duty_cycle_target.plot_display->setStyleSheet(QStringLiteral("background-color:#32cd32"));

  duty_cycle.plot_display = new QCheckBox("Duty cycle");
  duty_cycle.plot_display->setStyleSheet(QStringLiteral("background-color:#006400"));

  current.plot_display = new QCheckBox("Current (mA)");
  current.plot_display->setStyleSheet(QStringLiteral("background-color:#b8860b"));

  all_plots.push_front(current);
  all_plots.push_front(duty_cycle);
  all_plots.push_front(duty_cycle_target);
  all_plots.push_front(integral);
  all_plots.push_front(error);
  all_plots.push_front(scaled_feedback);
  all_plots.push_front(feedback);
  all_plots.push_front(target);
  all_plots.push_front(input);

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

void graph_widget::setup_plots()
// sets the range QDoubleSpinBox, display QCheckBox, and the axis of the plot.
// connects the valueChanged signal of the plot_range to change the ranges of the plots.
{
  for(int i=0; i<all_plots.size(); i++)
  {
    all_plots[i].plot_range->setDecimals(0);
    all_plots[i].plot_range->setSingleStep(1.0);
    all_plots[i].plot_range->setRange(-4095,4095);
    all_plots[i].plot_range->setValue(4095);
    all_plots[i].plot_display->setCheckable(true);
    all_plots[i].plot_display->setChecked(true);

    all_plots[i].plot_axis = custom_plot->axisRect(0)->addAxis(QCPAxis::atRight);
    all_plots[i].plot_axis->setRange(-4095,4095);
    all_plots[i].plot_axis->setVisible(false);

    all_plots[i].plot_graph = new QCPGraph(custom_plot->xAxis2,all_plots[i].plot_axis); // yellow line

    all_plots[i].plot_graph->setPen(QPen(plot_colors[i]));

    connect(all_plots[i].plot_range, SIGNAL(valueChanged(double)), this, SLOT(change_ranges()));

    connect(all_plots[i].plot_display, SIGNAL(clicked()), this, SLOT(set_line_visible()));
  }
}

void graph_widget::change_ranges()
{
  for(auto &x: all_plots)
  {
    x.plot_axis->setRangeUpper((x.plot_range->value()) * ((max_y->value())/100));
    x.plot_axis->setRangeLower((x.plot_range->value()) * ((min_y->value())/100));
  }
  custom_plot->yAxis->setRange(min_y->value(), max_y->value());
  custom_plot->replot();
}

void graph_widget::on_pauseRunButton_clicked()
// used the QMetaObject class to parse the function name and send the signal to the function.
{
  pauseRunButton->setText(pauseRunButton->isChecked() ? "R&un" : "&Pause");
  pauseRunButton->isChecked() ? data_timer.stop() : data_timer.start(refreshTimer);
  custom_plot->replot();
}

void graph_widget::set_line_visible()
// sets the visibility of the plot both when the plot is running and when it is paused.
{
  for(auto &x: all_plots)
  {
    x.plot_display->isChecked() ? x.plot_graph->setVisible(true) : x.plot_graph->setVisible(false);
    custom_plot->replot();
  }
}

void graph_widget::remove_data_to_scroll()
// modifies the x-axis based on the domain value and removes data outside of visible range
{
  custom_plot->xAxis->setRange(0, domain->value()*1000);

  // make key axis range scroll with the data at a rate of the time value obtained from QSpinBox
  custom_plot->xAxis2->setRange(key, domain->value(), Qt::AlignRight);

  // these range values are divided by 100 just for the sample data in order to draw sine curve
  custom_plot->replot();
}

void graph_widget::realtime_data_slot()
// plots data on graph
{
  key += (refreshTimer/1000);

  // this list is only used for demonstrations
  QList<double> value = {(sin(key)), (sin(key+1)), (sin(key+2)), (sin(key+3)),
   (sin(key+4)), (sin(key+5)), (sin(key+6)), (sin(key+7)), (static_cast<double> (this->current_value_int))};

  for(int i=0; i<all_plots.size(); i++)
  {
    all_plots[i].plot_graph->addData(key, value[i]);
    all_plots[i].plot_graph->removeDataBefore(key-(domain->value()));
  }

  remove_data_to_scroll();
}

