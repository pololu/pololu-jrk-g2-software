#include "main_window.h"
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
#include <QPropertyAnimation>

double refreshTimer = 30; // used as a constant for both places the refresh rate is used

GraphWindow::GraphWindow(QWidget *parent)
   
{
    setupUi(); 
    setupPlots();
    
    allPlots[0].plotGraph->setPen(QPen(Qt::yellow));
    allPlots[1].plotGraph->setPen(QPen(Qt::blue));
    allPlots[2].plotGraph->setPen(QPen(Qt::green));
    allPlots[3].plotGraph->setPen(QPen(Qt::red));

    connect(maxY, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, changeRanges);

    connect(minY, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, changeRanges);

    connect(domain, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, remove_data_to_scroll);

    // connect the signal from the dataTime to call realtimeDataSlot() using the refresh interval "refreshTimer"
    // set as a constant to be used throughout the class
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    dataTimer.start(refreshTimer); // Interval 0 means to refresh as fast as possible
}

void GraphWindow::setupUi()
// sets the gui objects in the graph window
{
    centralWidget = new QWidget(this);
    centralWidget->resize(802,508);
    
    pauseRunButton = new QPushButton(centralWidget);
    pauseRunButton->setObjectName(tr("pauseRunButton"));
    pauseRunButton->setGeometry(QRect(13, 478, 82, 22));
    pauseRunButton->setCheckable(true);
    pauseRunButton->setChecked(false);
    pauseRunButton->setText(tr("&Pause"));
    
    QFont font;
    font.setPointSize(8);

    label1 = new QLabel(centralWidget);
    label1->setGeometry(QRect(199,482,59,13));
    label1->setFont(font);
    label1->setText(tr("Range (%)"));
    
    customPlot = new QCustomPlot(centralWidget);
    customPlot->setGeometry(QRect(13, 12, 561, 460));
    
    label2 = new QLabel(centralWidget);
    label2->setGeometry(QRect(405,482,47,13));
    label2->setFont(font);
    label2->setText(tr("Time (s)"));
    
    label3 = new QLabel(centralWidget);
    label3->setGeometry(QRect(329,482,13,13));
    label3->setFont(font);
    label3->setText(tr("-"));
    
    minY = new QDoubleSpinBox(centralWidget);
    minY->setGeometry(QRect(263, 481, 60, 20));
    minY->setRange(-100,0);
    minY->setDecimals(0);
    minY->setSingleStep(1.0);
    minY->setValue(-100);
    
    maxY = new QDoubleSpinBox(centralWidget);
    maxY->setGeometry(QRect(336, 481, 60, 20));
    maxY->setRange(0,100);
    maxY->setDecimals(0);
    maxY->setSingleStep(1.0);
    maxY->setValue(100);
    
    domain = new QSpinBox(centralWidget);
    domain->setGeometry(QRect(456,481,42,20));
    domain->setValue(10); // initialized the graph to show 10 seconds of data

    yellowLine.plotRange = new QDoubleSpinBox(centralWidget);
    yellowLine.plotRange->setGeometry(QRect(580,88,63,20));
    
    blueLine.plotRange = new QDoubleSpinBox(centralWidget);
    blueLine.plotRange->setGeometry(QRect(580,12,63,20));

    greenLine.plotRange = new QDoubleSpinBox(centralWidget);
    greenLine.plotRange->setGeometry(QRect(580,38,63,20));

    redLine.plotRange = new QDoubleSpinBox(centralWidget);
    redLine.plotRange->setGeometry(QRect(580,63,63,20));

    yellowLine.plotDisplay = new QCheckBox("YELLOW", centralWidget);
    yellowLine.plotDisplay->setGeometry(QRect(650,88,87,17));

    blueLine.plotDisplay = new QCheckBox("BLUE", centralWidget);
    blueLine.plotDisplay->setGeometry(QRect(650,12,87,17));

    greenLine.plotDisplay = new QCheckBox("GREEN", centralWidget);
    greenLine.plotDisplay->setGeometry(QRect(650,38,87,17));

    redLine.plotDisplay = new QCheckBox("RED", centralWidget);
    redLine.plotDisplay->setGeometry(QRect(650,63,87,17));

    allPlots.push_front(yellowLine);
    allPlots.push_front(blueLine);
    allPlots.push_front(greenLine);
    allPlots.push_front(redLine);

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

    QMetaObject::connectSlotsByName(this);
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


