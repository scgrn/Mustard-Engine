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

#ifndef AUDIO_H
#define AUDIO_H

#include "../core/subsystem.h"

namespace AB {

class Audio : public SubSystem {
	public:
		bool startup();
		void shutdown();
		
		void loadSound(int index, std::string filename);
		void playSound(int index, bool loop = false, float volume = 1.0f);
		void stopSound(int index);
		bool isPlaying(int index);

		void loadMusic(int index, std::string filename, float loopPoint = 0.0f);
		void playMusic(int index);
		void pauseMusic();
		void resumeMusic();
		void fadeMusicIn(int index, float duration);
		void fadeMusicOut(float duration);
		void stopMusic();

		void setSoundVolume(float volume);
		void setMusicVolume(float volume);

	private:
		float soundVolume = 1.0f;
		float musicVolume = 1.0f;

};

}

#endif
