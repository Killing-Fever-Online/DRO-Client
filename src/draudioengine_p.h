#pragma once

#include <QMap>
#include <QObject>
#include <QPointer>

#include "draudio.h"
#include "draudiostreamfamily.h"

class QTimer;

#include <optional>

class DRAudioEnginePrivate : public QObject
{
  Q_OBJECT

public:
  using ptr = QPointer<DRAudioEnginePrivate>;

  QObjectList children;
  QPointer<DRAudioEngine> engine;

  QPointer<QTimer> update_timer;
  QVector<DRAudioDevice> device_list;
  std::optional<DRAudioDevice> device;
  QString favorite_device_driver;
  std::optional<DRAudioDevice> favorite_device;

  int32_t volume = 0;
  DRAudio::Options options;
  QMap<DRAudio::Family, DRAudioStreamFamily::ptr> family_map;

  DRAudioEnginePrivate();
  ~DRAudioEnginePrivate();

  template <typename... Args>
  void invoke_signal(const QString &p_method_name, Args &&...args)
  {
    const auto method_name = p_method_name.toUtf8();
    for (QObject *i_child : std::as_const(children))
    {
      QMetaObject::invokeMethod(i_child, method_name.constData(), std::forward<Args>(args)...);
    }
  }

public slots:
  void update_device_list();
  void update_current_device();
  void update_volume();
  void update_options();
};
