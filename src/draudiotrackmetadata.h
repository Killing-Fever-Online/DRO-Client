#pragma once

#include <QString>

class DRAudiotrackMetadata
{
public:
  static void update_cache();

  DRAudiotrackMetadata();
  DRAudiotrackMetadata(QString file_name);
  ~DRAudiotrackMetadata();

  QString filename() const;
  QString title() const;
  bool play_once() const;
  quint64 loop_start() const;
  quint64 loop_end() const;

private:
  QString m_filename;
  QString m_title;
  bool m_play_once = false;
  quint64 m_loop_start = 0;
  quint64 m_loop_end = 0;
};
