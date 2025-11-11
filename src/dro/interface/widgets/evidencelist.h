#ifndef EVIDENCELIST_H
#define EVIDENCELIST_H

class AOApplication;

#include <QWidget>
#include <QListWidget>
#include <QLabel>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include "dro/param/evidence/evidence_data.h"

class EvidenceList : public QListWidget
{
  Q_OBJECT
public:
  explicit EvidenceList(QWidget *parent = nullptr);
  void addItem(EvidenceData f_evidence);
  void setInfoWindowData(EvidenceData f_evidence);
  void setEvidenceList(QVector<EvidenceData> *evi_list);

private:
  AOApplication *m_app;

  QWidget *info_window;
  QLabel *icon_label;
  QLineEdit *name_edit;
  QTextEdit *desc;
  QPushButton *close_button;
  QPushButton *save_button;
  QPushButton *delete_button;

  QVector<EvidenceData> *current_evi_list;

  EvidenceData edited_evidence_data;

  int m_current_index = -1;

private slots:
  void onItemDoubleClicked(QListWidgetItem *item);
  void onCloseClicked();
  void onSaveClicked();
  void onDeleteClicked();
  void onEdited();
};

#endif // EVIDENCELIST_H
