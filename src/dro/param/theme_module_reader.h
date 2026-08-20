#ifndef THEMEMODULEREADER_H
#define THEMEMODULEREADER_H

#include "json_reader.h"
#include "dro/themes/theme_scene.h"

#include <datatypes.h>

class ThemeModuleReader : public JSONReader
{
public:
  ThemeModuleReader(QString t_moduleDirectory, QString t_moduleName);
  void ParseModule();
  void ParseModuleConfig();
  void ParseLayers();
  void ParseTabs();
  ThemeScene *ParseScene(const QString &t_scene);

  void ParseSubFont(const QJsonObject &t_fontObject, const QString &t_fontType, ThemeScene *t_scene);


  void SwitchModuleTarget(const QString &t_target);

  ThemeScene *getThemeScene(RPSceneType t_scene) const;

  bool getContainsSceneWidget(RPSceneType t_scene, const QString &t_name) const;

  //layers
  bool getContainsLayers() const;
  QVector<QStringList> getWidgetLayers() const;

  //Tabs
  QVector<ThemeTabInfo> getTabs() const;

  //Config
  bool getContainsBool(const QString &t_setting) const;
  bool getSettingBool(const QString &t_setting) const;

  //Config - Sounds
  bool getContainsSound(const QString &t_sfxName) const;
  QString getSoundFile(const QString &t_sfxName) const;

  QString getDirectoryPath() const;
private:
  //Config
  QHash<QString, bool> m_configBooleans = {};
  QHash<QString, QString> m_configSounds = {};

  //Scenes
  ThemeScene *m_CourtroomScene = nullptr;
  ThemeScene *m_LobbyScene = nullptr;
  ThemeScene *m_ReplaysScene = nullptr;
  ThemeScene *m_ViewportScene = nullptr;

  //Other
  QVector<ThemeTabInfo> m_themeTabs = {};
  QVector<QStringList> m_WidgetLayers = {};

  QString m_moduleDirectory = "";
};

#endif // THEMEMODULEREADER_H
