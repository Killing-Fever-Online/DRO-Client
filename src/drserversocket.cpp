#include "drserversocket.h"
#include "qabstractsocket.h"

#include <QTcpSocket>
#include <QTimer>

const int DRServerSocket::CONNECTING_DELAY = 5000;

namespace
{
QString drFormatServerInfo(const DRServerInfo &server)
{
  const QString l_server_info = server.to_info();
  return !l_server_info.isEmpty() ? QString("<%1>").arg(l_server_info) : nullptr;
}
} // namespace

DRServerSocket::DRServerSocket(QObject *p_parent)
    : QObject(p_parent)
{
  m_connecting_timeout = new QTimer(this);
  m_connecting_timeout->setSingleShot(true);
  m_connecting_timeout->setInterval(CONNECTING_DELAY);

  connect(m_connecting_timeout, &QTimer::timeout, this, &DRServerSocket::disconnect_from_server);
}

bool DRServerSocket::is_connected() const
{
  if (m_socket.m_active_type == DRServerProtocolType::INACTIVE)
  {
    return false;
  }

  if (m_socket.m_active_type == DRServerProtocolType::WEBSOCKET)
  {
    return m_socket.ws->state() == QAbstractSocket::ConnectedState;
  }
  else
  {
    return m_socket.tcp->state() == QAbstractSocket::ConnectedState;
  }
}

void DRServerSocket::connect_to_server(DRServerInfo p_server)
{
  disconnect_from_server();
  m_server = p_server;

  qDebug() << p_server.ws_port;

  if (p_server.protocol == DRServerProtocolType::WEBSOCKET)
  {
    qDebug() << "Using Websocket connection.";
    m_socket.ws = new QWebSocket("", QWebSocketProtocol::VersionLatest, this);
    connect(m_socket.ws, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this,
            &DRServerSocket::_p_check_socket_error);
    connect(m_socket.ws, &QWebSocket::stateChanged, this, &DRServerSocket::_p_update_state);
    connect(m_socket.ws, &QWebSocket::textMessageReceived, this, &DRServerSocket::_p_read_ws_socket);
    const QString url = QString("ws://%1:%2").arg(p_server.address).arg(p_server.ws_port);
    qDebug() << url;
    m_socket.m_active_type = DRServerProtocolType::WEBSOCKET;
    m_socket.ws->open(url);
  }
  else
  {
    qDebug() << "Using TCP connection.";
    m_socket.tcp = new QTcpSocket(this);
    connect(m_socket.tcp, &QTcpSocket::errorOccurred, this, &DRServerSocket::_p_check_socket_error);
    connect(m_socket.tcp, &QTcpSocket::stateChanged, this, &DRServerSocket::_p_update_state);
    connect(m_socket.tcp, &QTcpSocket::readyRead, this, &DRServerSocket::_p_read_socket);
    m_socket.m_active_type = DRServerProtocolType::TCP;
    m_socket.tcp->connectToHost(p_server.address, p_server.port);
  }
}

void DRServerSocket::disconnect_from_server()
{
  if (m_socket.m_active_type == DRServerProtocolType::WEBSOCKET)
  {
    m_socket.ws->close(QWebSocketProtocol::CloseCodeNormal);
    m_socket.ws->abort();
    m_socket.ws->deleteLater();
    m_socket.m_active_type = DRServerProtocolType::INACTIVE;
  }

  if (m_socket.m_active_type == DRServerProtocolType::TCP)
  {
    m_socket.tcp->close();
    m_socket.tcp->abort();
    m_socket.tcp->deleteLater();
    m_socket.m_active_type = DRServerProtocolType::INACTIVE;
  }
  m_buffer.clear();
}

void DRServerSocket::send_packet(DRPacket p_packet)
{
  if (!is_connected())
  {
    const QString l_server_info = m_server.to_info();
    qWarning().noquote() << QString("Failed to send packet; not connected to server%1").arg(drFormatServerInfo(m_server));
    return;
  }

  if (m_socket.m_active_type == DRServerProtocolType::WEBSOCKET)
  {
    m_socket.ws->sendBinaryMessage(p_packet.to_string(true).toUtf8());
  }
  else
  {
    m_socket.tcp->write(p_packet.to_string(true).toUtf8());
  }
}

void DRServerSocket::_p_update_state(QAbstractSocket::SocketState p_state)
{
  bool l_state_changed = true;
  switch (p_state)
  {
  case QAbstractSocket::ConnectingState:
    m_connecting_timeout->start();
    m_state = Connecting;
    break;

  case QAbstractSocket::ConnectedState:
    m_connecting_timeout->stop();
    m_state = Connected;
    break;

  case QAbstractSocket::UnconnectedState:
    m_connecting_timeout->stop();
    m_state = NotConnected;
    break;

  default:
    l_state_changed = false;
    break;
  }

  if (l_state_changed)
  {
    emit connection_state_changed(m_state);
  }
}

void DRServerSocket::_p_check_socket_error(QAbstractSocket::SocketError error)
{
  QString l_error_string;

  if (m_socket.m_active_type == DRServerProtocolType::WEBSOCKET)
  {
    l_error_string = m_socket.ws->errorString();
  }
  else
  {
    l_error_string = m_socket.tcp->errorString();
  }
  const QString l_error = QString("Server%1 error: %2").arg(drFormatServerInfo(m_server), l_error_string);
  qWarning().noquote() << l_error;
  Q_EMIT socket_error(l_error);
}

void DRServerSocket::_p_read_socket()
{
  m_buffer += QString::fromUtf8(m_socket.tcp->readAll());
  QStringList l_raw_packet_list = m_buffer.split("#%", DR::SplitBehavior::KeepEmptyParts);
  m_buffer = l_raw_packet_list.takeLast();
  for (const QString &i_raw_packet : l_raw_packet_list)
  {
    QStringList l_raw_data_list = i_raw_packet.split("#");
    const QString l_header = l_raw_data_list.takeFirst();
    Q_EMIT packet_received(DRPacket(l_header, l_raw_data_list));
  }
}

void DRServerSocket::_p_read_ws_socket(const QString &message)
{
  m_buffer += message;
  QStringList l_raw_packet_list = m_buffer.split("#%", DR::SplitBehavior::KeepEmptyParts);
  m_buffer = l_raw_packet_list.takeLast();
  for (const QString &i_raw_packet : l_raw_packet_list)
  {
    QStringList l_raw_data_list = i_raw_packet.split("#");
    const QString l_header = l_raw_data_list.takeFirst();
    Q_EMIT packet_received(DRPacket(l_header, l_raw_data_list));
  }
}
