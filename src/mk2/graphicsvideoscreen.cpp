/**************************************************************************
**
** mk2
** Copyright (C) 2022 Tricky Leifa
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU Affero General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Affero General Public License for more details.
**
** You should have received a copy of the GNU Affero General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
**************************************************************************/
#include "graphicsvideoscreen.h"

#include <QAudioOutput>
#include <QMediaDevices>
#include <QStyleOptionGraphicsItem>
#include <QUrl>

DRVideoScreen::DRVideoScreen(AOApplication *ao_app, QGraphicsItem *parent)
    : QGraphicsVideoItem(parent), ao_app(ao_app), m_config(new AOConfig(this)), m_engine(new DRAudioEngine(this)),
      m_family(m_engine->get_family(DRAudio::Family::FVideo)), m_scanned(false), m_video_available(false),
      m_running(false), m_player(new QMediaPlayer(this))
{
  setAspectRatioMode(Qt::KeepAspectRatioByExpanding);

  m_player->setVideoOutput(this);

  connect(m_player, &QMediaPlayer::hasVideoChanged, this, &DRVideoScreen::update_video_availability);
  connect(m_player, &QMediaPlayer::mediaStatusChanged, this, &DRVideoScreen::check_status);
  connect(m_player, &QMediaPlayer::playbackStateChanged, this, &DRVideoScreen::check_state);

  connect(m_engine, &DRAudioEngine::current_device_changed, this, &DRVideoScreen::update_audio_output);
  connect(m_config, &AOConfig::video_volume_changed, this, &DRVideoScreen::update_volume);
  connect(m_engine, &DRAudioEngine::volume_changed, this, &DRVideoScreen::update_volume);
  connect(m_engine, &DRAudioEngine::options_changed, this, &DRVideoScreen::update_volume);
  connect(m_family.get(), &DRAudioStreamFamily::volume_changed, this, &DRVideoScreen::update_volume);
  connect(m_family.get(), &DRAudioStreamFamily::options_changed, this, &DRVideoScreen::update_volume);

  update_audio_output();
}

DRVideoScreen::~DRVideoScreen()
{}

QString DRVideoScreen::get_file_name() const
{
  return m_file_name;
}

void DRVideoScreen::set_file_name(QString p_file_name)
{
  if (m_file_name == p_file_name)
  {
    return;
  }
  stop();
  qInfo() << "loading media file" << p_file_name;
  m_scanned = false;
  m_video_available = false;
  m_file_name = p_file_name;
  if (m_file_name.isEmpty())
  {
    m_scanned = true;
  }
  m_player->setSource(QUrl::fromLocalFile(m_file_name));
}

bool DRVideoScreen::set_character_video(QString p_character, QString p_video)
{
  QStringList l_filepath_list;
  const QString l_video_path = QString("videos/%1").arg(p_video);
  for (const QString &i_character_name : ao_app->get_char_include_tree(p_character))
  {
    l_filepath_list.append(ao_app->get_character_path(i_character_name, l_video_path));
  }

  const QString l_filepath = ao_app->find_asset_path(l_filepath_list);
  if (l_filepath.isEmpty())
  {
    set_file_name("");
    return false;
  }
  set_file_name(l_filepath);
  return true;
}

void DRVideoScreen::play_character_video(QString p_character, QString p_video)
{
  if (!set_character_video(p_character, p_video))
  {
    qWarning() << "error: no character media file" << p_character << p_video;
    finish_playback();
    return;
  }
  play();
}

void DRVideoScreen::play()
{
  stop();
  m_running = true;
  if (!m_scanned)
  {
    return;
  }
  if (!m_video_available)
  {
    finish_playback();
    return;
  }
  start_playback();
}

void DRVideoScreen::stop()
{
  m_running = false;
  if (m_player->playbackState() != QMediaPlayer::StoppedState)
  {
    m_player->stop();
  }
}

void DRVideoScreen::update_video_availability(bool p_video_available)
{
  m_video_available = p_video_available;
}

void DRVideoScreen::check_status(QMediaPlayer::MediaStatus p_status)
{
  if (m_running)
  {
    switch (p_status)
    {
    case QMediaPlayer::InvalidMedia:
      m_scanned = true;
      qWarning() << "error: media file is invalid:" << m_file_name;
      finish_playback();
      break;

    case QMediaPlayer::NoMedia:
      m_scanned = true;
      finish_playback();

    case QMediaPlayer::LoadedMedia:
      m_scanned = true;
      if (m_video_available)
      {
        start_playback();
      }
      else
      {
        finish_playback();
      }
      break;

    default:
      break;
    }
  }
}

void DRVideoScreen::check_state(QMediaPlayer::PlaybackState p_state)
{
  switch (p_state)
  {
  case QMediaPlayer::PlayingState:
    emit started();
    break;

  case QMediaPlayer::StoppedState:
    if (m_running)
    {
      finish_playback();
    }
    break;

  default:
    break;
  }
}

void DRVideoScreen::start_playback()
{
  if (m_player->playbackState() == QMediaPlayer::StoppedState)
  {
    update_audio_output();

    m_player->play();
  }
}

void DRVideoScreen::finish_playback()
{
  stop();
  emit finished();
}

void DRVideoScreen::update_audio_output()
{
  const auto l_target_device = m_engine->get_current_device();
  if (!l_target_device.has_value())
  {
    qWarning() << "error: no device to switch to";
    return;
  }

  // Find matching QAudioDevice from available outputs
  const QList<QAudioDevice> l_device_list = QMediaDevices::audioOutputs();
  QAudioDevice l_matched_device;

  for (const QAudioDevice &i_device : l_device_list)
  {
    const QString l_device_id = QString::fromUtf8(i_device.id());
    const QString l_device_desc = i_device.description();

    if (l_device_id == l_target_device->get_name() || l_device_id == l_target_device->get_driver() ||
        l_device_desc == l_target_device->get_name() || l_device_desc == l_target_device->get_driver())
    {
      l_matched_device = i_device;
      break;
    }
  }

  if (l_matched_device.isNull())
  {
    qWarning() << "error: no matching audio device found, device unchanged";
    return;
  }

  qDebug() << "Media player changed audio device to" << l_target_device->get_name();

  if (!m_audio_output)
  {
    m_audio_output = new QAudioOutput(this);
    m_player->setAudioOutput(m_audio_output);
  }

  m_audio_output->setDevice(l_matched_device);

  update_volume();
}

void DRVideoScreen::update_volume()
{
  int l_volume = (m_family->get_volume() * m_engine->get_volume()) / 100;

  if (!m_family->is_ignore_suppression() && (m_family->is_suppressed() || m_engine->is_suppressed()))
  {
    l_volume = 0;
  }

  if (m_player->audioOutput()->volume() == l_volume)
  {
    return;
  }
  m_player->audioOutput()->setVolume(l_volume);
}
