#include "flock.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>

Flock::Flock(const glm::vec2 &min_bound, const glm::vec2 &max_bound, glm::vec3 color)
    : cohesion_weight_(2.0f), separation_weight_(3.0f), alignment_weight_(2.0f),
    obst_avoid_weight_(15.0f),
    MIN_BOUND_(min_bound), MAX_BOUND_(max_bound),
    flock_renderer_(std::make_unique<FlockRenderer>(color))
{
}

void Flock::AddBoid(const glm::vec2 &position)
{
    if(boids_.size() < Flock::MAX_BOIDS)
        boids_.emplace_back(std::make_shared<Boid>(position));
}

void Flock::DeleteBoids(const glm::vec2 &position, float radius)
{
    boids_.erase(std::remove_if(boids_.begin(), boids_.end(),
        [&position, &radius](std::shared_ptr<Boid> b) { return glm::distance2(b->GetPosition(), position) <= radius*radius;}),
        boids_.end());
}

void Flock::Update(const std::vector<std::shared_ptr<Obstacle>> &obstacles, float dt)
{
    for(int i = 0;i<boids_.size(); i++)
        boids_[i]->Update(
            dt, boids_,
            cohesion_weight_, separation_weight_, alignment_weight_, obst_avoid_weight_,
            obstacles, MIN_BOUND_, MAX_BOUND_);
}

void Flock::Render()
{
    flock_renderer_->Render(boids_);
}

FlockRenderer::FlockRenderer(glm::vec3 color)
    : vb_(nullptr, sizeof(Vertex) * Flock::MAX_BOIDS, GL_DYNAMIC_DRAW),
    shader_("/Users/michael/Documents/projects/boids/res/shaders/flock/vertex.shader", 
        "/Users/michael/Documents/projects/boids/res/shaders/flock/geometry.shader", 
        "/Users/michael/Documents/projects/boids/res/shaders/flock/fragment.shader")
{
    VertexBufferLayout layout;
    layout.Push<float>(2); // position
    layout.Push<float>(1); // heading
    va_.AddBuffer(vb_, layout);

    shader_.SetUniformMatrix4f("u_proj_mat", Shader::projection_mat);
    shader_.SetUniform3f("u_color", color);
}

void FlockRenderer::Render(const std::vector<std::shared_ptr<Boid>> &boids)
{
    va_.Bind();
    shader_.Bind();

    for(int i = 0;i<boids.size();i++)
    {
        const glm::vec2 &position = boids[i]->GetPosition();
        data_[i].position[0] = position.x;
        data_[i].position[1] = position.y;
        data_[i].heading = boids[i]->GetHeading();
    }
    
    vb_.UpdateBuffer(&data_[0], sizeof(Vertex) * boids.size());

    glDrawArrays(GL_POINTS, 0, boids.size());
}
