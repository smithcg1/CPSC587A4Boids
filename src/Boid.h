#ifndef BOID_H
#define BOID_H

#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

class Boid
{
public:
    Boid();

    vec3 position = vec3(0.0f, 0.0f, 0.0f);
    vec3 velocity = vec3(0.0f, 0.0f, 0.0f);
    vec3 totalForce = vec3(0.0f, 0.0f, 0.0f);

    int gridIndex;
    int id;
};

#endif // BOID_H
