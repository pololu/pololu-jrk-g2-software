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

GraphWindow::GraphWindow(QWidget * parent)
    : QMainWindow(parent)
{
    setupUi(this); 
    setupPlots();
    
    allPlots[0].plotGraph->setPen(QPen(Qt::cyan));
    allPlots[1].plotGraph->setPen(QPen(Qt::blue));
    allPlots[2].plotGraph->setPen(QPen("#ffc0cb"));
    allPlots[3].plotGraph->setPen(QPen(Qt::red));

    connect(maxY, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, changeRanges);

    connect(minY, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, changeRanges);

    connect(domain, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, remove_data_to_scroll);

    // connect the signal from the dataTime to call realtimeDataSlot() using the refresh interval "refreshTimer"
    // set as a constant to be used throughout the class
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    dataTimer.start(refreshTimer); // Interval 0 means to refresh as fast as possible
}

GraphWindow::~GraphWindow()
{
}

void GraphWindow::setupUi(QMainWindow *GraphWindow)
// sets the gui objects in the graph window
{
    GraphWindow->setObjectName(tr("GraphWindow"));
    GraphWindow->setMinimumSize(818,547);

    centralWidget = new QWidget(GraphWindow);
    centralWidget->setObjectName(tr("centralWidget"));
    // centralWidget->setMinimumSize(802,508);

    pauseRunButton = new QPushButton();
    pauseRunButton->setObjectName(tr("pauseRunButton"));
    pauseRunButton->setMinimumSize(82, 22);
    pauseRunButton->setCheckable(true);
    pauseRunButton->setChecked(false);
    pauseRunButton->setText(tr("&Pause"));
    
    connect(pauseRunButton, SIGNAL(clicked()), this, SLOT(on_pauseRunButton_clicked()));
    
    QFont font;
    font.setPointSize(8);

    label1 = new QLabel();
    label1->setMinimumSize(59,13);
    label1->setFont(font);
    label1->setText(tr("Range (%)"));
    
    customPlot = new QCustomPlot();

    label2 = new QLabel();
    label2->setMinimumSize(47,13);
    label2->setFont(font);
    label2->setText(tr("Time (s)"));
 
    label3 = new QLabel();
    label3->setMinimumSize(0,0);
    label3->setFont(font);
    label3->setAlignment(Qt::AlignCenter);
    label3->setText(tr("-"));
    
    minY = new QDoubleSpinBox();
    minY->setMinimumSize(60,20);
    minY->setRange(-100,0);
    minY->setDecimals(0);
    minY->setSingleStep(1.0);
    minY->setValue(-100);
    
    maxY = new QDoubleSpinBox();
    maxY->setMinimumSize(60,20);
    maxY->setRange(0,100);
    maxY->setDecimals(0);
    maxY->setSingleStep(1.0);
    maxY->setValue(100);
    
    domain = new QSpinBox();
    domain->setMinimumSize(42,20);
    domain->setValue(10); // initialized the graph to show 10 seconds of data

    input.plotRange = new QDoubleSpinBox();
    input.plotRange->setMinimumSize(63,20);
    
    target.plotRange = new QDoubleSpinBox();
    target.plotRange->setMinimumSize(63,20);

    feedback.plotRange = new QDoubleSpinBox();
    feedback.plotRange->setMinimumSize(63,20);

    scaledFeedback.plotRange = new QDoubleSpinBox();
    scaledFeedback.plotRange->setMinimumSize(63,20);

    input.plotDisplay = new QCheckBox("Input");
    input.plotDisplay->setMinimumSize(87,17);
    input.plotDisplay->setStyleSheet(QStringLiteral("background-color:cyan"));

    target.plotDisplay = new QCheckBox("Target");
    target.plotDisplay->setMinimumSize(87,17);
    target.plotDisplay->setStyleSheet(QStringLiteral("background-color:blue"));

    feedback.plotDisplay = new QCheckBox("Feedback");
    feedback.plotDisplay->setMinimumSize(87,17);
    feedback.plotDisplay->setStyleSheet(QStringLiteral("background-color:#ffc0cb"));

    scaledFeedback.plotDisplay = new QCheckBox("Scaled Feedback");
    scaledFeedback.plotDisplay->setMinimumSize(87,17);
    scaledFeedback.plotDisplay->setStyleSheet(QStringLiteral("background-color:red"));

    allPlots.push_front(scaledFeedback);
    allPlots.push_front(feedback);
    allPlots.push_front(target);
    allPlots.push_front(input);

    customPlot->xAxis->QCPAxis::setRangeReversed(true);
    customPlot->yAxis->setRange(-100,100);
    customPlot->xAxis->setTickLabelType(QCPAxis::ltNumber);
    customPlot->xAxis->setAutoTickStep(false);
    customPlot->xAxis->setTickStep(1000);

    // this is used to see the x-axis to see accurate time.
    // customPlot->xAxis2->setVisible(true);
    // customPlot->xAxis2->setTickLabelType(QCPAxis::ltNumber);
    // customPlot->xAxis2->setAutoTickStep(true);
    // customPlot->xAxis2->setTickStep(2);

    QMetaObject::connectSlotsByName(GraphWindow);
}

void GraphWindow::setupPlots()
// sets the range QDoubleSpinBox, display QCheckBox, and the axis of the plot. 
// connects the valueChanged signal of the plotRange to change the ranges of the plots.
{
    for(auto &x: allPlots)
    {
        x.plotRange->setDecimals(1);
        x.plotRange->setSingleStep(0.1);
        x.plotRange->setRange(0,1);
        x.plotRange->setValue(1);
        x.plotDisplay->setCheckable(true);
        x.plotDisplay->setChecked(true);    

        x.plotAxis = customPlot->axisRect(0)->addAxis(QCPAxis::atRight);
        x.plotAxis->setRange(-1,1);
        x.plotAxis->setVisible(false);

        x.plotGraph = new QCPGraph(customPlot->xAxis2,x.plotAxis); // yellow line

        connect(x.plotRange, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, changeRanges);

        connect(x.plotDisplay, SIGNAL(clicked()), this, SLOT(set_line_visible()));
    }
}

void GraphWindow::changeRanges()
{
    for(auto &x: allPlots)
    {
        x.plotAxis->setRangeUpper((x.plotRange->value()) * ((maxY->value())/100));
        x.plotAxis->setRangeLower((x.plotRange->value()) * ((minY->value())/100));
    }
    customPlot->yAxis->setRange(minY->value(), maxY->value());
    customPlot->replot();
}

void GraphWindow::on_pauseRunButton_clicked()
// used the QMetaObject class to parse the function name and send the signal to the function.
{
    pauseRunButton->setText(pauseRunButton->isChecked() ? "R&un" : "&Pause");
    pauseRunButton->isChecked() ? dataTimer.stop() : dataTimer.start(refreshTimer);
    customPlot->replot();
}

void GraphWindow::set_line_visible()
// sets the visibility of the plot both when the plot is running and when it is paused.
{
    for(auto &x: allPlots)
    {
        x.plotDisplay->isChecked() ? x.plotGraph->setVisible(true) : x.plotGraph->setVisible(false);
        customPlot->replot();
    }    
}

void GraphWindow::remove_data_to_scroll()
// modifies the x-axis based on the domain value and removes data outside of visible range
{ 
    customPlot->xAxis->setRange(0, domain->value()*1000);

    // make key axis range scroll with the data at a rate of the time value obtained from QSpinBox
    customPlot->xAxis2->setRange(key, domain->value(), Qt::AlignRight);
    
    // these range values are divided by 100 just for the sample data in order to draw sine curve
    customPlot->replot();
}

void GraphWindow::realtimeDataSlot()
// plots data on graph
{
    key += (refreshTimer/1000);
    
    // this list is only used for demonstrations
    QList<double> value = {(sin(key)), (sin(key+1)), (sin(key+2)), (sin(key+3))};
    
    for(int i=0; i<allPlots.size(); i++)
    {
        allPlots[i].plotGraph->addData(key, value[i]);
        allPlots[i].plotGraph->removeDataBefore(key-(domain->value()));
    }
    
    remove_data_to_scroll();
}

