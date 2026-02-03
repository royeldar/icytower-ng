# Icy Tower

## Brief description

This is an open source remake of Icy Tower 1.3.1 using Allegro 5.

## Building instructions

First, one need to extract the assets from the original game.

Download and install Icy Tower 1.3.1, then put the following files in `assets/`:

- `data.dat` (found in `<installation directory>/data/data.dat`)
- `sfx13.dat` (found in `<installation directory>/data/sfx13.dat`)
- `harold.dat` (found in `<installation directory>/characters/harold_the_homeboy/harold.dat`)
- `dave.dat` (found in `<installation directory>/characters/disco_dave/dave.dat`)

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
