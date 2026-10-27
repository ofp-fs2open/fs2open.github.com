# Building OFP on Windows

This page sets up a Windows machine to build OFP's engine and QtFRED from source with Visual Studio. It's based on SCP's [Building on Windows](https://github.com/scp-fs2open/fs2open.github.com/wiki/Building-on-Windows) page, updated for current tools. For other systems see [Building OFP on Linux](Building-OFP-on-Linux) and [Building OFP on Mac](Building-OFP-on-Mac).

If you only want to play or make a mod with OFP, you don't need any of this: see [Using OFP with Knossos.NET](Using-OFP-with-Knossos.NET).

## What you need

| Tool | Notes |
|---|---|
| [Git](https://git-scm.com/download/win) | Any recent version. |
| [Visual Studio](https://visualstudio.microsoft.com/) 2022 or 2026 | The free Community edition is all you need. In the installer, pick the **Desktop development with C++** workload; Visual Studio takes care of the MFC parts the old FRED2 needs. |
| [CMake](https://cmake.org/download/) | 3.21 or newer. Visual Studio 2026 needs CMake **4.2 or newer**, since older versions have no generator for it. Install the standalone version to get cmake-gui. |
| [Qt 6](https://www.qt.io/download-qt-installer-oss) (optional) | Used by QtFRED. Install the **MSVC 2022 64-bit** kit (6.8.3 is known to work, with Visual Studio 2022 and 2026 alike). If CMake can't find a Qt install, it downloads a prebuilt Qt for you instead. |

## 1. Get the source

Clone the OFP repository with its submodules:

```
git clone --recursive https://github.com/ofp-fs2open/fs2open.github.com.git OFP
```

If you already cloned without `--recursive`, run this inside the clone:

```
git submodule update --init --recursive
```

Without the submodules, CMake stops with `External submodules could not be found`.

## 2. Configure with CMake

1. Start **cmake-gui**.
2. Set **Where is the source code** to your clone, and **Where to build the binaries** to a `build` folder inside it. The build folder must be different from the source folder.
3. If you installed Qt, click **Add Entry** and add `QT6_INSTALL_ROOT` (type PATH) set to your Qt kit folder, for example `C:/Qt/6.8.3/msvc2022_64`. **Do this before the first Configure.** If you add it later, CMake has already cached the prebuilt Qt and the two get mixed up; use **File > Delete Cache** and start over.
4. Click **Configure**. Pick the generator for your Visual Studio (**Visual Studio 17 2022** or **Visual Studio 18 2026**), platform **x64**, and leave the toolset blank. A 32-bit build can't build QtFRED.
5. When it finishes, look for `Using system Qt libraries` in the output if you set `QT6_INSTALL_ROOT`.
6. Set these variables, using forward slashes (`/`) in paths or the build fails:
   - `CMAKE_INSTALL_PREFIX`: your FreeSpace 2 folder. The **INSTALL** target copies the built executables here. The default is `C:/Program Files/FS2_Open`, which needs admin rights, so change it.
   - `FSO_FREESPACE_PATH`: also your FreeSpace 2 folder. It lets you run and debug the executables straight from Visual Studio.
7. Click **Configure** again, then **Generate**, and wait for `Generating done`.
8. Click **Open Project**.

CMake 4.3 and newer write `FS2_Open.slnx` instead of `FS2_Open.sln`. If an old `FS2_Open.sln` is left in the build folder from an earlier CMake, delete it (and the `build/.vs` folder) so you don't open the stale one.

### Useful variables

| Variable | Default | What it does |
|---|---|---|
| `FSO_BUILD_QTFRED` | ON | Build QtFRED. |
| `FSO_BUILD_FRED2` | ON | Build the old MFC FRED2. Turn it off if you don't use it. |
| `FSO_BUILD_WITH_VULKAN` | ON | Build the Vulkan renderer. |
| `FSO_BUILD_WITH_OPENGL` | ON | Build the OpenGL renderer. |
| `FSO_BUILD_TESTS` | OFF | Build the unit tests. |
| `FSO_BUILD_TOOLS` | OFF | Build some FSO tools. |
| `FSO_USE_SPEECH` / `FSO_USE_VOICEREC` | ON | Text-to-speech and voice recognition support. |
| `MSVC_SIMD_INSTRUCTIONS` | | The instruction set the executables are optimized for (for example AVX). |
| `FSO_RUN_ARGUMENTS` | | Command line arguments passed when you run an executable from Visual Studio, for example `-mod MyMod`. |

**File > Delete Cache** resets every one of these to its default, including `CMAKE_INSTALL_PREFIX` and `FSO_FREESPACE_PATH`, so note any you changed first.

## 3. Build

Pick a configuration in Visual Studio's toolbar:

- **Debug**: for running with the debugger attached. Slow.
- **FastDebug**: optimized, but keeps the debug checks for mod data. This is what modders usually want: it reports table and mission errors that Release skips.
- **Release**: fully optimized, for playing.

Then in Solution Explorer, open the **CMakePredefinedTargets** folder, right-click **INSTALL** and click **Build**. This builds everything and copies the executables (plus the DLLs and, for QtFRED, the Qt files they need) into `CMAKE_INSTALL_PREFIX`.

The first build takes a while (around half an hour for Debug); later builds only rebuild what changed. Warnings C4701/C4703 in `sexp.cpp` and `vk_mem_alloc.h` and the CMP0219 policy warning are expected.

## 4. Run and debug from Visual Studio

With `FSO_FREESPACE_PATH` set, right-click the `Freespace2` or `qtfred` project, click **Set as Startup Project**, and press F5. Put the mod's command line options (such as `-mod MyMod`) in `FSO_RUN_ARGUMENTS`, or in the project's **Properties > Debugging > Command Arguments**.

## Keeping up with OFP

To get the latest OFP changes:

```
git pull
git submodule update --init --recursive
```

Visual Studio reruns CMake by itself when CMake files change. If you move to a new Visual Studio version, the old build folder won't build (error MSB8020, toolset not found): delete the cache and configure again with the new generator, as in step 2.
