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

#include "console.h"

#include <list>
#include <time.h>

#include "../core/version.h"
#include "../script/script.h"
#include "../core/log.h"

namespace AB {

extern std::vector<SDL_Event> eventQueue;

extern Script script;

struct ConsoleMessage {
    std::string message;
    bool entered;
};
static std::list<ConsoleMessage> messages;

void addMessage(std::string message, bool entered) {
    ConsoleMessage m;
    m.message = message;
    m.entered = entered;
    messages.push_front(m);
}

//  modified from lbaselib.c
int luaPrint(lua_State *L) {
    int numArgs = lua_gettop(L);
    lua_getglobal(L, "tostring");

    std::string message;
    for (int i = 1; i <= numArgs; i++) {
        const char *s;
        lua_pushvalue(L, -1);  /* function to be called */
        lua_pushvalue(L, i);   /* value to print */
        lua_call(L, 1, 1);

        s = lua_tostring(L, -1);  /* get result */
        if (s == NULL) {
            return luaL_error(L, LUA_QL("tostring") " must return a string to " LUA_QL("print"));
        }
        if (i > 1) message += "   ";
        message += s;
        lua_pop(L, 1);  /* pop result */
    }
    addMessage(message, false);

    return 0;
}

Console::Console() {
    active = false;
    addMessage("", false);
    addMessage("AB Engine Lua Console", true);
#ifdef DEBUG
    addMessage("Debug build", false);
#endif
    addMessage((std::string("Buildstamp ") + BUILD_STAMP).c_str(), false);
	addMessage("", false);

    commandLineHistory.clear();
    commandLineHistoryPos = commandLineHistory.end();
}

Console::~Console() {
}

void Console::update() {
	lua_State* luaVM = script.getVM();
	
    for (std::vector<SDL_Event>::iterator event = eventQueue.begin(); event != eventQueue.end(); event++) {
        if (event->type == SDL_KEYDOWN) {
            // tilde key
            if (event->key.keysym.sym == SDLK_BACKQUOTE) {
                active = !active;
                if (active) {
                    // pause();
                    SDL_StartTextInput();
                } else {
                    // resume();
                    SDL_StopTextInput();
                }
            }

            if (active) {
                if (event->key.keysym.sym == SDLK_BACKSPACE && commandLine.length() > 0) {
                    commandLine.erase(commandLine.length() - 1);
                }

                if (event->key.keysym.sym == SDLK_ESCAPE) {
                    commandLine.clear();
                }

                if (event->key.keysym.sym == SDLK_UP) {
                    if (commandLineHistoryPos != commandLineHistory.begin()) {
                        commandLineHistoryPos--;
                        commandLine = *commandLineHistoryPos;
                    }
                }

                if (event->key.keysym.sym == SDLK_DOWN) {
                    if (commandLineHistoryPos != commandLineHistory.end()) {
                        commandLineHistoryPos++;
                        if (commandLineHistoryPos == commandLineHistory.end()) {
                            commandLine.clear();
                        } else {
                            commandLine = *commandLineHistoryPos;
                        }
                    }
                }

                if (event->key.keysym.sym == SDLK_RETURN) {
                    addMessage(commandLine, true);
                    commandLineHistory.push_back(commandLine);
                    commandLineHistoryPos = commandLineHistory.end();

                    int error = luaL_loadbuffer(luaVM, commandLine.c_str(), commandLine.length(), "line") || lua_pcall(luaVM, 0, 0, 0);
                    if (error) {
                        addMessage(lua_tostring(luaVM, -1), false);
                        lua_pop(luaVM, 1);
                    }
                    commandLine.clear();
                }
            }
        } else if (event->type == SDL_TEXTINPUT) {
            if (active) {
                if (strcmp(event->text.text, "`") != 0) {
                    commandLine += event->text.text;
                }
            }
        }
    }
}

void Console::render() {
    if (!active) return;

	ERR("Console rendering unimplemented! ", 0);
	/*
    glViewport(0, 0, graphics->xRes, graphics->yRes);
    glm::mat4 projection = glm::ortho(0.0f, (GLfloat)graphics->xRes, (GLfloat)graphics->yRes, 0.0f, 0.1f, 100.0f);
    GLuint projLoc = glGetUniformLocation(graphics->shaders.get(1)->getID(), "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    graphics->setColor(0.0f, 0.0f, 0.0f, 0.75f);
    graphics->renderQuad(20, 20, 780, 420);

    graphics->setColor(1.0f, 1.0f, 1.0f, 1.0f);
    graphics->renderQuad(30, 395, 770, 396);

    graphics->fonts.get(1)->setColor(1.0f, 1.0f, 1.0f, 1.0f);
    graphics->fonts.get(1)->printString(30, 413, 2.0f, Font::LEFT, "> " + commandLine + (time(NULL) % 2 == 0 ? "_" : ""));

    float y = 383;
    for (std::list<ConsoleMessage>::iterator message = messages.begin(); message != messages.end(); message++) {
        if (message->entered) {
            graphics->fonts.get(1)->setColor(1.0f, 1.0f, 1.0f, 1.0f);
        } else {
            graphics->fonts.get(1)->setColor(0.5f, 0.5f, 0.5f, 1.0f);
        }
        graphics->fonts.get(1)->printString(30, y, 2.0f, Font::LEFT, message->message);
        y -= 15;
        if (y < 25) {
            break;
        }
    }

    glViewport(0,0,graphics->canvasWidth,graphics->canvasHeight);
    projection = glm::ortho(0.0f, (GLfloat)graphics->canvasWidth, (GLfloat)graphics->canvasHeight, 0.0f, 0.1f, 100.0f);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	*/
}

}   //  namespace
