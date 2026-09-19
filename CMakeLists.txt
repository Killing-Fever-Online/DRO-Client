cmake_minimum_required(VERSION 3.16)

project(dro-client VERSION 1.8.3 LANGUAGES C CXX)

# forms load at runtime via QUiLoader, no AUTOUIC needed
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets UiTools Network WebSockets Multimedia MultimediaWidgets)

set(HEADERS
  src/dro/animation/keyframe_channel.h
  src/dro/animation/keyframe_sequence.h
  src/dro/fs/fs_mounting.h
  src/dro/fs/fs_reading.h
  src/dro/fs/fs_writing.h
  src/dro/interface/courtroom_layout.h
  src/dro/interface/lobby_layout.h
  src/dro/interface/menus/area_menu.h
  src/dro/interface/menus/bgm_menu.h
  src/dro/interface/menus/char_menu.h
  src/dro/interface/menus/emote_menu.h
  src/dro/interface/scenes/replay_window.h
  src/dro/interface/widgets/bgm_filter.h
  src/dro/interface/widgets/char_button.h
  src/dro/interface/widgets/character_select.h
  src/dro/interface/widgets/chat_log.h
  src/dro/interface/widgets/emotion_button.h
  src/dro/interface/widgets/health_bar.h
  src/dro/interface/widgets/image_display.h
  src/dro/interface/widgets/note_area.h
  src/dro/interface/widgets/note_picker.h
  src/dro/interface/widgets/player_list_slot.h
  src/dro/interface/widgets/rp_button.h
  src/dro/interface/widgets/rp_combo_box.h
  src/dro/interface/widgets/rp_hover_widget.h
  src/dro/interface/widgets/rp_label.h
  src/dro/interface/widgets/rp_line_edit.h
  src/dro/interface/widgets/rp_slider.h
  src/dro/interface/widgets/rp_text_edit.h
  src/dro/interface/widgets/rp_typewriter.h
  src/dro/interface/widgets/rp_widget.h
  src/dro/interface/widgets/screenshot_button.h
  src/dro/interface/widgets/emotion_selector.h
  src/dro/interface/widgets/choice_dialog.h
  src/dro/interface/widgets/sticker_viewer.h
  src/dro/interface/widgets/tab_toggle_button.h
  src/dro/interface/widgets/aotimer.h
  src/dro/interface/widgets/viewports/legacy_viewport.h
  src/dro/interface/widgets/viewports/rp_viewport.h
  src/dro/interface/widgets/evidencelist.h
  src/dro/network/json_packet.h
  src/dro/network/metadata/area_metadata.h
  src/dro/network/metadata/message_metadata.h
  src/dro/network/metadata/user_metadata.h
  src/dro/network/metadata/server_metadata.h
  src/dro/network/metadata/tracklist_metadata.h
  src/dro/param/animation_reader.h
  src/dro/param/replay_reader.h
  src/dro/system/animation.h
  src/dro/system/audio.h
  src/dro/system/audio/ambience_player.h
  src/dro/system/audio/audio_backend.h
  src/dro/system/audio/miniaudio_config.h
  src/dro/system/audio/network_stream.h
  src/dro/system/audio/blip_player.h
  src/dro/system/audio/music_player.h
  src/dro/system/audio/sfx_player.h
  src/dro/system/audio/shout_player.h
  src/dro/system/audio/system_player.h
  src/dro/system/effects.h
  src/dro/system/localization.h
  src/dro/system/replay_playback.h
  src/dro/system/runtime_loop.h
  src/dro/system/text_encoding.h
  src/dro/system/theme_scripting.h
  src/dro/system/debug/time_debugger.h
  src/dro/themes/theme_scene.h
  src/dro/tools/button_maker.h
  src/dro/param/json_reader.h
  src/dro/param/localization_reader.h
  src/dro/param/theme_mode_reader.h
  src/dro/param/theme_module_reader.h
  src/dro/param/theme_reader.h
  src/dro/param/actor/actor_loader.h
  src/dro/param/background/background_data.h
  src/dro/param/background/background_reader.h
  src/dro/param/background/legacy_background_reader.h
  src/dro/param/evidence/evidence_data.h
  src/aoapplication.h
  src/aoconfig.h
  src/aoconfigpanel.h
  src/aoguiloader.h
  src/aoobject.h
  src/aopixmap.h
  src/commondefs.h
  src/courtroom.h
  src/datatypes.h
  src/debug_functions.h
  src/draudio.h
  src/draudiodevice.h
  src/draudioengine.h
  src/draudioengine_p.h
  src/draudioerror.h
  src/draudiostream.h
  src/draudiostreamfamily.h
  src/draudiotrackmetadata.h
  src/drcharactermovie.h
  src/drdiscord.h
  src/dreffectmovie.h
  src/drgraphicscene.h
  src/drmasterclient.h
  src/drmediatester.h
  src/drmovie.h
  src/drpacket.h
  src/drposition.h
  src/drscenemovie.h
  src/drserverinfoeditor.h
  src/drserversocket.h
  src/drshoutmovie.h
  src/drsplashmovie.h
  src/drtheme.h
  src/drthememovie.h
  src/hardware_functions.h
  src/lobby.h
  src/mk2/drplayer.h
  src/mk2/graphicsspriteitem.h
  src/mk2/graphicsvideoscreen.h
  src/mk2/spritecachingreader.h
  src/mk2/spritedynamicreader.h
  src/mk2/spriteplayer.h
  src/mk2/spritereader.h
  src/mk2/spritereadersynchronizer.h
  src/mk2/spriteseekingreader.h
  src/mk2/spriteviewer.h
  src/modules/managers/character_manager.h
  src/modules/managers/notify_manager.h
  src/modules/managers/pair_manager.h
  src/modules/managers/scene_manager.h
  src/modules/theme/thememanager.h
  src/theme.h
  src/utils.h
  src/version.h
  src/eventfilters.h
)

set(SOURCES
  src/dro/animation/keyframe_channel.cpp
  src/dro/animation/keyframe_sequence.cpp
  src/dro/fs/fs_mounting.cpp
  src/dro/fs/fs_reading.cpp
  src/dro/fs/fs_writing.cpp
  src/dro/interface/courtroom_layout.cpp
  src/dro/interface/lobby_layout.cpp
  src/dro/interface/menus/area_menu.cpp
  src/dro/interface/menus/bgm_menu.cpp
  src/dro/interface/menus/char_menu.cpp
  src/dro/interface/menus/emote_menu.cpp
  src/dro/interface/scenes/replay_window.cpp
  src/dro/interface/widgets/bgm_filter.cpp
  src/dro/interface/widgets/char_button.cpp
  src/dro/interface/widgets/character_select.cpp
  src/dro/interface/widgets/chat_log.cpp
  src/dro/interface/widgets/emotion_button.cpp
  src/dro/interface/widgets/health_bar.cpp
  src/dro/interface/widgets/image_display.cpp
  src/dro/interface/widgets/note_area.cpp
  src/dro/interface/widgets/note_picker.cpp
  src/dro/interface/widgets/player_list_slot.cpp
  src/dro/interface/widgets/rp_button.cpp
  src/dro/interface/widgets/rp_combo_box.cpp
  src/dro/interface/widgets/rp_hover_widget.cpp
  src/dro/interface/widgets/rp_label.cpp
  src/dro/interface/widgets/rp_line_edit.cpp
  src/dro/interface/widgets/rp_slider.cpp
  src/dro/interface/widgets/rp_text_edit.cpp
  src/dro/interface/widgets/rp_typewriter.cpp
  src/dro/interface/widgets/rp_widget.cpp
  src/dro/interface/widgets/screenshot_button.cpp
  src/dro/interface/widgets/emotion_selector.cpp
  src/dro/interface/widgets/choice_dialog.cpp
  src/dro/interface/widgets/sticker_viewer.cpp
  src/dro/interface/widgets/tab_toggle_button.cpp
  src/dro/interface/widgets/aotimer.cpp
  src/dro/interface/widgets/viewports/legacy_viewport.cpp
  src/dro/interface/widgets/viewports/rp_viewport.cpp
  src/dro/interface/widgets/evidencelist.cpp
  src/dro/network/json_packet.cpp
  src/dro/network/metadata/area_metadata.cpp
  src/dro/network/metadata/message_metadata.cpp
  src/dro/network/metadata/user_metadata.cpp
  src/dro/network/metadata/server_metadata.cpp
  src/dro/network/metadata/tracklist_metadata.cpp
  src/dro/param/animation_reader.cpp
  src/dro/param/replay_reader.cpp
  src/dro/system/animation.cpp
  src/dro/system/audio.cpp
  src/dro/system/audio/ambience_player.cpp
  src/dro/system/audio/audio_backend.cpp
  src/dro/system/audio/miniaudio_impl.cpp
  3rd/miniaudio/extras/decoders/libvorbis/miniaudio_libvorbis.c
  3rd/miniaudio/extras/decoders/libopus/miniaudio_libopus.c
  3rd/soundtouch/source/SoundTouch/AAFilter.cpp
  3rd/soundtouch/source/SoundTouch/cpu_detect_x86.cpp
  3rd/soundtouch/source/SoundTouch/FIFOSampleBuffer.cpp
  3rd/soundtouch/source/SoundTouch/FIRFilter.cpp
  3rd/soundtouch/source/SoundTouch/InterpolateCubic.cpp
  3rd/soundtouch/source/SoundTouch/InterpolateLinear.cpp
  3rd/soundtouch/source/SoundTouch/InterpolateShannon.cpp
  3rd/soundtouch/source/SoundTouch/mmx_optimized.cpp
  3rd/soundtouch/source/SoundTouch/RateTransposer.cpp
  3rd/soundtouch/source/SoundTouch/SoundTouch.cpp
  3rd/soundtouch/source/SoundTouch/sse_optimized.cpp
  3rd/soundtouch/source/SoundTouch/TDStretch.cpp
  src/dro/system/audio/network_stream.cpp
  src/dro/system/audio/blip_player.cpp
  src/dro/system/audio/music_player.cpp
  src/dro/system/audio/sfx_player.cpp
  src/dro/system/audio/shout_player.cpp
  src/dro/system/audio/system_player.cpp
  src/dro/system/effects.cpp
  src/dro/system/localization.cpp
  src/dro/system/replay_playback.cpp
  src/dro/system/runtime_loop.cpp
  src/dro/system/text_encoding.cpp
  src/dro/system/theme_scripting.cpp
  src/dro/system/debug/time_debugger.cpp
  src/dro/themes/theme_scene.cpp
  src/dro/tools/button_maker.cpp
  src/dro/param/json_reader.cpp
  src/dro/param/localization_reader.cpp
  src/dro/param/theme_mode_reader.cpp
  src/dro/param/theme_module_reader.cpp
  src/dro/param/theme_reader.cpp
  src/dro/param/actor/actor_loader.cpp
  src/dro/param/background/background_data.cpp
  src/dro/param/background/background_reader.cpp
  src/dro/param/background/legacy_background_reader.cpp
  src/dro/param/evidence/evidence_data.cpp
  src/aoapplication.cpp
  src/aoconfig.cpp
  src/aoconfigpanel.cpp
  src/aoguiloader.cpp
  src/aoobject.cpp
  src/aopixmap.cpp
  src/charselect.cpp
  src/commondefs.cpp
  src/courtroom.cpp
  src/courtroom_character.cpp
  src/courtroom_sfx.cpp
  src/courtroom_widgets.cpp
  src/datatypes.cpp
  src/debug_functions.cpp
  src/draudio.cpp
  src/draudiodevice.cpp
  src/draudioengine.cpp
  src/draudioengine_p.cpp
  src/draudioerror.cpp
  src/draudiostream.cpp
  src/draudiostreamfamily.cpp
  src/draudiotrackmetadata.cpp
  src/drcharactermovie.cpp
  src/dreffectmovie.cpp
  src/drgraphicscene.cpp
  src/drmasterclient.cpp
  src/drmediatester.cpp
  src/drmovie.cpp
  src/drpacket.cpp
  src/drposition.cpp
  src/drscenemovie.cpp
  src/drserverinfoeditor.cpp
  src/drserversocket.cpp
  src/drshoutmovie.cpp
  src/drsplashmovie.cpp
  src/drdiscord.cpp
  src/drtheme.cpp
  src/drthememovie.cpp
  src/emotes.cpp
  src/hardware_functions.cpp
  src/lobby.cpp
  src/main.cpp
  src/mk2/drplayer.cpp
  src/mk2/graphicsspriteitem.cpp
  src/mk2/graphicsvideoscreen.cpp
  src/mk2/spritecachingreader.cpp
  src/mk2/spritedynamicreader.cpp
  src/mk2/spriteplayer.cpp
  src/mk2/spriteseekingreader.cpp
  src/modules/managers/character_manager.cpp
  src/modules/managers/notify_manager.cpp
  src/modules/managers/pair_manager.cpp
  src/modules/managers/scene_manager.cpp
  src/modules/theme/thememanager.cpp
  src/path_functions.cpp
  src/server_socket.cpp
  src/mk2/spritereader.cpp
  src/mk2/spritereadersynchronizer.cpp
  src/mk2/spriteviewer.cpp
  src/text_file_functions.cpp
  src/theme.cpp
  src/utils.cpp
  src/version.cpp
  src/eventfilters.cpp
)

set(RESOURCES
  res.qrc
)

if(NOT DRO_VCPKG_DIR AND DEFINED ENV{VCPKG_INSTALLATION_ROOT})
  set(DRO_VCPKG_DIR "$ENV{VCPKG_INSTALLATION_ROOT}/installed/x64-windows")
endif()

find_path(OPUSFILE_INCLUDE_DIR opus/opusfile.h HINTS "${DRO_VCPKG_DIR}/include")
find_path(VORBIS_INCLUDE_DIR vorbis/vorbisfile.h HINTS "${DRO_VCPKG_DIR}/include")
find_library(OGG_LIBRARY NAMES ogg libogg HINTS "${DRO_VCPKG_DIR}/lib")
find_library(VORBIS_LIBRARY NAMES vorbis libvorbis HINTS "${DRO_VCPKG_DIR}/lib")
find_library(VORBISFILE_LIBRARY NAMES vorbisfile libvorbisfile HINTS "${DRO_VCPKG_DIR}/lib")
find_library(OPUS_LIBRARY NAMES opus libopus HINTS "${DRO_VCPKG_DIR}/lib")
find_library(OPUSFILE_LIBRARY NAMES opusfile libopusfile HINTS "${DRO_VCPKG_DIR}/lib")

foreach(dep OPUSFILE_INCLUDE_DIR VORBIS_INCLUDE_DIR OGG_LIBRARY VORBIS_LIBRARY VORBISFILE_LIBRARY OPUS_LIBRARY OPUSFILE_LIBRARY)
  if(NOT ${dep})
    message(FATAL_ERROR "Audio codec dependency not found: ${dep}. Set DRO_VCPKG_DIR to your vcpkg installed triplet directory.")
  endif()
endforeach()

add_executable(dro-client
  ${HEADERS}
  ${SOURCES}
  ${RESOURCES}
  src/dro/param/actor_repository.h src/dro/param/actor_repository.cpp
  src/drevidencemovie.cpp
  src/drevidencemovie.h
)

target_compile_definitions(dro-client PRIVATE
    QT_WARN_DEPRECATED_UP_TO=0x060500
)

target_include_directories(dro-client PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/3rd
  ${CMAKE_CURRENT_SOURCE_DIR}/3rd/lua
  ${CMAKE_CURRENT_SOURCE_DIR}/3rd/soundtouch/include
  ${OPUSFILE_INCLUDE_DIR}
  ${OPUSFILE_INCLUDE_DIR}/opus
  ${VORBIS_INCLUDE_DIR}
)

set_target_properties(dro-client PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY $<1:${CMAKE_CURRENT_LIST_DIR}/bin>
        RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_CURRENT_LIST_DIR}/bin>
)

target_link_directories(dro-client PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/3rd/discord-rpc
  ${CMAKE_CURRENT_SOURCE_DIR}/3rd/lua
)

target_link_libraries(dro-client PRIVATE
  Qt6::Core
  Qt6::Gui
  Qt6::Widgets
  Qt6::UiTools
  Qt6::Network
  Qt6::WebSockets
  Qt6::Multimedia
  Qt6::MultimediaWidgets
  ${OGG_LIBRARY}
  ${VORBIS_LIBRARY}
  ${VORBISFILE_LIBRARY}
  ${OPUS_LIBRARY}
  ${OPUSFILE_LIBRARY}
  discord-rpc
  lua54
)

if(WIN32)
    set_target_properties(dro-client PROPERTIES WIN32_EXECUTABLE TRUE)
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/icons/logo-client.rc")
    target_sources(dro-client PRIVATE ${APP_ICON_RESOURCE_WINDOWS})
endif()

