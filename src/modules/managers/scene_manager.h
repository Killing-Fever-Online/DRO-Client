#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "src/dro/interface/widgets/rp_label.h"
#include "src/drgraphicscene.h"

#include <mk2/drplayer.h>
#include "dro/param//background/background_data.h"

#include <aoconfig.h>

class SceneManager
{
public:
  SceneManager(const SceneManager&) = delete;

  static SceneManager& get()
  {
    return s_Instance;
  }

  void execLoadPlayerBackground(const QString &t_backgroundName);
  QString getBackgroundPath(const QString &t_position) const;
  QString getForegroundPath(const QString &t_position) const;
  DRBackgroundSettings getBackgroundSettings() const;
  BackgroundData *getCurrentBackground() const;

  RPLabel *CreateTransition(QWidget *parents, AOApplication* ao_app, DRGraphicsView *viewport);
  RPLabel *GetTransition() const;

  void RenderTransition();
  void AnimateTransition();
  void setFadeDuration(int duration);

  void clearPlayerDataList();

  QVector<DrPlayer> mPlayerDataList;

  AOConfig *pConfigAO = nullptr;
private:
  SceneManager() {}
  static SceneManager s_Instance;

  QString resolvePosition(const QString &t_position) const;

  int mFadeDuration = 200;
  RPLabel *pUiTransition = nullptr;
  DRGraphicsView *pViewport = nullptr;


  QString mBackgroundName = "";
  BackgroundData *pCurrentBackground = nullptr;

};

#endif // SCENEMANAGER_H
