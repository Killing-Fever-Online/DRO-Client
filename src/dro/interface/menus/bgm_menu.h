#ifndef BGM_MENU_H
#define BGM_MENU_H

#include <QMenu>
#include <QObject>
#include <QWidget>

class BGMMenu : public QMenu
{
  Q_OBJECT
public:
  BGMMenu(QWidget *parent = nullptr);

public slots:
  void OnMenuRequested(QPoint p_point);
  void OnPlayTriggered();
  void OnInsertTriggered();
  void OnPinTriggered();
  void OnStopTriggered();
  void OnSmoothPlayAction();
  void OnInstantPlayAction();
  void OnSyncPlayAction();
  void OnCrossFadePlayAction();


private:
  QString m_TargetTrack = "";
  QAction *p_PlayAction = nullptr;
  QAction *p_InsertAction = nullptr;
  QAction *p_PinAction = nullptr;
  QAction *p_StopAction = nullptr;

  QAction *m_PlaySmooth = nullptr;
  QAction *m_PlayInstant = nullptr;
  QAction *m_PlaySync = nullptr;
  QAction *m_PlayCrossFade = nullptr;

};

#endif // BGM_MENU_H
