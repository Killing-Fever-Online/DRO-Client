# Danganronpa-Online-Client
This is the a forked version of the offical DRO client last updated by Crystal Warrior. it is a derivative of [Attorney-Online-Client-Remake](https://github.com/AttorneyOnline/AO2-Client). Created by SyntaxNyah not a replacement all credits to the upstream forks from each interation for laying the ground work.

## ROADMAP so far. Current stable version 1.8.4

Main goal get people upgrade from 1.8.0 to 1.8.4 by addressing bugs so crystal warriors work doesn't go to waste. 


Currently fixed, changed or Added since V1.82.0.
---------------------

Audio engine Bass.DLL removed and changed to miniaudio. Replaced a propriety audio engine with an AGPLv3 compatible one 

Backgrounds no longer are blank in the view port upon entering areas. 

Text no longer cuts off at all. It always follows the text wraps perfectly and doesn't go off screen.
Cleaned and fixed additive text! 

Make the additive text function a UI element a check box of sorts a tiny check box, so people dont have to type everytime they want to use additive, its one tiny check box then all messages are treated as additive. A clean option just like how AO2 client does it. This basically allows you to use additive without constantly typing everytime like a checkbox. 

You can now set custom asset/base paths. 

Changed the reload button to "change theme" to be more clear and user friendly. 

A toggle to disable blankposts so if you press ENTER, nothing sends. 

Softer toggle called soft blankpost option where u have to press ENTER at least three times on an empty message to send a BLANKPOST. Purpose is to prevent accidental blankposts. 

Added a new library called SoundTouch which lets you adjust pitch and tempo. Also fixed music streaming was broken in 1.8.4. You can also stream opus and ogg files now. WAV OPUS MP3 OGG 

Current Bug reports so far that I will work on in the future
-----------------------------
Fix linux macos builds

Feature requests or quality of life improvement requests
----------------------------------------------------------------------

Voting GUI Server-side voting system with a client-side GUI popup. Polling options for group decisions. Results display at conclusion 🚧

Add a secondary message/mini message system called react messages. It's bassically when someone sends a message they somehow have a way of adding a mini message on top of it. So main message would be like, "I like danganronpa", the react message would be a tiny message somewhere else maybe the side of the port or it just flys across the viewport, that says me too, so it feels more like a danganronpa game. Also for server side, maybe make it a toggle if reacts can be used, and displayed. And for the GM they can customize how much reacts can be used per area. Use the DR 1 asset to display these 🚧🚧

Add Asset streaming capabilities and add a compatibility mode of sorts so DRO client can connect to modern servers just like AO2 client. This includes base and asset streaming. AO2 compat mode so modern servers like akashi can accept DRO client without issue, and if it shows no files should ask you for an option to stream the assets. 🚧🚧 🚧

Multi-viewport interface: Follow multiple players simultaneously in separate viewports within one client; viewports display real-time chat activity. Main viewport: Click any viewport to make it the active/chat-focused one. GM ping system: Players can ping the GM triggering a red flashing border on that player's viewport for instant attention. Performance optimization: Skip animations on non-dominant viewports to save battery/performance; careful attention needed to avoid excessive redrawing and performance bugs. 🚧🚧🚧

Add HTTPS/WSS support for DRO client and TsuserverDR so server owners can have the OPTION utilize TLS and modern https if they so want to to protect against snooping MITM attacks(this is funny to write but yeah you get the point, https is just more secure and faster) and even cloudflare caching. This will mainly benefit servers that use websockets though and have a web server, for most DRO users this won't really change much. 🚧 🚧

Write prooject documentation with detailed easy to understand guides such as building the project with qt 6 maybe cross platform etc 🚧

Add Linux Builds back make DRO linux compatible again 🚧 🚧

Not a replacement for DRO client, just a fork intended to clean-up some legacy crust and fix some issues.

## Qt
This project uses Qt6.8.3, which is licensed under the [GNU Lesser General Public License](https://www.gnu.org/licenses/lgpl-3.0.txt) with [certain licensing restrictions and exceptions](https://www.qt.io/qt-licensing-terms/). To comply with licensing requirements for static linking, object code is available if you would like to relink with an alternative version of Qt, and the source code for Qt may be found at https://github.com/qt/qtbase, http://code.qt.io/cgit/, or at https://qt.io.

Copyright (C) 2026 The Qt Company Ltd.

## miniaudio
This client runs on [miniaudio](https://miniaud.io/). It is released into the public domain, or MIT-0 at your option.

## SoundTouch
This client uses SoundTouch (https://www.surina.net/soundtouch) to process audio for independent pitch and tempo adjustment. SoundTouch is licensed under the GNU Lesser General Public License, version 2.1 (LGPL 2.1). The full license text is available at https://www.gnu.org/licenses/lgpl-2.1.html.

SoundTouch audio processing library
Copyright (c) Olli Parviainen

