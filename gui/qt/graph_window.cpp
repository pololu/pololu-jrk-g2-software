#include "graph_window.h"
#include "main_window.h"
#include "graph_widget.h"

#include <iostream>

graph_window::graph_window(QWidget *parent)
{
  setup_ui();
}

void graph_window::setup_ui()
{
  setObjectName("graph_window");

  setWindowTitle(tr("Pololu Jrk G2 Configuration Utility - Graph"));

  central_layout = new QGridLayout();

  menu_bar = new QMenuBar();

  options_menu = menu_bar->addMenu(tr("Options"));

  save_pdf_action = new QAction(this);
  save_pdf_action->setText(tr("&Save PDF"));
  save_pdf_action->setShortcut(Qt::CTRL + Qt::Key_S);

  dark_theme_action = new QAction(this);
  dark_theme_action->setText(tr("&Use dark theme"));

  default_theme_action = new QAction(this);
  default_theme_action->setText(tr("&Use default theme"));

  options_menu->addAction(save_pdf_action);
  options_menu->addAction(dark_theme_action);

  connect(save_pdf_action, &QAction::triggered, this,
    &graph_window::save_pdf);

  connect(dark_theme_action, &QAction::triggered, this,
    &graph_window::switch_to_dark);

  connect(default_theme_action, &QAction::triggered, this,
      &graph_window::switch_to_default);

  central_layout->setMenuBar(menu_bar);

  setLayout(central_layout);
}

void graph_window::closeEvent(QCloseEvent *event)
{
  central_layout->removeWidget(grabbed_widget);
  emit pass_widget();

  QWidget::closeEvent(event);
}

void graph_window::receive_widget(graph_widget *widget)
{
  grabbed_widget = widget;

  grabbed_widget->set_preview_mode(false);

  grabbed_widget->plot_visible_layout->setParent(0);

  central_layout->addWidget(grabbed_widget->custom_plot,
    1, 0, 2, 2);
  central_layout->addLayout(grabbed_widget->plot_visible_layout,
    1, 2, Qt::AlignTop | Qt::AlignRight);

  central_layout->setColumnMinimumWidth(0,
    grabbed_widget->plot_visible_layout->sizeHint().height());

   central_layout->setColumnStretch(1, 3);
   central_layout->setRowStretch(2, 3);
}

// Shows the window and directs focus to it.
//
// We use "showNormal" instead of "show" to fix a Qt bug that happens when the
// window is maximized, closed, and then reopened.  The bug causes the window be
// maximized while only painting controls in its upper left corner.
void graph_window::raise_window()
{
  if (!isVisible())
  {
    showNormal();
  }

  raise();

  activateWindow();
}

void graph_window::save_pdf()
{
  bool temp_pause = grabbed_widget->graph_paused;

  grabbed_widget->graph_paused = true;

  QPrinter printer(QPrinter::HighResolution);
  printer.setOutputFormat(QPrinter::PdfFormat);

  QString fileName = QFileDialog::getSaveFileName((QWidget* )0,
    "Export PDF", QString(), "*.pdf");

  if (fileName.isEmpty())
  {
    return;
  }

  if (QFileInfo(fileName).suffix().isEmpty())
  {
    fileName.append(".pdf");
  }

  printer.setOutputFileName(fileName);
  printer.setPageMargins(10, 10, 10, 10, QPrinter::Millimeter);
  printer.setPageOrientation(QPageLayout::Landscape);
  printer.setFullPage(false);

  QPainter painter(&printer);
  painter.setRenderHint(QPainter::TextAntialiasing);

  QFont font;
  font.setPixelSize(12.0);
  font.setUnderline(true);

  QLabel * position_label = new QLabel(tr("Position"));

  QLabel * scale_label = new QLabel(tr("Scale"));

  position_label->setFont(font);
  scale_label->setFont(font);

  QGridLayout * temp_table = new QGridLayout();
  temp_table->setHorizontalSpacing(10);
  temp_table->addWidget(position_label, 0, 1);
  temp_table->addWidget(scale_label, 0, 2);

  int i = 1;

  for (;i < grabbed_widget->all_plots.count(); ++i)
  {
    int column = 0;

    QFont font;
    font.setPointSizeF(10.0);

    QString color = grabbed_widget->all_plots[i]->default_color;

    if (dark_theme)
    {
      color = grabbed_widget->all_plots[i]->dark_color;
    }

    QLabel * display = new QLabel();
    display->setStyleSheet("QLabel{border: 2px solid "+ color + ";}");
    QLabel * center_value = new QLabel();
    QLabel * range = new QLabel();

    display->setFont(font);
    center_value->setFont(font);
    range->setFont(font);

    display->setText(grabbed_widget->all_plots[i]->display->text());
    center_value->setText(grabbed_widget->all_plots[i]->center_value->cleanText());
    range->setText(grabbed_widget->all_plots[i]->range->cleanText());
    temp_table->addWidget(display, i, column);
    temp_table->addWidget(center_value, i, ++column);
    temp_table->addWidget(range, i, ++column);

    temp_table->setRowStretch(i, 0);
  }

  QWidget * temp_widget = new QWidget();
  temp_widget->setLayout(temp_table);

  qreal xscale = printer.pageRect().width() /
    qreal(grabbed_widget->custom_plot->width() + temp_widget->width());
  qreal yscale = printer.pageRect().height() /
    qreal(grabbed_widget->custom_plot->height() + temp_widget->height());
  qreal scale = qMin(xscale * 2, yscale * 2);
  painter.translate(printer.paperRect().x() + printer.pageRect().width()/ 2,
                    printer.paperRect().y() + printer.pageRect().height()/2);
  painter.scale(scale, scale);
  painter.translate(-width()/ 2, -height()/ 2);

  QPixmap p = grabbed_widget->custom_plot->grab();
  painter.drawPixmap(20, 0, p);

  QPixmap pix = temp_widget->grab();
  painter.drawPixmap(p.width()/2 + 40, 20, pix);

  grabbed_widget->graph_paused = temp_pause;
}

void graph_window::switch_to_dark()
{
  for (auto plot : grabbed_widget->all_plots)
  {
    plot->display->setStyleSheet("border: 2px solid "+ plot->dark_color + ";"
      "padding: 2px;"
      "background-color: white;");
    plot->graph->setPen(QPen(plot->dark_color));
    plot->axis->setTickLabelColor(plot->dark_color);
  }

  QLinearGradient axis_rect_gradient;
  axis_rect_gradient.setStart(0, 0);
  axis_rect_gradient.setFinalStop(0, 350);
  axis_rect_gradient.setColorAt(0, QColor(30, 30, 30));
  axis_rect_gradient.setColorAt(1, QColor(10, 10, 10));
  grabbed_widget->custom_plot->axisRect()->setBackground(axis_rect_gradient);
  grabbed_widget->custom_plot->setBackground(QColor(170, 170, 170));
  grabbed_widget->custom_plot->xAxis->grid()->setPen(QPen(QColor(200, 200, 200, 140), 0, Qt::SolidLine));
  grabbed_widget->custom_plot->yAxis->grid()->setPen(QPen(QColor(200, 200, 200, 140), 0, Qt::SolidLine));
  grabbed_widget->custom_plot->xAxis->setBasePen(QPen(QColor(Qt::white), 1, Qt::SolidLine));
  grabbed_widget->custom_plot->yAxis->setBasePen(QPen(QColor(Qt::white), 1, Qt::SolidLine));
  grabbed_widget->custom_plot->yAxis->grid()->setZeroLinePen(QPen(QColor(200, 200, 200, 140), 0, Qt::SolidLine));

  options_menu->removeAction(dark_theme_action);
  options_menu->addAction(default_theme_action);

  dark_theme = true;

  grabbed_widget->custom_plot->replot();
}

void graph_window::switch_to_default()
{
  for (auto plot : grabbed_widget->all_plots)
  {
    plot->display->setStyleSheet("border: 2px solid "+ plot->default_color + ";"
      "padding: 2px;"
      "background-color: white;");
    plot->graph->setPen(QPen(plot->default_color));
    plot->axis->setTickLabelColor(plot->default_color);
  }

  grabbed_widget->custom_plot->axisRect()->setBackground(QColor(Qt::white));
  grabbed_widget->custom_plot->setBackground(QColor(Qt::white));
  grabbed_widget->custom_plot->xAxis->grid()->setPen(QPen(QColor(100, 100, 100, 140), 0, Qt::SolidLine));
  grabbed_widget->custom_plot->yAxis->grid()->setPen(QPen(QColor(100, 100, 100, 140), 0, Qt::SolidLine));
  grabbed_widget->custom_plot->xAxis->setBasePen(QPen(QColor(Qt::black), 1, Qt::SolidLine));
  grabbed_widget->custom_plot->yAxis->setBasePen(QPen(QColor(Qt::black), 1, Qt::SolidLine));
  grabbed_widget->custom_plot->yAxis->grid()->setZeroLinePen(QPen(QColor(100, 100, 100, 140), 0, Qt::SolidLine));

  options_menu->removeAction(default_theme_action);
  options_menu->addAction(dark_theme_action);

  dark_theme = false;

  grabbed_widget->custom_plot->replot();
}
