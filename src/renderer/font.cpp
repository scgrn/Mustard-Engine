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

#include <iostream>

#include "tinyxml/tinyxml.h"

#include "font.h"
#include "../core/log.h"

namespace AB {

Font::Character::Character(std::shared_ptr<Texture> texture, int x, int y, int width, int height, int xOffset, int yOffset, int xAdvance, float scaleW, float scaleH) {
	//	init texture coordinates
	float u1 = x / scaleW;
	float v1 = y / scaleH;
	float u2 = (x + width) / scaleW;
	float v2 = (y + height) / scaleH;

    this->texture = texture;
    this->width = width;
    this->height = height;
    this->u1 = u1;
    this->v1 = v1;
    this->u2 = u2;
    this->v2 = v2;

    this->xOffset = xOffset;
    this->yOffset = -yOffset;
 	this->xAdvance = xAdvance;
}

Font::Character::~Character() {}

void Font::load(std::string const& filename) {
    if (filename == "default") {
        LOG("Creating built-in font", 0);
        buildDefault();
    } else {
        LOG("Loading font <%s>", filename.c_str());

        TiXmlDocument *DOM;
        unsigned int lengthRead;
        DataObject dataObject(filename.c_str(), &lengthRead);
        char *buffer = (char*)dataObject.getData();
        if (buffer == 0) {
            ERR("Couldn't open <%s>", filename.c_str());
        }
        LOG("%d bytes read from file <%s>", lengthRead, filename.c_str());

        DOM = new TiXmlDocument();
        const char *xmlData = buffer;
        LOG("Parsing XML data from <%s>", filename.c_str());
        DOM->Parse(xmlData);

        //  error!?!
        if (DOM->Error()) {
            ERR("%s from <%s>", DOM->ErrorDesc(), filename.c_str());
        }

        //	parse the DOM
        TiXmlElement *fontElement = DOM->FirstChildElement("font");
        TiXmlElement *pagesElement = fontElement->FirstChildElement("pages");

        //  get texture dimensions
        int tempW, tempH;
        TiXmlElement *commonElement = fontElement->FirstChildElement("common");
        commonElement->Attribute("lineHeight", &lineHeight);
        commonElement->Attribute("base", &base);
        commonElement->Attribute("scaleW", &tempW);
        commonElement->Attribute("scaleH", &tempH);
        scaleW = tempW;
        scaleH = tempH;

        //  get texture filename
        TiXmlElement *pageData = pagesElement->FirstChildElement("page");
        std::string textureFile(pageData->Attribute("file"));
        textureFile = filename.substr(0, filename.find_last_of("\\/") + 1) + textureFile;
        texture = std::make_shared<Texture>(textureFile);

        //	init chars array
        for (int i = 0; i < 256; i++) {
            chars[i] = 0;
        }

        TiXmlElement *charsElement = fontElement->FirstChildElement("chars");
        TiXmlElement *charElement = charsElement->FirstChildElement("char");
        height = 0;

        while (charElement) {
            int id, x, y, w, h, xo, yo, xa;

            charElement->Attribute("id", &id);
            charElement->Attribute("x", &x);
            charElement->Attribute("y", &y);
            charElement->Attribute("width", &w);
            charElement->Attribute("height", &h);
            charElement->Attribute("xoffset", &xo);
            charElement->Attribute("yoffset", &yo);
            charElement->Attribute("xadvance", &xa);

            if (id < 0 || id > 255) {
                LOG("WARNING: Character index <%d> out of range!", id);
            } else {
                if (chars[id] != 0) {
                    ERR("Duplicate character ID <%d>", id);
                }

                if (h > height) {
                    height = h;
                }

                chars[id] = new Character(texture, x, y, w, h, xo, yo, xa, scaleW, scaleH);
            }
            charElement = charElement->NextSiblingElement("char");
        }

        // read kernings
        TiXmlElement *kerningsElement = fontElement->FirstChildElement("kernings");
        if (kerningsElement) {
            TiXmlElement *kerningElement = kerningsElement->FirstChildElement("kerning");
            while (kerningElement) {
                int first, second, amount;

                kerningElement->Attribute("first", &first);
                kerningElement->Attribute("second", &second);
                kerningElement->Attribute("amount", &amount);

                kernings[first][second] = amount;

                kerningElement = kerningElement->NextSiblingElement("kerning");
            }
        }

        DOM->Clear();
        delete DOM;
        DOM = NULL;
    }

	color = glm::vec4(1.0f);
}

void Font::release() {
	for (int i = 0; i < 256; i++) {
		if (chars[i] != 0) {
			delete chars[i];
			chars[i] = 0;
		}
	}

    texture.reset();
}

void Font::buildDefault() {
#ifdef ANDROID
	// https://docs.oracle.com/javase/7/docs/technotes/guides/jni/spec/types.html
	typedef jchar Uint16;
	typedef jboolean Uint8;
#endif

    Uint16 fontData[] = {63450, 63326, 29254, 55004, 62286, 62280,
        29270, 47066, 59694, 58580, 46938, 37454, 49114, 55002, 63198, 63432,
        63206, 63322, 62366, 59684, 46814, 46804, 47098, 46426, 47012, 58702,
        63198, 22830, 50510, 58782, 46994, 62236, 62430, 58532, 63454, 63378,
        896, 18692, 50436, 4, 40, 20480, 46080, 10530, 35112, 26918, 51500,
        9544, 2080, 14, 22214, 48890, 63988, 42314, 21504, 56158, 43946,
        2088, 10786, 34984, 37138, 18724, 27430, 51628, 15872, 34816, 2976, 7280, 65534};

    Uint8 fontLookup[] = {69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
        69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
        69, 69, 69, 69, 69, 70, 38, 43, 52, 53, 54, 56, 42, 44, 45, 57, 67,
        41, 37, 40, 48, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 49, 58, 59,
        68, 60, 39, 51, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
        16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 46, 61, 47, 55, 50, 69,
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
        20, 21, 22, 23, 24, 25, 26, 63, 62, 64, 65, 69, 69, 69, 69, 69, 69,
        69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
        69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
        69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
        69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
        69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
        69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
        69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
        69, 69, 69, 69};

    LOG("fontData size: %d", sizeof(fontData)/sizeof(*fontData));
    LOG("fontLookup size: %d", sizeof(fontLookup)/sizeof(*fontLookup));

    int width = 32;
    int height = 64;

    unsigned char *image = new unsigned char[width * height * 4];
    memset(image, 0, width * height * 4);

    // decode font data onto texture
    for (int charY = 0; charY < 10; charY++) {
        for (int charX = 0; charX < 8; charX++) {
            int index = charY * 8 + charX;
            if (index < 69) {
                Uint16 bitmask = 32768;
                for (int pixelY = 0; pixelY < 5; pixelY++) {
                    for (int pixelX = 0; pixelX < 3; pixelX++) {
                        Uint16 bit = fontData[index] & bitmask;
                        if (bit != 0) {
                            int u = charX * 4 + pixelX;
                            // int v = (height - 1) - (charY * 6 + pixelY);
                            int v = (charY * 6 + pixelY);
                            int offset = (v * width + u) * 4;
                            image[offset + 0] = 0xFF;
                            image[offset + 1] = 0xFF;
                            image[offset + 2] = 0xFF;
                            image[offset + 3] = 0xFF;
                        }
                        bitmask >>= 1;
                    }
                }
            }
        }
    }
    texture = std::make_shared<Texture>(width, height);
    CALL_GL(glBindTexture(GL_TEXTURE_2D, texture.get()->glHandle));
	CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    CALL_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image));
    delete image;

    //  extract lookup data
    for (int ascii = 0; ascii < 256; ascii++) {
        int index = fontLookup[ascii] - 1;

        int x = (index % 8) * 4;
        int y = (index / 8) * 6;

        chars[ascii] = new Character(texture, x, y, 3, 5, 0, 0, 4, width, height);
    }

    // thanks, Rilo!
    // r45555555ertttttttttttttt

    lineHeight = 7;
    base = 5;
    this->height = 5;
}

void Font::printString(BatchRenderer *renderer, GLfloat x, GLfloat y, GLfloat scale, Align alignment, std::string const& string) {
    // glBindTexture(GL_TEXTURE_2D, texture.get()->glHandle); // not needed?

	float tx;
	switch (alignment) {
		case LEFT: tx = x; break;
		case RIGHT: tx = x - stringLength(string, scale); break;
		case CENTER: tx = x - stringLength(string, scale) / 2; break;
		default: tx = x; break;
	}

	for (unsigned int i = 0; i < string.length(); i++) {
		unsigned char ascii = string[i];
		if (ascii < 128) {
            if (chars[ascii]) {
                float cx = tx + (chars[ascii]->xOffset * scale);
                float cy = (-base * scale) + y - (chars[ascii]->yOffset * scale);
                cx += (chars[ascii]->width / 2.0f) * scale;
                cy += (chars[ascii]->height / 2.0f) * scale;
                
				chars[ascii]->render(renderer, glm::vec3(cx, cy, -1.0f), 0, scale, color);
				
                tx += chars[ascii]->xAdvance * scale;
            } else {
                if (ascii != 13) {
                    ERR("UNKNOWN CHARACTER: %d", ascii);
                }
            }
		} else {
            // LOG_EXP(string);

            //  TODO: test!
            Uint16 extended = (ascii << 8) + string[i + 1];
            // LOG_EXP(extended);
		}

        // kerning
        if (i < string.length() - 1) {
            if (kernings.count(ascii) > 0) {
                unsigned char asciiNext = string[i + 1];
                if (kernings[ascii].count(asciiNext) > 0) {
                    tx += kernings[ascii][asciiNext] * scale;
                }
            }
        }
	}
}

int Font::stringLength(std::string const& string, GLfloat scale) {
	int length = 0;

	for (unsigned int i = 0; i < string.length(); i++) {
		unsigned char ascii = string[i];
        if (chars[ascii]) {
            if (i < string.length() - 1) {
                length += chars[ascii]->xAdvance;

                // kerning
                if (kernings.count(ascii) > 0) {
                    unsigned char asciiNext = string[i + 1];
                    if (kernings[ascii].count(asciiNext) > 0) {
                        length += kernings[ascii][asciiNext];
                    }
                }
            } else {
                length += chars[ascii]->width;
            }
        }
	}
	length = (int)(length * scale);

	return length;
}

void Font::setColor(float r, float g, float b, float a) {
	color = glm::vec4(r, g, b, a);
}

}   //  namespace
