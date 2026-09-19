#pragma once

#include "draudiostreamfamily.h"

class DRAudioEngine : public QObject
{
  Q_OBJECT

public:
  static QVector<DRAudioDevice> get_device_list();
  static std::optional<DRAudioDevice> get_current_device();
  static QString get_favorite_device_driver();
  static std::optional<DRAudioDevice> get_favorite_device();
  static DRAudioStreamFamily::ptr get_family(DRAudio::Family p_family);
  static QList<DRAudioStreamFamily::ptr> get_family_list();
  static int32_t get_volume();
  static float get_pitch();
  static float get_speed();
  static DRAudio::Options get_options();
  // option get
  static bool is_option(DRAudio::Option p_option);
  static bool is_suppressed();
  static bool is_suppress_background_audio();

  DRAudioEngine(QObject *p_parent = nullptr);
  ~DRAudioEngine();

public slots:
  void set_favorite_device_driver(QString driver);
  void set_volume(int32_t p_volume);
  void set_pitch(float p_pitch);
  void set_speed(float p_speed);
  void set_options(DRAudio::Options p_options);
  // option set
  void set_option(DRAudio::Option p_option, bool p_enabled);
  void set_suppressed(bool p_enabled);
  void set_suppress_background_audio(bool p_enabled);

signals:
  void device_list_changed(QVector<DRAudioDevice>);
  void current_device_changed(DRAudioDevice);
  void favorite_device_changed(DRAudioDevice);
  void volume_changed(int32_t);
  void pitch_changed(float);
  void speed_changed(float);
  void options_changed(DRAudio::Options);
};