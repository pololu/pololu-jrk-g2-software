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
    // centralWidget->resize(150,150);

    
    verticalLayout = new QVBoxLayout(centralWidget);
    verticalLayout->setSpacing(6);
    verticalLayout->setContentsMargins(11,11,11,11);
    verticalLayout->setObjectName(QStringLiteral("verticalLayout"));

    // separateBtn = new QPushButton(tr("&Separate"), centralWidget);
    // separateBtn->setObjectName(QStringLiteral("separateBtn"));

    // verticalLayout->addWidget(separateBtn);


    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing(6);
    horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));

    // QGridLayout *layout = new QGridLayout(centralWidget);
    // layout->setColumnMinimumWidth(1,150);
    // layout->setRowMinimumHeight(1,150);

    // redWidget = new QWidget(centralWidget);
    // redWidget->setObjectName(QStringLiteral("redWidget"));
    // redWidget->setStyleSheet(QStringLiteral("background-color:#FF0000"));

    previewWindow = new GraphWindow();
    previewWindow->setObjectName(QStringLiteral("previewWindow"));
    previewWindow->customPlot->xAxis->setTicks(false);
    previewWindow->customPlot->yAxis->setTicks(false);
    previewWindow->customPlot->setInteraction(QCP::iSelectOther);
    QWidget *previewPlot = previewWindow->customPlot;
    previewPlot->resize(150,150);

    // horizontalLayout->addWidget(spacerWidget);
    horizontalLayout->addWidget(previewPlot);

    verticalLayout->addLayout(horizontalLayout);
    verticalLayout->addLayout(horizontalLayout);
    verticalLayout->addLayout(horizontalLayout);
    connect(previewPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(on_launchGraph_clicked(QMouseEvent*)));
    // layout->addWidget(altw->customPlot,0,0);

    MainWindow->setCentralWidget(centralWidget);

    QMetaObject::connectSlotsByName(MainWindow);
  
}

void MainWindow::on_separateBtn_clicked()
{
    // separateBtn->setEnabled(false);
    GraphWindow *red = previewWindow;
    // GraphWindow *preview = previewWindow;
    // preview->customPlot->resize(561, 460);
    // preview->customPlot->xAxis->setTicks(true);
    // preview->customPlot->yAxis->setTicks(true);
    // preview->setWindowTitle("jrk Graph");
    horizontalLayout->removeWidget(red);

    if(altw == 0)
    {
        altw = new AltWindow(this);
        connect(altw, SIGNAL(passWidget(GraphWindow*)), this, SLOT(receiveWidget(GraphWindow*)));
    }

    
    //altw->resize(QSize(818,547));
    //altw->setWindowFlags(Qt::FramelessWindowHint);
    altw->receiveWidget(red);
    altw->show();
    widgetAtHome = false;
}

void MainWindow::on_launchGraph_clicked(QMouseEvent *event)
{
    // separateBtn->setEnabled(false);
    GraphWindow *red = previewWindow;
    // GraphWindow *preview = previewWindow;
    // preview->customPlot->resize(561, 460);
    // preview->customPlot->xAxis->setTicks(true);
    // preview->customPlot->yAxis->setTicks(true);
    // preview->setWindowTitle("jrk Graph");
    horizontalLayout->removeWidget(red);

    if(altw == 0)
    {
        altw = new AltWindow(this);
        connect(altw, SIGNAL(passWidget(GraphWindow*)), this, SLOT(receiveWidget(GraphWindow*)));
    }

    
    //altw->resize(QSize(818,547));
    //altw->setWindowFlags(Qt::FramelessWindowHint);
    altw->receiveWidget(red);
    altw->show();
    widgetAtHome = false;
}

void MainWindow::receiveWidget(GraphWindow *widget)
{
    widget->customPlot->xAxis->setTicks(false);
    widget->customPlot->yAxis->setTicks(false);
    horizontalLayout->addWidget(widget->customPlot);
    // separateBtn->setEnabled(true);
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
    // separateBtn->setText(QApplication::translate("MainWindow", "Separate", Q_NULLPTR));
}