#pragma once

#include <QMetaType>
#include <QString>

namespace DRAudio
{
enum class Family
{
  FSystem,
  FEffect,
  FMusic,
  FVideo,
  FBlip,
};

enum Option
{
  OSuppressed = 0x1,
  OIgnoreSuppression = 0x2,

  // engine

  /**
   * If enabled, the engine will suppress all audio when the application is inactive.
   */
  OEngineSuppressBackgroundAudio = 0x4,

  // independent SoundTouch pitch/tempo instead of linked resampling
  OEngineIndependentPitchTempo = 0x8,
};
Q_DECLARE_FLAGS(Options, Option)
} // namespace DRAudio
Q_DECLARE_METATYPE(DRAudio::Options)
