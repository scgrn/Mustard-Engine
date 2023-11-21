/**

zlib License

(C) 2020 Andrew Krause

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

**/

/**
    This module facilitates capturing the frame buffer to so a series of TGA images that can be
    stitched together into a video with ffmpeg, ie

    \ffmpeg\bin\ffmpeg -framerate 30 -i REC-F%%05d.tga -c:v libx264 -r 30 -pix_fmt yuv420p -qp 0 output.mp4

    It also logs sfx indices and timestamps that can be played back and captured via [an external tool]

    Beware there is basically no error handling here because it's not something end-users will ever touch

*/

#ifndef AB_CAPTURE_H
#define AB_CAPTURE_H

namespace AB {

//  TODO: pass buffer size
void beginCapture(int w, int h, std::string imgPath, std::string sfxPath);
bool takeScreenshot(std::string filename, int width, int height);
void captureFrame();
void recordSFX(int index);
void endCapture();

extern bool recording;

}   //  namespace

#endif // AB_CAPTURE_H
