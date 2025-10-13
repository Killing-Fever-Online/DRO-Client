#ifndef FS_MOUNTING_H
#define FS_MOUNTING_H

#include <QVector>

namespace FS::Packages
{
  QVector<QString> &Scan();
  QVector<QString> &CachedNames();
  QVector<QString> &PackageConfig();
  void SetConfig(QVector<QString> disableList);
  void SaveConfig();
}

#endif // FS_MOUNTING_H
