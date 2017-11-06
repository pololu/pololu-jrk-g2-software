#include <QMainWindow>
<<<<<<< Updated upstream
<<<<<<< Updated upstream
=======
#include <QList>
>>>>>>> Stashed changes
=======
#include <QList>
>>>>>>> Stashed changes
#include "qcustomplot.h"

class GraphWindow : public QMainWindow
{
	Q_OBJECT
public:
<<<<<<< Updated upstream
<<<<<<< Updated upstream
	MainWindow(QWidget * parent = 0);
	double lastPointKey;
	double key;
=======
=======
>>>>>>> Stashed changes
	GraphWindow(QMainWindow * parent = 0);

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

private slots:
	void on_pauseRunButton_clicked();
	void remove_data_to_scroll();
	void realtimeDataSlot();
	void setupUi();
	void set_line_visible();
	void setRanges();

private:
	double key = 0;
<<<<<<< Updated upstream
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
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

<<<<<<< Updated upstream
<<<<<<< Updated upstream
private slots:
	void setupPlayground(QCustomPlot * customPlot);
	void on_pauseRunButton_clicked();
	void remove_data_to_scroll();
	void realtimeDataSlot();
	void setupUi();
	void retranslateUi();
	void set_line_range();
	void set_line_visible();
	void change_upper_y_range();
	void change_lower_y_range(double);

private:
	QDoubleSpinBox *blueLineRange;
    QDoubleSpinBox *greenLineRange;
    QDoubleSpinBox *redLineRange;

    QCheckBox *blueLineDisplay;
    QCheckBox *greenLineDisplay;
    QCheckBox *redLineDisplay;
=======
    QList<Plot> allPlots;
>>>>>>> Stashed changes
=======
    QList<Plot> allPlots;
>>>>>>> Stashed changes
};