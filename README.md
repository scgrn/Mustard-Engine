# Mustard engine

Minimalist C++ game framework with no graphical editor that provides utility functions on top of SDL2 / OpenGL.  
Uses the common subset of OpenGL 3.3 core profile and OpenGL ES 3.0.  
Tested under Ubuntu and Windows using GCC/Clang/MSVC.


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
* Online texture atlassing
* Full Lua API for 2D (you don't need to touch any C++ code if you don't want to)
* Pixel-precise collision detection with rotation and scaling
* OpenGL immediate mode emulation
* Steamworks integration

Build and installation
-------
Install the latest [SDL2 Development Libraries](https://www.libsdl.org/) as SDL2 is not vendored in this repo.

<!-- TODO -->

Usage
-------

Copy the projectTemplate folder into Mustard's parent folder and rename it to your project name.

You're also free to copy the projectTemplate folder to wherever you'd like, but you'll have to  
edit the build scripts to point them back to Mustard.

...

Both debug and release mode will read assets from the assets folder.

<!-- TODO -->

Steamworks Integration
-------

<!-- TODO (where to copy files, etc...) -->

Asset Compiler
-------
During development you will typically not need to invoke the asset compiler-
building a project in `dist` config will call the asset compiler to build the archive for you.

Binary is Mustard-AssetCompiler in the `bin` directory. This is a command line
tool that compiles *every* file in the directory it was launched, and every file in
every subdirectory recursively, into a single glob.

There is an option to specify a key for a laughably weak encryption scheme.

There are few options and no error checking with this program. It is a loose cannon.

License
-------
Mustard is licensed under an unmodified zlib/libpng license, which is an OSI-certified, BSD-like license that allows static linking with closed source software. Check LICENSE for further details.

Contact
-------
Andrew Krause - ajkrause@gmail.com

Project Link: [https://github.com/scgrn/Mustard-Engine](https://github.com/scgrn/Mustard-Engine)


