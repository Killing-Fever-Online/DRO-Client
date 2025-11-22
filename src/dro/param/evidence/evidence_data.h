#ifndef EVIDENCEDATA_H
#define EVIDENCEDATA_H

#include <QString>
#include <QStringList>

class EvidenceData
{
public:
  EvidenceData();
  EvidenceData(QString p_name, QString p_desc, QString p_image_path);

  // Getters
  QString getName();
  QString getDesc();
  QString getImagePath();

  QStringList getData();

  // Setters
  void setName(QString p_name);
  void setDesc(QString p_desc);
  void setImagePath(QString p_image_path);

private:
  QString m_name = "";
  QString m_desc = "";
  QString m_image_path = "";
};

#endif // EVIDENCEDATA_H
