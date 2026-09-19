#include "drpacket.h"

#include <QDebug>

QString DRPacket::encode(const QString &p_data)
{
  QString l_data = p_data;
  return l_data.replace("#", "<num>").replace("%", "<percent>").replace("$", "<dollar>").replace("&", "<and>");
}

DRPacket::DRPacket(const QString &p_header)
    : DRPacket(p_header, {})
{}

DRPacket::DRPacket(const QString &p_header, const QStringList &p_content)
{
  m_header = p_header;
  m_content = p_content;
}

const QString &DRPacket::get_header() const
{
  return m_header;
}

const QStringList &DRPacket::get_content() const
{
  return m_content;
}

QString DRPacket::to_string(const bool p_encode) const
{
  QString r_data;
  for (const QString &i_value : std::as_const(m_content))
    r_data += (p_encode ? encode(i_value) : i_value) + "#";
  return m_header + "#" + r_data + "%";
}

void DRPacket::unescape(QStringList &contents)
{
  contents.replaceInStrings("<num>", "#")
          .replaceInStrings("<percent>", "%")
          .replaceInStrings("<dollar>", "$")
          .replaceInStrings("<and>", "&");
}
