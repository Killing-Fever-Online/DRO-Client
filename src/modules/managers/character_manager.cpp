#include "character_manager.h"
#include "commondefs.h"
#include "dro/fs/fs_reading.h"

#include <aoapplication.h>
#include <QFile>
#include "courtroom.h"
#include <QTextStream>
#include "dro/system/theme_scripting.h"
#include "dro/interface/courtroom_layout.h"
#include "dro/param/actor_repository.h"
#include <modules/theme/thememanager.h>

#include <QFileInfo>

CharacterManager CharacterManager::s_Instance;


ActorData *CharacterManager::ReadCharacter(const QString &t_folder)
{
  static QMap<QString, QPair<QDateTime, ActorData*>> s_cache;

  QString l_jsonPath = AOApplication::getInstance()->get_character_path(t_folder, CHARACTER_CHAR_JSON);

  if(FS::Checks::FileExists(l_jsonPath))
  {
    QFileInfo fileInfo(l_jsonPath);
    QDateTime lastModified = fileInfo.lastModified();

    if (s_cache.contains(t_folder)) {
      const auto& cached = s_cache[t_folder];
      if (cached.first == lastModified)
      {
        cached.second->reload();
        return cached.second;
      } else
      {
        delete cached.second;
      }
    }

    ActorData* l_returnData = new ActorDataReader();
    l_returnData->LoadActor(t_folder);
    s_cache[t_folder] = qMakePair(lastModified, l_returnData);
    return l_returnData;
  }

  QString l_iniPath = AOApplication::getInstance()->get_character_path(t_folder, CHARACTER_CHAR_INI);
  QFileInfo iniInfo(l_iniPath);
  QDateTime lastModified = iniInfo.lastModified();

  if (s_cache.contains(t_folder)) {
    const auto& cached = s_cache[t_folder];
    if (cached.first == lastModified)
    {
      return cached.second;
    } else
    {
      delete cached.second;
    }
  }

  ActorData *l_returnData = new LegacyActorReader();
  l_returnData->LoadActor(t_folder);
  s_cache[t_folder] = qMakePair(lastModified, l_returnData);
  return l_returnData;
}

ActorData *CharacterManager::SwitchCharacter(const QString &t_folder)
{
  QStringList animations = {"None"};

  QFile characterAnimations(FS::Paths::FindFile("characters/" + t_folder + "/animations.ini"));
  if (characterAnimations.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream in(&characterAnimations);
    while (!in.atEnd())
    {
      QString line = in.readLine().trimmed();
      if (!line.isEmpty()) animations.append(line);
    }
    characterAnimations.close();
  }
  else
  {
    animations.append(FS::Paths::GetFileList("characters/" + t_folder + "/animations", true, "json"));
  }

  for(QString animationIniPath : FS::Paths::FindFiles("configs/animations.ini"))
  {
    QFile file(animationIniPath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      QTextStream in(&file);
      while (!in.atEnd())
      {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) animations.append(line);
      }
      file.close();
    }
  }


  courtroom::lists::setAnimations(animations);
  LuaBridge::LuaEventCall("OnCharacterLoad", t_folder.toStdString());
  QStringList l_OutfitNames = {"<All>"};

  ActorData* actor = dro::actor::user::load(t_folder);
  if(actor != nullptr)
  {
    QStringList l_charaOutfits = actor->GetOutfitNames();
    l_OutfitNames.append(l_charaOutfits);
  }
  setOutfitList(l_OutfitNames);

  return dro::actor::user::retrieve();
}

void CharacterManager::setOutfitList(const QStringList &t_outfits)
{
  QWidget *l_outfitSelectorWidget = ThemeManager::get().getWidget("outfit_selector");

  if (dynamic_cast<QComboBox*>(l_outfitSelectorWidget) != nullptr)
  {
    QComboBox* l_outfitSelectorCombo = dynamic_cast<QComboBox*>(l_outfitSelectorWidget);
    l_outfitSelectorCombo->clear();
    l_outfitSelectorCombo->addItems(t_outfits);
    if(l_outfitSelectorCombo->count() > 1) l_outfitSelectorCombo->setCurrentIndex(1);
  }
}

QVector<char_type> CharacterManager::GetCharList() const
{
  return mServerCharacters;
}

QVector<char_type> CharacterManager::GetCharList(const QString &package)
{
  lastCharList = package;
  if(package == "Server Characters") return mServerCharacters;
  if(package == "Favorites") return mFavoriteCharacters;

  if(package == "All")
  {
    QVector<char_type> ALlCharacters = {};
    QHash<QString, QVector<char_type>>::const_iterator i;
    for (i = mPackageCharacters.constBegin(); i != mPackageCharacters.constEnd(); ++i)
    {
      ALlCharacters.append(i.value());
    }
    return ALlCharacters;
  }

  if(mPackageCharacters.contains(package))
  {
    return mPackageCharacters[package];
  }
  return QVector<char_type>();
}

QVector<char_type> CharacterManager::GetLastCharList() const
{
  if(lastCharList == "Server Characters") return mServerCharacters;
  if(lastCharList == "Favorites") return mFavoriteCharacters;

  if(lastCharList == "All")
  {
    QVector<char_type> ALlCharacters = {};
    QHash<QString, QVector<char_type>>::const_iterator i;
    for (i = mPackageCharacters.constBegin(); i != mPackageCharacters.constEnd(); ++i)
    {
      ALlCharacters.append(i.value());
    }
    return ALlCharacters;
  }

  if(mPackageCharacters.contains(lastCharList))
  {
    return mPackageCharacters[lastCharList];
  }

  return mServerCharacters;
}

QVector<char_type> CharacterManager::GetServerCharList()
{
  return mServerCharacters;
  CharacterTaken = {};
}

QString CharacterManager::GetFilteredCharaName(int id) const
{
  return mFilteredChrList.at(id).name;
}

QString CharacterManager::GetServerCharaName(int id) const
{
  return mServerCharacters.at(id).name;
}

void CharacterManager::ResetPackages()
{
  lastCharList = "Server Characters";
  mPackageCharacters = {};
  mCharacterPackages.clear();
  mCharacterPackages.append({"Server Characters", "Favorites", "All"});
}

void CharacterManager::SetCharList(const QVector<char_type> &charList)
{
  mServerCharacters = charList;
}

void CharacterManager::SetCharList(const QString &package, const QVector<char_type> &charList)
{
  if(!mCharacterPackages.contains(package)) mCharacterPackages.append(package);
  mPackageCharacters[package] = charList;
}

bool CharacterManager::GetCharacterInServer(const QString &name) const
{
  for (int j = 0; j < mServerCharacters.size(); j++)
  {
    if (mServerCharacters[j].name == name)
    {
      return true;
    }
  }

  return false;
}

bool CharacterManager::GetCharacterInServer(int filterID) const
{
  for (int j = 0; j < mServerCharacters.size(); j++)
  {
    if (mServerCharacters[j].name == mFilteredChrList.at(filterID).name)
    {
      return true;
    }
  }

  return false;
}

void CharacterManager::SetCharaTaken(int id, bool status)
{
  CharacterTaken[mServerCharacters.at(id).name] = status;
}

void CharacterManager::AddToFavorites(const QString &chara)
{
  for (int j = 0; j < mFavoriteCharacters.size(); j++)
  {
    if (chara == mFavoriteCharacters.at(j).name)
    {
      return;
    }
  }
  char_type ct;
  ct.name = chara;
  mFavoriteCharacters.append(ct);
  SaveFavoritesList();
}

void CharacterManager::RemoveFromFavorites(const QString &chara)
{
  for (int j = 0; j < mFavoriteCharacters.size(); j++)
  {
    if (chara == mFavoriteCharacters.at(j).name)
    {
      mFavoriteCharacters.removeAt(j);
      return;
    }
  }
  SaveFavoritesList();
}

void CharacterManager::LoadFavoritesList()
{
  mFavoriteCharacters.clear();
  QFile file(FS::Paths::FindFile(SAVE_FAVORITES, false));
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    qDebug() << "Failed to open file for reading";
    return;
  }

  QTextStream in(&file);
  while (!in.atEnd())
  {
    QString characterName = in.readLine().trimmed();

    if (!characterName.isEmpty())
    {
      char_type character;
      character.name = characterName;
      mFavoriteCharacters.append(character);
    }
  }

  file.close();

}

void CharacterManager::SaveFavoritesList()
{
  QStringList favoritesList = {};
  for (int j = 0; j < mFavoriteCharacters.size(); j++)
  {
    favoritesList.append(mFavoriteCharacters[j].name);
  }

  QFile file(FS::Paths::FindFile(SAVE_FAVORITES, false));
  if (file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QTextStream out(&file);
    for (const QString &character : favoritesList)
    {
      out << character << "\n";
    }
    file.close();
  }
  else
  {
    qDebug() << "Save Favorites Failed";
  }
}

int CharacterManager::GetFilteredId(int Id) const
{
  for (int j = 0; j < mServerCharacters.size(); j++)
  {
    if (mServerCharacters[j].name == mFilteredChrList.at(Id).name)
    {
      return j;
    }
  }

  return -1;
}

int CharacterManager::GetFilteredId(const QString &name) const
{
  for (int j = 0; j < mServerCharacters.size(); j++)
  {
    if (mServerCharacters[j].name == name)
    {
      return j;
    }
  }

  return -1;
}

int CharacterManager::GetAvaliablePersona() const
{
  for(int i = 0; i <= 25; i++)
  {
    QString personaName = QString("Persona" + QString::number(i));
    if(CharacterTaken.contains(personaName))
    {
      if(!CharacterTaken[personaName])
      {
        for (int j = 0; j < mServerCharacters.size(); j++)
        {
          if (mServerCharacters[j].name == personaName)
          {
            return j;
          }
        }

      }
    }
  }

  return -1;
}

QStringList CharacterManager::GetCharacterPackages() const
{
  return mCharacterPackages;
}
