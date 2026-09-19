#pragma once

#include <QByteArray>
#include <QString>
#include <QVector>

#include <cstdint>

typedef struct ma_engine ma_engine;

namespace audio_backend
{
struct DeviceEntry
{
  uint32_t index = 0;
  QString name;
  QString driver;
  QByteArray native_id;
  bool is_default = false;
};

QVector<DeviceEntry> enumerate_devices();

bool select_device(const QByteArray &p_native_id);
QByteArray selected_device();
bool is_started();

ma_engine *engine();
void shutdown();

QString error_string(int32_t p_result);
} // namespace audio_backend
