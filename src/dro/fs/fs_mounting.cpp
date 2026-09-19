#include "fs_mounting.h"
#include "fs_reading.h"

#include <QDir>
#include <QHash>
#include <QTextStream>

static QVector<QString> s_foundPackages = {};
static QVector<QString> s_disabledPackages = {};
static QVector<QString> s_mountPaths = {};
static QHash<QString, QString> s_mountRoots = {};

static void SaveMounts()
{
  const QString iniPath = FS::Paths::BasePath() + "mounts.ini";
  QFile iniFile(iniPath);
  iniFile.open(QIODevice::WriteOnly);
  QTextStream out(&iniFile);

  iniFile.resize(0);

  for (int i=0; i< s_mountPaths.size(); i++)
  {
    out << s_mountPaths[i] << "\r\n";
  }

  iniFile.close();
}

QVector<QString> &FS::Packages::Scan()
{
  s_foundPackages.clear();
  s_mountPaths.clear();
  s_mountRoots.clear();

  const QString mountsPath = Paths::BasePath() + "mounts.ini";
  QFile mountsFile(mountsPath);
  if (mountsFile.open(QFile::ReadOnly))
  {
    QTextStream mountsIn(&mountsFile);
    while (!mountsIn.atEnd())
    {
      QString l_line = mountsIn.readLine().trimmed();
      if(!l_line.isEmpty())
      {
        l_line = QDir::cleanPath(l_line);
        if(!s_mountPaths.contains(l_line)) s_mountPaths.append(l_line);
      }
    }
  }

  for(const QString &mountPath : s_mountPaths)
  {
    if(QDir(mountPath).exists())
    {
      QString root = QDir::cleanPath(mountPath);
      if(!root.endsWith('/')) root += '/';
      s_foundPackages.append(mountPath);
      s_mountRoots.insert(mountPath, root);
    }
  }

  QString packagesPath = FS::Paths::ApplicationPath() + "/packages/";
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
  const QString iniPath = Paths::BasePath() + "packages.ini";
  QFile iniFile(iniPath);
  if (!iniFile.open(QFile::ReadOnly)) return s_foundPackages;

  s_disabledPackages.clear();
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

QVector<QString> &FS::Packages::DisabledList()
{
  return s_disabledPackages;
}

void FS::Packages::SetDisabled(QVector<QString> disableList)
{
  s_disabledPackages.clear();
  s_disabledPackages = disableList;
  SaveDisabled();
}

void FS::Packages::SaveDisabled()
{
  const QString iniPath = Paths::BasePath() + "packages.ini";
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

QVector<QString> FS::Packages::MountPaths()
{
  return s_mountPaths;
}

void FS::Packages::AddMountPath(const QString& path)
{
  const QString cleanedPath = QDir::cleanPath(path);
  if(!s_mountPaths.contains(cleanedPath)) s_mountPaths.append(cleanedPath);
  SaveMounts();
}

void FS::Packages::RemoveMountPath(const QString& path)
{
  s_mountPaths.removeAll(path);
  SaveMounts();
}

QString FS::Packages::RootFor(const QString& name)
{
  return s_mountRoots.value(name);
}
