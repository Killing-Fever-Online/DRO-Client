#include "bgm_menu.h"

#include <QListWidget>

#include <modules/theme/thememanager.h>
#include "courtroom.h"
#include "dro/network/metadata/tracklist_metadata.h"
#include "dro/interface/courtroom_layout.h"

BGMMenu::BGMMenu(QWidget *parent) : QMenu(parent)
{

  p_PlayAction = addAction(tr("Play"));

  QMenu *playMenu = new QMenu(tr("Settings"), this);
  addMenu(playMenu);

  m_PlaySmooth = playMenu->addAction((tr("Smooth Play")));
  m_PlayInstant = playMenu->addAction((tr("Instant Play")));
  m_PlaySync = playMenu->addAction((tr("Sync Playback")));
  m_PlayCrossFade = playMenu->addAction((tr("Radio CrossFade")));

  m_PlaySmooth->setCheckable(true);
  m_PlayInstant->setCheckable(true);
  m_PlaySync->setCheckable(true);
  m_PlayCrossFade->setCheckable(true);

  // Smooth play is enabled by default
  m_PlaySmooth->setChecked(true);

  p_InsertAction = addAction(tr("Insert into OOC"));

  addSeparator();

  p_StopAction = addAction(tr("Stop"));

  addSeparator();

  p_PinAction = addAction(tr("Pin"));

  connect(m_PlaySmooth, &QAction::triggered, this, &BGMMenu::OnSmoothPlayAction);
  connect(m_PlayInstant, &QAction::triggered, this, &BGMMenu::OnInstantPlayAction);
  connect(m_PlaySync, &QAction::triggered, this, &BGMMenu::OnSyncPlayAction);
  connect(m_PlayCrossFade, &QAction::triggered, this, &BGMMenu::OnCrossFadePlayAction);
  connect(p_PlayAction, &QAction::triggered, this, &BGMMenu::OnPlayTriggered);
  connect(p_InsertAction, &QAction::triggered, this, &BGMMenu::OnInsertTriggered);
  connect(p_StopAction, &QAction::triggered, this, &BGMMenu::OnStopTriggered);
  connect(p_PinAction, &QAction::triggered, this, &BGMMenu::OnPinTriggered);
}

void BGMMenu::OnMenuRequested(QPoint p_point)
{
  QListWidget *musicList = ThemeManager::get().GetWidgetType<QListWidget>("music_list");
  if(musicList == nullptr) return;

  QListWidgetItem *l_item = musicList->currentItem();

  if (!l_item) { m_TargetTrack = "";  return; }

  m_TargetTrack = l_item->data(Qt::UserRole).toString();
  const QPoint l_global_point = musicList->viewport()->mapToGlobal(p_point);
  popup(l_global_point);
}

void BGMMenu::OnPlayTriggered()
{
  if(m_TargetTrack.isEmpty()) return;
  Courtroom *courtroom = AOApplication::getInstance()->get_courtroom();
  courtroom->send_mc_packet(m_TargetTrack, courtroom->get_bgm_playback_type());
  courtroom::ic::focusMessageBox();
}

void BGMMenu::OnInsertTriggered()
{
  if(m_TargetTrack.isEmpty()) return;
  QLineEdit *oocChat = ThemeManager::get().GetWidgetType<QLineEdit>("ooc_chat_message");
  oocChat->insert(m_TargetTrack);
  oocChat->setFocus();
}

void BGMMenu::OnStopTriggered()
{
  Courtroom *courtroom = AOApplication::getInstance()->get_courtroom();
  // Send a blank song
  courtroom->send_mc_packet("", courtroom->get_bgm_playback_type());
  courtroom::ic::focusMessageBox();
}

void BGMMenu::OnSmoothPlayAction()
{
  AOApplication::getInstance()->get_courtroom()->set_bgm_playback_type(BGMPlayback_Standard);
  m_PlaySmooth->setChecked(true);
  m_PlayInstant->setChecked(false);
  m_PlaySync->setChecked(false);
  m_PlayCrossFade->setChecked(false);
}

void BGMMenu::OnInstantPlayAction()
{
  AOApplication::getInstance()->get_courtroom()->set_bgm_playback_type(BGMPlayback_NoFade);
  m_PlaySmooth->setChecked(false);
  m_PlayInstant->setChecked(true);
  m_PlaySync->setChecked(false);
  m_PlayCrossFade->setChecked(false);
}

void BGMMenu::OnSyncPlayAction()
{
  AOApplication::getInstance()->get_courtroom()->set_bgm_playback_type(BGMPlayback_Continue);
  m_PlaySmooth->setChecked(false);
  m_PlayInstant->setChecked(false);
  m_PlaySync->setChecked(true);
  m_PlayCrossFade->setChecked(false);
}

void BGMMenu::OnCrossFadePlayAction()
{
  AOApplication::getInstance()->get_courtroom()->set_bgm_playback_type(BGMPlayback_CrossFade);
  m_PlaySmooth->setChecked(false);
  m_PlayInstant->setChecked(false);
  m_PlaySync->setChecked(false);
  m_PlayCrossFade->setChecked(true);
}

void BGMMenu::OnPinTriggered()
{
  if(m_TargetTrack.isEmpty()) return;
  TracklistMetadata::PinTrack(m_TargetTrack);
}

