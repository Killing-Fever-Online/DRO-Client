#include "blip_player.h"

#include "aoapplication.h"
#include "dro/fs/fs_reading.h"

const int AOBlipPlayer::BLIP_COUNT = 5;

AOBlipPlayer::AOBlipPlayer(QObject *p_parent)
    : AOObject(p_parent)
{
  m_family = DRAudioEngine::get_family(DRAudio::Family::FBlip);
  m_family->set_capacity(BLIP_COUNT);
}

void AOBlipPlayer::set_blips(QString p_blip)
{
  if (m_name.has_value() && m_name.value() == p_blip)
    return;

  m_name = p_blip;
  // First, test for raw asset in the sounds/blips/ folder
  QString file = ao_app->find_asset_path({ao_app->get_noext_path(p_blip, "sounds/blips")}, FS::Formats::SupportedAudio());
  if (!FS::Checks::FileExists(file))
    // Next, check the DUMB WAY of sounds/general/sfx-blipmale.wav style. But also don't forget to allow non-wav blips.
    file = ao_app->find_asset_path({ao_app->get_sfx_noext_path("sfx-blip" + p_blip)}, FS::Formats::SupportedAudio());
  m_file = file;
}

void AOBlipPlayer::set_sound(QString p_blip)
{
  if (m_name.has_value() && m_name.value() == p_blip)
    return;

  m_name = p_blip;
  // Search sound folder for the blip sfx
  QString file = ao_app->find_asset_path({ao_app->get_sfx_noext_path(p_blip)}, FS::Formats::SupportedAudio());
  m_file = file;
}

void AOBlipPlayer::blip_tick()
{
  if (!m_file.has_value())
    return;
  m_family->play_stream(m_file.value());
}
