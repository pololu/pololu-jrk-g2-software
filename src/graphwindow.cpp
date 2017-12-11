#include "graphwindow.h"
#include "mainwindow.h"
#include "qcustomplot.h"
#include <ctime>
#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include <QMainWindow>
#include <QSerialPort>
#include <QVector>
#include <array>
#include <QtCore>
#include <QCloseEvent>


double refreshTimer = 25; // used as a constant for both places the refresh rate is used

graph_window::graph_window(QWidget * parent)
  : QMainWindow(parent)
{
  setup_ui(this); 
  setup_plots();
  
  all_plots[0].plot_graph->setPen(QPen(Qt::cyan));
  all_plots[1].plot_graph->setPen(QPen(Qt::blue));
  all_plots[2].plot_graph->setPen(QPen("#ffc0cb"));
  all_plots[3].plot_graph->setPen(QPen(Qt::red));

  connect(max_y, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, change_ranges);

  connect(min_y, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, change_ranges);

  connect(domain, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, remove_data_to_scroll);

  // connect the signal from the dataTime to call realtime_data_slot() using the refresh interval "refreshTimer"
  // set as a constant to be used throughout the class
  connect(&data_timer, SIGNAL(timeout()), this, SLOT(realtime_data_slot()));
  data_timer.start(refreshTimer); // Interval 0 means to refresh as fast as possible
}

graph_window::~graph_window()
{
}

void graph_window::setup_ui(QMainWindow *graph_window)
// sets the gui objects in the graph window
{
  graph_window->setObjectName(tr("graph_window"));
  // graph_window->setMinimumSize(818,547);

  central_widget = new QWidget(graph_window);
  central_widget->setObjectName(tr("central_widget"));
  // central_widget->setMinimumSize(802,508);

  pauseRunButton = new QPushButton();
  pauseRunButton->setObjectName(tr("pauseRunButton"));
  // pauseRunButton->setMinimumSize(82, 22);
  pauseRunButton->setCheckable(true);
  pauseRunButton->setChecked(false);
  pauseRunButton->setText(tr("&Pause"));
  
  connect(pauseRunButton, SIGNAL(clicked()), this, SLOT(on_pauseRunButton_clicked()));
  
  QFont font;
  font.setPointSize(8);

  label1 = new QLabel();
  // label1->setMinimumSize(59,13);
  label1->setFont(font);
  label1->setText(tr("Range (%)"));
  
  custom_plot = new QCustomPlot();

  label2 = new QLabel();
  // label2->setMinimumSize(47,13);
  label2->setFont(font);
  label2->setText(tr("Time (s)"));

  label3 = new QLabel();
  // label3->setMinimumSize(0,0);
  label3->setFont(font);
  label3->setAlignment(Qt::AlignCenter);
  label3->setText(tr("-"));
  
  min_y = new QDoubleSpinBox();
  // min_y->setMinimumSize(60,20);
  min_y->setRange(-100,0);
  min_y->setDecimals(0);
  min_y->setSingleStep(1.0);
  min_y->setValue(-100);
  
  max_y = new QDoubleSpinBox();
  // max_y->setMinimumSize(60,20);
  max_y->setRange(0,100);
  max_y->setDecimals(0);
  max_y->setSingleStep(1.0);
  max_y->setValue(100);
  
  domain = new QSpinBox();
  // domain->setMinimumSize(42,20);
  domain->setValue(10); // initialized the graph to show 10 seconds of data

  input.plot_range = new QDoubleSpinBox();
  // input.plot_range->setMinimumSize(63,20);
  
  target.plot_range = new QDoubleSpinBox();
  // target.plot_range->setMinimumSize(63,20);

  feedback.plot_range = new QDoubleSpinBox();
  // feedback.plot_range->setMinimumSize(63,20);

  scaled_feedback.plot_range = new QDoubleSpinBox();
  // scaled_feedback.plot_range->setMinimumSize(63,20);

  input.plot_display = new QCheckBox("Input");
  // input.plot_display->setMinimumSize(87,17);
  input.plot_display->setStyleSheet(QStringLiteral("background-color:cyan"));

  target.plot_display = new QCheckBox("Target");
  // target.plot_display->setMinimumSize(87,17);
  target.plot_display->setStyleSheet(QStringLiteral("background-color:blue"));

  feedback.plot_display = new QCheckBox("Feedback");
  // feedback.plot_display->setMinimumSize(87,17);
  feedback.plot_display->setStyleSheet(QStringLiteral("background-color:#ffc0cb"));

  scaled_feedback.plot_display = new QCheckBox("Scaled Feedback");
  // scaled_feedback.plot_display->setMinimumSize(87,17);
  scaled_feedback.plot_display->setStyleSheet(QStringLiteral("background-color:red"));

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

  QMetaObject::connectSlotsByName(graph_window);
}

void graph_window::setup_plots()
// sets the range QDoubleSpinBox, display QCheckBox, and the axis of the plot. 
// connects the valueChanged signal of the plot_range to change the ranges of the plots.
{
  for(auto &x: all_plots)
  {
    x.plot_range->setDecimals(1);
    x.plot_range->setSingleStep(0.1);
    x.plot_range->setRange(0,1);
    x.plot_range->setValue(1);
    x.plot_display->setCheckable(true);
    x.plot_display->setChecked(true);    

    x.plot_axis = custom_plot->axisRect(0)->addAxis(QCPAxis::atRight);
    x.plot_axis->setRange(-1,1);
    x.plot_axis->setVisible(false);

    x.plot_graph = new QCPGraph(custom_plot->xAxis2,x.plot_axis); // yellow line

    connect(x.plot_range, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, change_ranges);

    connect(x.plot_display, SIGNAL(clicked()), this, SLOT(set_line_visible()));
  }
}

void graph_window::change_ranges()
{
  for(auto &x: all_plots)
  {
    x.plot_axis->setRangeUpper((x.plot_range->value()) * ((max_y->value())/100));
    x.plot_axis->setRangeLower((x.plot_range->value()) * ((min_y->value())/100));
  }
  custom_plot->yAxis->setRange(min_y->value(), max_y->value());
  custom_plot->replot();
}

void graph_window::on_pauseRunButton_clicked()
// used the QMetaObject class to parse the function name and send the signal to the function.
{
  pauseRunButton->setText(pauseRunButton->isChecked() ? "R&un" : "&Pause");
  pauseRunButton->isChecked() ? data_timer.stop() : data_timer.start(refreshTimer);
  custom_plot->replot();
}

void graph_window::set_line_visible()
// sets the visibility of the plot both when the plot is running and when it is paused.
{
  for(auto &x: all_plots)
  {
    x.plot_display->isChecked() ? x.plot_graph->setVisible(true) : x.plot_graph->setVisible(false);
    custom_plot->replot();
  }    
}

void graph_window::remove_data_to_scroll()
// modifies the x-axis based on the domain value and removes data outside of visible range
{ 
  custom_plot->xAxis->setRange(0, domain->value()*1000);

  // make key axis range scroll with the data at a rate of the time value obtained from QSpinBox
  custom_plot->xAxis2->setRange(key, domain->value(), Qt::AlignRight);
  
  // these range values are divided by 100 just for the sample data in order to draw sine curve
  custom_plot->replot();
}

void graph_window::realtime_data_slot()
// plots data on graph
{
  key += (refreshTimer/1000);
  
  // this list is only used for demonstrations
  QList<double> value = {(sin(key)), (sin(key+1)), (sin(key+2)), (sin(key+3))};
  
  for(int i=0; i<all_plots.size(); i++)
  {
    all_plots[i].plot_graph->addData(key, value[i]);
    all_plots[i].plot_graph->removeDataBefore(key-(domain->value()));
  }
  
  remove_data_to_scroll();
}

