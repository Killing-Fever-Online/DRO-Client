#include "evidence_data.h"

EvidenceData::EvidenceData()
{
}

EvidenceData::EvidenceData(QString p_name, QString p_desc, QString p_image_path)
{
  m_name = p_name;
  m_desc = p_desc;
  m_image_path = p_image_path;
}

QString EvidenceData::getName()
{
  return m_name;
}

QString EvidenceData::getDesc()
{
  return m_desc;
}

QString EvidenceData::getImagePath()
{
  return m_image_path;
}

QStringList EvidenceData::getData()
{
  return {m_name, m_desc, m_image_path};
}


void EvidenceData::setName(QString p_name)
{
  m_name = p_name;
}

void EvidenceData::setDesc(QString p_desc)
{
  m_desc = p_desc;
}

void EvidenceData::setImagePath(QString p_image_path)
{
  m_image_path = p_image_path;
}

