#ifndef THEMESCENE_H
#define THEMESCENE_H
#include <QHash>
#include "datatypes.h"


class ThemeScene
{
public:
  ThemeScene();

  QStringList getWidgetNames() const
  {
    QStringList l_returnValue = {};
    for (auto it = m_Widgets.begin(); it != m_Widgets.end(); ++it) l_returnValue << it.key();

    return l_returnValue;
  }

  WidgetThemeData *getWidgetData(const QString &t_name) const;
  widgetFontStruct *getWidgetFont(const QString &t_name) const;
  QVector2D getWidgetSpacing(const QString &t_name) const;
  void setDummyTransform(const QString &t_name);
  void setWidgetTransform(const QString &t_name, pos_size_type t_transform);
  void setWidgetRotation(const QString &t_name, double t_rotation);
  void setWidgetSpacing(const QString &t_name, QVector2D t_spacing);
  void setWidgetFont(const QString &t_name, widgetFontStruct *t_font);


  //Chatlog Sub Fonts
  // - Contains
  bool containsWidget(const QString &t_name) const { return m_Widgets.contains(t_name);}
  bool containsChatlogBold(const QString &t_type) const { return m_ChatlogBold.contains(t_type);}
  bool containsChatlogColor(const QString &t_type) const { return m_ChatlogColor.contains(t_type); }



  // - Get
  bool getChatlogBold(const QString &t_type) const;
  QString getChatlogColor(const QString &t_type) const;
  // - Set
  void setChatlogBold(const QString &t_name, bool t_isBold);
  void setChatlogColor(const QString &t_name, const QString &t_color);


private:
  QHash<QString, WidgetThemeData*> m_Widgets = {};
  QHash<QString, bool> m_ChatlogBold = {};
  QHash<QString, QString> m_ChatlogColor = {};
};

#endif // THEMESCENE_H
