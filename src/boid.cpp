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
    float obst_avoid_weight, const std::vector<std::shared_ptr<Obstacle>> &obstacles,
    const glm::vec2 &MIN_BOUND, const glm::vec2 &MAX_BOUND)
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
        if(obst->enabled)
        {
            float dist = obst->Distance(position_);
            if(0.001f <= dist && dist <= obst->GetRange())
            {
                glm::vec2 dist_vec = obst->DistanceVector(position_) / dist;
                float proportional_dist = dist / obst->GetRange();
                velocity_ += obst_avoid_weight * -dist_vec / proportional_dist;
            }
        }
    }

    float speed = glm::length(velocity_);
    if(speed > MAX_SPEED_) velocity_ *= MAX_SPEED_ / speed;

    position_ += velocity_ * dt;

    glm::vec2 shape = MAX_BOUND - MIN_BOUND;
    const float r = 3.0f;
    if(position_.x < MIN_BOUND.x-r) position_.x += shape.x+2*r;
    if(position_.x > MAX_BOUND.x+r) position_.x -= shape.x+2*r;
    if(position_.y < MIN_BOUND.y-r) position_.y += shape.y+2*r;
    if(position_.y > MAX_BOUND.y+r) position_.y -= shape.y+2*r;
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
