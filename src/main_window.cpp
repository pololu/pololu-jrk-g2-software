#include "main_window.h"
#include "qcustomplot.h"
#include <time.h>
#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include <QMainWindow>
#include <QSerialPort>

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
        blueLineRange->setValue(1);
        greenLineRange->setValue(3);
        redLineRange->setValue(5);

        
        QCPAxis *blueLineAxis = customPlot->axisRect(0)->addAxis(QCPAxis::atRight);
        blueLineAxis->setRange(-1,1);
        blueLineAxis->setTickLabelColor(Qt::blue);
        QCPAxis *greenLineAxis = customPlot->axisRect(0)->addAxis(QCPAxis::atRight);
        greenLineAxis->setRange(1,3);
        greenLineAxis->setTickLabelColor(Qt::green);
        QCPAxis *redLineAxis = customPlot->axisRect(0)->addAxis(QCPAxis::atRight);
        redLineAxis->setRange(3,5);
        redLineAxis->setTickLabelColor(Qt::red);

        customPlot->addGraph(customPlot->xAxis2,blueLineAxis); // blue line
        customPlot->graph(0)->setPen(QPen(Qt::blue));
        customPlot->graph(0)->setValueAxis(blueLineAxis);

        customPlot->addGraph(customPlot->xAxis2,greenLineAxis); // green line
        customPlot->graph(1)->setPen(QPen(Qt::green));
        customPlot->graph(1)->setValueAxis(greenLineAxis);

        customPlot->addGraph(customPlot->xAxis2,redLineAxis); // red line
        customPlot->graph(2)->setPen(QPen(Qt::red));
        customPlot->graph(2)->setValueAxis(redLineAxis);

        
        customPlot->xAxis->setRange(0,1);
        customPlot->xAxis->QCPAxis::setRangeReversed(true);
        customPlot->yAxis->setRange(-100,100);
        //customPlot->yAxis2->setRange(-5,5);
        //customPlot->yAxis2->setRange(-1,1);
        customPlot->xAxis->setTickLabelType(QCPAxis::ltNumber);
        customPlot->xAxis->setAutoTickStep(false);
        customPlot->xAxis->setTickStep(1000);
        customPlot->yAxis2->setRange(minY->value()/20, maxY->value()/20);
        customPlot->yAxis2->setVisible(true);

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
        [=](double d){

            customPlot->yAxis2->setRangeUpper(d/20);
            customPlot->replot();
            customPlot->yAxis->setRangeUpper(d);
            customPlot->replot();
        });
        connect(minY, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        [=](double d){
            customPlot->yAxis2->setRangeLower(d/20);
            customPlot->replot();
            customPlot->yAxis->setRangeLower(d);
            customPlot->replot();
        });
        connect(domain, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        [=](int d){
            remove_data_to_scroll();
        });

        // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
        connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
        dataTimer.start(25); // Interval 0 means to refresh as fast as possible
}

void MainWindow::setupUi()
{
        if (this->objectName().isEmpty())
            this->setObjectName(QStringLiteral("MainWindow"));
        this->resize(818, 547);
        centralWidget = new QWidget(this);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        centralWidget->resize(802,508);
        pauseRunButton = new QPushButton(centralWidget);
        pauseRunButton->setObjectName(QStringLiteral("pauseRunButton"));
        pauseRunButton->setGeometry(QRect(13, 478, 82, 22));
        pauseRunButton->setCheckable(true);
        label1 = new QLabel(centralWidget);
        label1->setObjectName(QStringLiteral("label1"));
        label1->setGeometry(QRect(199,482,59,13));
        QFont font;
        font.setPointSize(8);
        label1->setFont(font);
        customPlot = new QCustomPlot(centralWidget);
        customPlot->setObjectName(QStringLiteral("customPlot"));
        customPlot->setGeometry(QRect(13, 12, 561, 460));
        label2 = new QLabel(centralWidget);
        label2->setObjectName(QStringLiteral("label2"));
        label2->setGeometry(QRect(385,482,47,13));
        QFont font2;
        font2.setPointSize(8);
        label2->setFont(font2);
        label3 = new QLabel(centralWidget);
        label3->setObjectName(QStringLiteral("label3"));
        label3->setGeometry(QRect(314,482,13,13));
        label3->setFont(font2);
        minY = new QDoubleSpinBox(centralWidget);
        minY->setObjectName(QStringLiteral("minY"));
        minY->setGeometry(QRect(263, 481, 50, 20));
        minY->setRange(-100,0);
        minY->setDecimals(0);
        minY->setSingleStep(1.0);
        maxY = new QDoubleSpinBox(centralWidget);
        maxY->setObjectName(QStringLiteral("maxY"));
        maxY->setGeometry(QRect(326, 481, 50, 20));
        maxY->setRange(0,100);
        maxY->setDecimals(0);
        maxY->setSingleStep(1.0);
        domain = new QSpinBox(centralWidget);
        domain->setObjectName(QStringLiteral("domain"));
        domain->setGeometry(QRect(436,481,32,20));
        blueLineRange = new QDoubleSpinBox(centralWidget);
        blueLineRange->setObjectName(QStringLiteral("blueLineRange"));
        blueLineRange->setGeometry(QRect(580,12,63,20));
        blueLineRange->setDecimals(1);
        blueLineRange->setSingleStep(0.5);
        greenLineRange = new QDoubleSpinBox(centralWidget);
        greenLineRange->setObjectName(QStringLiteral("greenLineRange"));
        greenLineRange->setGeometry(QRect(580,38,63,20));
        greenLineRange->setDecimals(1);
        greenLineRange->setSingleStep(0.5);
        redLineRange = new QDoubleSpinBox(centralWidget);
        redLineRange->setObjectName(QStringLiteral("redLineRange"));
        redLineRange->setGeometry(QRect(580,63,63,20));
        redLineRange->setDecimals(1);
        redLineRange->setSingleStep(0.5);
        blueLineDisplay = new QCheckBox(centralWidget);
        blueLineDisplay->setObjectName(QStringLiteral("blueLineDisplay"));
        blueLineDisplay->setGeometry(QRect(650,12,87,17));
        blueLineDisplay->setCheckable(true);
        greenLineDisplay = new QCheckBox(centralWidget);
        greenLineDisplay->setObjectName(QStringLiteral("greenLineDisplay"));
        greenLineDisplay->setGeometry(QRect(650,38,87,17));
        greenLineDisplay->setCheckable(true);
        redLineDisplay = new QCheckBox(centralWidget);
        redLineDisplay->setObjectName(QStringLiteral("redLineDisplay"));
        redLineDisplay->setGeometry(QRect(650,63,87,17));
        redLineDisplay->setCheckable(true);

        retranslateUi();	

        QMetaObject::connectSlotsByName(this);
}

void MainWindow::retranslateUi()
    {
        this->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        pauseRunButton->setText(QApplication::translate("MainWindow", "&Pause", 0));
        label1->setText(QApplication::translate("MainWindow", "Range (%)", 0));
        label2->setText(QApplication::translate("MainWindow", "Time (s)", 0));
        label3->setText(QApplication::translate("MainWindow", "-", 0));
    } // retranslateUi

void MainWindow::on_pauseRunButton_clicked()
{
    pauseRunButton->setText(pauseRunButton->isChecked() ? "R&un" : "&Pause");
    pauseRunButton->isChecked() ? dataTimer.stop() : dataTimer.start(25);
}


void MainWindow::set_line_range()
{
    customPlot->replot();
}

void MainWindow::set_line_visible()
{
}

void MainWindow::change_upper_y_range(double d)
{
    customPlot->yAxis2->setRangeUpper(d);
    customPlot->replot();
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

    if (blueLineDisplay->isChecked())
    {
        customPlot->graph(0)->setVisible(true);        
    }
    else
        customPlot->graph(0)->setVisible(false);
    customPlot->graph(0)->addData(key, value0);
    
    if (greenLineDisplay->isChecked())
    {
        customPlot->graph(1)->setVisible(true);        
    }
    else
        customPlot->graph(1)->setVisible(false);
    customPlot->graph(1)->addData(key, value1);

    if (redLineDisplay->isChecked())
    {
        customPlot->graph(2)->setVisible(true);        
    }
    else
        customPlot->graph(2)->setVisible(false);
    customPlot->graph(2)->addData(key, value2);
    
    remove_data_to_scroll();
}
void MainWindow::setupPlayground(QCustomPlot * customPlot)
{
	  Q_UNUSED(customPlot)
}

