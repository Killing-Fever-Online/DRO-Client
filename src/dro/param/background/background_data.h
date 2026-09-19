#ifndef BACKGROUNDDATA_H
#define BACKGROUNDDATA_H

#include "datatypes.h"

class BackgroundData
{
public:
  BackgroundData();
  virtual ~BackgroundData() = default;

  void setPosition(const QString &t_position, const DRBackgroundPosition &t_data);
  QString getBackgroundFilename(const QString &t_position) const;
  QString getForegroundFilename(const QString &t_position) const;
  void setSettings(const DRBackgroundSettings &t_settings);
  DRBackgroundSettings getSettings() const;

  const QMap<QString, DRBackgroundPosition> &getPositions() const;

  virtual void execLoadBackground(QString t_backgroundName) = 0;

private:
  DRBackgroundSettings mSettings;
  QMap<QString, DRBackgroundPosition> mPositions = {};
};

#endif // BACKGROUNDDATA_H
