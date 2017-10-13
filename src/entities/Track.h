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

#endif
