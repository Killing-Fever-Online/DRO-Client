#ifndef EVIDENCELIST_H
#define EVIDENCELIST_H

class AOApplication;

#include <QWidget>
#include <QCheckBox>
#include <QListWidget>
#include <QLabel>
#include <QTextBrowser>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include "dro/param/evidence/evidence_data.h"

class EvidenceList : public QWidget
{
  Q_OBJECT
public:
  explicit EvidenceList(QWidget *parent = nullptr);
  void addItem(EvidenceData f_evidence);
  void setInfoWindowData(EvidenceData f_evidence);
  void setEvidenceList(QVector<EvidenceData> *evi_list);

  QWidget *info_window;

private:
  AOApplication *m_app;

  QPushButton *close_button;
  QPushButton *create_button;
  QCheckBox *present_checkbox;

  QVector<EvidenceData> *current_evi_list;

  // Info Window vars
  QLabel *icon_label;
  QLineEdit *name_edit;
  QTextBrowser *desc;
  QLineEdit *image_path;
  QPushButton *image_browse_button;
  int m_current_index = -1;
  QPushButton *info_close_button;
  QPushButton *info_apply_button;
  QPushButton *info_delete_button;

  QListWidget *evidence_list_widget;

  EvidenceData edited_evidence_data;

  QString getIconPath(QString f_path);

private slots:
  void onItemDoubleClicked(QListWidgetItem *item);

  void onCloseClicked();
  void onCreateClicked();
  void onPresentToggled(int state);

  // Info Window slots
  void onInfoCloseClicked();
  void onInfoApplyClicked();
  void onInfoDeleteClicked();

  void onInfoImageBrowseRequested();

  void onInfoImageEdited();
  void onInfoEdited();
};

#endif // EVIDENCELIST_H
