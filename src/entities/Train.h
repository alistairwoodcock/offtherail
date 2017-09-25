#ifndef TRAIN_H
#define TRAIN_H

#include <glm/glm.hpp>
#include "Entity.h"

class Train: public Entity {
public:
    glm::vec3 size;
    glm::vec3 scale;
};

#endif
