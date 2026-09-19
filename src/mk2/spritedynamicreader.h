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

#include <QSemaphoreReleaser>
#include <QSharedPointer>

namespace mk2
{
class SpriteDynamicReader : public SpriteReader
{
  Q_OBJECT

public:
  static int get_system_memory_threshold();
  static void set_system_memory_threshold(int percent);

  explicit SpriteDynamicReader(QObject *parent = nullptr);
  virtual ~SpriteDynamicReader();

  QSize get_sprite_size() const final;

  int get_frame_count() const final;

  SpriteFrame get_frame(int number) final;

protected:
  void load() final;

private:
  QSharedPointer<SpriteReader> m_reader;
  std::atomic_uint64_t m_used_memory;

  void _p_create_reader(bool caching);
  void _p_free_memory();
};
} // namespace mk2
