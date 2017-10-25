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

#ifndef SKYDOME_H
#define SKYDOME_H

#include "Entity.h"

class SkyBox : public Entity {
public:
	unsigned int skyboxVAO, skyboxVBO;
	unsigned int cubemapTexture;
	
};

#endif
