#include "mainwindow.h"
#include "qcustomplot.h"
#include "altwindow.h"

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include <QMainWindow>
#include <QSerialPort>
#include <QVector>
#include <QtCore>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget * parent)
    : QMainWindow(parent)
{
    setupUi(this);
    altw = 0;

    sepAct = new QAction(tr("&Separate"), this);
    sepAct->setStatusTip(tr("Make the red part pop out into another window."));
    connect(sepAct, SIGNAL(triggered()), this, SLOT(on_separateBtn_clicked()));
    widgetAtHome = true;
}

MainWindow::~MainWindow()
{
    if (altw != 0)
    {
        delete altw;
    }
}

void MainWindow::setupUi(QMainWindow *MainWindow)
{
    MainWindow->setObjectName(QStringLiteral("MainWindow"));
    MainWindow->resize(818,547);
    MainWindow->setWindowTitle("jrk main window");

    centralWidget = new QWidget(MainWindow);
    centralWidget->setObjectName(QStringLiteral("centralWidget"));

    
    gridLayout = new QGridLayout(centralWidget);
    gridLayout->setSpacing(6);
    gridLayout->setContentsMargins(11,11,11,11);
    gridLayout->setObjectName(QStringLiteral("gridLayout"));

    

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing(6);
    horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));

    previewWindow = new GraphWindow();
    previewWindow->setObjectName(QStringLiteral("previewWindow"));
    previewWindow->customPlot->xAxis->setTicks(false);
    previewWindow->customPlot->yAxis->setTicks(false);
    QWidget *previewPlot = previewWindow->customPlot;
    previewPlot->setFixedSize(150,150);

    horizontalLayout->addWidget(previewPlot);

    gridLayout->addLayout(horizontalLayout,0,0);
    
    connect(previewPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(on_launchGraph_clicked(QMouseEvent*)));

    MainWindow->setCentralWidget(centralWidget);

    QMetaObject::connectSlotsByName(MainWindow);
  
}

void MainWindow::on_launchGraph_clicked(QMouseEvent *event)
{
    GraphWindow *red = previewWindow;
    horizontalLayout->removeWidget(red);
    if(altw == 0)
    {
        altw = new AltWindow(this);
        connect(altw, SIGNAL(passWidget(GraphWindow*)), this, SLOT(receiveWidget(GraphWindow*)));
    }

    altw->receiveWidget(red);
    altw->show();
    widgetAtHome = false;
}

void MainWindow::receiveWidget(GraphWindow *widget)
{
    widget->customPlot->setFixedSize(150,150);
    widget->customPlot->xAxis->setTicks(false);
    widget->customPlot->yAxis->setTicks(false);
    horizontalLayout->addWidget(widget->customPlot);
    widgetAtHome = true;
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    if (widgetAtHome)
    {
        QMenu menu(this);
        menu.addAction(sepAct);
        menu.exec(event->globalPos());
    }
}

void MainWindow::retranslateUi(QMainWindow *MainWindow)
{
    MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
}