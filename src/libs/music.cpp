/* 
 * Course: COMP3320
 * Project: Off The Rail
 * Members: 
 *	Alistair Woodcock	(c3138738)
 *	Lachlan Meyer		(c3203676)
 *	Joshua Crompton		(c3165877)
 *	Scott Walker		(c3232582)
 *	Timothy Pitts		(c3220826)
 *
 */

#include "music.h"

namespace Music {

	/*
	 * These are 3D cartesian vector coordinates. A structure or class would be
	 * a more flexible of handling these, but for the sake of simplicity we will
	 * just leave it as is.
	 */
	ALfloat SourcePos[] = { 0.0, 0.0, 0.0 };
	ALfloat SourceVel[] = { 10.0, 0.0, 0.0 };
	ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };
	ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };
	ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };

	// -- Public --
	// Role: Load a given sound file into the provided Sound struct
	//
	ALboolean LoadMusic(Sound &sound, const char* file) {
		game->current = file;
		auto search = game->sounds->find(file);
		if (search != game->sounds->end()) {
			sound = search->second;
			return AL_TRUE;
		}

		// Instantiate current sound
		Sound s = {0};
		printf("Loading %s\n", file);
		s.Buffer = alutCreateBufferFromFile(file);

		// Check for alut error
		ALenum error = alutGetError();
		if (error != ALUT_ERROR_NO_ERROR) {
			printf("%s\n", alutGetErrorString(error));
			return AL_FALSE;
		}
		printf("Successfully loaded %s into the buffer\n", file);

		// Bind the buffer with the source.
		alGenSources(1, &s.Source);

		if(alGetError() != AL_NO_ERROR)
			return AL_FALSE;

		// Assign source properties
		alSourcei (s.Source, AL_BUFFER,   s.Buffer);
		alSourcef (s.Source, AL_PITCH,	  1.0);
		alSourcef (s.Source, AL_GAIN,	  1.0);
		alSourcefv(s.Source, AL_POSITION, SourcePos);
		alSourcefv(s.Source, AL_VELOCITY, SourceVel);
		alSourcei (s.Source, AL_LOOPING,  AL_TRUE); // Background music should loop
		
		// Do another error check and return.
		if(alGetError() != AL_NO_ERROR)
			return AL_FALSE;
		
		// Insert the sound into out list, and set update the provided reference.
		game->sounds->insert(game->sounds->begin(), std::pair<const char*,Sound>(file,s));
		sound = s;
		return AL_TRUE;
	}

	// -- Public --
	// Role: Calculate the duration of a given Sound struct
	//
	float getDuration(Sound &sound) {
		ALint size, channels, bits, frequency;

		alGetBufferi(sound.Buffer, AL_SIZE, &size);
		alGetBufferi(sound.Buffer, AL_CHANNELS, &channels);
		alGetBufferi(sound.Buffer, AL_BITS, &bits);
		alGetBufferi(sound.Buffer, AL_FREQUENCY, &frequency);

		float lengthInSamples = size * 8 / (channels * bits);
		return lengthInSamples / (float) frequency;
	}

	// -- Public --
	// Role: Assign listenever values 
	//
	void SetListenerValues() {
		alListenerfv(AL_POSITION,	 ListenerPos);
		alListenerfv(AL_VELOCITY,	 ListenerVel);
		alListenerfv(AL_ORIENTATION, ListenerOri);
	}

	// -- Public --
	// Role: Close our OpenAL instance
	//
	void destroy() {
		//alDeleteBuffers(1, &Buffer);
		alDeleteSources(1, &game->music.Source);
		if (alutExit() == AL_FALSE) {
			printf("Error unloading ALUT: %s\n", alutGetErrorString(alutGetError()));
		}
	}

	// -- Public --
	// Role: Instantiate OpenAL within our project
	//
	void init() {
		alutInit(NULL, 0);
		#ifdef __APPLE__
		// Mac needs both inits
		alutInitWithoutContext(NULL, 0);
		#endif
		alGetError();
		SetListenerValues();

		// Set gamestate properties
		game->current = "";
		game->sounds = new std::map<const char*, Sound>();
	}

	// -- Public --
	// Role: Change the background music to be the provided file 
	//
	void play(const char* file) {
		// If it's already playing, don't do anything so we don't restart the song
		if (game->current != file) {
			// Stop an existing song if required
			if (game->music.Source)
				alSourceStop(game->music.Source);
			LoadMusic(game->music, file);
			alSourcePlay(game->music.Source);
		}
	}

	// -- Public --
	// Role: Pause or unpause the currently playing song 
	//
	void pause(bool paused) {
		paused ? alSourcePause(game->music.Source) : alSourcePlay(game->music.Source);
	}

	// -- Public --
	// Role: Play a sound effect over the top of the current song. Sleep freezes executing until complete (For closing) 
	//
	void soundEffect(const char* file, bool sleep=false) {
		Sound effect;
		LoadMusic(effect, file);
		alSourcei(effect.Source, AL_LOOPING, AL_FALSE);
		alSourcePlay(effect.Source);

		// Pause program until sound is complete.
		if (sleep)
			alutSleep(getDuration(effect));
	}
}

