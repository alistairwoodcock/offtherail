#ifndef SKYDOME_H
#define SKYDOME_H

#include "Entity.h"

class SkyDome : public Entity {
public:
	glm::vec3 m_vBaseDayAmbient;
	glm::vec3 m_vBaseNightAmbient;
	glm::vec3 m_vBaseDayColor;
	glm::vec3 m_vBaseNightColor;
};

#endif