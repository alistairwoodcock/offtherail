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

#ifndef GAME_PARTICLE
#define GAME_PARTICLE

#include "Entity.h"

class Particle: public Entity {
public:
		float r,g,b;
		float alpha;
		float scale;
		bool render;

};

#endif
