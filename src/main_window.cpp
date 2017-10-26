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

QSerialPort *serial;
QString comport="";
char buffer[2];

MainWindow::MainWindow(QWidget * parent)
	: QMainWindow(parent)
{
	setupUi();
    pauseRunButton->setChecked(false);
    blueLineDisplay->setChecked(true);
    greenLineDisplay->setChecked(true);
    redLineDisplay->setChecked(true);

    minY->setValue(-100);
    maxY->setValue(100);
    domain->setValue(10);
    blueLineRange->setValue(5);
    greenLineRange->setValue(5);
    redLineRange->setValue(5);

    double upperRanges[] = {5.0,5.0,5.0,100.0};
    double lowerRanges[] = {-5.0,-5.0,-5.0,-100.0};


    QCPAxis *blueLineAxis = customPlot->axisRect(0)->addAxis(QCPAxis::atRight);
    blueLineAxis->setRange(lowerRanges[0], upperRanges[0]);
    blueLineAxis->setVisible(false);
    
    QCPAxis *greenLineAxis = customPlot->axisRect(0)->addAxis(QCPAxis::atRight);
    greenLineAxis->setRange(lowerRanges[1], upperRanges[1]);
    greenLineAxis->setVisible(false);

    QCPAxis *redLineAxis = customPlot->axisRect(0)->addAxis(QCPAxis::atRight);
    redLineAxis->setRange(lowerRanges[2], upperRanges[2]);
    redLineAxis->setVisible(false);

    QList<QCPAxis*> axes = {blueLineAxis,greenLineAxis,redLineAxis,customPlot->yAxis};
    
    customPlot->addGraph(customPlot->xAxis2,blueLineAxis); // blue line
    customPlot->graph(0)->setPen(QPen(Qt::blue));

    customPlot->addGraph(customPlot->xAxis2,greenLineAxis); // green line
    customPlot->graph(1)->setPen(QPen(Qt::green));

    customPlot->addGraph(customPlot->xAxis2,redLineAxis); // red line
    customPlot->graph(2)->setPen(QPen(Qt::red));

    
    customPlot->xAxis->setRange(0,1);
    customPlot->xAxis->QCPAxis::setRangeReversed(true);
    customPlot->yAxis->setRange(lowerRanges[3],upperRanges[3]);
    customPlot->xAxis->setTickLabelType(QCPAxis::ltNumber);
    customPlot->xAxis->setAutoTickStep(false);
    customPlot->xAxis->setTickStep(1000);

    // this is used to see the x-axis to see accurate time.

    // customPlot->xAxis2->setVisible(true);
    // customPlot->xAxis2->setTickLabelType(QCPAxis::ltDateTime);
    // customPlot->xAxis2->setDateTimeFormat("hh:mm:ss");
    // customPlot->xAxis2->setAutoTickStep(true);
    // customPlot->xAxis2->setTickStep(2);

    connect(blueLineRange, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
    [=](double d){
        blueLineAxis->setRangeUpper(d);
        customPlot->replot();
    });
    connect(greenLineRange, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
    [=](double d){
        greenLineAxis->setRangeUpper(d);
        customPlot->replot();
    });
    connect(redLineRange, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
    [=](double d){
        redLineAxis->setRangeUpper(d);
        customPlot->replot();
    });


    connect(maxY, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
    [=](double d) mutable {
        for (int i = 0; i < axes.size(); ++i)
        {
            axes[i]->setRangeUpper((upperRanges[i])*(d/100));
        }
        customPlot->replot();
    });
    connect(minY, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
    [=](double d) mutable {
        for (int i = 0; i < axes.size(); ++i)
        {
            axes[i]->setRangeLower((lowerRanges[i])*(-d/100));
        }
        customPlot->replot();
    });
    connect(domain, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
    [=](int d){
        remove_data_to_scroll();
    }); 

    connect(blueLineDisplay, SIGNAL(clicked()), this, SLOT(set_line_visible()));
    connect(greenLineDisplay, SIGNAL(clicked()), this, SLOT(set_line_visible()));
    connect(redLineDisplay, SIGNAL(clicked()), this, SLOT(set_line_visible()));

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    dataTimer.start(25); // Interval 0 means to refresh as fast as possible
}

void MainWindow::setupUi()
{
    this->resize(818, 547);
    centralWidget = new QWidget(this);

    centralWidget->resize(802,508);
    
    pauseRunButton = new QPushButton(centralWidget);
    pauseRunButton->setGeometry(QRect(13, 478, 82, 22));
    pauseRunButton->setCheckable(true);
    
    QFont font;
    font.setPointSize(8);

    label1 = new QLabel(centralWidget);
    label1->setGeometry(QRect(199,482,59,13));
    label1->setFont(font);
    
    customPlot = new QCustomPlot(centralWidget);
    customPlot->setGeometry(QRect(13, 12, 561, 460));
    
    label2 = new QLabel(centralWidget);
    label2->setGeometry(QRect(385,482,47,13));
    label2->setFont(font);
    
    label3 = new QLabel(centralWidget);
    label3->setGeometry(QRect(314,482,13,13));
    label3->setFont(font);
    
    minY = new QDoubleSpinBox(centralWidget);
    minY->setGeometry(QRect(263, 481, 50, 20));
    minY->setRange(-100,0);
    minY->setDecimals(0);
    minY->setSingleStep(1.0);
    
    maxY = new QDoubleSpinBox(centralWidget);
    maxY->setGeometry(QRect(326, 481, 50, 20));
    maxY->setRange(0,100);
    maxY->setDecimals(0);
    maxY->setSingleStep(1.0);
    
    domain = new QSpinBox(centralWidget);
    domain->setGeometry(QRect(436,481,32,20));
    
    blueLineRange = new QDoubleSpinBox(centralWidget);
    blueLineRange->setGeometry(QRect(580,12,63,20));
    blueLineRange->setDecimals(1);
    blueLineRange->setSingleStep(0.5);
    
    greenLineRange = new QDoubleSpinBox(centralWidget);
    greenLineRange->setGeometry(QRect(580,38,63,20));
    greenLineRange->setDecimals(1);
    greenLineRange->setSingleStep(0.5);
    
    redLineRange = new QDoubleSpinBox(centralWidget);
    redLineRange->setGeometry(QRect(580,63,63,20));
    redLineRange->setDecimals(1);
    redLineRange->setSingleStep(0.5);
    
    blueLineDisplay = new QCheckBox(centralWidget);
    blueLineDisplay->setGeometry(QRect(650,12,87,17));
    blueLineDisplay->setCheckable(true);
    
    greenLineDisplay = new QCheckBox(centralWidget);
    greenLineDisplay->setGeometry(QRect(650,38,87,17));
    greenLineDisplay->setCheckable(true);
    
    redLineDisplay = new QCheckBox(centralWidget);
    redLineDisplay->setGeometry(QRect(650,63,87,17));
    redLineDisplay->setCheckable(true);

    retranslateUi();	

    QMetaObject::connectSlotsByName(this);
}

void MainWindow::retranslateUi()
{
    this->setWindowTitle(QApplication::translate("MainWindow", "jrk graph", 0));
    pauseRunButton->setText(QApplication::translate("MainWindow", "&Pause", 0));
    label1->setText(QApplication::translate("MainWindow", "Range (%)", 0));
    label2->setText(QApplication::translate("MainWindow", "Time (s)", 0));
    label3->setText(QApplication::translate("MainWindow", "-", 0));
} // retranslateUi

void MainWindow::on_pauseRunButton_clicked()
{
    pauseRunButton->setText(pauseRunButton->isChecked() ? "R&un" : "&Pause");
    pauseRunButton->isChecked() ? dataTimer.stop() : dataTimer.start(25);
    customPlot->replot();
}


void MainWindow::set_line_range()
{
    customPlot->replot();
}

void MainWindow::set_line_visible()
{
    blueLineDisplay->isChecked() ? customPlot->graph(0)->setVisible(true) : customPlot->graph(0)->setVisible(false);
    greenLineDisplay->isChecked() ? customPlot->graph(1)->setVisible(true) : customPlot->graph(1)->setVisible(false);
    redLineDisplay->isChecked() ? customPlot->graph(2)->setVisible(true) : customPlot->graph(2)->setVisible(false);
    customPlot->replot();
}

void MainWindow::change_upper_y_range()
{
    
}

void MainWindow::change_lower_y_range(double d)
{
    customPlot->yAxis2->setRangeLower(d);
    customPlot->replot();
}

void MainWindow::remove_data_to_scroll()
{
    // remove data of lines that's outside visible range:
    customPlot->graph(0)->removeDataBefore(key-(domain->value()));
    customPlot->graph(1)->removeDataBefore(key-(domain->value()));
    customPlot->graph(2)->removeDataBefore(key-(domain->value()));
    
    lastPointKey = key;

    customPlot->xAxis->setRange(0, domain->value()*1000);

    // make key axis range scroll with the data at a rate of the time value obtained from QSpinBox
    customPlot->xAxis2->setRange(key, domain->value(), Qt::AlignRight);
    
    // these range values are divided by 100 just for the sample data in order to draw sine curve
    customPlot->replot();
}

void MainWindow::realtimeDataSlot()
{
    key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    lastPointKey = 0;
    
    double value0 = sin(key);  
    double value1 = sin(key+1) + 2;  
    double value2 = sin(key+2) + 4;  

    set_line_visible();

    customPlot->graph(0)->addData(key, value0);
    customPlot->graph(1)->addData(key, value1);
    customPlot->graph(2)->addData(key, value2);
    
    remove_data_to_scroll();
}
void MainWindow::setupPlayground(QCustomPlot * customPlot)
{
	  Q_UNUSED(customPlot)
}

