#include "graph_window.h"
#include "main_window.h"
#include "graph_widget.h"

#include <QApplication>
#include <QCheckBox>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

graph_window::graph_window(QWidget *parent)
{
	setup_ui();
	grabbed_widget = 0;
}

void graph_window::setup_ui()
{
	setObjectName(QStringLiteral("graph_window"));

	central_widget = new QWidget(this);
	central_widget->setObjectName(QStringLiteral("central_widget"));

	vertical_layout = new QVBoxLayout(central_widget);
	vertical_layout->setObjectName(QStringLiteral("vertical_layout"));

	central_layout = new QGridLayout(this);

	main_layout = new QGridLayout();

	plot_layout = new QHBoxLayout();

	bottom_control_layout = new QHBoxLayout();

	plot_visible_layout = new QVBoxLayout();

	range_label_layout = new QVBoxLayout();

	horizontal_layout = new QHBoxLayout();
	horizontal_layout->setObjectName(QStringLiteral("horizontal_layout"));

	vertical_layout->addLayout(horizontal_layout);

	retranslate_ui();

	QMetaObject::connectSlotsByName(this);
}

void graph_window::closeEvent(QCloseEvent *event)
{
	grabbed_widget->custom_plot->xAxis->setTicks(false);
	grabbed_widget->custom_plot->yAxis->setTicks(false);
	horizontal_layout->removeWidget(grabbed_widget);
	emit pass_widget(grabbed_widget);
	grabbed_widget = 0;
	QWidget::closeEvent(event);
}

void graph_window::receive_widget(graph_widget *widget)
{
	if(grabbed_widget != 0)
		qWarning() << "You might have lost a widget just now.";

	grabbed_widget = widget;
	bottom_control_layout->addWidget(grabbed_widget->pauseRunButton);
	bottom_control_layout->addWidget(grabbed_widget->label1);
	bottom_control_layout->addWidget(grabbed_widget->min_y);
	bottom_control_layout->addWidget(grabbed_widget->label3);
	bottom_control_layout->addWidget(grabbed_widget->max_y);
	bottom_control_layout->addWidget(grabbed_widget->label2);
	bottom_control_layout->addWidget(grabbed_widget->domain);
	grabbed_widget->custom_plot->xAxis->setTicks(true);
	grabbed_widget->custom_plot->yAxis->setTicks(true);
	grabbed_widget->custom_plot->setFixedSize(561,460);
	grabbed_widget->custom_plot->setCursor(Qt::ArrowCursor);
	grabbed_widget->custom_plot->setToolTip("");
	plot_layout->addWidget(grabbed_widget->custom_plot,Qt::AlignTop);

	for(auto &x: grabbed_widget->all_plots)
	{
		plot_visible_layout->addLayout(x.graph_data_selection_bar);
	}

	main_layout->addLayout(plot_layout,0,0,Qt::AlignTop);
	main_layout->addLayout(bottom_control_layout,1,0);
	central_layout->addLayout(main_layout,0,0);
	central_layout->addLayout(plot_visible_layout,0,1);

	horizontal_layout->addLayout(central_layout);
}

void graph_window::retranslate_ui()
{
  this->setWindowTitle(QApplication::translate("graph_window",
      "Pololu Jrk G2 Configuration Utility - Plots of Variables vs. Time", Q_NULLPTR));
}
