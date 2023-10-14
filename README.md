# Mustard engine

Minimalist C++ game framework with no graphical editor that provides utility functions on top of SDL2 / OpenGL. Uses the common subset of OpenGL 3.3 core profile and OpenGL ES 3.0.  
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
| [glad](https://glad.dav1d.de/) | OpenGL Extensions |
| [lua](https://www.lua.org/) | Scripting |
| [SDL2](https://www.libsdl.org/) | Window / input management (desktop / web only) |
| [soloud](https://sol.gfxile.net/soloud/) | Audio |
| [tinyxml](http://www.grinninglizard.com/tinyxml/) | XML parsing |
| [zlib](https://zlib.net/) | File compression |

Features
-------
* Online texture atlassing
* Full Lua API (you don't need to touch any C++ code if you don't want to)
* Pixel-precise collision detection with rotation and scaling
* OpenGL immediate mode emulation
* Steamworks integration

Build and installation
-------

TODO

Usage
-------

TODO

Steamworks Integration
-------

TODO (where to copy files, etc...)

Asset Compiler
-------
Binary is Mustard-AssetCompiler in engine root. This is a command line
tool that compiles *every* file in the directory it was launched, and every file in
every subdirectory recursively, into a single glob.

There is an option to specify a key for a laughably weak encryption scheme.

There are few options and no error checking with this program. It is a loose cannon.
Be careful.

License
-------
Mustard is licensed under an unmodified zlib/libpng license, which is an OSI-certified, BSD-like license that allows static linking with closed source software. Check LICENSE for further details.
