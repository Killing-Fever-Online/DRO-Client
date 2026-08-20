#include "localization_reader.h"
#include "dro/fs/fs_reading.h"
#include "aoapplication.h"



LocalizationReader::LocalizationReader(QString t_languageCode)
{
  mLangCode = t_languageCode;
  QString l_localizationPath = FS::Paths::BasePath() + "localization/" + t_languageCode + ".json";
  if(FS::Checks::FileExists(l_localizationPath))
  {
    ReadFromFile(l_localizationPath);
    mIsLoaded = true;

    mName = getStringValue("name");
    mCredit = getStringValue("translators");

    QJsonArray l_translationsArray = getArrayValue("translations");
    for(QJsonValueRef r_translationData : l_translationsArray)
    {
      SetTargetObject(r_translationData.toObject());
      mValues[getStringValue("id")] = getStringValue("text");
    }
  }
}

bool LocalizationReader::getIsLoaded() const
{
  return mIsLoaded;
}

QString LocalizationReader::getLanguageName() const
{
  return mName;
}

bool LocalizationReader::containsLocalizationValue(const QString &t_value) const
{
  return mValues.contains(t_value);
}

QString LocalizationReader::getLocalizationValue(const QString &t_value) const
{
  return mValues.value(t_value);
}

QString LocalizationReader::getCode() const
{
  return mLangCode;
}

QString LocalizationReader::getCredit() const
{
  return mCredit;
}
