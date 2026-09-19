#include "version.h"

#include <QFile>
#include <QImageReader>
#include <QString>
#include <miniaudio/miniaudio.h>
#include "datatypes.h"

int get_release_version()
{
  return 1;
}

int get_major_version()
{
  return 8;
}

int get_minor_version()
{
  return 5;
}

VersionNumber get_version_number(int baseVersion)
{
  int releaseBase = 0;
  int majorBase = 0;
  int minorBase = 0;

  const QString baseVersionString = QString::number(baseVersion);
  if(baseVersionString.length() >= 6 && !baseVersionString.startsWith("-"))
  {
    releaseBase = QString(baseVersionString.at(3)).toInt();
    majorBase = QString(baseVersionString.at(1)).toInt();
    minorBase = QString(baseVersionString.at(5)).toInt();
  }

  return VersionNumber(get_release_version() + releaseBase, get_major_version() + majorBase, get_minor_version() + minorBase);
}

QString get_post_version()
{
  return "";
}

QString get_version_string(int baseVersion)
{
  QString l_version = get_version_number(baseVersion).to_string() + get_post_version();
  return l_version;
}

QString get_resource_file_text(QString filename)
{
  QString data;

  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly))
  {
    return "";
  }
  data = file.readAll();
  file.close();
  return data;
}

QString build_about_message()
{
  const bool hasApng = QImageReader::supportedImageFormats().contains("apng");
  const QString git_branch = get_resource_file_text(":/res/git/git_branch.txt");
  const QString git_hash = get_resource_file_text(":/res/git/git_hash.txt");

  QString msg = QString("<h2>Danganronpa Online</h2>"
                        "version: %1"
                        "<p><b>Source code:</b> "
                        "<a href='https://github.com/NyanOnline/DRO-Client-Fork'>"
                        "https://github.com/NyanOnline/DRO-Client-Fork</a>"
                        "<p><b>Development:</b><br>"
                        "Cerapter, Elf, Iuvee, Tricky Leifa, Keightiie, Crystalwarrior, SyntaxNyah"
                        "<p><b>Fork:</b><br>"
                        "This is the a forked version of the offical DRO client last updated by Crystal Warrior. "
                        "it is a derivative of Attorney-Online-Client-Remake. Created by SyntaxNyah not a replacement "
                        "all credits to the upstream forks from each interation for laying the ground work. "
                        "Not a replacement for DRO client, just a fork intended to clean-up some legacy crust "
                        "and fix some issues."
                        "<p>Based on Attorney Online 2:<br>"
                        "<a href='https://github.com/AttorneyOnline/AO2-Client'>"
                        "https://github.com/AttorneyOnline/AO2-Client</a>"
                        "<p>Running on Qt version %2 with the miniaudio %3 audio engine.<br>"
                        "APNG plugin loaded: %4"
                        "<p><b>Libraries:</b><br>"
                        "<a href='https://www.qt.io/'>Qt 6.8.3</a>, "
                        "<a href='https://github.com/mackron/miniaudio'>miniaudio 0.11.25</a>, "
                        "<a href='https://codeberg.org/soundtouch/soundtouch'>SoundTouch 2.3.3</a>, "
                        "<a href='https://xiph.org/ogg/'>libogg</a>, "
                        "<a href='https://xiph.org/vorbis/'>libvorbis</a>, "
                        "<a href='https://xiph.org/vorbis/'>libvorbisfile</a>, "
                        "<a href='https://opus-codec.org/'>opus</a>, "
                        "<a href='https://opus-codec.org/'>opusfile</a>, "
                        "<a href='https://www.lua.org/'>Lua 5.4.2</a>, "
                        "<a href='https://github.com/ThePhD/sol2'>sol2 3.3.0</a>, "
                        "<a href='https://github.com/discord/discord-rpc'>discord-rpc 3.4.0</a>, "
                        "<a href='https://github.com/jurplel/QtApng'>QtApng plugin</a>"
                        "<p>Built on %5 %6<br>")
                    .arg(get_version_string())
                    .arg(QLatin1String(QT_VERSION_STR))
                    .arg(QLatin1String(MA_VERSION_STRING))
                    .arg(hasApng ? "Yes" : "No")
                    .arg(QLatin1String(__DATE__))
                    .arg(QLatin1String(__TIME__));

  if (git_branch.isEmpty())
    msg += QString("No git branch information available.<br>");
  else
    msg += QString("Built from git branch %1<br>").arg(git_branch);

  if (git_hash.isEmpty())
    msg += QString("No git hash information available.");
  else
    msg += QString("Hash of the latest commit: %1").arg(git_hash);

  return msg;
}

