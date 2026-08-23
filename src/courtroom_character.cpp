#include "courtroom.h"

#include "aoapplication.h"
#include "aoconfig.h"
#include "commondefs.h"
#include "drpacket.h"
#include "theme.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QLineEdit>
#include <QCompleter>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFutureWatcher>
#include <QPixmap>
#include <QUrl>
#include <QtConcurrent/QtConcurrent>
#include "dro/fs/fs_reading.h"
#include "dro/fs/fs_mounting.h"

void Courtroom::set_character_id(const int p_chr_id)
{
  if (!metadata::user::SetCharacterId(p_chr_id)) return;
  load_character();
  Q_EMIT character_id_changed(p_chr_id);
}

QString Courtroom::get_character_ini() const
{
  return ao_config->character_ini(metadata::user::GetCharacterName());
}

QString Courtroom::get_character_content_url() const
{
  QFile l_contentFile(ao_app->get_character_path(get_character_ini(), "CONTENT.txt"));
  if (!l_contentFile.open(QIODevice::ReadOnly))
    return nullptr;

  const QUrl l_url(QString(l_contentFile.readAll()).simplified());
  if (l_url.isRelative() || l_url.isLocalFile())
    return nullptr;

  return l_url.toString(QUrl::FullyEncoded);
}

namespace
{
void drSetItemIcon(QComboBox *p_widget, const int p_index, const QString &p_chr_name, AOApplication *ao_app)
{
  static const QIcon s_blank_icon = []() -> QIcon {
    QPixmap l_blank_texture(64, 64);
    l_blank_texture.fill(Qt::transparent);
    return QIcon(l_blank_texture);
  }();

  const QString l_icon_file = ao_app->get_character_path(p_chr_name, "char_icon.png");
  p_widget->setItemIcon(p_index, FS::Checks::FileExists(l_icon_file) ? QIcon(l_icon_file) : s_blank_icon);
}
} // namespace

void Courtroom::update_iniswap_list()
{
  //ui_iniswap_dropdown->setEditable(false);

  {
    QSignalBlocker b_ini_list(ui_iniswap_dropdown);
    ui_iniswap_dropdown->clear();

    QFutureWatcher<QStringList> *watcher = new QFutureWatcher<QStringList>(this);
    connect(watcher, &QFutureWatcher<QStringList>::finished, this, [this, watcher] {
      if (watcher->future().resultCount() == 0)
        return;
      currentIniswapList = watcher->result();
      UpdateIniswapList();
    });
    connect(watcher, &QFutureWatcher<QStringList>::finished, watcher, &QObject::deleteLater);

    QFuture<QStringList> future = QtConcurrent::run(&Courtroom::SearchForCharacterListAsync, this);
    watcher->setFuture(future);
  }

}


QStringList Courtroom::SearchForCharacterListAsync()
{
  QStringList l_iniswap_list{"Default"};

  QStringList l_package_folders{};
  QVector<QString> packageNames = FS::Packages::CachedNames();
  QVector<QString> disabledPackages = FS::Packages::DisabledList();

  for (int i=0; i< packageNames.size(); i++)
  {
    if(!disabledPackages.contains(packageNames.at(i)))
    {
      const QString l_path = FS::Paths::Package(packageNames.at(i)) + "/characters";
      if(FS::Checks::DirectoryExists(l_path)) l_package_folders.append(l_path);
    }
  }

  l_package_folders.append(FS::Paths::BasePath() + "/characters");

  for (int i = 0; i < l_package_folders.length(); ++i)
  {
    const QFileInfoList l_info_list = QDir(l_package_folders.at(i)).entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);
    for (const QFileInfo &i_info : l_info_list)
    {
      const QString l_name = i_info.fileName();
      if (!FS::Checks::FileExists(ao_app->get_character_path(l_name, CHARACTER_CHAR_INI)) && !FS::Checks::FileExists(ao_app->get_character_path(l_name, CHARACTER_CHAR_JSON)))
        continue;
      if(!l_iniswap_list.contains(l_name))
      {
        l_iniswap_list.append(l_name);
      }
    }
  }

  return l_iniswap_list;
}

void Courtroom::UpdateIniswapList()
{
  ui_iniswap_dropdown->addItems(currentIniswapList);

  update_default_iniswap_item();
  select_base_character_iniswap();
  update_iniswap_dropdown_searchable();


}

void Courtroom::SetChatboxFocus()
{
  ui_ic_chat_message_field->setFocus();
}


void Courtroom::update_default_iniswap_item()
{
  drSetItemIcon(ui_iniswap_dropdown, 0, metadata::user::GetCharacterName(), ao_app);
}

void Courtroom::select_base_character_iniswap()
{
  const QString l_current_chr = get_character_ini();
  if (metadata::user::GetCharacterName() == l_current_chr)
  {
    ui_iniswap_dropdown->setCurrentIndex(0);
    return;
  }
  ui_iniswap_dropdown->setCurrentText(l_current_chr);
}

void Courtroom::refresh_character_content_url()
{
  const QString l_new_content_url = get_character_content_url();
  if (m_character_content_url == l_new_content_url)
    return;
  m_character_content_url = l_new_content_url;
  ao_app->send_server_packet(DRPacket("FS", {m_character_content_url}));
}

void Courtroom::update_character_content_url(QString url)
{
  QString contentFilePath = ao_app->get_character_path(get_character_ini(), "CONTENT.txt");

  QFile contentFile(contentFilePath);
  if (contentFile.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QTextStream out(&contentFile);
    out << url;
    contentFile.close();
  }
  refresh_character_content_url();
}


void Courtroom::on_iniswap_dropdown_changed(int p_index)
{
  ao_config->set_character_ini(metadata::user::GetCharacterName(),
                               p_index == 0 ? metadata::user::GetCharacterName() : ui_iniswap_dropdown->itemText(p_index));
}

void Courtroom::onCharacterSelectPackageChanged(int p_index)
{
  m_current_chr_page = 0;
  set_char_select_page();
}

void Courtroom::update_iniswap_dropdown_searchable()
{
  const bool l_enabled = ao_config->searchable_iniswap_enabled();
  ui_iniswap_dropdown->setEditable(l_enabled);
  if (l_enabled)
  {
    QCompleter *l_completer = ui_iniswap_dropdown->completer();
    l_completer->setCompletionMode(QCompleter::PopupCompletion);
    l_completer->setFilterMode(Qt::MatchContains);
    QAbstractItemView *l_list = l_completer->popup();
    l_list->setTextElideMode(Qt::TextElideMode::ElideNone);
    l_list->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
  }
}

void Courtroom::UpdateIniswapStylesheet()
{
  update_iniswap_dropdown_searchable();
  ui_iniswap_dropdown->blockSignals(true);
  ui_iniswap_dropdown->clear();
  set_stylesheet(ui_iniswap_dropdown, "[INISWAP DROPDOWN]", COURTROOM_STYLESHEETS_CSS, ao_app);
  update_iniswap_list();
  ui_iniswap_dropdown->blockSignals(false);
}
