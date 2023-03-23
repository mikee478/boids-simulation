#include "boid.h"
#include <glm/gtx/norm.hpp>
#include <glm/gtc/random.hpp>


#include <iostream>
Boid::Boid(glm::vec2 position) : position_(position)
{
    velocity_ = glm::diskRand(MAX_SPEED_);
}

void Boid::Update(float dt, const std::vector<std::shared_ptr<Boid>> &boids, 
    float cohesion_weight, float separation_weight, float alignment_weight,
    const std::vector<std::shared_ptr<Obstacle>> &obstacles)
{
    std::vector<std::shared_ptr<Boid>> neighbors;
    for(auto &b : boids)
    {
        if(this != b.get() && glm::distance(b->position_, position_) <= NEIGHBOR_RANGE_)
        {
            neighbors.push_back(b);
        }
    }

    auto coh_dir = Cohesion(neighbors);
    auto sep_dir = Separation(neighbors);
    auto align_dir = Alignment(neighbors);

    velocity_ += coh_dir * cohesion_weight + sep_dir * separation_weight + align_dir * alignment_weight; 

    for(const auto& obst : obstacles)
    {
        float dist = obst->Distance(position_);
        if(0.001f <= dist && dist <= obst->GetRange())
            velocity_ += 20.0f * -obst->DistanceVector(position_) / dist;
    }

    float speed = glm::length(velocity_);
    if(speed > MAX_SPEED_) velocity_ *= MAX_SPEED_ / speed;

    position_ += velocity_ * dt;

    const float r = 2.0f;
    if(position_.x < 0-r) position_.x += 900+2*r;
    if(position_.x > 900+r) position_.x -= 900+2*r;
    if(position_.y < 0-r) position_.y += 600+2*r;
    if(position_.y > 600+r) position_.y -= 600+2*r;
}

glm::vec2 Boid::Cohesion(const std::vector<std::shared_ptr<Boid>> &neighbors) const
{
    if(neighbors.empty()) return {0.0f,0.0f};

    glm::vec2 center(0.0f,0.0f);
    for(const auto &b : neighbors)
        center += b->GetPosition();
    center /= neighbors.size();

    return glm::normalize(center - position_);
}

glm::vec2 Boid::Separation(const std::vector<std::shared_ptr<Boid>> &neighbors) const
{
    if(neighbors.empty()) return {0.0f,0.0f};

    bool found = false;
    glm::vec2 displace(0.0f,0.0f);
    for(const auto &b : neighbors)
    {
        float dist;
        if((dist = glm::distance(position_, b->GetPosition())) <= SEPARATION_RANGE_)
        {
            displace += (position_ - b->GetPosition()) / (dist*dist);
            found = true;
        }
    }
    if(found) displace = glm::normalize(displace);

    return displace;
}

glm::vec2 Boid::Alignment(const std::vector<std::shared_ptr<Boid>> &neighbors) const
{
    if(neighbors.empty()) return {0.0f,0.0f};

    glm::vec2 avg_velocity(0.0f,0.0f);
    for(const auto &b : neighbors)
        avg_velocity += b->GetVelocity();

    return glm::normalize(avg_velocity);
}
