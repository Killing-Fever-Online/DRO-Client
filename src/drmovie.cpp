#include "drmovie.h"

DRMovie::DRMovie(QGraphicsItem *parent)
    : mk2::GraphicsSpriteItem{parent}
    , m_hide_when_done{false}
    , m_mirrored{false}
{
  connect(this, &DRMovie::started, this, &DRMovie::update_visibility);
  connect(this, &DRMovie::finished, this, &DRMovie::done);
  connect(this, &DRMovie::finished, this, &DRMovie::update_visibility);
}

DRMovie::~DRMovie()
{}

QString DRMovie::file_name()
{
  return get_file_name();
}

void DRMovie::set_hide_on_done(bool p_enabled)
{
  m_hide_when_done = p_enabled;
}

void DRMovie::set_mirrored(bool p_enabled)
{
  set_mirror(p_enabled);
}

void DRMovie::update_visibility()
{
  if (is_running())
  {
    show();
  }
  else if (m_hide_when_done)
  {
    hide();
  }
}
