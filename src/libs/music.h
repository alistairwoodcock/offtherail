#ifndef MUSIC_H_
#define MUSIC_H_

#ifdef __APPLE__

#include "OpenAL/al.h"
#include "OpenAL/alc.h"
#include "AL/alut.h"

#else 

#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alut.h"

#endif

#include <map>

class Sound {
public:
    ALuint Buffer; // Buffer to hold the sound data.
    ALuint Source; // Source is the point of emission.

    float getDuration() {
    	ALint size, channels, bits, frequency;

    	alGetBufferi(Buffer, AL_SIZE, &size);
    	alGetBufferi(Buffer, AL_CHANNELS, &channels);
    	alGetBufferi(Buffer, AL_BITS, &bits);
    	alGetBufferi(Buffer, AL_FREQUENCY, &frequency);

    	float lengthInSamples = size * 8 / (channels * bits);
    	return lengthInSamples / (float) frequency;
    }
};

#endif