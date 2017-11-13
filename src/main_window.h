#include <QMainWindow>
#include <QList>
#include "qcustomplot.h"

class GraphWindow;

class OtherWindow : public QWidget
{
	Q_OBJECT

public:
	OtherWindow(QWidget * parent = 0);

	
    QPushButton *openOther;
	QWidget *centralWidget;   
    GraphWindow *newWindow;
    GraphWindow *previewWindow;
   	QTimer dataTimer; 
   

public slots:
	
	void on_openOther_clicked();

private:
	
};

class GraphWindow : public QWidget
{
	Q_OBJECT
public:
	GraphWindow(QWidget * parent = 0);

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

    QGridLayout *controlLayout;

    QList<Plot> allPlots;

public slots:
	void on_pauseRunButton_clicked();
	void remove_data_to_scroll();
	void realtimeDataSlot();
	void setupUi();
	void set_line_visible();
	void setupPlots();
	void changeRanges();

private:
	
};