#include "fs_mounting.h"
#include "commondefs.h"
#include "fs_reading.h"

#include <QDir>
#include <QTextStream>

static QVector<QString> s_foundPackages = {};
static QVector<QString> s_disabledPackages = {};

QVector<QString> &FS::Packages::Scan()
{
  s_foundPackages.clear();

  QString packagesPath = FS::Paths::PackagesPath();
  QList<QFileInfo> packagesFileinfo = QDir(packagesPath).entryInfoList();

  for(QFileInfo packageDirectory : packagesFileinfo)
  {
    if(packageDirectory.isDir())
    {
      QString packageName = packageDirectory.baseName();
      if(!packageName.isEmpty())
      {
        s_foundPackages.append(packageName);
      }
    }
  }

  //Check for disabled packages configuration
  const QString iniPath = Paths::BasePath() + BASE_PACKAGES_INI;
  QFile iniFile(iniPath);
  if (!iniFile.open(QFile::ReadOnly)) return s_foundPackages;

  QTextStream in(&iniFile);
  while (!in.atEnd())
  {
    QString l_line = in.readLine().trimmed();
    if(s_foundPackages.contains(l_line)) s_disabledPackages.append(l_line);
  }

  return s_foundPackages;
}

QVector<QString> &FS::Packages::CachedNames()
{
  return s_foundPackages;
}

QVector<QString> &FS::Packages::PackageConfig()
{
  return s_disabledPackages;
}

void FS::Packages::SetConfig(QVector<QString> disableList)
{
  s_disabledPackages.clear();
  s_disabledPackages = disableList;
  SaveConfig();
}

void FS::Packages::SaveConfig()
{
  const QString iniPath = Paths::BasePath() + BASE_PACKAGES_INI;
  QFile iniFile(iniPath);
  iniFile.open(QIODevice::WriteOnly);
  QTextStream out(&iniFile);

  iniFile.resize(0);

  for (int i=0; i< s_disabledPackages.size(); i++)
  {
    out << s_disabledPackages[i] << "\r\n";
  }

  iniFile.close();

}
