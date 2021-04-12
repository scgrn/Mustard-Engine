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

#include "../pch.h"

#include "capture.h"
#include "../misc/misc.h"
#include "../core/log.h"

namespace AB {

static char* screenshotBuffer;
static int width, height;
static int frame, session;
static std::ofstream sfxLog;
static std::string imagePath;

bool recording;

void beginCapture(int w, int h, std::string imgPath, std::string sfxPath) {
    width = w;
    height = h;

    // http://www.david-amador.com/2012/09/how-to-take-screenshot-in-opengl/
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    int nSize = width * height * 3;
    screenshotBuffer = (char*)malloc(nSize*sizeof(char));

    frame = 0;
    session = 0;
    recording = false;

    //  create SFX log file
    sfxLog = std::ofstream(sfxPath + "sfx.csv");

    imagePath = imgPath;
}

//  TODO: guh this needs a better API
bool takeScreenshot(std::string filename, int width, int height) {
    int nSize = width * height * 3;

    char* tempScreenshotBuffer = (char*)malloc(nSize*sizeof(char));

    if (!tempScreenshotBuffer) {
        LOG("NO SCREENSHOT BUFFER!", 0);
        return false;
    }

    glReadPixels((GLint)0, (GLint)0, (GLint)width, (GLint)height, GL_BGR, GL_UNSIGNED_BYTE, tempScreenshotBuffer);

    FILE *filePtr = fopen(filename.c_str(), "wb");
    if (!filePtr) {
        free(tempScreenshotBuffer);
        LOG("COULDN'T TAKE SCREENSHOT: %s", filename.c_str());
        return false;
    }

    unsigned char TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};
    unsigned char header[6] = { (unsigned char)(width % 256), (unsigned char)(width / 256),
		(unsigned char)(height % 256), (unsigned char)(height / 256), 24, 0};

    fwrite(TGAheader, sizeof(unsigned char), 12, filePtr);
    fwrite(header, sizeof(unsigned char), 6, filePtr);
    fwrite(tempScreenshotBuffer, sizeof(GLubyte), nSize, filePtr);
    fclose(filePtr);

    free(tempScreenshotBuffer);

    return true;
}

void captureFrame() {
    std::string f = toString(frame, false);
    if (f.length() == 1) {
        f = "0000" + f;
    }
    if (f.length() == 2) {
        f = "000" + f;
    }
    if (f.length() == 3) {
        f = "00" + f;
    }
    if (f.length() == 4) {
        f = "0" + f;
    }
    takeScreenshot(imagePath + "REC-F" + f + ".tga", width, height);

    frame++;
    //graphics->fonts.get(1)->setColor(1,0.2,0.2,1);
    //graphics->fonts.get(1)->printString(20,30,1,Font::Align::LEFT,"REC - F" + f);
}

void routeSound(int index) {
}

void endCapture() {
    free(screenshotBuffer);

    //  close SFX log file
    sfxLog.close();
}

void recordSFX(int index) {
    if (recording) {
        sfxLog << frame << ", " << index << "," << std::endl;
    }
}

}   //  namespace
