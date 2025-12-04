#pragma once

#include "datatypes.h"
#include "drpacket.h"

#include <QObject>
#include <QTcpSocket>
#include <QtWebSockets/QWebSocket>

class QTcpSocket;
class QTimer;

class DRServerSocket : public QObject
{
  Q_OBJECT

public:
  enum ConnectionState
  {
    NotConnected,
    Connecting,
    Connected,
  };

  DRServerSocket(QObject *parent = nullptr);

  bool is_connected() const;

public slots:
  void connect_to_server(DRServerInfo server);

  void disconnect_from_server();

  void send_packet(DRPacket packet);

signals:
  void connection_state_changed(DRServerSocket::ConnectionState);
  void packet_received(DRPacket);
  void socket_error(QString);

private:
  static const int CONNECTING_DELAY;

  struct DRSocket
  {
    DRServerProtocolType m_active_type = DRServerProtocolType::INACTIVE;
    union
    {
      QTcpSocket *tcp;
      QWebSocket *ws;
    };
  };

  DRServerInfo m_server;
  DRSocket m_socket;
  QTimer *m_connecting_timeout = nullptr;
  ConnectionState m_state = NotConnected;
  QString m_buffer;

private slots:
  void _p_update_state(QAbstractSocket::SocketState);
  void _p_check_socket_error(QAbstractSocket::SocketError error);
  void _p_read_socket();
  void _p_read_ws_socket(const QString &message);
};
