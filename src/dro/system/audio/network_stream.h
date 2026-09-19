#pragma once

#include "miniaudio_config.h"

#include <QByteArray>
#include <QMutex>
#include <QObject>
#include <QUrl>
#include <QWaitCondition>

class QNetworkAccessManager;
class QNetworkReply;

class NetworkStream : public QObject
{
  Q_OBJECT

public:
  explicit NetworkStream(const QUrl &p_url);
  ~NetworkStream();

  bool wait_for_header();

  static ma_result on_read(ma_decoder *pDecoder, void *pBufferOut, size_t bytesToRead, size_t *pBytesRead);
  static ma_result on_seek(ma_decoder *pDecoder, ma_int64 byteOffset, ma_seek_origin origin);
  static ma_result on_tell(ma_decoder *pDecoder, ma_int64 *pCursor);

private slots:
  void handle_ready_read();
  void handle_finished();

private:
  bool wait_for_buffered(qint64 p_needed, int p_timeout_ms);
  ma_result read(void *p_buffer, size_t p_bytes, size_t *r_read);
  ma_result seek(ma_int64 p_offset, ma_seek_origin p_origin);
  ma_result tell(ma_int64 *r_cursor) const;

  QNetworkAccessManager *m_manager = nullptr;
  QNetworkReply *m_reply = nullptr;
  QByteArray m_buffer;
  qint64 m_cursor = 0;
  qint64 m_total = -1;
  bool m_finished = false;
  bool m_failed = false;
  mutable QMutex m_mutex;
  QWaitCondition m_wait;

  Q_DISABLE_COPY(NetworkStream)
};
