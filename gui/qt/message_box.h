#pragma once

#include <QMessageBox>
#include <QPushButton>
#include <stdexcept>
#include <string>

namespace
{
  void show_error_message(const std::string & message, QWidget * parent)
  {
    QMessageBox mbox(QMessageBox::Critical, parent->windowTitle(),
      QString::fromStdString(message), QMessageBox::NoButton, parent);
    mbox.exec();
  }

  void show_info_message(const std::string & message, QWidget * parent)
  {
    QMessageBox mbox(QMessageBox::Information, parent->windowTitle(),
      QString::fromStdString(message), QMessageBox::NoButton, parent);
    mbox.exec();
  }

  void show_warning_message(const std::string & message, QWidget * parent)
  {
    QMessageBox mbox(QMessageBox::Warning, parent->windowTitle(),
      QString::fromStdString(message), QMessageBox::NoButton, parent);
    mbox.exec();
  }

  bool apply_and_continue(const std::string & question, QWidget * parent)
  {
    QMessageBox mbox(QMessageBox::Information, parent->windowTitle(),
      QString::fromStdString(question), QMessageBox::Apply | QMessageBox::Cancel, parent);

    mbox.setButtonText(QMessageBox::Apply, "Apply Settings");

    int button = mbox.exec();

    return button == QMessageBox::Apply;
  }

  bool confirm(const std::string & question, QWidget * parent)
  {
    QMessageBox mbox(QMessageBox::Question, parent->windowTitle(),
      QString::fromStdString(question),
      QMessageBox::Ok | QMessageBox::Cancel, parent);
    int button = mbox.exec();
    return button == QMessageBox::Ok;
  }

  void show_exception(const std::exception & e,
    const std::string & context, QWidget * parent)
  {
    std::string message;
    if (context.size() > 0)
    {
      message += context;
      message += "  ";
    }
    message += e.what();
    show_error_message(message, parent);
  }

  void show_exception(const std::exception & e, QWidget * parent)
  {
    show_exception(e, "", parent);
  }
}
