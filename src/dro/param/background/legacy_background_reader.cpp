#include "legacy_background_reader.h"
#include "aoapplication.h"
#include "dro/fs/fs_reading.h"
#include <QSettings>
#include <QDir>

LegacyBackgroundReader::LegacyBackgroundReader()
{

}

void LegacyBackgroundReader::execLoadBackground(QString t_backgroundName)
{

  const QString l_positions_ini = AOApplication::getInstance()->find_asset_path(AOApplication::getInstance()->get_background_path(t_backgroundName) + "/" + "positions.ini");

  QSettings l_settings(l_positions_ini, QSettings::IniFormat);
  l_settings.setIniCodec("UTF-8");
  // If the positions.ini doesn't exist...
  if (l_positions_ini.isEmpty() || l_settings.status() != QSettings::NoError)
  {
    // Process the entire folder instead
    processBackgroundNoIni(t_backgroundName);
    return;
  }

  const QStringList l_group_list = l_settings.childGroups();
  for (const QString &i_group : l_group_list)
  {
    DRBackgroundPosition l_positionData;

    const QString l_lower_group = i_group.toLower();
    l_settings.beginGroup(i_group);
    l_positionData.mBackground = l_settings.value("back").toString();
    l_positionData.mForeground = l_settings.value("front").toString();

    //TO-DO: Implement
    const QString l_ambient_sfx = l_settings.value("ambient_sfx").toString();

    setPosition(l_lower_group, l_positionData);

    l_settings.endGroup();
  }
  return;
}

void LegacyBackgroundReader::processBackgroundNoIni(QString t_backgroundName)
{
  AOApplication *p_app = AOApplication::getInstance();
  const QString bgfolder = p_app->get_background_path(t_backgroundName);
  const QDir l_dir(bgfolder);
  const auto l_file_list = l_dir.entryList(QDir::Files);

  // Legacy AO hardcoded bg_fg pairs monstrosity
  QList<QStringList> hardcoded_bg_fg;
  hardcoded_bg_fg.append(QStringList{"wit", "witnessempty", "stand"});
  hardcoded_bg_fg.append(QStringList{"def", "defenseempty", "defensedesk"});
  hardcoded_bg_fg.append(QStringList{"pro", "prosecutorempty", "prosecutiondesk"});
  hardcoded_bg_fg.append(QStringList{"jud", "judgestand", "judgedesk"});
  hardcoded_bg_fg.append(QStringList{"hld", "helperstand", "helperdesk"});
  hardcoded_bg_fg.append(QStringList{"hlp", "prohelperstand", "prohelperdesk"});
  hardcoded_bg_fg.append(QStringList{"jur", "jurystand", "jurydesk"});
  hardcoded_bg_fg.append(QStringList{"sea", "seancestand", "seancedesk"});

  QStringList blacklist = {};

  // Scan directory for valid positions
  for (QString filename : l_file_list)
  {
    // make sure filename is lowercase
    filename = filename.toLower();
    QString basicname = filename;
    bool valid_extension = false;
    // NOTE: this will have unintended behavior if both a .png and .webp file exists in the same folder.
    // Most likely, the static version will be picked instead. Seeing as this is "legacy" support,
    // Directing the user to setting up positions.ini or background.json is preferred.
    for (QString f_extension : FS::Formats::SupportedImages())
    {
      if (filename.endsWith(f_extension))
      {
        valid_extension = true;
        // Strip the extension out of the basic name
        basicname = filename.left(filename.length()-f_extension.length());
        break;
      }
    }
    if (!valid_extension)
      continue;
    // Skip overlay name files
    if (basicname.endsWith("_overlay"))
      continue;
    QString posname = basicname;
    // First make sure we're not on the overlay name
    bool is_overlay = false;
    QString overlay_path = basicname + "_overlay";

    // Determine if our bg is part of the hardcoded AO bg-fg pairs
    for (const QStringList &pos_bg_fg : hardcoded_bg_fg)
    {
      QString bgfg_pos_name = pos_bg_fg[0];
      QString bg_name = pos_bg_fg[1];
      QString fg_name = pos_bg_fg[2];
      if (basicname == fg_name)
      {
        is_overlay = true;
        break;
      }
      // Break out of the loop with the proper overlay path for the hardcoded name
      if (basicname == bg_name)
      {
        posname = bgfg_pos_name;
        overlay_path = fg_name;
        break;
      }
    }
    // Skip overlay name files
    if (is_overlay)
      continue;

    // Begin constructing position data
    DRBackgroundPosition l_positionData;
    // Set the background file path
    l_positionData.mBackground = filename;
    // Check if _overlay is valid
    QString filecheck = p_app->find_asset_path({bgfolder + "/" + overlay_path}, FS::Formats::SupportedImages());
    if (FS::Checks::FileExists(filecheck))
    {
      // Valid foreground detected, set it as well setting just the filename
      l_positionData.mForeground = filecheck.section("/",-1,-1);
    }
    // Set the position key to data
    qInfo() << "found valid pos in bg folder. POS: " << posname << " BG: " << l_positionData.mBackground << " FG: " << l_positionData.mForeground;
    setPosition(posname, l_positionData);
  }
}


