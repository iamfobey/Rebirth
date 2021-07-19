#pragma once

#ifndef SOUND_H
#define SOUND_H

#include <irrklang/irrKlang.h>

namespace rb
{
	class Sound
	{
		float mVolume = 1.0f;
	public:
		Sound() { SoundEngine = irrklang::createIrrKlangDevice(); }
		~Sound() {}

		void play(const char* path) {
			SoundEngine->setSoundVolume(mVolume);
			SoundEngine->play2D(path);
		}

		void stop() {
			SoundEngine->stopAllSounds();
		}

		void SetVolume(float volume) {
			mVolume = volume;
			SoundEngine->setSoundVolume(volume);
		}
		irrklang::ISoundEngine* SoundEngine;
	private:
	};
}

#endif