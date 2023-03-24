#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "obstacle.h"

class Boid
{
public:
    Boid(glm::vec2 position);
    const glm::vec2& GetPosition() const {return position_;}
    const glm::vec2& GetVelocity() const {return velocity_;}
    float GetHeading() const {return glm::atan(velocity_.y, velocity_.x);}
    void Update(float dt, const std::vector<std::shared_ptr<Boid>> &boids,
        float cohesion_weight, float separation_weight, float alignment_weight,
        float obst_avoid_weight, const std::vector<std::shared_ptr<Obstacle>> &obstacles,
        const glm::vec2 &MIN_BOUND, const glm::vec2 &MAX_BOUND);
    glm::vec2 Cohesion(const std::vector<std::shared_ptr<Boid>> &neighbors) const;
    glm::vec2 Separation(const std::vector<std::shared_ptr<Boid>> &neighbors) const;
    glm::vec2 Alignment(const std::vector<std::shared_ptr<Boid>> &neighbors) const;
private:
    glm::vec2 position_;
    glm::vec2 velocity_;
    static constexpr float NEIGHBOR_RANGE_ = 50.0f;
    static constexpr float SEPARATION_RANGE_ = 25.0f;
    static constexpr float MAX_SPEED_ = 150.0f;
};