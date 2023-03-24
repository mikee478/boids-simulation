#include "obstacle.h"

class PointObstacleRenderer
{
public:
    PointObstacleRenderer(float range, glm::vec4 color)
        : vb_(nullptr, sizeof(Vertex), GL_DYNAMIC_DRAW),
        shader_("/Users/michael/Documents/projects/boids/res/shaders/obstacle/point/vertex.shader", 
            "/Users/michael/Documents/projects/boids/res/shaders/obstacle/point/geometry.shader", 
            "/Users/michael/Documents/projects/boids/res/shaders/obstacle/point/fragment.shader")
    {
        VertexBufferLayout layout;
        layout.Push<float>(2); // position
        va_.AddBuffer(vb_, layout);

        shader_.SetUniform1f("u_range", range);
        shader_.SetUniformMatrix4f("u_proj_mat", Shader::projection_mat);
        shader_.SetUniform4f("u_color", color);
    }
    void Render(const glm::vec2 &center)
    {
        va_.Bind();
        shader_.Bind();

        data_.position[0] = center.x;
        data_.position[1] = center.y;

        vb_.UpdateBuffer(&data_, sizeof(Vertex));

        glDrawArrays(GL_POINTS, 0, 1);
    }
private:
    VertexBuffer vb_;
    VertexArray va_;
    Shader shader_;

    struct Vertex
    {
        float position[2];
    };

    Vertex data_;
};

class PointObstacle : public Obstacle
{
public:
    PointObstacle(float x, float y, float range, glm::vec4 color = {1.0f,0.0f,0.0f,0.25f})
        : Obstacle(range), center_(x,y), 
        renderer_(std::make_unique<PointObstacleRenderer>(range, color)) {}
    PointObstacle(glm::vec2 center, float range) : Obstacle(range), center_(center) {}
    ~PointObstacle() override {}
    const glm::vec2& GetCenter() const {return center_;}
    void SetCenter(float x, float y) {center_.x = x; center_.y = y;}
    void SetCenter(const glm::vec2 &center) {center_ = center;}
    float Distance(glm::vec2 p) const override
    {
        return glm::length(center_ - p);
    }
    glm::vec2 DistanceVector(glm::vec2 p) const override
    {
        return center_ - p;
    }
    void Render() override
    {
        if(enabled)
            renderer_->Render(center_);
    }

private:
    glm::vec2 center_;
    std::unique_ptr<PointObstacleRenderer> renderer_;
};
