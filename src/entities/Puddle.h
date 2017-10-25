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

#ifndef PUDDLE_H
#define PUDDLE_H

#include "Entity.h"

class Puddle: public Entity {
public:
		float alpha;
		Model* model;
};

#endif
