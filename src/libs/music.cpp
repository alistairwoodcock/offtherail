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

	// Buffers to hold sound data.
	ALuint Buffer;
	// Sources are points of emitting sound.
	ALuint Source;

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

	ALboolean LoadALData(const char* file) {
		current = file;
		printf("%s\n", file);

		Buffer = alutCreateBufferFromFile(file);

		printf("yeww\n");

		ALenum error = alutGetError();
		if (error != ALUT_ERROR_NO_ERROR) {
			printf("%s\n", alutGetErrorString(error));
			return AL_FALSE;
		}

		// Bind the buffer with the source.
		alGenSources(1, &Source);

		if(alGetError() != AL_NO_ERROR)
			return AL_FALSE;

		alSourcei (Source, AL_BUFFER,   Buffer   );
		alSourcef (Source, AL_PITCH,    1.0      );
		alSourcef (Source, AL_GAIN,     1.0      );
		alSourcefv(Source, AL_POSITION, SourcePos);
		alSourcefv(Source, AL_VELOCITY, SourceVel);
		alSourcei (Source, AL_LOOPING,  AL_TRUE     );
	
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
		alDeleteSources(1, &Source);
		if (alutExit() == AL_FALSE) {
			printf("Error unloading ALUT: %s\n", alutGetErrorString(alutGetError()));
		}
	}

	void init() {
		alutInit(NULL, 0);
		alGetError();
		SetListenerValues();
	}

	void play(const char* file) {
		if (current != file) {
			if (Source)
				alSourceStop(Source);
			LoadALData(file);
			alSourcePlay(Source);
		}
	}

	void pause(bool paused) {
		if (paused)
			alSourcePause(Source);
		else
			alSourcePlay(Source);
	}

}
