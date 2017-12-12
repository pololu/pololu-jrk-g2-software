#include "altwindow.h"
#include "mainwindow.h"
#include "graphwindow.h"
#include <QDebug>
#include <QWidget>

AltWindow::AltWindow(QWidget *parent) :
	QMainWindow(parent)
{
	setup_ui(this);
	grabbedWidget = 0;
	connect(backBtn, SIGNAL(clicked()), this, SLOT(close()));
}

AltWindow::~AltWindow()
{
		
}

void AltWindow::setup_ui(QMainWindow *AltWindow)
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

	horizontal_layout = new QHBoxLayout();
	horizontal_layout->setObjectName(QStringLiteral("horizontal_layout"));

	verticalLayout->addLayout(horizontal_layout);

	AltWindow->setCentralWidget(centralwidget);

	retranslate_ui(AltWindow);

	QMetaObject::connectSlotsByName(AltWindow);
}

void AltWindow::closeEvent(QCloseEvent *event)
{
	grabbedWidget->custom_plot->xAxis->setTicks(false);
	grabbedWidget->custom_plot->yAxis->setTicks(false);
	horizontal_layout->removeWidget(grabbedWidget);
	emit pass_widget(grabbedWidget);
	grabbedWidget = 0;
	QMainWindow::closeEvent(event);
}

void AltWindow::receive_widget(graph_widget *widget)
{
	if(grabbedWidget != 0)
		qWarning() << "You might have lost a widget just now.";

	grabbedWidget = widget;
	bottomControlLayout->addWidget(grabbedWidget->pauseRunButton);
	bottomControlLayout->addWidget(grabbedWidget->label1);
	bottomControlLayout->addWidget(grabbedWidget->min_y);
	bottomControlLayout->addWidget(grabbedWidget->label3);
	bottomControlLayout->addWidget(grabbedWidget->max_y);
	bottomControlLayout->addWidget(grabbedWidget->label2);
	bottomControlLayout->addWidget(grabbedWidget->domain);
	grabbedWidget->custom_plot->xAxis->setTicks(true);
	grabbedWidget->custom_plot->yAxis->setTicks(true);
	grabbedWidget->custom_plot->setFixedSize(561,460);
	grabbedWidget->custom_plot->setCursor(Qt::ArrowCursor);
	grabbedWidget->custom_plot->setToolTip("");
	plotLayout->addWidget(grabbedWidget->custom_plot,Qt::AlignTop);

	for(auto &x: grabbedWidget->all_plots)
	{
		plotRangeLayout->addWidget(x.plot_range);
		plotVisibleLayout->addWidget(x.plot_display);
	}

	mainLayout->addLayout(plotLayout,0,0,Qt::AlignTop);
	mainLayout->addLayout(bottomControlLayout,1,0);
	centralLayout->addLayout(mainLayout,0,0);
	centralLayout->addLayout(plotRangeLayout,0,1);
	centralLayout->addLayout(plotVisibleLayout,0,2);

	horizontal_layout->addLayout(centralLayout);
}

void AltWindow::retranslate_ui(QMainWindow *AltWindow)
{
	AltWindow->setWindowTitle(QApplication::translate("AltWindow", "Graph Popout Window", Q_NULLPTR));
	backBtn->setText(QApplication::translate("AltWindow", "Back", Q_NULLPTR));
}