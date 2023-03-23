#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "vertex_buffer_layout.h"
#include "shader.h"
#include "vertex_array.h"

class Obstacle
{
public:
    virtual float Distance(glm::vec2 p) const = 0; 
    virtual glm::vec2 DistanceVector(glm::vec2 p) const = 0; 
    float GetRange() const {return range_;}
    virtual void Render() = 0;
    virtual ~Obstacle() {}
protected:
    Obstacle(float range) : range_(range), enabled(true) {}

protected:
    float range_;
public:
    bool enabled;
};
