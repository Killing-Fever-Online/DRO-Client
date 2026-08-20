#pragma once

#include <QString>
#include <QStringList>

class DRPacket
{
public:
  static QString encode(const QString &data);

  DRPacket(const QString &header);
  DRPacket(const QString &header, const QStringList &content);

  const QString &get_header() const;
  const QStringList &get_content() const;
  QString to_string(const bool encode = false) const;

  static void unescape(QStringList &contents);
private:
  QString m_header;
  QStringList m_content;
};
