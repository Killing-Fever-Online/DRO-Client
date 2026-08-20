#include "theme_scene.h"

ThemeScene::ThemeScene()
{

}

WidgetThemeData *ThemeScene::getWidgetData(const QString &t_name) const
{
  return m_Widgets.value(t_name);
}

widgetFontStruct *ThemeScene::getWidgetFont(const QString &t_name) const
{
  WidgetThemeData *l_widget = m_Widgets.value(t_name);
  if(l_widget) return l_widget->Font;
  return nullptr;
}

QVector2D ThemeScene::getWidgetSpacing(const QString &t_name) const
{
  WidgetThemeData *l_widget = m_Widgets.value(t_name);
  if(l_widget) return l_widget->Spacing;
  return QVector2D(-1, -1);
}

void ThemeScene::setDummyTransform(const QString &t_name)
{
  m_Widgets[t_name] = new WidgetThemeData();
  m_Widgets[t_name]->Transform.x = -1;
  m_Widgets[t_name]->Transform.y = -1;
  m_Widgets[t_name]->Transform.width = -1;
  m_Widgets[t_name]->Transform.height = -1;
}

void ThemeScene::setWidgetTransform(const QString &t_name, pos_size_type t_transform)
{
  if(!m_Widgets.contains(t_name)) m_Widgets[t_name] = new WidgetThemeData();
  m_Widgets[t_name]->Transform = t_transform;
}

void ThemeScene::setWidgetRotation(const QString &t_name, double t_rotation)
{
  if(!m_Widgets.contains(t_name)) m_Widgets[t_name] = new WidgetThemeData();
  m_Widgets[t_name]->Rotation = t_rotation;
}

void ThemeScene::setWidgetSpacing(const QString &t_name, QVector2D t_spacing)
{
  if(!m_Widgets.contains(t_name)) setDummyTransform(t_name);
  m_Widgets[t_name]->Spacing = t_spacing;
}

void ThemeScene::setWidgetFont(const QString &t_name, widgetFontStruct *t_font)
{
  if(!m_Widgets.contains(t_name)) setDummyTransform(t_name);
  m_Widgets[t_name]->Font = t_font;
}





bool ThemeScene::getChatlogBold(const QString &t_type) const
{
  if(m_ChatlogBold.contains(t_type)) return m_ChatlogBold[t_type];
  return false;
}

QString ThemeScene::getChatlogColor(const QString &t_type) const
{
  if(m_ChatlogColor.contains(t_type)) return m_ChatlogColor[t_type];
  return "#FFFFFF";
}

void ThemeScene::setChatlogBold(const QString &t_name, bool t_isBold)
{
  m_ChatlogBold[t_name] = t_isBold;
}

void ThemeScene::setChatlogColor(const QString &t_name, const QString &t_color)
{
  m_ChatlogColor[t_name] = t_color;
}
