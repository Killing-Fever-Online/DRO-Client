#include "draudiodevice.h"

#include "dro/system/audio/audio_backend.h"

QVector<DRAudioDevice> DRAudioDevice::get_device_list()
{
  QVector<DRAudioDevice> r_device_list;

  const QVector<audio_backend::DeviceEntry> l_entry_list = audio_backend::enumerate_devices();
  const QByteArray l_selected = audio_backend::selected_device();

  for (const audio_backend::DeviceEntry &i_entry : l_entry_list)
  {
    DRAudioDevice l_device;
    l_device.m_id = i_entry.index;
    l_device.m_name = i_entry.name;
    l_device.m_driver = i_entry.driver;
    l_device.m_native_id = i_entry.native_id;
    l_device.m_states = SEnabled;
    if (i_entry.is_default)
    {
      l_device.m_states |= SDefault;
    }
    if (!l_selected.isEmpty() && l_selected == i_entry.native_id)
    {
      l_device.m_states |= SInit;
    }
    r_device_list.append(l_device);
  }

  return r_device_list;
}

DRAudioDevice::DRAudioDevice()
    : m_id(0)
    , m_name("<unknown>")
{}

DRAudioDevice::~DRAudioDevice()
{}

uint32_t DRAudioDevice::get_id() const
{
  return m_id;
}

QString DRAudioDevice::get_name() const
{
  return m_name;
}

QString DRAudioDevice::get_driver() const
{
  return m_driver.isEmpty() ? "NOSOUND" : m_driver;
}

QByteArray DRAudioDevice::get_native_id() const
{
  return m_native_id;
}

DRAudioDevice::States DRAudioDevice::get_states() const
{
  return m_states;
}

bool DRAudioDevice::is_state(DRAudioDevice::State p_state) const
{
  return m_states.testFlag(p_state);
}

bool DRAudioDevice::is_enabled() const
{
  return is_state(SEnabled);
}

bool DRAudioDevice::is_default() const
{
  return is_state(SDefault);
}

bool DRAudioDevice::is_init() const
{
  return is_state(SInit);
}

void DRAudioDevice::set_init(bool p_enabled)
{
  m_states.setFlag(SInit, p_enabled);
}

bool DRAudioDevice::operator==(const DRAudioDevice &other) const
{
  return m_id == other.m_id && m_name == other.m_name && m_driver == other.m_driver && m_states == other.m_states;
}

bool DRAudioDevice::operator!=(const DRAudioDevice &other) const
{
  return !operator==(other);
}
