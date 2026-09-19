#ifndef CHARACTER_SELECT_H
#define CHARACTER_SELECT_H

#include <aoapplication.h>
#include <QObject>
#include <QWidget>

class CharacterSelectWidget : public QWidget
{
  Q_OBJECT
public:
  CharacterSelectWidget(QWidget *parent, AOApplication *p_ao_app);
protected:
  void wheelEvent(QWheelEvent *event) override;

private:
  AOApplication *ao_app = nullptr;

};

#endif // CHARACTER_SELECT_H
