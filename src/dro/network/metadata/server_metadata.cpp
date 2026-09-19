#include "server_metadata.h"
#include <QStringList>
#include <QString>

QStringList s_featureList = {};

void ServerMetadata::SetFeatureList(const QStringList &features)
{
  s_featureList = features;
}

bool ServerMetadata::FeatureSupported(const QString &featureName)
{
  return s_featureList.contains(featureName);
}
