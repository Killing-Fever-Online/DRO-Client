#include "scene_manager.h"
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <courtroom.h>
#include "aoapplication.h"
#include "dro/fs/fs_reading.h"
#include "dro/param/background/background_reader.h"
#include "dro/param/background/legacy_background_reader.h"
#include <modules/theme/thememanager.h>

SceneManager SceneManager::s_Instance;

void SceneManager::execLoadPlayerBackground(const QString &t_backgroundName)
{
  mBackgroundName = t_backgroundName;
  const QString l_backgroundJSONPath = AOApplication::getInstance()->find_asset_path(AOApplication::getInstance()->get_background_path(t_backgroundName) + "/" + "background.json");
  if(FS::Checks::FileExists(l_backgroundJSONPath))
  {
    pCurrentBackground = new BackgroundReader();
  }
  else
  {
    pCurrentBackground = new LegacyBackgroundReader();
  }

  pCurrentBackground->execLoadBackground(t_backgroundName);
}

QString SceneManager::resolvePosition(const QString &t_position) const
{
  if(pCurrentBackground == nullptr) return "";
  if(!pCurrentBackground->getBackgroundFilename(t_position).isEmpty()) return t_position;
  if(!pCurrentBackground->getBackgroundFilename(t_position.toLower()).isEmpty()) return t_position.toLower();
  if(!pCurrentBackground->getBackgroundFilename("wit").isEmpty()) return "wit";
  const QMap<QString, DRBackgroundPosition> l_positions = pCurrentBackground->getPositions();
  for(auto it = l_positions.constBegin(); it != l_positions.constEnd(); ++it)
  {
    if(!it.value().mBackground.isEmpty()) return it.key();
  }
  return "";
}

QString SceneManager::getBackgroundPath(const QString &t_position) const
{
  if(pCurrentBackground == nullptr) return "";
  const QString l_position = resolvePosition(t_position);
  const QString l_filename = pCurrentBackground->getBackgroundFilename(l_position);
  if(l_filename.isEmpty()) return "";
  return AOApplication::getInstance()->find_asset_path(AOApplication::getInstance()->get_background_path(mBackgroundName) + "/" + l_filename, QStringList{""} + FS::Formats::SupportedImages());
}

QString SceneManager::getForegroundPath(const QString &t_position) const
{
  if(pCurrentBackground == nullptr) return "";
  const QString l_position = resolvePosition(t_position);
  const QString l_filename = pCurrentBackground->getForegroundFilename(l_position);
  if(l_filename.isEmpty()) return "";
  return AOApplication::getInstance()->find_asset_path(AOApplication::getInstance()->get_background_path(mBackgroundName) + "/" + l_filename, QStringList{""} + FS::Formats::SupportedImages());
}

DRBackgroundSettings SceneManager::getBackgroundSettings() const
{
  if(pCurrentBackground == nullptr) return DRBackgroundSettings();
  return pCurrentBackground->getSettings();
}

BackgroundData *SceneManager::getCurrentBackground() const
{
  return pCurrentBackground;
}

RPLabel *SceneManager::CreateTransition(QWidget *parents, AOApplication *ao_app, DRGraphicsView *viewport)
{
  pUiTransition = new RPLabel(parents, ao_app);
  pViewport = viewport;
  return pUiTransition;
}

RPLabel *SceneManager::GetTransition() const
{
  return pUiTransition;
}

void SceneManager::RenderTransition()
{
  QImage image(pViewport->scene()->sceneRect().size().toSize(), QImage::Format_ARGB32);
  image.fill(Qt::transparent);

  QPainter painter(&image);
  pViewport->scene()->render(&painter);
  pUiTransition->setPixmap(QPixmap::fromImage(image));
}

void SceneManager::AnimateTransition()
{
  QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect();
  pUiTransition->setGraphicsEffect(eff);

  QPropertyAnimation *a = new QPropertyAnimation(eff, "opacity");
  a->setDuration(mFadeDuration);
  a->setStartValue(1);
  a->setEndValue(0);
  a->setEasingCurve(QEasingCurve::OutBack);
  a->start(QPropertyAnimation::DeleteWhenStopped);
}

void SceneManager::setFadeDuration(int duration)
{
  mFadeDuration = duration;
}

void SceneManager::clearPlayerDataList()
{
  mPlayerDataList.clear();
}
