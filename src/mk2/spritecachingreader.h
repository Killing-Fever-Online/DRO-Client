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

#pragma once

#include "mk2/spritereader.h"

#include <QFuture>
#include <QMutex>
#include <QSemaphore>

#include <atomic>

namespace mk2
{
class SpriteCachingReader : public SpriteReader
{
  Q_OBJECT

public:
  explicit SpriteCachingReader(QObject *parent = nullptr);
  virtual ~SpriteCachingReader();

public:
  QSize get_sprite_size() const final;

  int get_frame_count() const final;

  SpriteFrame get_frame(int number) final;

protected:
  void load() final;

private:
  mutable QMutex m_lock;

  QSize m_sprite_size;
  int m_frame_count;
  mutable QSemaphore m_available_frames;
  QVector<SpriteFrame> m_frame_list;

  QFuture<void> m_task;
  std::atomic_bool m_exit_task;

  void _p_preload(QByteArray raw_data);
  void _p_stop_preload();
};
} // namespace mk2
