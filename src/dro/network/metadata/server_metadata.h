#ifndef SERVER_METADATA_H
#define SERVER_METADATA_H

class QString;
#include <QtCore/qcontainerfwd.h>

namespace ServerMetadata
{
  void SetFeatureList(QStringList features);
  bool FeatureSupported(QString featureName);
}

#endif // SERVER_METADATA_H
