#include "network_stream.h"

#include <QDeadlineTimer>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QThread>
#include <QTimer>
#include <cstring>

namespace
{
const int HEADER_WAIT_MS = 10000;
const int READ_WAIT_MS = 10000;
const qint64 HEADER_BYTES = 32768;
} // namespace

NetworkStream::NetworkStream(const QUrl &p_url)
    : QObject(nullptr)
{
  m_manager = new QNetworkAccessManager(this);

  QNetworkRequest l_request(p_url);
  l_request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

  m_reply = m_manager->get(l_request);
  connect(m_reply, &QIODevice::readyRead, this, &NetworkStream::handle_ready_read);
  connect(m_reply, &QNetworkReply::finished, this, &NetworkStream::handle_finished);
}

NetworkStream::~NetworkStream()
{
  if (m_reply != nullptr)
  {
    m_reply->disconnect(this);
    m_reply->abort();
    m_reply->deleteLater();
    m_reply = nullptr;
  }

  QMutexLocker l_locker(&m_mutex);
  m_finished = true;
  m_wait.wakeAll();
}

void NetworkStream::handle_ready_read()
{
  const QByteArray l_data = m_reply->readAll();
  if (l_data.isEmpty())
  {
    return;
  }

  QMutexLocker l_locker(&m_mutex);
  if (m_total < 0)
  {
    m_total = m_reply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
  }
  m_buffer.append(l_data);
  m_wait.wakeAll();
}

void NetworkStream::handle_finished()
{
  QMutexLocker l_locker(&m_mutex);
  if (m_reply != nullptr && m_reply->error() != QNetworkReply::NoError)
  {
    m_failed = true;
  }
  m_finished = true;
  m_wait.wakeAll();
}

bool NetworkStream::wait_for_buffered(qint64 p_needed, int p_timeout_ms)
{
  if (QThread::currentThread() == thread())
  {
    QDeadlineTimer l_deadline(p_timeout_ms);

    while (true)
    {
      {
        QMutexLocker l_locker(&m_mutex);
        if (m_finished || m_buffer.size() >= p_needed)
        {
          break;
        }
      }

      if (l_deadline.hasExpired())
      {
        break;
      }

      QEventLoop l_loop;
      QTimer l_timer;
      l_timer.setSingleShot(true);
      l_timer.setInterval(qMax(0, int(l_deadline.remainingTime())));
      QObject::connect(&l_timer, &QTimer::timeout, &l_loop, &QEventLoop::quit);
      QObject::connect(m_reply, &QIODevice::readyRead, &l_loop, &QEventLoop::quit);
      QObject::connect(m_reply, &QNetworkReply::finished, &l_loop, &QEventLoop::quit);
      l_timer.start();
      l_loop.exec();
    }

    QMutexLocker l_locker(&m_mutex);
    return !m_failed && !m_buffer.isEmpty();
  }

  QMutexLocker l_locker(&m_mutex);
  QDeadlineTimer l_deadline(p_timeout_ms);

  while (!m_finished && m_buffer.size() < p_needed)
  {
    if (!m_wait.wait(&m_mutex, l_deadline))
    {
      break;
    }
  }

  return !m_failed && !m_buffer.isEmpty();
}

bool NetworkStream::wait_for_header()
{
  return wait_for_buffered(HEADER_BYTES, HEADER_WAIT_MS);
}

ma_result NetworkStream::read(void *p_buffer, size_t p_bytes, size_t *r_read)
{
  // on a stall settle for what has arrived instead of retrying forever
  qint64 l_progress = -1;
  while (true)
  {
    qint64 l_needed = 0;
    {
      QMutexLocker l_locker(&m_mutex);
      if (m_failed)
      {
        return MA_ERROR;
      }
      if (m_finished || m_buffer.size() > m_cursor)
      {
        break;
      }
      if (m_buffer.size() <= l_progress)
      {
        break;
      }
      l_progress = m_buffer.size();
      l_needed = m_cursor + 1;
    }

    wait_for_buffered(l_needed, READ_WAIT_MS);
  }

  QMutexLocker l_locker(&m_mutex);

  const qint64 l_available = m_buffer.size() - m_cursor;
  if (l_available <= 0)
  {
    if (r_read != nullptr)
    {
      *r_read = 0;
    }
    return MA_AT_END;
  }

  const size_t l_to_copy = size_t(qMin(qint64(p_bytes), l_available));
  memcpy(p_buffer, m_buffer.constData() + m_cursor, l_to_copy);
  m_cursor += qint64(l_to_copy);

  if (r_read != nullptr)
  {
    *r_read = l_to_copy;
  }
  return MA_SUCCESS;
}

ma_result NetworkStream::seek(ma_int64 p_offset, ma_seek_origin p_origin)
{
  if (p_origin == ma_seek_origin_end)
  {
    // opusfile/vorbisfile ignore a failed end seek and trust tell() after it, so for a
    // finite ogg stream wait out the download; on a stall settle for what has arrived
    qint64 l_progress = -1;
    while (true)
    {
      {
        QMutexLocker l_locker(&m_mutex);
        if (m_failed)
        {
          return MA_ERROR;
        }
        if (m_finished)
        {
          break;
        }
        if (m_total <= 0 || !m_buffer.startsWith("OggS"))
        {
          return MA_INVALID_OPERATION;
        }
        if (m_buffer.size() <= l_progress)
        {
          break;
        }
        l_progress = m_buffer.size();
      }
      wait_for_buffered(l_progress + 1, READ_WAIT_MS);
    }
  }

  qint64 l_target = 0;
  {
    QMutexLocker l_locker(&m_mutex);

    if (p_origin == ma_seek_origin_start)
    {
      l_target = p_offset;
    }
    else if (p_origin == ma_seek_origin_current)
    {
      l_target = m_cursor + p_offset;
    }
    else
    {
      l_target = m_buffer.size() + p_offset;
    }

    if (l_target < 0)
    {
      return MA_INVALID_ARGS;
    }
  }

  wait_for_buffered(l_target, READ_WAIT_MS);

  QMutexLocker l_locker(&m_mutex);
  if (l_target > m_buffer.size())
  {
    return MA_BAD_SEEK;
  }

  m_cursor = l_target;
  return MA_SUCCESS;
}

ma_result NetworkStream::on_read(ma_decoder *pDecoder, void *pBufferOut, size_t bytesToRead, size_t *pBytesRead)
{
  NetworkStream *l_self = static_cast<NetworkStream *>(pDecoder->pUserData);
  if (l_self == nullptr)
  {
    return MA_INVALID_ARGS;
  }
  return l_self->read(pBufferOut, bytesToRead, pBytesRead);
}

ma_result NetworkStream::on_seek(ma_decoder *pDecoder, ma_int64 byteOffset, ma_seek_origin origin)
{
  NetworkStream *l_self = static_cast<NetworkStream *>(pDecoder->pUserData);
  if (l_self == nullptr)
  {
    return MA_INVALID_ARGS;
  }
  return l_self->seek(byteOffset, origin);
}

ma_result NetworkStream::tell(ma_int64 *r_cursor) const
{
  QMutexLocker l_locker(&m_mutex);
  if (r_cursor != nullptr)
  {
    *r_cursor = m_cursor;
  }
  return MA_SUCCESS;
}

ma_result NetworkStream::on_tell(ma_decoder *pDecoder, ma_int64 *pCursor)
{
  NetworkStream *l_self = static_cast<NetworkStream *>(pDecoder->pUserData);
  if (l_self == nullptr)
  {
    return MA_INVALID_ARGS;
  }
  return l_self->tell(pCursor);
}
