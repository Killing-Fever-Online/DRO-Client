#ifndef EVIDENCELIST_H
#define EVIDENCELIST_H

class AOApplication;

#include <QWidget>
#include <QCheckBox>
#include <QListWidget>
#include <QLabel>
#include <QTextEdit>
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
  void setPresenting(bool toggle);
  int getCurrentSelection();

  QString getIconPath(QString f_path);

  QWidget *info_window = nullptr;

  int current_index = -1;

  bool presenting = false;
private:
  AOApplication *m_app = nullptr;

  QPushButton *close_button = nullptr;
  QPushButton *create_button = nullptr;
  QCheckBox *present_checkbox = nullptr;

  QVector<EvidenceData> *current_evi_list = nullptr;

  // Info Window vars
  QLabel *icon_label = nullptr;
  QLineEdit *name_edit = nullptr;
  QTextEdit *desc = nullptr;
  QLineEdit *image_path = nullptr;
  QCheckBox *edit_checkbox = nullptr;
  QPushButton *image_browse_button = nullptr;
  QPushButton *info_close_button = nullptr;
  QPushButton *info_apply_button = nullptr;
  QPushButton *info_delete_button = nullptr;

  QListWidget *evidence_list_widget = nullptr;

  EvidenceData edited_evidence_data;

  void setInfoDesc(QString f_description);


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
  void onInfoDescEdited();
  void onInfoEdited();

  void setInfoCanEdit(bool toggle);
};

#endif // EVIDENCELIST_H
