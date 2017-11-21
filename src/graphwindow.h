#pragma once

#include <QMainWindow>
#include <QList>
#include "qcustomplot.h"

class GraphWindow : public QMainWindow
{
	Q_OBJECT

public:
	GraphWindow(QWidget * parent = 0);
	~GraphWindow();
	
	struct Plot
    {
    	QDoubleSpinBox *plotRange;
    	QCheckBox *plotDisplay;
    	QCPAxis *plotAxis;
    	QCPGraph *plotGraph;
    };

    Plot yellowLine;
    Plot blueLine;
    Plot greenLine;
    Plot redLine;

    double key = 0;
	QTimer dataTimer;
	QWidget *centralWidget;
    QCustomPlot *customPlot;
    QPushButton *pauseRunButton;
    QPushButton *runButton;
    QDoubleSpinBox *minY;
    QDoubleSpinBox *maxY;
    QSpinBox *domain;
    QLabel *label1;
    QLabel *label2;
    QLabel *label3;
    
    GraphDataSelectionBar *blackBar;

    QList<Plot> allPlots;

    

    

signals:
	

public slots:
	void on_pauseRunButton_clicked();
	void remove_data_to_scroll();
	void realtimeDataSlot();
	void setupUi(QMainWindow *GraphWindow);
	void set_line_visible();
	void setupPlots();
	void changeRanges();
	


	
};