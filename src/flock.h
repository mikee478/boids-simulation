#pragma once

#include <vector>
#include <array>
#include "boid.h"
#include "renderer.h"

class Flock
{
public:
    Flock();
    void AddBoid(const glm::vec2 &position);
    void Update(float dt);
    void Render();
private:
    std::vector<Boid> boids_;
    static const size_t MAX_FLOCK_SIZE_ = 500;
    VertexBuffer vb_;
    VertexArray va_;

    struct Vertex
    {
        float position[2];
        float heading;
    };

    std::array<Vertex, MAX_FLOCK_SIZE_> data_;
    bool data_dirty_ = true;
    float BORDER_BUFFER = 5.0f;
};