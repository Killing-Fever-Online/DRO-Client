#ifndef JSONREADER_H
#define JSONREADER_H

#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QVector2D>
#include <QVector3D>
#include <datatypes.h>
#include <QRect>


class JSONReader
{
public:
  JSONReader();
  void  ReadFromFile(const QString &path);
  void  ReadFromString(const QString &data);

  void  ResetTargetObject();
  void  SetTargetObject(const QJsonObject &target);
  void  SetTargetObject(const QString &string);

  bool        isValueExists(const QString &value) const;
  QVector3D   getVector3DValue(const QString &string) const;
  QJsonArray  getArrayValue(const QString &string) const;
  QRect  getRectangleValue(const QString &string) const;

  bool        getBoolValue(const QString &string) const;
  int         getIntValue(const QString &string) const;
  double      getDoubleValue(const QString &string) const;

  //Strings
  QString     getStringValue(const QString &string) const;
  QStringList getStringArrayValue(const QString &string) const;

  QJsonDocument mDocument;
  QJsonObject   mMainObject;
  QJsonObject   mTargetObject;
};

#endif // JSONREADER_H
