#include "evidencelist.h"
#include "aoapplication.h"
#include "aoguiloader.h"

#include "dro/fs/fs_reading.h"

#include <QDir>
#include <QFileDialog>

EvidenceList::EvidenceList(QWidget *parent) : QWidget(parent), m_app(AOApplication::getInstance())
{
  AOGuiLoader loader;
  loader.load_from_file(":res/ui/evidencelist.ui", this);

  setWindowTitle("Evidence List");
  // setWindowIcon();
  setWindowFlag(Qt::WindowMinMaxButtonsHint, false);
  setWindowFlag(Qt::Tool);

  this->resize(QSize(375, 350));

  close_button = this->findChild<QPushButton *>("close_button");
  create_button = this->findChild<QPushButton *>("create_button");
  present_checkbox = this->findChild<QCheckBox*>("present_checkbox");


  evidence_list_widget = this->findChild<QListWidget *>("evidence_list_widget");

  evidence_list_widget->setResizeMode(QListView::Adjust);
  evidence_list_widget->setViewMode(QListView::IconMode);
  // TODO: enable drag and drop
  evidence_list_widget->setDragEnabled(false);
  evidence_list_widget->setDragDropMode(QListWidget::NoDragDrop);
  evidence_list_widget->setDropIndicatorShown(false);
  evidence_list_widget->setAcceptDrops(false);
  //
  evidence_list_widget->setIconSize(QSize(70, 70));
  evidence_list_widget->setSpacing(5);
  evidence_list_widget->setWordWrap(true);
  evidence_list_widget->setMinimumSize(90, 90);
  evidence_list_widget->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);

  info_window = new QWidget(parent);
  info_window->setWindowFlag(Qt::WindowMinMaxButtonsHint, false);
  info_window->setWindowFlag(Qt::Tool);
  info_window->resize(QSize(430, 340));

  loader.load_from_file(":res/ui/info.ui", info_window);

  icon_label = info_window->findChild<QLabel *>("icon_label");
  name_edit = info_window->findChild<QLineEdit *>("name_edit");
  desc = info_window->findChild<QTextEdit *>("desc");

  // TODO: replace the disgusting hack with actually using QTextBrowser
  // QTextBrowser has some issues when it's made editable that'd have to be manually resolved though
  auto &clist = desc->children();
  foreach (QObject *pObj, clist)
  {
    if(pObj->inherits("QWidgetTextControl"))
      pObj->setProperty("openExternalLinks", true);
  }
  // TODO: allow this to be edited through theme stylesheets somehow.
  desc->document()->setDefaultStyleSheet("a {color: cyan;}");

  image_path = info_window->findChild<QLineEdit *>("image_path");
  image_browse_button = info_window->findChild<QPushButton *>("browse_button");

  edit_checkbox = info_window->findChild<QCheckBox *>("edit_checkbox");

  info_close_button = info_window->findChild<QPushButton *>("close_button");
  info_apply_button = info_window->findChild<QPushButton *>("apply_button");
  info_delete_button = info_window->findChild<QPushButton *>("delete_button");

  // Signals
  connect(evidence_list_widget, &QListWidget::itemDoubleClicked, this, &EvidenceList::onItemDoubleClicked);
  connect(close_button, &QPushButton::clicked, this, &EvidenceList::onCloseClicked);
  connect(create_button, &QPushButton::clicked, this, &EvidenceList::onCreateClicked);
  // TODO: Presenting Evidence
  // connect(present_checkbox, &QCheckBox::stateChanged, this, &EvidenceList::onPresentToggled);

  // Submission Actions
  connect(info_close_button, &QPushButton::clicked, this, &EvidenceList::onInfoCloseClicked);
  connect(info_apply_button, &QPushButton::clicked, this, &EvidenceList::onInfoApplyClicked);
  connect(info_delete_button, &QPushButton::clicked, this, &EvidenceList::onInfoDeleteClicked);

  // Editing Actions
  connect(edit_checkbox, &QCheckBox::stateChanged, this, &EvidenceList::setInfoCanEdit);
  connect(name_edit, &QLineEdit::textChanged, this, &EvidenceList::onInfoEdited);
  connect(image_path, &QLineEdit::textChanged, this, &EvidenceList::onInfoImageEdited);
  connect(image_browse_button, &QPushButton::clicked, this, &EvidenceList::onInfoImageBrowseRequested);
  connect(desc, &QTextEdit::textChanged, this, &EvidenceList::onInfoDescEdited);

  // Default state is non edit
  name_edit->setReadOnly(true);
  desc->setReadOnly(true);
  image_path->hide();
  image_browse_button->hide();
  info_delete_button->hide();
}

void EvidenceList::setInfoCanEdit(bool toggle)
{
  if (toggle)
  {
    name_edit->setReadOnly(false);
    desc->setReadOnly(false);
    image_path->show();
    image_browse_button->show();
    info_delete_button->show();
  }
  else
  {
    name_edit->setReadOnly(true);
    desc->setReadOnly(true);
    image_path->hide();
    image_browse_button->hide();
    info_delete_button->hide();
  }

  // make sure links are clickable
  setInfoDesc(desc->toPlainText());
}

void EvidenceList::setEvidenceList(QVector<EvidenceData> *evi_list)
{
  current_evi_list = evi_list;

  evidence_list_widget->clear();
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
                  image_path->text() != current_evidence.getImagePath() ||
                  desc->toPlainText() != current_evidence.getDesc();
    if (is_new && info_apply_button->isVisible())
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

QString EvidenceList::getIconPath(QString f_path)
{
  // TODO: cache this!!!
  QString path = m_app->get_evidence_path(f_path);
  QString evidence_image_path = m_app->find_asset_path({path});//, FS::Formats::StaticImages());
  if (evidence_image_path.isEmpty())
  {
    evidence_image_path = m_app->find_asset_path({m_app->get_evidence_path("empty.png")});
    if (evidence_image_path.isEmpty())
    {
      evidence_image_path = ":data/empty.png";
    }
  }
  return evidence_image_path;
}

void EvidenceList::addItem(EvidenceData f_evidence)
{
  QListWidgetItem *l_item = new QListWidgetItem(f_evidence.getName(), evidence_list_widget);
  l_item->setToolTip(f_evidence.getName());
  QIcon l_evidence_icon = QPixmap(this->getIconPath(f_evidence.getImagePath()));
  l_item->setIcon(l_evidence_icon);
}

void EvidenceList::setInfoWindowData(EvidenceData f_evidence)
{
  QString name = f_evidence.getName();
  name_edit->setText(name);

  QString imgpath = f_evidence.getImagePath();
  image_path->setText(imgpath);

  QString desc_text = f_evidence.getDesc();
  setInfoDesc(desc_text);

  info_window->setWindowTitle(name);
  edited_evidence_data = f_evidence;
  // we call onInfoImageEdited() here because it also sets the icon
  onInfoImageEdited();
}

void EvidenceList::setInfoDesc(QString f_description)
{
  desc->blockSignals(true);
  if (edit_checkbox->isChecked())
  {
    desc->setTextInteractionFlags(desc->textInteractionFlags() & ~Qt::LinksAccessibleByMouse);
    desc->setPlainText(f_description);//.toHtmlEscaped().replace("\n", "<br />"));
  }
  else
  {
    QString l_description = f_description.toHtmlEscaped();
    const QRegExp l_regex("(https?://[^\\s/$.?#].[^\\s]*)");
    if (l_description.contains(l_regex))
    {
      l_description.replace(l_regex, "<a href=\"\\1\">\\1</a>");
    }
    desc->setTextInteractionFlags(desc->textInteractionFlags() | Qt::LinksAccessibleByMouse);
    desc->setHtml(l_description.replace("\n", "<br />"));
  }
  desc->blockSignals(false);
}

void EvidenceList::onItemDoubleClicked(QListWidgetItem *item)
{
  if (info_apply_button->isVisible())
  {
    QMessageBox::StandardButton reply;
    QString warning = "You have unsaved changes.\n"
                      "Discard them?";
    reply = QMessageBox::warning(info_window, "Warning", warning, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (reply == QMessageBox::No)
      return;
  }
  m_current_index = evidence_list_widget->row(item);
  EvidenceData f_evidence = current_evi_list->at(m_current_index);

  setInfoWindowData(f_evidence);
  info_window->show();
  info_window->raise();
}

void EvidenceList::onCloseClicked()
{
  hide();
}

void EvidenceList::onCreateClicked()
{
  // TODO: move this somewhere else so it can be adjusted for json evidence
  m_app->send_server_packet(DRPacket("PE", {"<name>", "<description>", "empty.png"}));
}

void EvidenceList::onPresentToggled(int state)
{
  qInfo() << state;
  //m_app->
}

void EvidenceList::onInfoCloseClicked()
{
  if (info_apply_button->isVisible())
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

void EvidenceList::onInfoApplyClicked()
{
  // TODO: move this somewhere else so it can be adjusted for json evidence
  QStringList f_contents;
  f_contents.append(QString::number(m_current_index));
  f_contents.append(name_edit->text());
  f_contents.append(desc->toPlainText());
  f_contents.append(image_path->text());

  m_app->send_server_packet(DRPacket("EE", f_contents));
}

void EvidenceList::onInfoDeleteClicked()
{
  // Ctrl+Shift etc. will not be recognized as valid, you need to specifically hold shift only
  if (QGuiApplication::keyboardModifiers() != Qt::ShiftModifier)
  {
    QMessageBox::StandardButton reply;
    QString warning = "Are you sure you want to delete this evidence?\n"
                      "Hold 'Shift' when deleting to bypass this warning.";
    reply = QMessageBox::warning(info_window, "Warning", warning, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (reply == QMessageBox::No)
      return;
  }

  // TODO: move this somewhere else so it can be adjusted for json evidence
  m_app->send_server_packet(DRPacket("DE", {QString::number(m_current_index)}));
  info_window->hide();
}

void EvidenceList::onInfoImageBrowseRequested()
{
  QDir dir(FS::Paths::BasePath() + "evidence/");
  QFileDialog dialog(info_window);
  // non-native dialog doesn't support icon previews :/
  // a lot more work will be needed to put into an actual good
  // evidence icon browser

  // dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setNameFilter(tr("Images (*.png)"));
  dialog.setViewMode(QFileDialog::List);
  dialog.setDirectory(dir);

  QStringList filenames;

  if (dialog.exec())
    filenames = dialog.selectedFiles();

  if (filenames.size() != 1)
    return;

  QString filename = filenames.at(0);
  filename = dir.relativeFilePath(filename);
  image_path->setText(filename);
  onInfoImageEdited();
}

void EvidenceList::onInfoImageEdited()
{
  icon_label->setPixmap(QPixmap(this->getIconPath(image_path->text())));
  onInfoEdited();
}

void EvidenceList::onInfoDescEdited()
{
  onInfoEdited();
}

void EvidenceList::onInfoEdited()
{
  bool is_edited = name_edit->text() != edited_evidence_data.getName() ||
                   image_path->text() != edited_evidence_data.getImagePath() ||
                   desc->toPlainText() != edited_evidence_data.getDesc();
  info_apply_button->setVisible(is_edited);
}
