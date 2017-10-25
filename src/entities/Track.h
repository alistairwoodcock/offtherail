#ifndef TRACK_H
#define TRACK_H

#include "Entity.h"

class Track : public Entity {
public:
	
};

class TrackSwitch : public Entity {
public:
	int fromTrack;
	int toTrack;
	float rotate_speed;
	float target_y_rot;

};

#endif
