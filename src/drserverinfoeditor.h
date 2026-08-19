#pragma once

#include "datatypes.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QSpinBox>

class DRServerInfoEditor : public QDialog
{
  Q_OBJECT

public:
  DRServerInfoEditor(QWidget *parent = nullptr);
  ~DRServerInfoEditor();

  DRServerInfo get_server_info();

public slots:
  void set_server_info(DRServerInfo p_server_info);
  void clear_server_info();

private:
  QLineEdit *ui_name = nullptr;
  QPlainTextEdit *ui_description = nullptr;
  QLineEdit *ui_address = nullptr;
  QSpinBox *ui_port = nullptr;
  QSpinBox *ui_ws_port = nullptr;
  QDialogButtonBox *ui_button_box = nullptr;
};
