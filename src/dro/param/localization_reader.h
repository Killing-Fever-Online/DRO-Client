#ifndef LOCALIZATIONREADER_H
#define LOCALIZATIONREADER_H

#include "json_reader.h"



class LocalizationReader : public JSONReader
{
public:
  LocalizationReader(QString t_languageCode);
  bool getIsLoaded() const;
  QString getLanguageName() const;
  bool containsLocalizationValue(const QString &t_value) const;
  QString getLocalizationValue(const QString &t_value) const;
  QString getCode() const;
  QString getCredit() const;

private:
  QMap<QString, QString> mValues = { };
  QString mName = "";
  QString mCredit = "";
  QString mLangCode = "en";
  bool mIsLoaded = false;
};

#endif // LOCALIZATIONREADER_H
