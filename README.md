# Mustard engine

Minimalist C++ game framework with no graphical editor that provides utility functions on top of SDL2 / OpenGL.  
Uses the common subset of OpenGL 3.3 core profile and OpenGL ES 3.0.  
Tested under Ubuntu and Windows using GCC/Clang/MSVC.

![Mustard engine gif](https://github.com/scgrn/Mustard-Engine/blob/main/docs/mustard-20fps.gif)  

[![License](https://img.shields.io/badge/license-zlib%2Flibpng-blue.svg)](LICENSE)

Target platforms:
-------
* Windows
* Linux
* Android
* Web (via Emscripten)

Dependencies
-------
| Library | Purpose |
|:---|---:|
| [glad](https://glad.dav1d.de/) | OpenGL extensions on Windows |
| [lua](https://www.lua.org/) | Scripting |
| [SDL2](https://www.libsdl.org/) | Window / input management (desktop / web only) |
| [soloud](https://sol.gfxile.net/soloud/) | Audio mixing and playback |
| [tinyxml](http://www.grinninglizard.com/tinyxml/) | XML parsing |
| [zlib](https://zlib.net/) | Compression |

Features
-------
* Full Lua API for 2D (you don't need to touch any C++ code if you don't want to)
* Online texture atlassing
* Pixel-precise collision detection with rotation and scaling
* OpenGL immediate mode emulation
* Steamworks integration
* Lightweight and fast

Build and installation
-------
You will need [CMake](https://cmake.org/) version 3.12 or above for build configuration. Mustard has been tested generating Unix Makefiles on Linux and MinGW Makefiles
on Windows. No other generators have been tested, so YMMV.  

You will also need to install SDL2 as it is not vendored in this repo.

#### Windows:  
Download the latest [SDL2 Development Library](https://www.libsdl.org/) for your compiler and extract it somewhere permanently.  
Clone the Mustard repository and navigate into it.  

Edit this line of src/platform/desktop/CMakeLists.txt:  

`set(SDL2_DIR "/libs/SDL2-2.26.5/cmake")`  

...to point to your SDL2 installation's cmake directory.  

From the command line, run `build all`  

#### Linux:  
Install SDL2 with `sudo apt-get install libsdl2-dev`  
Clone the Mustard repository and navigate into it.  
Run `./build.sh all`

<!-- TODO -->

Usage
-------

Copy the projectTemplate folder somwhere (Mustard's parent folder is a good place) and rename it to your project name.

Edit `project.cfg`:
Change PROJECT_NAME to the name of your project, obvi.  

MUSTARD_PATH should point to wherever you've installed Mustard. You'll need to edit this if you've copied the 
projectTemplate folder to anywhere other than Mustard's parent folder.  

If you're on Windows, you'll also need to edit SDL2_PATH to point to your SDL2 installation's cmake directory.

Run `build` or `./build.sh` (depending on your development platform) without arguments to see a list of build options. Use the
`run` script to run your project after building.

<!-- Both debug and release mode will read assets from the assets folder. -->

<!-- TODO (link to APIs, etc -->

Steamworks Integration
-------

<!-- TODO (where to copy files, etc...) -->

Asset Compiler
-------
During development you will typically not need to invoke the asset compiler-
building a project in the `dist` configuration will call the asset compiler to build the archive for you.

If you must use it for some exotic reason, the binary is Mustard-AssetCompiler in the `bin` directory. This is a command line
tool that compiles *every* file in the directory it was launched, and every file in every subdirectory recursively, into a single glob.

There is an option to specify a key for a laughably weak encryption scheme.

There are few options and no error checking with this program. It is a loose cannon.

License
-------
Mustard is licensed under an unmodified zlib/libpng license, which is an OSI-certified, BSD-like license that allows static linking with closed source software. Check LICENSE for further details.

Contact
-------
Andrew Krause - ajkrause@gmail.com

Project Link: [https://github.com/scgrn/Mustard-Engine](https://github.com/scgrn/Mustard-Engine)


