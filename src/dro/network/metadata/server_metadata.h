#ifndef SERVER_METADATA_H
#define SERVER_METADATA_H

class QString;
#include <QtCore/qcontainerfwd.h>

namespace ServerMetadata
{
  void SetFeatureList(const QStringList &features);
  bool FeatureSupported(const QString &featureName);
}

#endif // SERVER_METADATA_H
