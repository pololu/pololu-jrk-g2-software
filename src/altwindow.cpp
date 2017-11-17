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

    centralLayout = new QHBoxLayout(centralwidget);
    centralLayout->setGeometry(QRect(0,0,802,508));

    mainLayout = new QVBoxLayout;

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
    grabbedWidget->customPlot->resize(561, 460);
    grabbedWidget->customPlot->xAxis->setTicks(true);
    grabbedWidget->customPlot->yAxis->setTicks(true);
    plotLayout->addWidget(grabbedWidget->customPlot);

    plotRangeLayout->addWidget(grabbedWidget->blueLine.plotRange);
    plotRangeLayout->addWidget(grabbedWidget->greenLine.plotRange);
    plotRangeLayout->addWidget(grabbedWidget->redLine.plotRange);
    plotRangeLayout->addWidget(grabbedWidget->yellowLine.plotRange);

    plotVisibleLayout->addWidget(grabbedWidget->blueLine.plotDisplay);
    plotVisibleLayout->addWidget(grabbedWidget->greenLine.plotDisplay);
    plotVisibleLayout->addWidget(grabbedWidget->redLine.plotDisplay);
    plotVisibleLayout->addWidget(grabbedWidget->yellowLine.plotDisplay);

    mainLayout->addLayout(plotLayout);
    mainLayout->addLayout(bottomControlLayout);
    centralLayout->addLayout(mainLayout);
    centralLayout->addLayout(plotRangeLayout);
    centralLayout->addLayout(plotVisibleLayout);

    horizontalLayout->addLayout(centralLayout);
}

void AltWindow::retranslateUi(QMainWindow *AltWindow)
{
    AltWindow->setWindowTitle(QApplication::translate("AltWindow", "Alternative Window", Q_NULLPTR));
    backBtn->setText(QApplication::translate("AltWindow", "Back", Q_NULLPTR));
}