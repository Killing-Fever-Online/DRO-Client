#include "background_data.h"

BackgroundData::BackgroundData()
{

}

void BackgroundData::setPosition(const QString &t_position, const DRBackgroundPosition &t_data)
{
  mPositions[t_position] = t_data;
}

const QMap<QString, DRBackgroundPosition> &BackgroundData::getPositions() const
{
  return mPositions;
}

QString BackgroundData::getBackgroundFilename(const QString &t_position) const
{
  if(mPositions.contains(t_position))
  {
    if(!mPositions[t_position].mBackground.isEmpty())
    {
      return mPositions[t_position].mBackground;
    }
  }

  return "";
}

QString BackgroundData::getForegroundFilename(const QString &t_position) const
{
  if(mPositions.contains(t_position))
  {
    if(!mPositions[t_position].mForeground.isEmpty())
    {
      return mPositions[t_position].mForeground;
    }
  }

  return "";
}

void BackgroundData::setSettings(const DRBackgroundSettings &t_settings)
{
  mSettings = t_settings;
}

DRBackgroundSettings BackgroundData::getSettings() const
{
  return mSettings;
}
