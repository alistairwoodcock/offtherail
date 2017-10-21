#ifndef SWITCH_H
#define SWITCH_H

#include "Entity.h"
#include <glm/glm.hpp>

class Switch : public Entity {
public:
	
	unsigned int switchVBO;
	vector<glm::mat4> trackPieceTransforms;
	vector<vector<GLfloat>> controlPoints;

};


#endif
