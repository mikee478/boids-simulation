#pragma once

#include <vector>
#include <array>
#include <glm/glm.hpp>
#include "boid.h"
#include "renderer.h"
#include "obstacle.h"

class FlockRenderer;

class Flock
{
public:
    Flock(const glm::vec2 &min_bound, const glm::vec2 &max_bound, glm::vec3 color = glm::vec3(0.0f,1.0f,0.0f));
    void AddBoid(const glm::vec2 &position);
    void Update(const std::vector<std::shared_ptr<Obstacle>> &obstacles, float dt);
    const std::vector<std::shared_ptr<Boid>> GetBoids() const {return boids_;};
    size_t GetSize() const {return boids_.size();}
    float GetCohesionWeight() const {return cohesion_weight_;}
    float GetSeparationWeight() const {return separation_weight_;}
    float GetAlignmentWeight() const {return alignment_weight_;}
    float GetObstacleAvoidanceWeight() const {return obst_avoid_weight_;}
    void SetCohesionWeight(float w) {cohesion_weight_ = w;}
    void SetSeparationWeight(float w) {separation_weight_ = w;}
    void SetAlignmentWeight(float w) {alignment_weight_ = w;}
    float SetObstacleAvoidanceWeight(float w) {obst_avoid_weight_ = w;}
    void Render();

    static const size_t MAX_BOIDS = 500;

private:
    std::vector<std::shared_ptr<Boid>> boids_;
    float cohesion_weight_;
    float separation_weight_;
    float alignment_weight_;
    float obst_avoid_weight_;
    const glm::vec2 MIN_BOUND_;
    const glm::vec2 MAX_BOUND_;
    std::unique_ptr<FlockRenderer> flock_renderer_;
};

class FlockRenderer
{
public:
    FlockRenderer(glm::vec3 color = {0.0f, 1.0f, 0.0f});
    void Render(const std::vector<std::shared_ptr<Boid>> &boids);
private:
    VertexBuffer vb_;
    VertexArray va_;
    Shader shader_;

    struct Vertex
    {
        float position[2];
        float heading;
    };

    std::array<Vertex, Flock::MAX_BOIDS> data_;
};
