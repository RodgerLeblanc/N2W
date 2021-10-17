# N2W

Pebble watchface that shows Android notifications

### Requirements
- 64-bit Windows operating system.
- [Docker](https://www.docker.com/) installed.

### How to build

This tutorial will use [pebble-dev](https://hub.docker.com/r/bboehmke/pebble-dev) 64-bit Docker image to build the Pebble watchface.
- Open a terminal console.
- Run `git clone https://github.com/RodgerLeblanc/N2W.git C:\Path\Where\You\Want\The\Project\`.
- Run `docker run --rm -it -v "C:/Path/Where/You/Want/The/Project/":/pebble/ bboehmke/pebble-dev pebble build`
- The watchface will build and will generate a `pebble.pbw` file in `/build` folder if build is successful.

### How to install

- Make sure Pebble app is installed on your Android device. The Pebble app is not available through Google Play anymore, you'll have to find the `.apk` [somewhere online](https://www.google.com/search?q=pebble+apk).
- Connect your Pebble watch to the Pebble app.
- Download the `.pbw` on your Android device.
- Find the `.pbw` on your Android device and try to open it. Some device will allow you to choose which app should open this file, select Pebble from the list.
- If you are not able to choose Pebble app as the target to open this file, download [Sideload Helper by Rebble](https://play.google.com/store/apps/details?id=io.rebble.charon&hl=en_CA&gl=US), open the app and install the `.pbw` from there.