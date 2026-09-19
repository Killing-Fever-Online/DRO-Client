#ifndef CHAR_MENU_H
#define CHAR_MENU_H

#include <QMenu>
#include <QObject>
#include <QWidget>

class Courtroom;

class CharMenu : public QMenu
{
  Q_OBJECT
public:
  CharMenu(Courtroom *parent = nullptr);

public slots:
  void OnMenuRequested(QPoint p_point);
  void OnUpdateCharacterFilesTriggered();
  void OnOpenCharacterTriggered();
  void OnRandomizeServerTriggered();
  void OnRandomizeFavoritesTriggered();
  void OnRandomizeIniswapTriggered();

private:
  QAction *p_UpdateCharacterFilesAction = nullptr;
  QAction *p_OpenDirectoryAction = nullptr;


  QMenu* p_RandomizeMenu = nullptr;
  QAction *p_RandomServerAction = nullptr;
  QAction *p_RandomFavoritesAction = nullptr;
  QAction *p_RandomFilesAction = nullptr;

  Courtroom *parentCourtroom = nullptr;
};

#endif // CHAR_MENU_H
