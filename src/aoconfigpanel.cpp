#include "aoconfigpanel.h"

#include "aoapplication.h"
#include "aoconfig.h"
#include "aoguiloader.h"
#include "datatypes.h"
#include "drtheme.h"
#include "mk2/spritedynamicreader.h"
#include "version.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QDir>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QProcess>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QSpinBox>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QTabWidget>

#include <modules/theme/thememanager.h>

#include "dro/system/localization.h"
#include "dro/fs/fs_mounting.h"

using namespace dro::system;

AOConfigPanel::AOConfigPanel(AOApplication *p_ao_app, QWidget *p_parent)
    : QWidget(p_parent)
    , m_config(new AOConfig(this))
    , m_engine(new DRAudioEngine(this))
{
  ao_app = p_ao_app;

  setWindowTitle(tr("Config"));
  setWindowFlag(Qt::WindowMinMaxButtonsHint, false);

  AOGuiLoader loader;
  loader.load_from_file(":res/ui/config_panel.ui", this);

  // tab
  tab_widget = AO_GUI_WIDGET(QTabWidget, "tab_widget");
  setFocusProxy(tab_widget);
  tab_widget->setCurrentIndex(0);

  // behaviour
  ui_save = AO_GUI_WIDGET(QPushButton, "save");
  ui_close = AO_GUI_WIDGET(QPushButton, "close");
  ui_autosave = AO_GUI_WIDGET(QCheckBox, "autosave");

  pTabFilters = AO_GUI_WIDGET(QListView, "tabFilterListView");

  QStringListModel *model = new QStringListModel(this);
  QStringList items;
  items << "General" << "Audio" << "Packages" << "Theme" << "Message" << "Advanced" << "About";
  model->setStringList(items);
  pTabFilters->setModel(model);
  QModelIndex firstIndex = model->index(0, 0);


  connect(pTabFilters->selectionModel(), &QItemSelectionModel::currentChanged, this, &AOConfigPanel::updateTabsVisibility);

  pTabFilters->setCurrentIndex(firstIndex);

  // notifications
  ui_clear_notifications = AO_GUI_WIDGET(QPushButton, "clear_notifications");

  // general
  ui_username = AO_GUI_WIDGET(QLineEdit, "username");
  ui_callwords = AO_GUI_WIDGET(QLineEdit, "callwords");
  ui_advertiser = AO_GUI_WIDGET(QLineEdit, "advertiser");
  ui_server_alerts = AO_GUI_WIDGET(QCheckBox, "server_alerts");
  ui_discord_presence = AO_GUI_WIDGET(QGroupBox, "discord_presence");
  ui_discord_hide_server = AO_GUI_WIDGET(QCheckBox, "discord_hide_server");
  ui_discord_hide_character = AO_GUI_WIDGET(QCheckBox, "discord_hide_character");

  // game
  //ui_themeModules = AO_GUI_WIDGET(QTreeView, "themeModules");
  ui_theme = AO_GUI_WIDGET(QComboBox, "theme");
  wSettingsLanguage = AO_GUI_WIDGET(QComboBox, "languageSelector");
  wLanguageCredits = AO_GUI_WIDGET(QLabel, "translationCredit");
  ui_switch_theme = AO_GUI_WIDGET(QPushButton, "switch_theme");
  ui_reload_theme = AO_GUI_WIDGET(QPushButton, "reload_theme");
  ui_gamemode = AO_GUI_WIDGET(QLineEdit, "gamemode");
  ui_manual_gamemode = AO_GUI_WIDGET(QComboBox, "manual_gamemode");
  ui_manual_gamemode_selection = AO_GUI_WIDGET(QCheckBox, "manual_gamemode_selection");
  ui_timeofday = AO_GUI_WIDGET(QLineEdit, "timeofday");
  ui_manual_timeofday = AO_GUI_WIDGET(QComboBox, "manual_timeofday");
  ui_manual_timeofday_selection = AO_GUI_WIDGET(QCheckBox, "manual_timeofday_selection");
  ui_showname = AO_GUI_WIDGET(QLineEdit, "showname");
  ui_reload_character = AO_GUI_WIDGET(QPushButton, "reload_character");
  ui_searchable_iniswap = AO_GUI_WIDGET(QCheckBox, "searchable_iniswap");
  ui_always_pre = AO_GUI_WIDGET(QCheckBox, "always_pre");
  ui_chat_tick_interval = AO_GUI_WIDGET(QSpinBox, "chat_tick_interval");
  ui_chat_ratelimit = AO_GUI_WIDGET(QSpinBox, "chat_ratelimit");
  ui_emote_preview = AO_GUI_WIDGET(QCheckBox, "emote_preview");
  ui_sticky_sfx = AO_GUI_WIDGET(QCheckBox, "sticky_sfx");


  // IC message
  ui_length_threshold = AO_GUI_WIDGET(QSlider, "length_threshold");
  ui_length_threshold_label = AO_GUI_WIDGET(QLabel, "length_threshold_label");


  // IC Chatlog
  ui_log_max_lines = AO_GUI_WIDGET(QSpinBox, "log_length");
  ui_log_display_timestamp = AO_GUI_WIDGET(QCheckBox, "log_display_timestamp");
  ui_log_display_client_id = AO_GUI_WIDGET(QCheckBox, "log_display_client_id");
  ui_log_display_self_highlight = AO_GUI_WIDGET(QCheckBox, "log_display_self_highlight");
  ui_log_format_use_newline = AO_GUI_WIDGET(QCheckBox, "log_format_use_newline");
  ui_log_display_empty_messages = AO_GUI_WIDGET(QCheckBox, "log_display_empty_messages");
  ui_log_display_music_switch = AO_GUI_WIDGET(QCheckBox, "log_display_music_switch");
  ui_log_orientation_top_down = AO_GUI_WIDGET(QRadioButton, "log_orientation_top_down");
  ui_log_orientation_bottom_up = AO_GUI_WIDGET(QRadioButton, "log_orientation_bottom_up");
  ui_log_is_recording = AO_GUI_WIDGET(QCheckBox, "log_recording");

  // performance
  ui_enable_opengl = AO_GUI_WIDGET(QCheckBox, "enable_opengl");
  ui_focus_performance_mode = AO_GUI_WIDGET(QCheckBox, "focus_performance_mode");
  ui_cache_backgrounds = AO_GUI_WIDGET(QCheckBox, "cache_backgrounds");
  ui_cache_characters = AO_GUI_WIDGET(QCheckBox, "cache_characters");
  ui_cache_effects = AO_GUI_WIDGET(QCheckBox, "cache_effects");
  ui_cache_shouts = AO_GUI_WIDGET(QCheckBox, "cache_shouts");
  ui_cache_gui = AO_GUI_WIDGET(QCheckBox, "cache_gui");
  ui_cache_stickers = AO_GUI_WIDGET(QCheckBox, "cache_stickers");
  ui_system_memory_threshold = AO_GUI_WIDGET(QSlider, "system_memory_threshold");
  ui_system_memory_threshold_label = AO_GUI_WIDGET(QLabel, "system_memory_threshold_label");
  ui_caching_threshold = AO_GUI_WIDGET(QSlider, "caching_threshold");
  ui_caching_threshold_label = AO_GUI_WIDGET(QLabel, "caching_threshold_label");
  ui_loading_bar_delay = AO_GUI_WIDGET(QSlider, "loading_bar_delay");
  ui_loading_bar_delay_label = AO_GUI_WIDGET(QLabel, "loading_bar_delay_label");

  m_cache_checkbox_map.insert(SpriteStage, ui_cache_backgrounds);
  m_cache_checkbox_map.insert(SpriteCharacter, ui_cache_characters);
  m_cache_checkbox_map.insert(SpriteEffect, ui_cache_effects);
  m_cache_checkbox_map.insert(SpriteShout, ui_cache_shouts);
  m_cache_checkbox_map.insert(SpriteGUI, ui_cache_gui);
  m_cache_checkbox_map.insert(SpriteSticker, ui_cache_stickers);
  m_cache_checkbox_map.insert(SpriteWeather, ui_cache_backgrounds);

  // audio
  ui_device = AO_GUI_WIDGET(QComboBox, "device");
  ui_favorite_device = AO_GUI_WIDGET(QCheckBox, "favorite_device");
  ui_master = AO_GUI_WIDGET(QSlider, "master");
  ui_master_value = AO_GUI_WIDGET(QLabel, "master_value");
  ui_suppress_background_audio = AO_GUI_WIDGET(QGroupBox, "suppress_background_audio");
  ui_system = AO_GUI_WIDGET(QSlider, "system");
  ui_system_value = AO_GUI_WIDGET(QLabel, "system_value");
  ui_effect = AO_GUI_WIDGET(QSlider, "effect");
  ui_effect_ignore_suppression = AO_GUI_WIDGET(QCheckBox, "effect_ignore_suppression");
  ui_effect_value = AO_GUI_WIDGET(QLabel, "effect_value");
  ui_music = AO_GUI_WIDGET(QSlider, "music");
  ui_music_ignore_suppression = AO_GUI_WIDGET(QCheckBox, "music_ignore_suppression");
  ui_music_value = AO_GUI_WIDGET(QLabel, "music_value");
  ui_video = AO_GUI_WIDGET(QSlider, "video");
  ui_video_ignore_suppression = AO_GUI_WIDGET(QCheckBox, "video_ignore_suppression");
  ui_video_value = AO_GUI_WIDGET(QLabel, "video_value");
  ui_blip = AO_GUI_WIDGET(QSlider, "blip");
  ui_blip_ignore_suppression = AO_GUI_WIDGET(QCheckBox, "blip_ignore_suppression");
  ui_blip_value = AO_GUI_WIDGET(QLabel, "blip_value");
  ui_blip_rate = AO_GUI_WIDGET(QSpinBox, "blip_rate");
  ui_blank_blips = AO_GUI_WIDGET(QCheckBox, "blank_blips");
  ui_punctuation_delay = AO_GUI_WIDGET(QSpinBox, "punctuation_delay");
  ui_reload_audiotracks = AO_GUI_WIDGET(QPushButton, "reload_audiotracks");
  ui_theme_resize = AO_GUI_WIDGET(QDoubleSpinBox, "themeResizeSpinbox");
  ui_font_resize = AO_GUI_WIDGET(QDoubleSpinBox, "font_resize");
  ui_fade_duration = AO_GUI_WIDGET(QSpinBox, "FadeDurationBox");

  volumeSliderMap = {
      {ui_master, ui_master_value},
      {ui_system, ui_system_value},
      {ui_effect, ui_effect_value},
      {ui_music, ui_music_value},
      {ui_video, ui_video_value}, // Corrected entry
      {ui_blip, ui_blip_value}
  };

  // about
  ui_about = AO_GUI_WIDGET(QLabel, "about_label");

  // packages
  ui_packages_list = AO_GUI_WIDGET(QListWidget, "packages_list");
  ui_load_new_packages = AO_GUI_WIDGET(QPushButton, "load_new_packages");
  refresh_packages_list();

  // themes
  refreshLanguageList();
  refresh_theme_list();
  refresh_gamemode_list();
  refresh_timeofday_list();

  ui_manual_resize = AO_GUI_WIDGET(QCheckBox, "manual_resize");

  // input
  // meta
  connect(m_config, &AOConfig::autosave_changed, ui_autosave, &QAbstractButton::setChecked);

  // notifications
  connect(ui_clear_notifications, &QPushButton::clicked, m_config, &AOConfig::clear_notification_filter);

  // general
  connect(m_config, &AOConfig::username_changed, ui_username, &QLineEdit::setText);
  connect(m_config, &AOConfig::callwords_changed, ui_callwords, &QLineEdit::setText);
  connect(m_config, &AOConfig::server_advertiser_changed, ui_advertiser, &QLineEdit::setText);
  connect(m_config, &AOConfig::server_alerts_changed, ui_server_alerts, &QAbstractButton::setChecked);
  connect(m_config, &AOConfig::discord_presence_changed, ui_discord_presence, &QGroupBox::setChecked);
  connect(m_config, &AOConfig::discord_hide_server_changed, ui_discord_hide_server, &QAbstractButton::setChecked);
  connect(m_config, &AOConfig::discord_hide_character_changed, ui_discord_hide_character, &QAbstractButton::setChecked);

  // game
  connect(m_config, &AOConfig::theme_changed, this, &AOConfigPanel::on_theme_changed);
  connect(m_config, &AOConfig::gamemode_changed, this, &AOConfigPanel::on_gamemode_changed);
  connect(m_config, &AOConfig::manual_gamemode_changed, this, &AOConfigPanel::on_manual_gamemode_changed);
  connect(m_config, &AOConfig::manual_gamemode_selection_changed, this, &AOConfigPanel::on_manual_gamemode_selection_changed);
  connect(m_config, &AOConfig::timeofday_changed, this, &AOConfigPanel::on_timeofday_changed);
  connect(m_config, &AOConfig::manual_timeofday_changed, this, &AOConfigPanel::on_manual_timeofday_changed);
  connect(m_config, &AOConfig::manual_timeofday_selection_changed, this, &AOConfigPanel::on_manual_timeofday_selection_changed);
  connect(m_config, &AOConfig::showname_changed, ui_showname, &QLineEdit::setText);
  connect(m_config, &AOConfig::showname_placeholder_changed, this, &AOConfigPanel::on_showname_placeholder_changed);
  connect(m_config, &AOConfig::searchable_iniswap_changed, ui_searchable_iniswap, &QAbstractButton::setChecked);
  connect(m_config, &AOConfig::always_pre_changed, ui_always_pre, &QAbstractButton::setChecked);
  connect(m_config, &AOConfig::chat_tick_interval_changed, ui_chat_tick_interval, &QSpinBox::setValue);
  connect(m_config, &AOConfig::chat_ratelimit_changed, ui_chat_ratelimit, &QSpinBox::setValue);
  connect(m_config, &AOConfig::emote_preview_changed, ui_emote_preview, &QAbstractButton::setChecked);
  connect(m_config, &AOConfig::sticky_sfx_changed, ui_sticky_sfx, &QAbstractButton::setChecked);

  // log
  connect(m_config, &AOConfig::log_max_lines_changed, ui_log_max_lines, &QSpinBox::setValue);
  connect(m_config, &AOConfig::log_display_timestamp_changed, ui_log_display_timestamp, &QAbstractButton::setChecked);
  connect(m_config, &AOConfig::log_display_client_id_changed, ui_log_display_client_id, &QAbstractButton::setChecked);
  connect(m_config, &AOConfig::log_display_self_highlight_changed, ui_log_display_self_highlight, &QAbstractButton::setChecked);
  connect(m_config, &AOConfig::log_format_use_newline_changed, ui_log_format_use_newline, &QAbstractButton::setChecked);
  connect(m_config, &AOConfig::log_display_empty_messages_changed, ui_log_display_empty_messages, &QAbstractButton::setChecked);
  connect(m_config, &AOConfig::log_display_music_switch_changed, ui_log_display_music_switch, &QAbstractButton::setChecked);
  connect(m_config, &AOConfig::log_is_topdown_changed, this, &AOConfigPanel::on_log_is_topdown_changed);
  connect(m_config, &AOConfig::log_is_recording_changed, ui_log_is_recording, &QAbstractButton::setChecked);

  // audio
  connect(m_config, &AOConfig::master_volume_changed, ui_master, &QSlider::setValue);
  connect(m_config, &AOConfig::suppress_background_audio_changed, ui_suppress_background_audio, &QGroupBox::setChecked);
  connect(m_config, &AOConfig::system_volume_changed, ui_system, &QSlider::setValue);
  connect(m_config, &AOConfig::effect_volume_changed, ui_effect, &QSlider::setValue);
  connect(m_config, &AOConfig::effect_ignore_suppression_changed, ui_effect_ignore_suppression, &QAbstractButton::setChecked);
  connect(m_config, &AOConfig::music_volume_changed, ui_music, &QSlider::setValue);
  connect(m_config, &AOConfig::music_ignore_suppression_changed, ui_music_ignore_suppression, &QAbstractButton::setChecked);
  connect(m_config, &AOConfig::video_volume_changed, ui_video, &QSlider::setValue);
  connect(m_config, &AOConfig::video_ignore_suppression_changed, ui_video_ignore_suppression, &QAbstractButton::setChecked);
  connect(m_config, &AOConfig::blip_volume_changed, ui_blip, &QSlider::setValue);
  connect(m_config, &AOConfig::blip_ignore_suppression_changed, ui_blip_ignore_suppression, &QAbstractButton::setChecked);
  connect(m_config, &AOConfig::blip_rate_changed, ui_blip_rate, &QSpinBox::setValue);
  connect(m_config, &AOConfig::punctuation_delay_changed, ui_punctuation_delay, &QSpinBox::setValue);
  connect(m_config, &AOConfig::blank_blips_changed, ui_blank_blips, &QAbstractButton::setChecked);

  connect(m_config, &AOConfig::theme_resize_changed, ui_theme_resize, &QDoubleSpinBox::setValue);
  connect(m_config, &AOConfig::font_resize_changed, ui_font_resize, &QDoubleSpinBox::setValue);
  connect(m_config, &AOConfig::fade_duration_changed, ui_fade_duration, &QSpinBox::setValue);

  connect(m_engine, &DRAudioEngine::current_device_changed, this, &AOConfigPanel::on_audio_device_changed);
  connect(m_engine, &DRAudioEngine::device_list_changed, this, &AOConfigPanel::on_audio_device_list_changed);
  connect(m_engine, &DRAudioEngine::favorite_device_changed, this, &AOConfigPanel::on_favorite_audio_device_changed);

  connect(m_config, &AOConfig::manual_resize_changed, ui_manual_resize, &QAbstractButton::setChecked);

  // meta
  connect(ui_close, &QPushButton::clicked, this, &QWidget::close);
  connect(ui_save, &QPushButton::clicked, m_config, &AOConfig::save_file);
  connect(ui_autosave, &QAbstractButton::toggled, m_config, &AOConfig::set_autosave);

  // general
  connect(ui_username, &QLineEdit::editingFinished, this, &AOConfigPanel::username_editing_finished);
  connect(ui_callwords, &QLineEdit::editingFinished, this, &AOConfigPanel::callwords_editing_finished);
  connect(ui_advertiser, &QLineEdit::editingFinished, this, &AOConfigPanel::advertiser_editing_finished);
  connect(ui_server_alerts, &QAbstractButton::toggled, m_config, &AOConfig::set_server_alerts);
  connect(ui_discord_presence, &QGroupBox::toggled, m_config, &AOConfig::set_discord_presence);
  connect(ui_discord_hide_server, &QAbstractButton::toggled, m_config, &AOConfig::set_discord_hide_server);
  connect(ui_discord_hide_character, &QAbstractButton::toggled, m_config, &AOConfig::set_discord_hide_character);

  // game
  connect(wSettingsLanguage, &QComboBox::currentTextChanged, this, &AOConfigPanel::updateLanguage);
  connect(ui_theme, &QComboBox::currentTextChanged, this, &AOConfigPanel::update_theme_controls);
  connect(ui_switch_theme, &QPushButton::clicked, this, &AOConfigPanel::on_switch_theme_clicked);
  connect(ui_reload_theme, &QPushButton::clicked, this, &AOConfigPanel::on_reload_theme_clicked);
  connect(ui_reload_character, &QPushButton::clicked, this, &AOConfigPanel::on_reload_character_clicked);
  connect(ui_reload_audiotracks, &QPushButton::clicked, this, &AOConfigPanel::on_reload_audiotracks_clicked);
  connect(ui_manual_gamemode, &QComboBox::currentTextChanged, this, &AOConfigPanel::on_manual_gamemode_index_changed);
  connect(ui_manual_gamemode_selection, &QAbstractButton::toggled, m_config, &AOConfig::set_manual_gamemode_selection_enabled);
  connect(ui_manual_timeofday, &QComboBox::currentTextChanged, this, &AOConfigPanel::on_manual_timeofday_index_changed);
  connect(ui_manual_timeofday_selection, &QAbstractButton::toggled, m_config, &AOConfig::set_manual_timeofday_selection_enabled);
  connect(ui_showname, &QLineEdit::editingFinished, this, &AOConfigPanel::showname_editing_finished);
  connect(ui_searchable_iniswap, &QAbstractButton::toggled, m_config, &AOConfig::set_searchable_iniswap);
  connect(ui_always_pre, &QAbstractButton::toggled, m_config, &AOConfig::set_always_pre);
  connect(ui_chat_tick_interval, &QSpinBox::valueChanged, m_config, &AOConfig::set_chat_tick_interval);
  connect(ui_chat_ratelimit, &QSpinBox::valueChanged, m_config, &AOConfig::set_chat_ratelimit);
  connect(ui_emote_preview, &QAbstractButton::toggled, m_config, &AOConfig::set_emote_preview);
  connect(ui_sticky_sfx, &QAbstractButton::toggled, m_config, &AOConfig::set_sticky_sfx);

  //packages
  connect(ui_load_new_packages, &QPushButton::clicked, this, &AOConfigPanel::on_load_packages_clicked);

  // ic message
  connect(m_config, &AOConfig::message_length_threshold_changed, ui_length_threshold, &QSlider::setValue);
  connect(ui_length_threshold, &QSlider::valueChanged, m_config, &AOConfig::set_message_length_threshold);
  connect(ui_length_threshold, &QSlider::valueChanged, this, &AOConfigPanel::on_length_threshold_value_changed);

  // out, log
  connect(ui_log_max_lines, &QSpinBox::valueChanged, m_config, &AOConfig::set_log_max_lines);
  connect(ui_log_display_timestamp, &QAbstractButton::toggled, m_config, &AOConfig::set_log_display_timestamp);
  connect(ui_log_display_client_id, &QAbstractButton::toggled, m_config, &AOConfig::set_log_display_client_id);
  connect(ui_log_display_self_highlight, &QAbstractButton::toggled, m_config, &AOConfig::set_log_display_self_highlight);
  connect(ui_log_format_use_newline, &QAbstractButton::toggled, m_config, &AOConfig::set_log_format_use_newline);
  connect(ui_log_display_empty_messages, &QAbstractButton::toggled, m_config, &AOConfig::set_log_display_empty_messages);
  connect(ui_log_display_music_switch, &QAbstractButton::toggled, m_config, &AOConfig::set_log_display_music_switch);
  connect(ui_log_orientation_top_down, &QAbstractButton::toggled, m_config, &AOConfig::set_log_is_topdown);
  connect(ui_log_is_recording, &QAbstractButton::toggled, m_config, &AOConfig::set_log_is_recording);
  connect(ui_suppress_background_audio, &QGroupBox::toggled, m_config, &AOConfig::set_suppress_background_audio);
  connect(ui_device, &QComboBox::currentIndexChanged, this, &AOConfigPanel::on_device_current_index_changed);


  for (auto it = volumeSliderMap.constBegin(); it != volumeSliderMap.constEnd(); ++it)
  {
    connect(it.key(), &QAbstractSlider::valueChanged, this, &AOConfigPanel::on_volume_value_changed);
  }

  connect(ui_effect_ignore_suppression, &QAbstractButton::toggled, m_config, &AOConfig::set_effect_ignore_suppression);
  connect(ui_music_ignore_suppression, &QAbstractButton::toggled, m_config, &AOConfig::set_music_ignore_suppression);
  connect(ui_video_ignore_suppression, &QAbstractButton::toggled, m_config, &AOConfig::set_video_ignore_suppression);
  connect(ui_blip_ignore_suppression, &QAbstractButton::toggled, m_config, &AOConfig::set_blip_ignore_suppression);

  connect(ui_master, &QAbstractSlider::valueChanged, m_config, &AOConfig::set_master_volume);
  connect(ui_system, &QAbstractSlider::valueChanged, m_config, &AOConfig::set_system_volume);
  connect(ui_effect, &QAbstractSlider::valueChanged, m_config, &AOConfig::set_effect_volume);
  connect(ui_music, &QAbstractSlider::valueChanged, m_config, &AOConfig::set_music_volume);
  connect(ui_video, &QAbstractSlider::valueChanged, m_config, &AOConfig::set_video_volume);
  connect(ui_blip, &QAbstractSlider::valueChanged, m_config, &AOConfig::set_blip_volume);

  connect(ui_blip_rate, &QSpinBox::valueChanged, m_config, &AOConfig::set_blip_rate);
  connect(ui_punctuation_delay, &QSpinBox::valueChanged, m_config, &AOConfig::set_punctuation_delay);
  connect(ui_blank_blips, &QAbstractButton::toggled, m_config, &AOConfig::set_blank_blips);

  connect(ui_theme_resize, &QDoubleSpinBox::valueChanged, m_config, &AOConfig::setThemeResize);
  connect(ui_font_resize, &QDoubleSpinBox::valueChanged, m_config, &AOConfig::setFontResize);
  connect(ui_fade_duration, &QSpinBox::valueChanged, m_config, &AOConfig::setFadeDuration);

  connect(ui_manual_resize, &QAbstractButton::toggled, m_config, &AOConfig::set_manual_resize);
  // set values
  // meta
  ui_autosave->setChecked(m_config->autosave());

  // general
  ui_username->setText(m_config->username());
  ui_callwords->setText(m_config->callwords());
  ui_advertiser->setText(m_config->server_advertiser());
  ui_server_alerts->setChecked(m_config->server_alerts_enabled());

  // game
  wSettingsLanguage->setCurrentText(m_config->language());
  ui_theme->setCurrentText(m_config->theme());
  ui_manual_gamemode->setCurrentText(m_config->manual_gamemode());
  ui_manual_gamemode_selection->setChecked(m_config->is_manual_gamemode_selection_enabled());
  ui_manual_timeofday->setCurrentText(m_config->manual_timeofday());
  ui_manual_timeofday_selection->setChecked(m_config->is_manual_timeofday_selection_enabled());
  ui_showname->setText(m_config->showname());
  on_showname_placeholder_changed(m_config->showname_placeholder());
  ui_searchable_iniswap->setChecked(m_config->searchable_iniswap_enabled());
  ui_always_pre->setChecked(m_config->always_pre_enabled());
  ui_chat_tick_interval->setValue(m_config->chat_tick_interval());
  ui_chat_ratelimit->setValue(m_config->chat_ratelimit());
  ui_emote_preview->setChecked(m_config->emote_preview_enabled());
  ui_sticky_sfx->setChecked(m_config->sticky_sfx_enabled());

  // ic message
  ui_length_threshold->setValue(m_config->message_length_threshold());

  // log
  ui_log_max_lines->setValue(m_config->log_max_lines());

  if (m_config->log_is_topdown_enabled())
  {
    ui_log_orientation_top_down->setChecked(true);
  }
  else
  {
    ui_log_orientation_bottom_up->setChecked(true);
  }

  ui_log_display_timestamp->setChecked(m_config->log_display_timestamp_enabled());
  ui_log_display_client_id->setChecked(m_config->log_display_client_id_enabled());
  ui_log_display_self_highlight->setChecked(m_config->log_display_self_highlight_enabled());
  ui_log_format_use_newline->setChecked(m_config->log_format_use_newline_enabled());
  ui_log_display_empty_messages->setChecked(m_config->log_display_empty_messages_enabled());
  ui_log_display_music_switch->setChecked(m_config->log_display_music_switch_enabled());
  ui_log_is_recording->setChecked(m_config->log_is_recording_enabled());

  ui_discord_presence->setChecked(m_config->discord_presence());
  ui_discord_hide_server->setChecked(m_config->discord_hide_server());
  ui_discord_hide_character->setChecked(m_config->discord_hide_character());

  // performance

  connect(ui_enable_opengl, &QAbstractButton::toggled, m_config, &AOConfig::set_opengl_enabled);
  connect(m_config, &AOConfig::enable_opengl_changed, ui_enable_opengl, &QAbstractButton::setChecked);
  ui_enable_opengl->setChecked(m_config->opengl_enabled());

  connect(ui_focus_performance_mode, &QAbstractButton::toggled, m_config, &AOConfig::set_focus_performance_mode);
  connect(m_config, &AOConfig::focus_performance_mode_changed, ui_focus_performance_mode, &QAbstractButton::setChecked);

  ui_focus_performance_mode->setChecked(m_config->focus_performance_mode_enabled());

  connect(m_config, &AOConfig::sprite_caching_toggled, this, &AOConfigPanel::set_sprite_caching_toggled);
  connect(this, &AOConfigPanel::emit_sprite_caching_toggled, m_config, &AOConfig::set_sprite_caching);
  for (auto it = m_cache_checkbox_map.cbegin(); it != m_cache_checkbox_map.cend(); ++it)
  {
    QCheckBox *l_checkbox = it.value();
    connect(l_checkbox, &QAbstractButton::toggled, this, &AOConfigPanel::handle_sprite_caching_toggled);
    l_checkbox->setChecked(m_config->sprite_caching_enabled(it.key()));
  }

  connect(m_config, &AOConfig::system_memory_threshold_changed, this, &AOConfigPanel::set_system_memory_threshold);
  connect(ui_system_memory_threshold, &QSlider::valueChanged, m_config, &AOConfig::set_system_memory_threshold);
  set_system_memory_threshold(m_config->system_memory_threshold());

  connect(m_config, &AOConfig::loading_bar_delay_changed, this, &AOConfigPanel::set_loading_bar_delay);
  connect(ui_loading_bar_delay, &QSlider::valueChanged, m_config, &AOConfig::set_loading_bar_delay);
  set_loading_bar_delay(m_config->loading_bar_delay());

  connect(m_config, &AOConfig::caching_threshold_changed, this, &AOConfigPanel::set_caching_threshold);
  connect(ui_caching_threshold, &QSlider::valueChanged, m_config, &AOConfig::set_caching_threshold);
  set_caching_threshold(m_config->caching_threshold());

  // audio
  update_audio_device_list();
  ui_master->setValue(m_config->master_volume());
  ui_suppress_background_audio->setChecked(m_config->suppress_background_audio());
  ui_system->setValue(m_config->system_volume());
  ui_effect->setValue(m_config->effect_volume());
  ui_effect_ignore_suppression->setChecked(m_config->effect_ignore_suppression());
  ui_music->setValue(m_config->music_volume());
  ui_music_ignore_suppression->setChecked(m_config->music_ignore_suppression());
  ui_video->setValue(m_config->video_volume());
  ui_video_ignore_suppression->setChecked(m_config->video_ignore_suppression());
  ui_blip->setValue(m_config->blip_volume());
  ui_blip_ignore_suppression->setChecked(m_config->blip_ignore_suppression());
  ui_blip_rate->setValue(m_config->blip_rate());
  ui_punctuation_delay->setValue(m_config->punctuation_delay());
  ui_blank_blips->setChecked(m_config->blank_blips_enabled());

  ui_theme_resize->setValue(m_config->theme_resize());
  ui_font_resize->setValue(m_config->font_resize());
  ui_fade_duration->setValue(m_config->fade_duration());

  on_manual_gamemode_selection_changed(m_config->is_manual_gamemode_selection_enabled());
  on_manual_timeofday_selection_changed(m_config->is_manual_timeofday_selection_enabled());

  ui_manual_resize->setChecked(m_config->manual_resize());

  ui_about->setText(build_about_message());
}

void AOConfigPanel::showEvent(QShowEvent *event)
{
  QWidget::showEvent(event);

  if (isVisible())
  {
    refresh_theme_list();
    refresh_packages_list();
  }
}

void AOConfigPanel::refreshLanguageList()
{
  wSettingsLanguage->clear();
  for (const QString &r_langauge : localization::getAvailableLanguages())
  {
    wSettingsLanguage->addItem(r_langauge);
  }
  wSettingsLanguage->setCurrentText(m_config->language());
}

void AOConfigPanel::refresh_packages_list()
{
  ui_packages_list->clear();
  //Parse through the stored packages
  QVector<QString> packageNames = FS::Packages::CachedNames();
  QVector<QString> disabledPackages = FS::Packages::DisabledList();

  for (const QString &package : packageNames)
  {
    QListWidgetItem* item = new QListWidgetItem(package, ui_packages_list);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(disabledPackages.contains(package) ? Qt::Unchecked : Qt::Checked);
  }
}

void AOConfigPanel::refresh_theme_list()
{
  const QString l_current_theme = ui_theme->currentText();

  ui_theme->clear();
  std::optional<int> l_theme_index;

  QVector<QString> l_theme_directories = ao_app->get_all_package_and_base_paths("themes");

  for (QString &l_theme_dir : l_theme_directories)
  {
    for (const QFileInfo &i_info : QDir(ao_app->get_case_sensitive_path(l_theme_dir)).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
      const QString l_theme = i_info.fileName();
      if (l_theme == l_current_theme)
      {
        l_theme_index = ui_theme->count();
      }
      ui_theme->addItem(l_theme);
    }
  }

  if (l_theme_index.has_value())
  {
    ui_theme->setCurrentIndex(l_theme_index.value());
  }

  update_theme_controls();
}

void AOConfigPanel::refresh_gamemode_list()
{
  ui_manual_gamemode->blockSignals(true);
  ui_manual_gamemode->clear();

  // add empty entry indicating no gamemode chosen
  ui_manual_gamemode->addItem("<default>");
  // gamemodes
  QString path = ao_app->find_current_theme_path() + "/gamemodes/";

  for (const QString &i_folder : QDir(ao_app->get_case_sensitive_path(path)).entryList(QDir::Dirs))
  {
    if (i_folder == "." || i_folder == "..")
      continue;
    ui_manual_gamemode->addItem(i_folder, i_folder);
  }

  ui_manual_gamemode->setCurrentText(m_config->manual_gamemode());
  ui_manual_gamemode->blockSignals(false);
}

void AOConfigPanel::refresh_timeofday_list()
{
  ui_manual_timeofday->blockSignals(true);
  ui_manual_timeofday->clear();

  // add empty entry indicating no time of day chosen
  ui_manual_timeofday->addItem("<default>");

  const QString l_theme = m_config->theme();
  const QString l_gamemode =
      m_config->is_manual_gamemode_selection_enabled() ? m_config->manual_gamemode() : m_config->gamemode();

  // decide path to look for times of day. This differs whether there is a
  // gamemode chosen or not
  QString l_timeofday_path;

  if (l_gamemode.isEmpty())
    l_timeofday_path = ao_app->find_current_theme_path() + "/times/";
  else
    l_timeofday_path =
        ao_app->find_current_theme_path() + "/gamemodes/" + l_gamemode + "/times/";

  // times of day
  for (const QString &i_folder : QDir(ao_app->get_case_sensitive_path(l_timeofday_path)).entryList(QDir::Dirs))
  {
    if (i_folder == "." || i_folder == "..")
      continue;
    ui_manual_timeofday->addItem(i_folder, i_folder);
  }

  ui_manual_timeofday->setCurrentText(m_config->manual_timeofday());
  ui_manual_timeofday->blockSignals(false);
}

void AOConfigPanel::update_audio_device_list()
{
  const std::optional<DRAudioDevice> l_current_device = m_engine->get_current_device();
  const std::optional<DRAudioDevice> l_favorite_device = m_engine->get_favorite_device();

  std::optional<QString> l_prev_driver;
  std::optional<int> l_prev_driver_index;
  if (ui_device->currentIndex() != -1)
    l_prev_driver = ui_device->currentData().toString();
  QSignalBlocker l_blocker(ui_device);
  ui_device->clear();

  std::optional<int> l_current_driver_index;
  for (const DRAudioDevice &i_device : m_engine->get_device_list())
  {
    if (!i_device.is_enabled())
      continue;
    ui_device->addItem(i_device.get_name(), i_device.get_driver());
    int l_item_index = ui_device->count() - 1;

    const QString l_device_driver = i_device.get_driver();
    if (l_prev_driver.has_value() && l_prev_driver == l_device_driver)
      l_prev_driver_index = l_item_index;

    if (l_current_device.has_value() && l_current_device->get_driver() == l_device_driver)
    {
      ui_device->setItemData(l_item_index, QColor(Qt::lightGray), Qt::BackgroundRole);
      l_current_driver_index = l_item_index;
    }

    if (l_favorite_device.has_value() && l_favorite_device->get_driver() == l_device_driver)
      ui_device->setItemData(l_item_index, QColor(Qt::green), Qt::BackgroundRole);
  }
  ui_device->setCurrentIndex(l_prev_driver_index.value_or(l_current_driver_index.value_or(0)));
}

void AOConfigPanel::updateLanguage(QString t_data)
{
  QString l_language = t_data;
  localization::switchLanguage(l_language);
  m_config->setLanguage(l_language);

  QString l_translators = localization::getCredit();
  if(l_translators.isEmpty())
  {
    wLanguageCredits->hide();
    return;
  }
  wLanguageCredits->setText("Translated to " + l_language + " by " + l_translators);
  wLanguageCredits->show();
}

void AOConfigPanel::update_theme_controls()
{
  const bool l_different_theme = ui_theme->currentText() != m_config->theme();
  ui_switch_theme->setVisible(l_different_theme);
  ui_reload_theme->setHidden(l_different_theme);
}

void AOConfigPanel::on_switch_theme_clicked()
{
  ThemeManager::get().toggleReload();
  m_config->set_theme(ui_theme->currentText());
}

void AOConfigPanel::on_load_packages_clicked()
{
  QVector<QString> disabledList = {};

  for(int i = 0; i < ui_packages_list->count(); ++i)
  {
    QListWidgetItem* item = ui_packages_list->item(i);
    if(item->checkState() == Qt::Unchecked)
    {
      disabledList.append(item->text());
    }
  }
  FS::Packages::SetDisabled(disabledList);
  ao_app->reload_packages();
  refresh_packages_list();
}


void AOConfigPanel::on_reload_theme_clicked()
{
  ThemeManager::get().toggleReload();
  Q_EMIT reload_theme();
}

void AOConfigPanel::on_reload_character_clicked()
{
  Q_EMIT reload_character();
}

void AOConfigPanel::on_reload_audiotracks_clicked()
{
  Q_EMIT reload_audiotracks();
}

void AOConfigPanel::on_theme_changed(QString p_name)
{
  Q_UNUSED(p_name);
  refresh_theme_list();
  refresh_gamemode_list();
  refresh_timeofday_list();
}

void AOConfigPanel::on_gamemode_changed(QString p_text)
{
  ui_gamemode->setText(p_text.isEmpty() ? "<default>" : p_text);
}

void AOConfigPanel::on_manual_gamemode_selection_changed(bool p_enabled)
{
  ui_gamemode->setHidden(p_enabled);
  ui_manual_gamemode->setVisible(p_enabled);
  ui_manual_gamemode_selection->setChecked(p_enabled);
  refresh_gamemode_list();
  refresh_timeofday_list();
}

void AOConfigPanel::on_manual_gamemode_changed(QString p_name)
{
  Q_UNUSED(p_name);
  refresh_gamemode_list();
  refresh_timeofday_list();
}

void AOConfigPanel::on_manual_gamemode_index_changed(QString p_text)
{
  Q_UNUSED(p_text);
  m_config->set_manual_gamemode(ui_manual_gamemode->currentData().toString());
}

void AOConfigPanel::on_timeofday_changed(QString p_text)
{
  ui_timeofday->setText(p_text.isEmpty() ? "<default>" : p_text);
}

void AOConfigPanel::on_manual_timeofday_selection_changed(bool p_enabled)
{
  ui_timeofday->setHidden(p_enabled);
  ui_manual_timeofday->setVisible(p_enabled);
  ui_manual_timeofday_selection->setChecked(p_enabled);
  refresh_timeofday_list();
}

void AOConfigPanel::on_manual_timeofday_changed(QString p_name)
{
  Q_UNUSED(p_name);
  refresh_timeofday_list();
}

void AOConfigPanel::on_manual_timeofday_index_changed(QString p_text)
{
  Q_UNUSED(p_text);
  m_config->set_manual_timeofday(ui_manual_timeofday->currentData().toString());
}

void AOConfigPanel::on_showname_placeholder_changed(QString p_text)
{
  const QString l_showname(p_text.trimmed().isEmpty() ? "Showname" : p_text);
  ui_showname->setPlaceholderText(l_showname);
  ui_showname->setToolTip(l_showname);
}

void AOConfigPanel::on_log_is_topdown_changed(bool p_enabled)
{
  ui_log_orientation_top_down->setChecked(p_enabled);
  ui_log_orientation_bottom_up->setChecked(!p_enabled);
}

void AOConfigPanel::on_device_current_index_changed(int p_index)
{
  if (p_index == -1 || p_index >= ui_device->count())
    return;

  const QString target_device_driver = ui_device->itemData(p_index).toString();
  for (DRAudioDevice &i_device : m_engine->get_device_list())
  {
    if (target_device_driver == i_device.get_driver())
    {
      m_config->set_favorite_device_driver(i_device.get_driver());
      break;
    }
  }
}

void AOConfigPanel::on_audio_device_changed(DRAudioDevice p_device)
{
  Q_UNUSED(p_device)
  update_audio_device_list();
}

void AOConfigPanel::on_favorite_audio_device_changed(DRAudioDevice p_device)
{
  Q_UNUSED(p_device)
  update_audio_device_list();
}

void AOConfigPanel::on_audio_device_list_changed(QVector<DRAudioDevice> p_device_list)
{
  Q_UNUSED(p_device_list);
  update_audio_device_list();
}

void AOConfigPanel::on_volume_value_changed(int p_num)
{
  QSlider* slider = qobject_cast<QSlider*>(sender());
  if (slider && volumeSliderMap.contains(slider)) {
    QLabel* label = volumeSliderMap.value(slider);
    label->setText(QString::number(p_num) + "%");
  }
}

void AOConfigPanel::on_length_threshold_value_changed(int p_number)
{
  ui_length_threshold_label->setText(QString::number(p_number) + "%");
}

void AOConfigPanel::set_sprite_caching_toggled(int p_category, bool p_enabled)
{
  QCheckBox *l_checkbox = m_cache_checkbox_map[SpriteCategory(p_category)];
  l_checkbox->setChecked(p_enabled);
}

void AOConfigPanel::handle_sprite_caching_toggled(bool p_enabled)
{
  QObject *l_sender = sender();
  for (auto it = m_cache_checkbox_map.cbegin(); it != m_cache_checkbox_map.cend(); ++it)
  {
    if (l_sender == it.value())
    {
      emit emit_sprite_caching_toggled(it.key(), p_enabled);
      break;
    }
  }
}

void AOConfigPanel::set_system_memory_threshold(int p_percent)
{
  ui_system_memory_threshold->setValue(p_percent);
  ui_system_memory_threshold_label->setText(QString::number(p_percent) + "%");
}

void AOConfigPanel::set_loading_bar_delay(int p_number)
{
  ui_loading_bar_delay->setValue(p_number);
  ui_loading_bar_delay_label->setText(QString::number(p_number) + "ms");
}

void AOConfigPanel::set_caching_threshold(int p_number)
{
  ui_caching_threshold->setValue(p_number);
  ui_caching_threshold_label->setText(QString::number(p_number) + "%");
}

void AOConfigPanel::updateTabsVisibility(const QModelIndex &current)
{
  QString selected = current.data(Qt::DisplayRole).toString();

  //Create a structure to store which tabs are used for each category
  struct TabInfo
  {
    QVector<int> indices;
    bool visible;
    bool enabled;
  };

  std::map<QString, QVector<int>> tabInfoMap = {
      {"General", {0, 1}},
      {"Audio", {2}},
      {"Packages", {3}},
      {"Theme", {4}},
      {"Message", {5, 6, 7}},
      {"Advanced", {8, 9}},
      {"About", {10}}
  };

  for (int i = 0; i < 11; ++i) {
    tab_widget->setTabVisible(i, false);
    tab_widget->setTabEnabled(i, false);
  }

  tab_widget->setCurrentIndex(0);

  if (tabInfoMap.find(selected) != tabInfoMap.end()) {
    const QVector<int> info = tabInfoMap[selected];
    for (int index : info)
    {
      tab_widget->setTabVisible(index, true);
      tab_widget->setTabEnabled(index, true);
    }
  }

}

void AOConfigPanel::username_editing_finished()
{
  m_config->set_username(ui_username->text());
}

void AOConfigPanel::showname_editing_finished()
{
  m_config->set_showname(ui_showname->text());
}

void AOConfigPanel::advertiser_editing_finished()
{
  m_config->set_server_advertiser(ui_advertiser->text());
}

void AOConfigPanel::callwords_editing_finished()
{
  m_config->set_callwords(ui_callwords->text());
}

void AOConfigPanel::on_config_reload_theme_requested()
{
  refresh_theme_list();
  refresh_gamemode_list();
  refresh_timeofday_list();
}
