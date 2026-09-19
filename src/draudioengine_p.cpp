#include "draudioengine_p.h"

/*!
    I could've avoided making this necessary but I didn't
    want to make the code extra complex.

    So here, instead have this very convuloted workaround
    instead.
*/
#include "draudioengine.h"

#include "dro/system/audio/audio_backend.h"

#include <QDebug>
#include <QTimer>

DRAudioEnginePrivate::DRAudioEnginePrivate()
    : QObject(nullptr)
    , update_timer(new QTimer(this))
{}

DRAudioEnginePrivate::~DRAudioEnginePrivate()
{
  update_timer->stop();

  audio_backend::shutdown();
}

void DRAudioEnginePrivate::update_current_device()
{
  update_device_list();

  DRAudioDevice l_target_device;
  for (const DRAudioDevice &i_device : std::as_const(device_list))
  {
    if (!favorite_device_driver.isEmpty() && i_device.get_driver() == favorite_device_driver)
    {
      if (!favorite_device.has_value() || favorite_device.value() != i_device)
      {
        favorite_device = i_device;
        invoke_signal("favorite_device_changed", Q_ARG(DRAudioDevice, favorite_device.value()));
      }

      if (i_device.is_enabled())
      {
        l_target_device = i_device;
        break;
      }
    }

    if (i_device.is_default())
    {
      l_target_device = i_device;
    }
  }

  if (device.has_value() && device.value() == l_target_device)
    return;

  if (device.has_value() && device->get_native_id() == l_target_device.get_native_id())
  {
    device = l_target_device;
    return;
  }

  if (!l_target_device.is_init())
  {
    if (!audio_backend::select_device(l_target_device.get_native_id()))
    {
      qWarning() << "Error: failed to initialize audio device: (device:" << l_target_device.get_name() << ")";
      return;
    }
    l_target_device.set_init(true);
  }

  // only remember the device once it initialized, so a failure retries next tick
  device = l_target_device;

  qInfo() << "Audio device changed to" << device->get_name();
  invoke_signal("current_device_changed", Q_ARG(DRAudioDevice, device.value()));
}

void DRAudioEnginePrivate::update_device_list()
{
  const QVector<DRAudioDevice> l_new_device_list = DRAudioDevice::get_device_list();
  if (l_new_device_list != device_list)
  {
    device_list = std::move(l_new_device_list);
    invoke_signal("device_list_changed", Q_ARG(QVector<DRAudioDevice>, device_list));
  }
}

void DRAudioEnginePrivate::update_options()
{
  update_volume();
  for (auto &i_group : family_map.values())
    i_group->update_rate_mode();
}

void DRAudioEnginePrivate::update_volume()
{
  for (auto &i_group : family_map.values())
    i_group->update_volume();
}

void DRAudioEnginePrivate::update_pitch()
{
  for (auto &i_group : family_map.values())
    i_group->update_pitch();
}

void DRAudioEnginePrivate::update_speed()
{
  for (auto &i_group : family_map.values())
    i_group->update_speed();
}
