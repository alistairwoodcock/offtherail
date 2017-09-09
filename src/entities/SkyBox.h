#ifndef SKYDOME_H
#define SKYDOME_H

#include "Entity.h"

class SkyBox : public Entity {
public:
	Shader skyboxShader;
	unsigned int skyboxVAO, skyboxVBO;
	unsigned int cubemapTexture;
	
};

#endif