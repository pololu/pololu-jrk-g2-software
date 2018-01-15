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

	central_layout = new QGridLayout();

	plot_layout = new QHBoxLayout();

	bottom_control_layout = new QHBoxLayout();

	plot_visible_layout = new QVBoxLayout();

	retranslate_ui();

	setLayout(central_layout);

	QMetaObject::connectSlotsByName(this);
}

void graph_window::closeEvent(QCloseEvent *event)
{
	grabbed_widget->custom_plot->xAxis->setTicks(false);
	grabbed_widget->custom_plot->yAxis->setTicks(false);
	central_layout->removeWidget(grabbed_widget);
	emit pass_widget(grabbed_widget);
	grabbed_widget = 0;
	QWidget::closeEvent(event);
}

void graph_window::receive_widget(graph_widget *widget)
{
	if(grabbed_widget != 0)
		qWarning() << "You might have lost a widget just now.";

	grabbed_widget = widget;
	bottom_control_layout->addWidget(grabbed_widget->pause_run_button);
	bottom_control_layout->addWidget(grabbed_widget->label1);
	bottom_control_layout->addWidget(grabbed_widget->min_y);
	bottom_control_layout->addWidget(grabbed_widget->label3);
	bottom_control_layout->addWidget(grabbed_widget->max_y);
	bottom_control_layout->addWidget(grabbed_widget->label2);
	bottom_control_layout->addWidget(grabbed_widget->domain);
	grabbed_widget->custom_plot->xAxis->setTicks(true);
	grabbed_widget->custom_plot->yAxis->setTicks(true);
	grabbed_widget->custom_plot->setMinimumSize(561,460);
	grabbed_widget->custom_plot->setCursor(Qt::ArrowCursor);
	grabbed_widget->custom_plot->setToolTip("");
	plot_layout->addWidget(grabbed_widget->custom_plot);

	for(auto plot : grabbed_widget->all_plots)
	{
		plot->display->setChecked(plot->default_check);
		plot->graph->setVisible(plot->default_check);
		plot_visible_layout->addLayout(plot->graph_data_selection_bar, Qt::AlignRight);
	}

	central_layout->addLayout(plot_layout,0,0, 14,3);
	central_layout->addLayout(bottom_control_layout, 15, 0, Qt::AlignLeft|Qt::AlignTop);
	central_layout->addLayout(plot_visible_layout,0,4,11,1, Qt::AlignTop|Qt::AlignLeft);
}

void graph_window::retranslate_ui()
{
  this->setWindowTitle(QApplication::translate("graph_window",
      "Pololu Jrk G2 Configuration Utility - Plots of Variables vs. Time", Q_NULLPTR));
}
