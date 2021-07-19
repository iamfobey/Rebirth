#pragma once

#ifndef MUSIC_H
#define MUSIC_H

#include <irrklang/irrKlang.h>

namespace rb
{
	class Music
	{
		float mVolume = 0.0f;
		float mTempVolume = mVolume;
	public:
		Music() { SoundEngine = irrklang::createIrrKlangDevice(); }
		~Music() { }

		void play(const char* path) {
			mVolume = 0.0f;
			SoundEngine->setSoundVolume(0);
			SoundEngine->play2D(path, true);
			isPlaying = true;
		}

		void stop() {
			isStop = true;
			SoundEngine->setSoundVolume(mVolume);
		}

		void SetVolume(float volume) {
			mVolume = volume;
			SoundEngine->setSoundVolume(volume);
			mTempVolume = volume;
		}

		void update() {
			if (SoundEngine->getSoundVolume() < 1.01f && isPlaying && !isStop)
			{
				SoundEngine->setSoundVolume(mTempVolume);
				mTempVolume += 0.003f;
			}

			if (isStop && isPlaying)
			{
				if (mTempVolume < 0.0f)
				{
					SoundEngine->stopAllSounds();
					isPlaying = false;
					isStop = false;
					return;
				}
				SoundEngine->setSoundVolume(mTempVolume);
				mTempVolume -= 0.003f;
			}
		}
		irrklang::ISoundEngine* SoundEngine;
	private:
		bool isStop = false;
		bool isPlaying = false;

	};
}

#endif