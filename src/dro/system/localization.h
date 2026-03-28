#ifndef LOCALIZATION_H
#define LOCALIZATION_H

class QString;
#include <QtCore/qcontainerfwd.h>
namespace dro::system::localization
{
  void initialize();
  void switchLanguage(QString language);
  QStringList getAvailableLanguages();

  QString getText(QString value);
  QString getCode();
  QString getCredit();
}

#endif // LOCALIZATION_H

