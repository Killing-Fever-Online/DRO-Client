#ifndef FS_MOUNTING_H
#define FS_MOUNTING_H

#include <QString>
#include <QVector>

namespace FS::Packages
{
  QVector<QString> &Scan();
  QVector<QString> &CachedNames();
  QVector<QString> &DisabledList();
  void SetDisabled(QVector<QString> disableList);
  void SaveDisabled();
  QVector<QString> MountPaths();
  void AddMountPath(const QString& path);
  void RemoveMountPath(const QString& path);
  QString RootFor(const QString& name);
}

#endif // FS_MOUNTING_H
