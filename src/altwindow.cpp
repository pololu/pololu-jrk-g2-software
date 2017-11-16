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
    AltWindow->setObjectName(tr("AltWindow"));
    AltWindow->resize(818,547);
    centralwidget = new QWidget(AltWindow);
    centralwidget->setObjectName(tr("centralwidget"));
    verticalLayout = new QVBoxLayout(centralwidget);
    verticalLayout->setObjectName(tr("verticalLayout"));
    backBtn = new QPushButton("Merge Windows",centralwidget);
    backBtn->setObjectName(tr("backBtn"));

    verticalLayout->addWidget(backBtn);

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(tr("horizontalLayout"));

    verticalLayout->addLayout(horizontalLayout);

    AltWindow->setCentralWidget(centralwidget);


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
    horizontalLayout->addWidget(grabbedWidget->customPlot);
    // horizontalLayout->addWidget(grabbedWidget);
}

