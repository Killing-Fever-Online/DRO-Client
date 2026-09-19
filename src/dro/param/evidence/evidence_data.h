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
  QString getName() const;
  QString getDesc() const;
  QString getImagePath() const;

  QStringList getData() const;

  // Setters
  void setName(const QString &p_name);
  void setDesc(const QString &p_desc);
  void setImagePath(const QString &p_image_path);

private:
  QString m_name = "";
  QString m_desc = "";
  QString m_image_path = "";
};

#endif // EVIDENCEDATA_H
