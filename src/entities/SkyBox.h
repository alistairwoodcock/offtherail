#ifndef SKYDOME_H
#define SKYDOME_H

#include "Entity.h"

class SkyBox : public Entity {
public:
	unsigned int skyboxVAO, skyboxVBO;
	unsigned int cubemapTexture;
	
};

#endif
