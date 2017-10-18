#include "main_window.h"
#include "qcustomplot.h"

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
        minY->setValue(-100);
        maxY->setValue(100);
        domain->setValue(1);
        customPlot->addGraph(customPlot->xAxis2,customPlot->yAxis2); // blue line
        customPlot->graph(0)->setPen(QPen(Qt::blue));
        customPlot->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
        customPlot->graph(0)->setAntialiasedFill(false);
        
        customPlot->xAxis->setRange(0,1);
        customPlot->xAxis->QCPAxis::setRangeReversed(true);
        customPlot->yAxis->setRange(-100,100);
        //customPlot->yAxis2->setRange(-1,1);
        customPlot->xAxis->setTickLabelType(QCPAxis::ltNumber);
        customPlot->xAxis->setAutoTickStep(false);
        customPlot->xAxis->setTickStep(1000);


        // this is used to see the x-axis to see accurate time.

        // customPlot->xAxis2->setVisible(true);
        // customPlot->xAxis2->setTickLabelType(QCPAxis::ltDateTime);
        // customPlot->xAxis2->setDateTimeFormat("hh:mm:ss");
        // customPlot->xAxis2->setAutoTickStep(true);
        // customPlot->xAxis2->setTickStep(2);

        

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
	connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    pauseRunButton->setText(pauseRunButton->isChecked() ? "R&un" : "&Pause");
    pauseRunButton->isChecked() ? dataTimer.stop() : dataTimer.start();
}

void MainWindow::Receivedata()
{
	QByteArray ba;
    ba=serial->readAll();
    label1->setText(ba);
}

void MainWindow::realtimeDataSlot()
{
    
    double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    static double lastPointKey = 0;
    if (key-lastPointKey > 0.01) // at most add point every 10 ms
    {
        double value0 = sin(key);  
        customPlot->graph(0)->addData(key, value0);
        
        // remove data of lines that's outside visible range:
        customPlot->graph(0)->removeDataBefore(key-(domain->value()));
        
        lastPointKey = key;
    } 

    customPlot->xAxis->setRange(0, domain->value()*1000);

    // make key axis range scroll with the data at a rate of the time value obtained from QSpinBox
    customPlot->xAxis2->setRange(key, domain->value(), Qt::AlignRight);
    customPlot->yAxis->setRange((minY->value()), (maxY->value()));
    
    // these range values are divided by 100 just for the sample data in order to draw sine curve
    customPlot->yAxis2->setRange((minY->value())/100, (maxY->value())/100);
    customPlot->replot();
}
void MainWindow::setupPlayground(QCustomPlot * customPlot)
{
	  Q_UNUSED(customPlot)
}

