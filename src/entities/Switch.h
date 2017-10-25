/* 
 * Course: COMP3320
 * Project: Off The Rail
 * Members: 
 *	Alistair Woodcock	(c3138738)
 *	Lachlan Meyer		(c3203676)
 *	Joshua Crompton		(c3165877)
 *	Scott Walker		(c3232582)
 *	Timothy Pitts		(c3220826)
 *
 */

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
