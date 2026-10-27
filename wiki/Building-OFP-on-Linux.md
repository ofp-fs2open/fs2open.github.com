# Building OFP on Linux

This page builds OFP's engine and QtFRED from source on Linux. It follows the setup OFP's automated Linux builds use, but nobody has walked through it on a desktop machine yet. If something here doesn't match what you see, please let us know. For Windows see [Building OFP on Windows](Building-OFP-on-Windows), and for Mac see [Building OFP on Mac](Building-OFP-on-Mac).

If you only want to play or make a mod with OFP, you don't need any of this: see [Using OFP with Knossos.NET](Using-OFP-with-Knossos.NET).

## What you need

You need a C++17 compiler (GCC 9 or newer, or Clang 16 or newer), CMake 3.21 or newer, Ninja, pkg-config and Git. The libraries the engine uses either build from OFP's source tree or are downloaded ready-made by CMake, so you don't need to install them yourself.

**Ubuntu / Debian:**

```
sudo apt install build-essential git cmake ninja-build pkg-config
```

**Fedora:**

```
sudo dnf install gcc gcc-c++ git cmake ninja-build pkgconf-pkg-config
```

**Arch:**

```
sudo pacman -S base-devel git cmake ninja
```

Ubuntu 22.04 and older ship a CMake older than 3.21. Get a newer one from [cmake.org](https://cmake.org/download/) or with `pip install cmake`.

## 1. Get the source

```
git clone --recursive https://github.com/ofp-fs2open/fs2open.github.com.git OFP
cd OFP
```

If you already cloned without `--recursive`, run `git submodule update --init --recursive` inside the clone. Without the submodules, CMake stops with `External submodules could not be found`.

## 2. Configure

Make a build folder and run CMake from it. Replace `~/FreeSpace2` with the folder that holds your FreeSpace 2 data:

```
mkdir build
cd build
cmake -G Ninja .. \
  -DCMAKE_BUILD_TYPE=FastDebug \
  -DCMAKE_INSTALL_PREFIX="$HOME/FreeSpace2" \
  -DFSO_BUILD_INCLUDED_LIBS=ON \
  -DFFMPEG_USE_PRECOMPILED=ON \
  -DQT_USE_PRECOMPILED=ON
```

What the options do:

- `CMAKE_BUILD_TYPE`: **FastDebug** is optimized but keeps the debug checks for mod data, which is what modders usually want: it reports table and mission errors that Release skips. **Release** is for playing, **Debug** for running under a debugger.
- `CMAKE_INSTALL_PREFIX`: where `ninja install` copies the executables.
- `FSO_BUILD_INCLUDED_LIBS=ON`: builds Lua, jansson, libpng, libjpeg and zlib from OFP's source tree instead of using your system's copies.
- `FFMPEG_USE_PRECOMPILED=ON`: downloads ready-made LGPL FFmpeg libraries. Your distribution's FFmpeg is often a GPL build, and binaries built against it can't be shared with others.
- `QT_USE_PRECOMPILED=ON`: downloads a ready-made Qt 6 for QtFRED. To use your distribution's Qt 6 instead, leave this out and install its Qt 6 development packages (base, OpenGL and the help tools).

SDL3, OpenAL and FreeType use your system's copies when it has them, and are downloaded ready-made when it doesn't.

When it works, the last line is `-- Build files have been written to: ...`.

Other useful options, added as `-D<name>=<value>`:

| Option | Default | What it does |
|---|---|---|
| `FSO_BUILD_QTFRED` | ON | Build QtFRED. |
| `FSO_BUILD_WITH_VULKAN` | ON | Build the Vulkan renderer. |
| `FSO_BUILD_WITH_OPENGL` | ON | Build the OpenGL renderer. |
| `FSO_BUILD_TESTS` | OFF | Build the unit tests. |
| `FSO_BUILD_TOOLS` | OFF | Build some FSO tools. |

## 3. Build

```
ninja install
```

This builds everything and copies the executables into `CMAKE_INSTALL_PREFIX`. The first build takes a while; later builds only rebuild what changed.

## 4. Run

Run the executables from your FreeSpace 2 folder, for example `./fs2_open_<version>-FASTDBG -mod MyMod`. The exact names depend on the version and configuration. Settings, logs and pilots go in `~/.local/share/HardLightProductions/FreeSpaceOpen/`.

If QtFRED won't start and complains about the `xcb` platform plugin, install your distribution's `libxcb-cursor0` (Ubuntu/Debian) or `xcb-util-cursor` (Fedora, Arch) package.

## Keeping up with OFP

```
git pull
git submodule update --init --recursive
cd build
ninja install
```

Ninja reruns CMake by itself when CMake files change. To start over with different options, delete the `build` folder and configure again.
