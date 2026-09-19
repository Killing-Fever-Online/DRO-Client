#pragma once

#include "draudio.h"
#include "draudiodevice.h"
#include "draudioerror.h"

#include <QEnableSharedFromThis>
#include <QObject>
#include <QSharedPointer>

#include <memory>
#include <optional>

class DRAudioEngine;
class DRAudioEnginePrivate;
class DRAudioStreamFamily;
class DRAudioStreamPrivate;
class QTimer;

class DRAudioStream
    : public QObject
    , public QEnableSharedFromThis<DRAudioStream>
{
  Q_OBJECT

  DRAudioStream(DRAudio::Family p_family);

public:
  enum Fade
  {
    NoFade,
    FadeIn,
    FadeOut,
  };
  Q_ENUM(Fade)

  using ptr = QSharedPointer<DRAudioStream>;

  static void registerMetatypes();

  ~DRAudioStream();

  DRAudio::Family get_family() const;
  QString get_file_name() const;
  bool is_repeatable() const;
  bool is_playing() const;
  DRAudioStream::Fade get_fade() const;

public slots:
  std::optional<DRAudioError> set_file_name(QString file);
  std::optional<DRAudioError> SetWebAddress(QString t_url);

  void set_pitch(float pitch);
  void set_speed(float speed);
  void set_family_pitch(float pitch);
  void set_family_speed(float speed);

  void toggle_reverb(bool reverb);
  void set_volume(float volume);
  void set_repeatable(bool);
  void set_loop(quint64 start, quint64 end);

  /**
   * @brief Fades in or out the channel predicated by the duration.
   *
   * @param type The type of fade.
   *
   * @param duration The duration of the fade in milliseconds.
   */
  void fade(DRAudioStream::Fade type, int duration);
  void fadeIn(int duration);
  void fadeOut(int duration);

  void play();
  void playSynced(const DRAudioStream *reference);
  void stop();
  void refresh_rate_mode();

signals:
  void file_name_changed(QString file);

  void looped();

  void finished();

public:
  void handle_end();

private:
  friend class DRAudioStreamFamily;

  enum InitState
  {
    InitError,
    InitNotDone,
    InitFinished,
  };

  DRAudioEngine *m_engine = nullptr;
  DRAudio::Family m_family;
  QString m_filename;
  QString m_url;
  DRAudioStream::InitState m_init_state = InitNotDone;
  DRAudioStream::Fade m_fade;
  int m_fade_duration = 0;
  bool m_fade_running = false;
  bool m_reverb = false;
  float m_volume = 0.0f;
  float m_pitch = 0.0f;
  float m_speed = 0.0f;
  float m_family_pitch = 0.0f;
  float m_family_speed = 0.0f;
  bool m_repeatable = false;
  quint64 m_loop_start = 0;
  quint64 m_loop_end = 0;
  QTimer *m_fade_timer = nullptr;
  QTimer *m_drain_timer = nullptr;
  int m_drain_ticks = 0;
  std::unique_ptr<DRAudioStreamPrivate> d;

  bool ensure_init();
  bool ensure_init() const;
  void update_device(DRAudioDevice);
  void init_loop();
  void apply_rate();
  void rewire();
  void release_sound();

private slots:
  void update_volume();
  void handle_fade_finished();
  void check_drain();
};