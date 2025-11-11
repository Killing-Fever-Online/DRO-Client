#ifndef EVIDENCELIST_H
#define EVIDENCELIST_H

class AOApplication;

#include <QWidget>
#include <QListWidget>
#include <QLabel>
#include <QTextEdit>
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
  QLabel *name_label;
  QTextEdit *desc;

  QVector<EvidenceData> *current_evi_list;

  int m_current_index = -1;

private slots:
  void onItemDoubleClicked(QListWidgetItem *item);
};

#endif // EVIDENCELIST_H
