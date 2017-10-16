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

        customPlot->addGraph(); // blue line
        customPlot->graph(0)->setPen(QPen(Qt::blue));
        customPlot->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
        customPlot->graph(0)->setAntialiasedFill(false);
        customPlot->addGraph(); // red line
        customPlot->graph(1)->setPen(QPen(Qt::red));
        customPlot->graph(0)->setChannelFillGraph(customPlot->graph(1));

        customPlot->addGraph(); // blue dot
        customPlot->graph(2)->setPen(QPen(Qt::blue));
        customPlot->graph(2)->setLineStyle(QCPGraph::lsNone);
        customPlot->graph(2)->setScatterStyle(QCPScatterStyle::ssDisc);
        customPlot->addGraph(); // red dot
        customPlot->graph(3)->setPen(QPen(Qt::red));
        customPlot->graph(3)->setLineStyle(QCPGraph::lsNone);
        customPlot->graph(3)->setScatterStyle(QCPScatterStyle::ssDisc);

        customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
        customPlot->xAxis->setDateTimeFormat("hh:mm:ss");
        customPlot->xAxis->setAutoTickStep(false);
        customPlot->xAxis->setTickStep(2);
        customPlot->axisRect()->setupFullAxesBox();

        // make left and bottom axes transfer their ranges to right and top axes:
        connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
        connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

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
        minY = new QSpinBox(centralWidget);
        minY->setObjectName(QStringLiteral("minY"));
        minY->setGeometry(QRect(263, 481, 50, 20));
        maxY = new QSpinBox(centralWidget);
        minY->setObjectName(QStringLiteral("maxY"));
        maxY->setGeometry(QRect(326, 481, 50, 20));

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
	#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
  double key = 0;
#else
  double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
#endif
  static double lastPointKey = 0;
  if (key-lastPointKey > 0.01) // at most add point every 10 ms
  {
    double value0 = tan(key);  
    double value1 = -tan(key); 
    // add data to lines:
    customPlot->graph(0)->addData(key, value0);
    customPlot->graph(1)->addData(key, value1);
    // set data of dots:
    customPlot->graph(2)->clearData();
    customPlot->graph(2)->addData(key, value0);
    customPlot->graph(3)->clearData();
    customPlot->graph(3)->addData(key, value1);
    // remove data of lines that's outside visible range:
    customPlot->graph(0)->removeDataBefore(key-8);
    customPlot->graph(1)->removeDataBefore(key-8);
    // rescale value (vertical) axis to fit the current data:
    customPlot->graph(0)->rescaleValueAxis();
    customPlot->graph(1)->rescaleValueAxis(true);
    lastPointKey = key;
  }
  // make key axis range scroll with the data (at a constant range size of 8):
    customPlot->xAxis->setRange(key+0.25, 8, Qt::AlignRight);
    customPlot->replot();
}
void MainWindow::setupPlayground(QCustomPlot * customPlot)
{
	  Q_UNUSED(customPlot)
}