#include "fs_writing.h"

#include <QCoreApplication>
#include <QDir>

void DirUtils::CreateInitialFolders()
{
  QDir().mkdir("screenshots");
  QDir().mkdir("logs");
  QDir().mkdir("base/configs");
}
