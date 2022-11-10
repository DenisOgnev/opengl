#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <glm/mat4x4.hpp>

struct Transform
{
    glm::mat4 model{};
    glm::mat4 view{};
    glm::mat4 projection{};
};


#endif