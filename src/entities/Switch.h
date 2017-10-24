#ifndef SWITCH_H
#define SWITCH_H

#include "Entity.h"
#include <glm/glm.hpp>

class Switch : public Entity {
public:
	int fromTrack;
	int toTrack;
	vector<vector<GLfloat>> controlPoints;

	// float rotate_speed;
	// float target_y_rot;
	
	// unsigned int switchVBO;
	// vector<glm::mat4> trackPieceTransforms;
	// vector<vector<GLfloat>> controlPoints;

};


#endif
