#include "evidencelist.h"
#include "aoapplication.h"
#include "aoguiloader.h"

EvidenceList::EvidenceList(QWidget *parent) : QListWidget(parent), m_app(AOApplication::getInstance())
{
  setResizeMode(QListView::Adjust);
  setWindowTitle("Evidence List");
  // setWindowIcon();
  setWindowFlag(Qt::WindowMinMaxButtonsHint, false);
  setWindowFlag(Qt::Tool);
  setViewMode(QListView::IconMode);
  // TODO: enable drag and drop
  setDragEnabled(false);
  setDragDropMode(QListWidget::NoDragDrop);
  setDropIndicatorShown(false);
  setAcceptDrops(false);
  //
  setIconSize(QSize(70, 70));
  setSpacing(5);
  setWordWrap(true);
  setMinimumSize(90, 90);
  resize(QSize(270, 270));
  setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);

  info_window = new QWidget(parent);
  info_window->setWindowFlag(Qt::WindowMinMaxButtonsHint, false);
  info_window->setWindowFlag(Qt::Tool);
  info_window->resize(QSize(356, 300));

  AOGuiLoader loader;
  loader.load_from_file(":res/ui/info.ui", info_window);

  icon_label = info_window->findChild<QLabel *>("icon_label");
  name_edit = info_window->findChild<QLineEdit *>("name_edit");
  desc = info_window->findChild<QTextEdit *>("desc");
  close_button = info_window->findChild<QPushButton *>("close_button");
  save_button = info_window->findChild<QPushButton *>("save_button");
  delete_button = info_window->findChild<QPushButton *>("delete_button");

  connect(this, &QListWidget::itemDoubleClicked, this, &EvidenceList::onItemDoubleClicked);
  connect(close_button, &QPushButton::clicked, this, &EvidenceList::onCloseClicked);
  connect(save_button, &QPushButton::clicked, this, &EvidenceList::onSaveClicked);
  connect(delete_button, &QPushButton::clicked, this, &EvidenceList::onDeleteClicked);

  connect(name_edit, &QLineEdit::textChanged, this, &EvidenceList::onEdited);
  connect(desc, &QTextEdit::textChanged, this, &EvidenceList::onEdited);
}

void EvidenceList::setEvidenceList(QVector<EvidenceData> *evi_list)
{
  current_evi_list = evi_list;

  clear();
  for (const EvidenceData &f_evidence : *current_evi_list)
  {
    addItem(f_evidence);
  }

  int size = current_evi_list->length();
  if (size <= 0)
  {
    m_current_index = -1;
    info_window->hide();
    return;
  }

  if (m_current_index >= size)
  {
    m_current_index = 0;
  }

  if (!info_window->isHidden())
  {
    EvidenceData current_evidence = current_evi_list->at(m_current_index);
    bool is_new = name_edit->text() != current_evidence.getName() ||
                     desc->toPlainText() != current_evidence.getDesc();
    if (is_new && save_button->isVisible())
    {
      QMessageBox *msgBox = new QMessageBox;

      msgBox->setAttribute(Qt::WA_DeleteOnClose);
      msgBox->setText(tr("The piece of evidence you've been editing has changed."));
      msgBox->setInformativeText(tr("Do you wish to keep your changes?"));
      msgBox->setDetailedText(tr(
                                  "Name: %1\n"
                                  "Image: %2\n"
                                  "Description:\n%3").arg(
                                      current_evidence.getName(),
                                      current_evidence.getImagePath(),
                                      current_evidence.getDesc())
                              );
      msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
      msgBox->setDefaultButton(QMessageBox::LastButton);
      // msgBox->setWindowModality(Qt::NonModal);
      int ret = msgBox->exec();
      switch (ret) {
      case QMessageBox::Yes:
        // "Keep changes"
        return;
        break;
      case QMessageBox::No:
      default:
        break;
      }
    }
    setInfoWindowData(current_evidence);
  }
}

void EvidenceList::addItem(EvidenceData f_evidence)
{
  QListWidgetItem *l_item = new QListWidgetItem(f_evidence.getName(), this);
  l_item->setToolTip(f_evidence.getName());

  // TODO: cache this!!!
  QString path = m_app->get_evidence_path(f_evidence.getImagePath());
  QString evidence_image_path = m_app->find_asset_path({path});//, FS::Formats::StaticImages());
  if (evidence_image_path.isEmpty())
  {
    evidence_image_path = m_app->find_asset_path({m_app->get_evidence_path("empty.png")});
    if (evidence_image_path.isEmpty())
    {
      evidence_image_path = ":data/empty.png";
    }
  }
  QIcon l_evidence_icon = QPixmap(evidence_image_path);
  l_item->setIcon(l_evidence_icon);
}

void EvidenceList::setInfoWindowData(EvidenceData f_evidence)
{
  // TODO: cache this!!!
  QString path = m_app->get_evidence_path(f_evidence.getImagePath());
  QString evidence_image_path = m_app->find_asset_path({path});//, FS::Formats::StaticImages());
  if (evidence_image_path.isEmpty())
  {
    evidence_image_path = m_app->find_asset_path({m_app->get_evidence_path("empty.png")});
    if (evidence_image_path.isEmpty())
    {
      evidence_image_path = ":data/empty.png";
    }
  }
  icon_label->setPixmap(QPixmap(evidence_image_path));

  QString name = f_evidence.getName();
  name_edit->setText(name);

  QString desc_text = f_evidence.getDesc();
  desc->setText(desc_text);

  info_window->setWindowTitle(name);
  edited_evidence_data = f_evidence;
  onEdited();
}

void EvidenceList::onItemDoubleClicked(QListWidgetItem *item)
{
  m_current_index = row(item);
  EvidenceData f_evidence = current_evi_list->at(m_current_index);

  setInfoWindowData(f_evidence);
  info_window->show();
}

void EvidenceList::onCloseClicked()
{
  if (save_button->isVisible())
  {
    QMessageBox::StandardButton reply;
    QString warning = "You have unsaved changes.\n"
                      "Discard them?";
    reply = QMessageBox::warning(info_window, "Warning", warning, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (reply == QMessageBox::No)
      return;
  }
  info_window->hide();
}

void EvidenceList::onSaveClicked()
{
  // TODO: move this somewhere else so it can be adjusted for json evidence
  QStringList f_contents;
  f_contents.append(QString::number(m_current_index));
  f_contents.append(name_edit->text());
  f_contents.append(desc->toPlainText());
  f_contents.append(edited_evidence_data.getImagePath());

  m_app->send_server_packet(DRPacket("EE", f_contents));
}

void EvidenceList::onDeleteClicked()
{
  // TODO: move this somewhere else so it can be adjusted for json evidence
  m_app->send_server_packet(DRPacket("DE", {QString::number(m_current_index)}));
  info_window->hide();
}

void EvidenceList::onEdited()
{
  bool is_edited = name_edit->text() != edited_evidence_data.getName() ||
                   desc->toPlainText() != edited_evidence_data.getDesc();
  save_button->setVisible(is_edited);
}
