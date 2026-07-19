# Icy Tower

## Brief description

This is an open source remake of Icy Tower 1.3.1 using Allegro 5.

## Building instructions

First, one need to extract the assets from the original game.

Download the installer of the original game:
* **URL**: [icytower131_install.exe](https://archive.org/download/Icy_Tower/icytower13.rar/icytower13_install.exe)
* **SHA-1**: `a00aa6ebc4c37fac7c44de91671477ef7e32389a`

Install it, then put the following files in `assets/`:

- `data.dat` (found in `<installation directory>/data/data.dat`)
- `sfx13.dat` (found in `<installation directory>/data/sfx13.dat`)
- `harold.dat` (found in `<installation directory>/characters/harold_the_homeboy/harold.dat`)
- `dave.dat` (found in `<installation directory>/characters/disco_dave/dave.dat`)

Alternatively, you can extract these files without needing to install; simply run

```sh
innoextract -I data.dat -I sfx13.dat -I harold.dat -I dave.dat icytower131_install.exe
```

Then run

```sh
cd assets
./extract.sh
```

This will populate `src/misc/gfx/` & `src/misc/sfx/` with the needed assets.

### Linux

Building for a Linux distribution is pretty easy; simply run

```sh
meson setup build src
meson compile -C build
```

However, there might be dependency issues when trying to run the binary on a different pc.

Therefore, one can build a Flatpak bundle; in order to do that, run

```sh
./scripts/setup.sh
./scripts/build.sh
```

This will create an application bundle called `icytower.flatpak`.

One may also build a Flatpak bundle containing debugging information, by running

```sh
./scripts/debug/build.sh
```

This will create a runtime bundle called `icytower.Debug.flatpak`.

### Windows

Building for Windows is done using MinGW, inside a Docker container:

```sh
./scripts/win32/setup.sh
./scripts/win32/build.sh
```

This creates a base Docker image called `icytower-base-win32`, and then a zip archive called `icytower.zip`.

## Installation instructions

### Linux

Install the Flatpak bundle `icytower.flatpak` by running

```sh
./scripts/install.sh
```

Afterwards, run the app:

```sh
./scripts/run.sh
```

Uninstallation can be done by running

```sh
./scripts/uninstall.sh
```

### Windows

Simply unzip the archive `icytower.zip` anywhere, then run `icytower.exe`.

## Debugging (only for Linux)

Install the Flatpak bundle `icytower.Debug.flatpak` by running

```sh
./scripts/debug/install.sh
```

Enter a shell environment inside the Flatpak runtime:

```sh
./scripts/debug/run.sh
```

Then, one may run stuff like `gdb icytower`, etc.

Uninstallation can be done by running

```sh
./scripts/debug/uninstall.sh
```

## TODO

* Implement replay loading and saving
* Add support for custom characters
* Add ability to unlock floors
* Add highscore tables
* Implement missing visual effects (stars & hue-shifting)
* Add controller support
* Add screenshot functionality
* Add fade transitions between game scenes
* Add a desktop icon
