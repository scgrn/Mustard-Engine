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
    if (filename == "default1") {
        LOG("Creating 8x16 built-in font", 0);
        build8x8Default(true);
    } else if (filename == "default2") {
        LOG("Creating 8x8 built-in font", 0);
        build8x8Default(false);
    } else if (filename == "default3") {
        LOG("Creating 3x5 built-in font", 0);
        build3x5Default();
    } else {
        LOG("Loading font <%s>", filename.c_str());

        TiXmlDocument *DOM;
        DataObject dataObject(filename.c_str());
        char *buffer = (char*)dataObject.getData();
        if (buffer == 0) {
            ERR("Couldn't open <%s>", filename.c_str());
        }

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

	color = Vec4(1.0f);
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

void Font::build8x8Default(bool stretch) {
//	TODO: need android defines?
	uint64_t fontData8x8[] = {
		0ULL, 9115736159977570686ULL, 9151287796837384062ULL, 2282854566297398ULL, 2282854562012168ULL, 2019982628213243420ULL,
		2019933149099460616ULL, 26646985310208ULL, 18446717426724241407ULL, 17000933370510336ULL, 18429743140339041279ULL,
		2176139342289887472ULL, 1764874751670904380ULL, 508640727836445948ULL, 245418446131480318ULL, 11050211602421537433ULL,
		289306834831105ULL, 18138078526599232ULL, 1746409397468150808ULL, 28710887429138022ULL, 61037020932987902ULL, 2247610894284407678ULL,
		35604926696521728ULL, 18381508391387151384ULL, 6781891210263576ULL, 6821911708833816ULL, 26596571355136ULL, 13222040439808ULL,
		139650912157696ULL, 40024790148096ULL, 281472799610880ULL, 26648107810560ULL, 0ULL, 3377751463435788ULL, 3552822ULL,
		15259568745428534ULL, 3411991242948108ULL, 27978224826016512ULL, 31018577781601820ULL, 198150ULL, 6768619451452440ULL,
		1702147483241478ULL, 112412166219264ULL, 13246736894976ULL, 435738657110884352ULL, 1056964608ULL, 3390893860061184ULL,
		284799484309600ULL, 8500561678381854ULL, 17746169414029068ULL, 17789024868512542ULL, 8500531025687326ULL, 13564121760283704ULL,
		8500531360105279ULL, 8500543957960220ULL, 1695498875450175ULL, 8500543944340254ULL, 3967245154988830ULL, 3390894062174208ULL,
		435738657312997376ULL, 6768619401120792ULL, 270587068416ULL, 1702147885894662ULL, 3377751665947422ULL, 8448078188864318ULL,
		14411570347187724ULL, 17845512852366911ULL, 17000661724128828ULL, 17792736965310015ULL, 35824382850254463ULL, 4228816714483327ULL,
		35015541269947964ULL, 14411519008912179ULL, 8457495182576670ULL, 8500544246132856ULL, 29104305222346343ULL, 35859772977055247ULL,
		27975301350782819ULL, 27975370337642339ULL, 7941099845006876ULL, 4228748537128511ULL, 15795838306693918ULL, 29051426125014591ULL,
		8500565382738718ULL, 8457495182585151ULL, 17789218528113459ULL, 3410904971686707ULL, 27997411840582499ULL, 27975106747065187ULL,
		8457495487132467ULL, 35856474544092031ULL, 8450872241948190ULL, 18120158187619843ULL, 8470741063833630ULL, 1664490504ULL,
		18374686479671623680ULL, 1575948ULL, 31018589626433536ULL, 17282563567388167ULL, 8500338136842240ULL, 31018542617866296ULL,
		8447819276746752ULL, 4228747742295580ULL, 2247364603684978688ULL, 29104512722929159ULL, 8457495182704652ULL, 1016432632665079832ULL,
		29051425990772231ULL, 8457495182576654ULL, 27975336041316352ULL, 14411518806261760ULL, 8500544295272448ULL, 1082621369951911936ULL,
		8660490473060564992ULL, 4228954556399616ULL, 8778629739577344ULL, 6803829697678344ULL, 31018542433501184ULL, 3410904971673600ULL,
		15339747948036096ULL, 27925517493796864ULL, 2247364603681112064ULL, 17774756937793536ULL, 15775844493167672ULL, 6781890800916504ULL,
		1983571456429063ULL, 15214ULL, 35856600559655944ULL, 2175265138003882782ULL, 35522142057542400ULL, 8447819276746808ULL,
		71044378507658110ULL, 35522189253804083ULL, 35522189253804039ULL, 35522189253807116ULL, 4350545422700118016ULL, 16895638553543550ULL,
		8447819276746803ULL, 8447819276746759ULL, 8457495182704691ULL, 16914990365434686ULL, 8457495182704647ULL, 14424712945732659ULL,
		14424712591445004ULL, 17739649556348984ULL, 71551810921168896ULL, 32425918592136828ULL, 8500543940997918ULL, 8500543940997888ULL,
		8500543940986624ULL, 35522142057542430ULL, 35522142057531136ULL, 2247365703189410560ULL, 7949895935597667ULL, 8500544296648755ULL,
		1736276207759923224ULL, 17846199254398492ULL, 3447021551629107ULL, 8084941215824026383ULL, 1016432634376018032ULL, 35522189253804088ULL,
		8457495182704668ULL, 8500543940999168ULL, 35522142057543680ULL, 14411518468693760ULL, 14420366507311167ULL, 138540549027388ULL,
		138539478443580ULL, 8500337380687884ULL, 3312476749824ULL, 52983773528064ULL, 17877447492146770787ULL, 13905413497125811043ULL,
		1746336829692450816ULL, 224739321105408ULL, 56516608930560ULL, 1244138381106221380ULL, 6172840429334713770ULL, 7943063569329516091ULL,
		1736164148113840152ULL, 1736164178178611224ULL, 1736164178179069976ULL, 7812738679397182572ULL, 7812738746297614336ULL,
		1736164178179063808ULL, 7812738679196052588ULL, 7812738666512280684ULL, 7812738679197073408ULL, 547078761580ULL, 547279891564ULL,
		133548677144ULL, 1736164177774379008ULL, 1065556121624ULL, 1095620892696ULL, 1736165139847053312ULL, 1736165110186514456ULL,
		1095216660480ULL, 1736165140251285528ULL, 1736165110201194520ULL, 7812739216268094572ULL, 1082548579436ULL, 7812739214666891264ULL,
		1095232351340ULL, 7812739227350663168ULL, 7812739214665870444ULL, 1095233372160ULL, 7812739227349642348ULL, 1095233378328ULL,
		1097035705452ULL, 1736165139863764992ULL, 7812739296053428224ULL, 1084150803564ULL, 1065570801688ULL, 1736165110201188352ULL,
		7812739283168526336ULL, 7812739229152996460ULL, 1736165139863771160ULL, 133548218392ULL, 1736165109782282240ULL, 18446744073709551615ULL,
		18446744069414584320ULL, 1085102592571150095ULL, 17361641481138401520ULL, 4294967295ULL, 31027201225654272ULL, 217051511471152640ULL,
		847736406769408ULL, 15259255208050432ULL, 35859524171359871ULL, 8500544301563904ULL, 217930241499620864ULL, 6781891205885440ULL,
		4543039329359760447ULL, 7941100314768924ULL, 33555129452148252ULL, 8500544479431736ULL, 139482745405440ULL, 218001114719465568ULL,
		16895109614470716ULL, 14411518807585566ULL, 69270289530624ULL, 17732975277837324ULL, 17732949505477638ULL, 17733026813709336ULL,
		1736164148126472304ULL, 1016435931199576088ULL, 3390894917028876ULL, 63007173987840ULL, 473314844ULL, 103481868288ULL, 103079215104ULL,
		4052173375386169584ULL, 232837756446ULL, 266390482974ULL, 66229406269440ULL, 18446744073709551615ULL
	};


    int width = 128;
    int height = stretch ? 256 : 128;
	int charHeight = stretch ? 16 : 8;
	
	LOG_EXP(width);
	LOG_EXP(height);

    unsigned char *image = new unsigned char[width * height * 4];
    memset(image, 0, width * height * 4);

    // decode font data onto texture
    for (int chr = 0; chr < 256; chr++) {
		int cx = chr % 16;
		int cy = chr / 16;
		uint64_t bitmask = 1ULL;
		for (int py = 0; py < 8; py++) {
			for (int px = 0; px < 8; px++) {
				uint64_t bit = fontData8x8[chr] & bitmask;
				if (bit != 0) {
					int u = cx * 8 + px;
					int v = cy * 8 + py;
					
					if (stretch) {
						v *= 2;
					}
					
					int offset = (v * width + u) * 4;
					
					image[offset + 0] = 0xFF;
					image[offset + 1] = 0xFF;
					image[offset + 2] = 0xFF;
					image[offset + 3] = 0xFF;
					
					if (stretch) {
						offset += (width * 4);
						image[offset + 0] = 0xFF;
						image[offset + 1] = 0xFF;
						image[offset + 2] = 0xFF;
						image[offset + 3] = 0xFF;
					}
				}
				bitmask <<= 1;
			}
		}
    }

/*
	//	test images
	if (stretch) {
		saveTGA(image, width, height, "defaultFont1.tga");
	} else {
		saveTGA(image, width, height, "defaultFont2.tga");
	}
*/

    texture = std::make_shared<Texture>(width, height);
    CALL_GL(glBindTexture(GL_TEXTURE_2D, texture.get()->glHandle));
	CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    CALL_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image));
    delete image;

	//  generate lookup data
    for (int chr = 0; chr < 256; chr++) {
		int cx = chr % 16;
		int cy = chr / 16;

		chars[chr] = new Character(texture, cx * 8, cy * charHeight, 8, charHeight, 0, 0, 8, width, height);
	}
	
    lineHeight = stretch ? 20 : 10;
    base = charHeight;
    this->height = charHeight;
}

void Font::build3x5Default() {
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
	
	//	test image
	// saveTGA(image, width, height, "defaultFont2.tga");

    texture = std::make_shared<Texture>(width, height);
    CALL_GL(glBindTexture(GL_TEXTURE_2D, texture.get()->glHandle));
	CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
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
                
				chars[ascii]->render(renderer, Vec3(cx, cy, -1.0f), 0, scale, color);
				
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
	color = Vec4(r, g, b, a);
}

}   //  namespace
