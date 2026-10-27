# Building OFP on Mac

This page builds OFP's engine and QtFRED from source on macOS. It follows the setup OFP's automated Mac builds use, but nobody has walked through it on a desktop machine yet. If something here doesn't match what you see, please let us know. For Windows see [Building OFP on Windows](Building-OFP-on-Windows), and for Linux see [Building OFP on Linux](Building-OFP-on-Linux).

If you only want to play or make a mod with OFP, you don't need any of this: see [Using OFP with Knossos.NET](Using-OFP-with-Knossos.NET).

## What you need

Builds work on both Apple Silicon and Intel Macs, for macOS 10.15 or newer.

1. **Xcode Command Line Tools**, for the compiler. In Terminal:

   ```
   xcode-select --install
   ```

   The full Xcode app works too, but isn't needed.

2. **[Homebrew](https://brew.sh)**, then CMake, Ninja and Git:

   ```
   brew install cmake ninja git
   ```

The libraries the engine uses (including Qt for QtFRED, FFmpeg, SDL3, OpenAL and FreeType) either build from OFP's source tree or are downloaded ready-made by CMake, so you don't need to install them yourself.

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
  -DQT_USE_PRECOMPILED=ON
```

What the options do:

- `CMAKE_BUILD_TYPE`: **FastDebug** is optimized but keeps the debug checks for mod data, which is what modders usually want: it reports table and mission errors that Release skips. **Release** is for playing, **Debug** for running under a debugger.
- `CMAKE_INSTALL_PREFIX`: where `ninja install` copies the apps.
- `QT_USE_PRECOMPILED=ON`: downloads a ready-made Qt 6 for QtFRED. To use your own Qt 6 (for example from Homebrew), leave this out.

The build matches your Mac's processor. To build for the other kind, add `-DCMAKE_OSX_ARCHITECTURES=x86_64` (Intel) or `-DCMAKE_OSX_ARCHITECTURES=arm64` (Apple Silicon).

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

This builds everything and copies the engine and QtFRED into `CMAKE_INSTALL_PREFIX` as `.app` bundles. The first build takes a while; later builds only rebuild what changed.

## 4. Run

Start the apps from your FreeSpace 2 folder. To pass command line options such as `-mod MyMod`, run the executable inside the bundle from Terminal, for example:

```
./fs2_open_<version>-FASTDBG.app/Contents/MacOS/fs2_open_<version>-FASTDBG -mod MyMod
```

The exact names depend on the version and configuration. Settings, logs and pilots go in `~/Library/Application Support/HardLightProductions/FreeSpaceOpen/`.

## Keeping up with OFP

```
git pull
git submodule update --init --recursive
cd build
ninja install
```

Ninja reruns CMake by itself when CMake files change. To start over with different options, delete the `build` folder and configure again.
