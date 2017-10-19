#ifndef TRACK_H
#define TRACK_H

#include "Entity.h"
#include <glm/glm.hpp>

class Track : public Entity {
public:
	int numberOfCurves;
	int numberOfSegments = 100;
	unsigned int trackVBO;
	vector<glm::mat4> trackPieceTransforms;
	vector<vector<GLfloat>> controlPoints;
	Vertex currentEnd;
	Vertex currentEndCP;
};

class TrackSwitch : public Entity {
public:
	int fromTrack;
	int toTrack;
	float rotate_speed;
	float target_y_rot;

};


#endif
