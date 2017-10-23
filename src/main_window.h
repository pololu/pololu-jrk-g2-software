#include <QMainWindow>
#include "qcustomplot.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget * parent = 0);

private slots:
	void setupPlayground(QCustomPlot * customPlot);
	void on_pauseRunButton_clicked();
	void remove_data_to_scroll();
	void realtimeDataSlot();
	void setupUi();
	void retranslateUi();
	void set_line_range();
	void set_line_visible();
	void change_upper_y_range(double);
	void change_lower_y_range(double);

private:
	double lastPointKey;
	double key;
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

    QDoubleSpinBox *blueLineRange;
    QDoubleSpinBox *greenLineRange;
    QDoubleSpinBox *redLineRange;

    QCheckBox *blueLineDisplay;
    QCheckBox *greenLineDisplay;
    QCheckBox *redLineDisplay;

};