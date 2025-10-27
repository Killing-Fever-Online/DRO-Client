#pragma once
#pragma once

#include <QEvent>
#include <QApplication>
#include <QKeyEvent>
#include <QLineEdit>

class RPLineEditFilter : public QObject
{
  Q_OBJECT

signals:
  void chat_return_pressed();

public:
  bool text_edit_preserve_selection = false;
  bool rate_limited = false;

protected:
  bool eventFilter(QObject *obj, QEvent *event) override;
};
