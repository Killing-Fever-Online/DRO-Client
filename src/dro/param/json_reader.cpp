#include "json_reader.h"

JSONReader::JSONReader()
{

}

void JSONReader::ReadFromFile(const QString &path)
{
  QFile json_file(path); json_file.open(QIODevice::ReadOnly | QIODevice::Text);
  QString fileContents = json_file.readAll(); json_file.close();

  static QRegularExpression trailingCommas(R"(,\s*([\]}]))");
  fileContents.replace(trailingCommas, R"(\1)");

  ReadFromString(fileContents);
}

void JSONReader::ReadFromString(const QString &data)
{
  mDocument = QJsonDocument::fromJson(data.toUtf8());
  mMainObject = mDocument.object();
  mTargetObject = mMainObject;
}

void JSONReader::ResetTargetObject()
{
  mTargetObject = mMainObject;
}

void JSONReader::SetTargetObject(const QJsonObject &target)
{
  mTargetObject = target;
}

void JSONReader::SetTargetObject(const QString &string)
{
  SetTargetObject(mTargetObject.value(string).toObject());
}

bool JSONReader::isValueExists(const QString &value) const
{
  return mTargetObject.contains(value);
}

QString JSONReader::getStringValue(const QString &string) const
{
  return mTargetObject.value(string).toString();
}

bool JSONReader::getBoolValue(const QString &string) const
{
  return mTargetObject.value(string).toBool();
}

int JSONReader::getIntValue(const QString &string) const
{
  return mTargetObject.value(string).toInt();
}

double JSONReader::getDoubleValue(const QString &string) const
{
  return mTargetObject.value(string).toDouble();
}

QVector3D JSONReader::getVector3DValue(const QString &string) const
{
  if(!mTargetObject.contains(string)) return QVector3D(1, 1, 1);
  QVector3D return_data = {};
  return_data.setX(mTargetObject.value(string).toObject().value("x").toDouble());
  return_data.setY(mTargetObject.value(string).toObject().value("y").toDouble());
  return_data.setZ(mTargetObject.value(string).toObject().value("z").toDouble());
  return return_data;
}

QJsonArray JSONReader::getArrayValue(const QString &string) const
{
  return mTargetObject.value(string).toArray();
}

QRect JSONReader::getRectangleValue(const QString &string) const
{
  QRect l_ReturnValue;
  if(mTargetObject.contains(string))
  {
    l_ReturnValue.setX(mTargetObject.value(string).toObject().value("x").toInt());
    l_ReturnValue.setY(mTargetObject.value(string).toObject().value("y").toInt());
    l_ReturnValue.setWidth(mTargetObject.value(string).toObject().value("width").toInt());
    l_ReturnValue.setHeight(mTargetObject.value(string).toObject().value("height").toInt());
  }
  return l_ReturnValue;
}

QStringList JSONReader::getStringArrayValue(const QString &string) const
{
  QStringList return_data = {};
  QJsonArray data = mTargetObject.value(string).toArray();

  for(QJsonValueRef ref : data)
  {
    return_data.append(ref.toString());
  }
  return return_data;
}
