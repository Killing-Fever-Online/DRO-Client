#ifndef CHARACTERMANAGER_H
#define CHARACTERMANAGER_H

#include <dro/interface/widgets/emotion_button.h>
#include <datatypes.h>

#include "dro/param/actor/actor_loader.h"



class CharacterManager
{
public:
  CharacterManager(const CharacterManager&) = delete;

  ActorData *ReadCharacter(const QString &t_folder);
  ActorData *SwitchCharacter(const QString &t_folder);


  static CharacterManager& get()
  {
    return s_Instance;
  }

  void setOutfitList(const QStringList &t_outfits);

  QString lastCharList = "Server Characters";
  QVector<char_type> GetCharList() const;
  QVector<char_type> GetCharList(const QString &package);
  QVector<char_type> GetLastCharList() const;
  QVector<char_type> GetServerCharList();

  QString GetFilteredCharaName(int id) const;
  QString GetServerCharaName(int id) const;

  void ResetPackages();
  void SetCharList(const QVector<char_type> &charList);
  void SetCharList(const QString &package, const QVector<char_type> &charList);

  void SetCharaTaken(int id, bool status);

  void AddToFavorites(const QString &chara);
  void RemoveFromFavorites(const QString &chara);
  void LoadFavoritesList();
  void SaveFavoritesList();

  int GetAvaliablePersona() const;

  QStringList GetCharacterPackages() const;

  QVector<char_type> mFilteredChrList;

  QVector<char_type> mServerCharacters;
  QVector<char_type> mFavoriteCharacters;

  QHash<QString, QVector<char_type>> mPackageCharacters;

  bool GetCharacterInServer(const QString &name) const;
  bool GetCharacterInServer(int filterID) const;
  int GetFilteredId(int Id) const;
  int GetFilteredId(const QString &name) const;

private:
  CharacterManager()
  {

  }
  static CharacterManager s_Instance;
  QHash<QString, bool>CharacterTaken = {};
  QStringList mCharacterPackages = {"Server Characters", "Favorites", "All"};

};

#endif // CHARACTERMANAGER_H
