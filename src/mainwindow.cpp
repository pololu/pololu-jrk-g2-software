#include "mainwindow.h"
#include "qcustomplot.h"
#include "altwindow.h"

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

MainWindow::MainWindow(QWidget * parent)
    : QMainWindow(parent)
{
    main_window_ui(this);
    altw = 0;

    sepAct = new QAction(tr("&Separate"), this);
    sepAct->setStatusTip(tr("Make the red part pop out into another window."));
    connect(sepAct, SIGNAL(triggered()), this, SLOT(on_launchGraph_clicked()));
    widgetAtHome = true;
}

MainWindow::~MainWindow()
{
    if (altw != 0)
    {
        delete altw;
    }
}

void MainWindow::main_window_ui(QMainWindow *MainWindow)
{
    MainWindow->setObjectName(tr("MainWindow"));
    MainWindow->resize(818,547);
    MainWindow->setWindowTitle("jrk main window");

    central = new QWidget(MainWindow);
    central->setObjectName(tr("central"));
    // central->resize(150,150);

    verticalLayout = new QVBoxLayout(central);
    verticalLayout->setSpacing(6);
    verticalLayout->setContentsMargins(11,11,11,11);
    verticalLayout->setObjectName(tr("verticalLayout"));

    separateBtn = new QPushButton("Separate Windows", central);
    separateBtn->setObjectName(tr("launchGraph"));

    verticalLayout->addWidget(separateBtn);

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing(6);
    horizontalLayout->setObjectName(tr("horizontalLayout"));

    // QGridLayout *layout = new QGridLayout(central);
    // layout->setColumnMinimumWidth(1,150);
    // layout->setRowMinimumHeight(1,150);

    previewWindow = new GraphWindow();
    previewWindow->customPlot->setObjectName(tr("previewWindow"));
    previewWindow->customPlot->xAxis->setTicks(false);
    previewWindow->customPlot->yAxis->setTicks(false);
    previewWindow->customPlot->setInteraction(QCP::iSelectOther);

    horizontalLayout->addWidget(previewWindow->customPlot);

    verticalLayout->addLayout(horizontalLayout);
    // connect(altw->customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(on_launchGraph_clicked(QMouseEvent*)));
    // layout->addWidget(altw->customPlot,0,0);

    MainWindow->setCentralWidget(central);

    QMetaObject::connectSlotsByName(MainWindow);
  
}

void MainWindow::on_launchGraph_clicked()
{
    separateBtn->setEnabled(false);
    
    GraphWindow *preview = previewWindow;
    // preview->customPlot->resize(561, 460);
    // preview->customPlot->xAxis->setTicks(true);
    // preview->customPlot->yAxis->setTicks(true);
    // preview->setWindowTitle("jrk Graph");
    horizontalLayout->removeWidget(preview->customPlot);

    if (altw == 0)
    {
        altw = new AltWindow(this);
        connect(altw, SIGNAL(passWidget(GraphWindow*)), this, SLOT(receiveWidget(GraphWindow*)));
    }


    
    
    //altw->resize(QSize(818,547));
    //altw->setWindowFlags(Qt::FramelessWindowHint);
    altw->receiveWidget(preview);
    altw->show();
    widgetAtHome = false;
}

void MainWindow::receiveWidget(GraphWindow *widget)
{
    horizontalLayout->addWidget(widget->customPlot);
    separateBtn->setEnabled(true);
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