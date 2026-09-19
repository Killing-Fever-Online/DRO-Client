# How to run

1. Install Qt6 and the media libraries.

Ubuntu / Debian:
```
sudo apt install libqt6core6 libqt6gui6 libqt6widgets6 libqt6network6 libqt6websockets6 libqt6multimedia6 gstreamer1.0-libav
```

Fedora:
```
sudo dnf install qt6-qtbase qt6-qtwebsockets qt6-qtmultimedia gstreamer1-libav
```

Arch:
```
sudo pacman -S qt6-base qt6-websockets qt6-multimedia gst-libav
```

2. Make the files runnable.
```
chmod +x dro-client.sh dro-client
```

3. Start it.
```
./dro-client.sh
```

## If it does not start

Run `./dro-client` in a terminal and read the error. If it mentions a missing `libQt6` file, step 1 did not finish. Run it again.
