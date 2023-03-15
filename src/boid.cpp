#include "boid.h"
#include <glm/gtc/random.hpp>

Boid::Boid(glm::vec2 position) 
    : position_(position)

{
    velocity_ = glm::circularRand(1.0f) * 50.0f;
}

void Boid::Update(float dt, std::vector<Boid> &boids)
{
    std::vector<Boid*> neighbors;
    for(auto &b : boids)
    {
        if(this != &b && glm::distance(b.position_, position_) <= NEIGHBOR_RANGE_)
        {
            neighbors.push_back(&b);
        }
    }

    Cohesion(neighbors);
    Separation(neighbors);
    Alignment(neighbors);

    float speed = glm::length(velocity_);
    if(speed > MAX_SPEED_)
        velocity_ *= MAX_SPEED_ / speed;

    position_ += velocity_ * dt;
    const float r = 2.0f;
    if(position_.x < 0-r) position_.x += 900+2*r;
    if(position_.x > 900+r) position_.x -= 900+2*r;
    if(position_.y < 0-r) position_.y += 600+2*r;
    if(position_.y > 600+r) position_.y -= 600+2*r;
}

void Boid::Cohesion(const std::vector<Boid*> &neighbors)
{
    if(neighbors.empty()) return;

    glm::vec2 center(0.0f,0.0f);
    for(const auto &b : neighbors)
        center += b->GetPosition();
    center /= neighbors.size();

    glm::vec2 desired = glm::normalize(center - position_);

    velocity_ += desired * 2.0f;
}

void Boid::Separation(const std::vector<Boid*> &neighbors)
{
    if(neighbors.empty()) return;

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

    velocity_ += displace * 3.0f;
}

void Boid::Alignment(const std::vector<Boid*> &neighbors)
{
    if(neighbors.empty()) return;

    glm::vec2 avg_velocity(0.0f,0.0f);
    for(const auto &b : neighbors)
        avg_velocity += b->GetVelocity();

    avg_velocity = glm::normalize(avg_velocity);

    velocity_ += avg_velocity * 5.0f;
}