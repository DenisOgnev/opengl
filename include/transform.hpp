#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <glm/mat4x4.hpp>

struct Transform
{
    glm::vec3 translate;
    glm::vec3 rotate_axis;
    float angle;
    glm::vec3 scale;
};


#endif