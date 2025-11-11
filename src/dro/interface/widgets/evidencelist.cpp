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
  resize(QSize(256, 256));
  setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);

  info_window = new QWidget(parent);
  info_window->setWindowFlag(Qt::WindowMinMaxButtonsHint, false);
  info_window->setWindowFlag(Qt::Tool);
  AOGuiLoader loader;
  loader.load_from_file(":res/ui/info.ui", info_window);

  icon_label = info_window->findChild<QLabel *>("icon_label");
  name_label = info_window->findChild<QLabel *>("name_label");
  desc = info_window->findChild<QTextEdit *>("desc");

  connect(this, &QListWidget::itemDoubleClicked, this, &EvidenceList::onItemDoubleClicked);
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
    setInfoWindowData(current_evi_list->at(m_current_index));
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
  name_label->setText(name);

  QString desc_text = f_evidence.getDesc();
  desc->setText(desc_text);

  info_window->setWindowTitle(name);
}

void EvidenceList::onItemDoubleClicked(QListWidgetItem *item)
{
  m_current_index = row(item);
  EvidenceData f_evidence = current_evi_list->at(m_current_index);

  setInfoWindowData(f_evidence);
  info_window->show();
}
