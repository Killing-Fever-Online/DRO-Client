#pragma once

#include "mk2/graphicsspriteitem.h"

class DRMovie : public mk2::GraphicsSpriteItem
{
  Q_OBJECT

public:
  DRMovie(QGraphicsItem *parent = nullptr);
  ~DRMovie();

  QString file_name();
  void set_hide_on_done(bool enabled);
  void set_mirrored(bool enabled);

signals:
  void done();

private:
  bool m_hide_when_done;

private slots:
  void update_visibility();
};
