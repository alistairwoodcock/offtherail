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
