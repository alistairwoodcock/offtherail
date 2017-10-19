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
};

#endif