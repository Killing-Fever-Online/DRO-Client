#include "lobby_layout.h"
#include "dro/interface/widgets/rp_button.h"
#include "lobby.h"
#include "aoapplication.h"
#include "theme.h"
#include "commondefs.h"

namespace Layout::ServerSelect
{
  RPButton *CreateButton(Lobby *lobby, AOApplication *application, QString name, QString image, std::function<void()> releasedFunction)
  {
    RPButton* newButton = new RPButton(lobby);
    set_size_and_pos(newButton, name, LOBBY_DESIGN_INI, application);
    newButton->set_image(image + ".png");

    QObject::connect(newButton, &QPushButton::pressed, [newButton, image]() { newButton->set_image(image + "_pressed.png"); });

    QObject::connect(newButton, &QPushButton::released, [newButton, image, releasedFunction]()
    {
      newButton->set_image(image + ".png");
      if (releasedFunction) {  releasedFunction(); }
    });

    return newButton;
  }

}
