#include "draudioerror.h"

DRAudioError::DRAudioError()
{}

DRAudioError::DRAudioError(QString p_error)
    : m_error(QString("[miniaudio] %1").arg(p_error))
{}

QString DRAudioError::what() const
{
  return m_error;
}
