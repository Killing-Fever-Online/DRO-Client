#pragma once

#include <QByteArray>
#include <QMetaType>
#include <QString>
#include <QVector>

#include <cstdint>
#include <optional>

class DRAudioDevice
{
public:
  static QVector<DRAudioDevice> get_device_list();

  enum State : uint32_t
  {
    SEnabled = 0x1,
    SDefault = 0x2,
    SInit = 0x4,
  };
  Q_DECLARE_FLAGS(States, State)

  DRAudioDevice();
  ~DRAudioDevice();

  uint32_t get_id() const;
  QString get_name() const;
  QString get_driver() const;
  QByteArray get_native_id() const;
  States get_states() const;
  bool is_state(State state) const;
  bool is_enabled() const;
  bool is_default() const;
  bool is_init() const;

  void set_init(bool p_enabled);

  bool operator==(const DRAudioDevice &other) const;
  bool operator!=(const DRAudioDevice &other) const;

private:
  uint32_t m_id;
  QString m_name;
  QString m_driver;
  QByteArray m_native_id;
  States m_states;
};
Q_DECLARE_METATYPE(DRAudioDevice)
