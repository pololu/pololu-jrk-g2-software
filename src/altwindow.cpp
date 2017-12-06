#include "altwindow.h"
#include "mainwindow.h"
#include "graphwindow.h"
#include <QDebug>
#include <QWidget>

AltWindow::AltWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);
    grabbedWidget = 0;
    connect(backBtn, SIGNAL(clicked()), this, SLOT(close()));
}

AltWindow::~AltWindow()
{
    
}

void AltWindow::setupUi(QMainWindow *AltWindow)
{
    AltWindow->setObjectName(QStringLiteral("AltWindow"));
    AltWindow->resize(818,547);
    centralwidget = new QWidget(AltWindow);
    centralwidget->setObjectName(QStringLiteral("centralwidget"));
    verticalLayout = new QVBoxLayout(centralwidget);
    verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
    backBtn = new QPushButton(centralwidget);
    backBtn->setObjectName(QStringLiteral("backBtn"));

    centralLayout = new QGridLayout(this);

    mainLayout = new QGridLayout;

    plotLayout = new QHBoxLayout;

    bottomControlLayout = new QHBoxLayout;

    plotRangeLayout = new QVBoxLayout;

    plotVisibleLayout = new QVBoxLayout;
    
    verticalLayout->addWidget(backBtn);

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));

    verticalLayout->addLayout(horizontalLayout);

    AltWindow->setCentralWidget(centralwidget);

    retranslateUi(AltWindow);

    QMetaObject::connectSlotsByName(AltWindow);
}

void AltWindow::closeEvent(QCloseEvent *event)
{
    grabbedWidget->customPlot->xAxis->setTicks(false);
    grabbedWidget->customPlot->yAxis->setTicks(false);
    horizontalLayout->removeWidget(grabbedWidget);
    emit passWidget(grabbedWidget);
    grabbedWidget = 0;
    QMainWindow::closeEvent(event);
}

void AltWindow::receiveWidget(GraphWindow *widget)
{
    if(grabbedWidget != 0)
        qWarning() << "You might have lost a widget just now.";

    grabbedWidget = widget;
    bottomControlLayout->addWidget(grabbedWidget->pauseRunButton);
    bottomControlLayout->addWidget(grabbedWidget->label1);
    bottomControlLayout->addWidget(grabbedWidget->minY);
    bottomControlLayout->addWidget(grabbedWidget->label3);
    bottomControlLayout->addWidget(grabbedWidget->maxY);
    bottomControlLayout->addWidget(grabbedWidget->label2);
    bottomControlLayout->addWidget(grabbedWidget->domain);
    grabbedWidget->customPlot->xAxis->setTicks(true);
    grabbedWidget->customPlot->yAxis->setTicks(true);
    grabbedWidget->customPlot->setFixedSize(561,460);
    grabbedWidget->customPlot->setCursor(Qt::ArrowCursor);
    grabbedWidget->customPlot->setToolTip("");
    plotLayout->addWidget(grabbedWidget->customPlot,Qt::AlignTop);

    for(auto &x: grabbedWidget->allPlots)
    {
        plotRangeLayout->addWidget(x.plotRange);
        plotVisibleLayout->addWidget(x.plotDisplay);
    }

    mainLayout->addLayout(plotLayout,0,0,Qt::AlignTop);
    mainLayout->addLayout(bottomControlLayout,1,0);
    centralLayout->addLayout(mainLayout,0,0);
    centralLayout->addLayout(plotRangeLayout,0,1);
    centralLayout->addLayout(plotVisibleLayout,0,2);

    horizontalLayout->addLayout(centralLayout);
}

void AltWindow::retranslateUi(QMainWindow *AltWindow)
{
    AltWindow->setWindowTitle(QApplication::translate("AltWindow", "Graph Popout Window", Q_NULLPTR));
    backBtn->setText(QApplication::translate("AltWindow", "Back", Q_NULLPTR));
}