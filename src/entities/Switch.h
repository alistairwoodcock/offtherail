#ifndef SWITCH_H
#define SWITCH_H

#include "Entity.h"
#include <glm/glm.hpp>

class Switch : public Entity {
public:
	bool left;
	int fromTrack;
	int toTrack;
	
	// unsigned int switchVBO;
	// vector<vector<GLfloat>> controlPoints;
	// vector<glm::mat4> trackPieceTransforms;

	// float rotate_speed;
	// float target_y_rot;
	
	// vector<glm::mat4> trackPieceTransforms;
	// vector<vector<GLfloat>> controlPoints;

};


#endif
