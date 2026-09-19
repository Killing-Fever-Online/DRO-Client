#ifndef LOBBY_LAYOUT_H
#define LOBBY_LAYOUT_H

#include <QString>
#include <functional>

class RPButton;
class AOApplication;
class Lobby;

namespace Layout::ServerSelect
{
  RPButton *CreateButton(Lobby *lobby, AOApplication *application, QString name, QString image, std::function<void()> releasedFunction = nullptr);
}

#endif // LOBBY_LAYOUT_H
