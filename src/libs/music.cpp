#ifdef __APPLE__

#include "OpenAL/al.h"
#include "OpenAL/alc.h"
#include "AL/alut.h"

#else 

#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alut.h"

#endif

namespace Music {

    struct Sound {
        ALuint Buffer; // Buffer to hold the sound data.
        ALuint Source; // Source is the point of emission.
    };

    Sound music;
	const char* current;

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

	ALboolean LoadMusic(const char* file) {
		
        printf("Loading %s\n", file);
		current = file;
		music.Buffer = alutCreateBufferFromFile(file);

        // Check for alut error
		ALenum error = alutGetError();
		if (error != ALUT_ERROR_NO_ERROR) {
			printf("%s\n", alutGetErrorString(error));
			return AL_FALSE;
		}
        printf("Successfully loaded %s into the buffer\n", file);

		// Bind the buffer with the source.
		alGenSources(1, &music.Source);

		if(alGetError() != AL_NO_ERROR)
			return AL_FALSE;

		alSourcei (music.Source, AL_BUFFER,   music.Buffer);
		alSourcef (music.Source, AL_PITCH,    1.0      );
		alSourcef (music.Source, AL_GAIN,     1.0      );
		alSourcefv(music.Source, AL_POSITION, SourcePos);
		alSourcefv(music.Source, AL_VELOCITY, SourceVel);
		alSourcei (music.Source, AL_LOOPING,  AL_TRUE  );
	
		// Do another error check and return.
		if(alGetError() == AL_NO_ERROR)
			return AL_TRUE;
		return AL_FALSE;
	}

	void SetListenerValues() {
		alListenerfv(AL_POSITION,    ListenerPos);
		alListenerfv(AL_VELOCITY,    ListenerVel);
		alListenerfv(AL_ORIENTATION, ListenerOri);
	}

	void destroy() {
		//alDeleteBuffers(1, &Buffer);
		alDeleteSources(1, &music.Source);
		if (alutExit() == AL_FALSE) {
			printf("Error unloading ALUT: %s\n", alutGetErrorString(alutGetError()));
		}
	}

	void init() {
		alutInit(NULL, 0);
		alGetError();
		SetListenerValues();
        music = {0};
	}

	void play(const char* file) {
		if (current != file) {
			if (music.Source)
				alSourceStop(music.Source);
			LoadMusic(file);
			alSourcePlay(music.Source);
		}
	}

	void pause(bool paused) {
		paused ? alSourcePause(music.Source) : alSourcePlay(music.Source);
	}

}
