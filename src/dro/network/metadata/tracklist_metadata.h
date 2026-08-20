#ifndef TRACKLIST_METADATA_H
#define TRACKLIST_METADATA_H

#include <QStringList>


class TracklistMetadata
{
public:
  static QStringList Parse(const QStringList &musicList);
  static QStringList GetEverything();
  static QStringList GetCategory(const QString &categoryName);
  static QStringList GetCategories();

  static void PinTrack(const QString &musicPath);
};

#endif // TRACKLIST_METADATA_H
