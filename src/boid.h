#pragma once

#include <glm/glm.hpp>
#include <vector>

class Boid
{
public:
    Boid(glm::vec2 position);
    const glm::vec2& GetPosition() const {return position_;}
    const glm::vec2& GetVelocity() const {return velocity_;}
    float GetHeading() const {return glm::atan(velocity_.y, velocity_.x);}
    void Update(float dt, std::vector<Boid> &boids);
    void Cohesion(const std::vector<Boid*> &neighbors);
    void Separation(const std::vector<Boid*> &neighbors);
    void Alignment(const std::vector<Boid*> &neighbors);
private:
    glm::vec2 position_;
    glm::vec2 velocity_;
    static constexpr float NEIGHBOR_RANGE_ = 50.0f;
    static constexpr float SEPARATION_RANGE_ = 25.0f;
    static constexpr float MAX_SPEED_ = 100.0f;
};