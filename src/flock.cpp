#include "flock.h"

Flock::Flock()
    : vb_(nullptr, sizeof(Vertex) * MAX_FLOCK_SIZE_, GL_DYNAMIC_DRAW)
{
    VertexBufferLayout layout;
    layout.Push<float>(2); // position
    layout.Push<float>(1); // heading
    va_.AddBuffer(vb_, layout);
}

void Flock::AddBoid(const glm::vec2 &position)
{
    if(boids_.size() < MAX_FLOCK_SIZE_)
    {
        boids_.emplace_back(position);
        data_dirty_ = true;
    }
}

void Flock::Update(float dt)
{
    for(int i = 0;i<boids_.size(); i++)
        boids_[i].Update(dt, boids_);
    data_dirty_ = true;
}

void Flock::Render()
{
    va_.Bind();

    if(data_dirty_)
    {
        for(int i = 0;i<boids_.size();i++)
        {
            const glm::vec2 &position = boids_[i].GetPosition();
            data_[i].position[0] = position.x;
            data_[i].position[1] = position.y;
            data_[i].heading = boids_[i].GetHeading();
        }
        
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * boids_.size(), &data_[0]);

        data_dirty_ = false;
    }
    glDrawArrays(GL_POINTS, 0, boids_.size());
}
