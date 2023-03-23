#include "obstacle.h"

class LineObstacleRenderer
{
public:
    LineObstacleRenderer(float range, glm::vec4 color)
        : vb_(nullptr, sizeof(Vertex), GL_DYNAMIC_DRAW),
        shader_("/Users/michael/Documents/projects/boids/res/shaders/obstacle/line/vertex.shader", 
            "/Users/michael/Documents/projects/boids/res/shaders/obstacle/line/geometry.shader", 
            "/Users/michael/Documents/projects/boids/res/shaders/obstacle/line/fragment.shader")
    {
        VertexBufferLayout layout;
        layout.Push<float>(2); // position a
        layout.Push<float>(2); // position b
        va_.AddBuffer(vb_, layout);

        shader_.SetUniform1f("u_range", range);

        shader_.SetUniformMatrix4f(
            "u_proj_mat", 
            glm::ortho(0.0f, static_cast<float>(900), 0.0f, static_cast<float>(600)));
        shader_.SetUniform4f("u_color", color);
    }
    void Render(const glm::vec2 &a, const glm::vec2 &b)
    {
        va_.Bind();
        shader_.Bind();

        data_.position_a[0] = a.x;
        data_.position_a[1] = a.y;
        data_.position_b[0] = b.x;
        data_.position_b[1] = b.y;

        vb_.UpdateBuffer(&data_, sizeof(Vertex));

        glDrawArrays(GL_POINTS, 0, 1);
    }
private:
    VertexBuffer vb_;
    VertexArray va_;
    Shader shader_;

    struct Vertex
    {
        float position_a[2];
        float position_b[2];
    };

    Vertex data_;
};

class LineObstacle : public Obstacle
{
public:
    LineObstacle(float ax, float ay, float bx, float by, float range, glm::vec4 color = {1.0f,0.0f,0.0f,0.25f})
        : Obstacle(range), a_(ax, ay), b_(bx, by),
        renderer_(std::make_unique<LineObstacleRenderer>(range, color))
    {
        a_to_b_ = b_ - a_;
        len_ = glm::length(a_to_b_);
    }
    ~LineObstacle() override {}
    const glm::vec2& GetA() const {return a_;}
    const glm::vec2& GetB() const {return b_;}
    float Distance(glm::vec2 p) const override
    {
        return glm::length(DistanceVector(p));
    }
    float Cross2D(glm::vec2 v1, glm::vec2 v2) const
    {
        return v1.x * v2.y - v2.y - v1.x;
    }
    glm::vec2 DistanceVector(glm::vec2 p) const override
    {
        float dist_along_a_b = glm::dot(a_to_b_, p - a_) / len_;
        if(dist_along_a_b <= 0.0f) 
            return a_ - p;
        else if(dist_along_a_b >= len_) 
            return b_ - p;
        else
            return a_ + a_to_b_ * dist_along_a_b / len_ - p;
    }
    void Render() override
    {
        renderer_->Render(a_, b_);
    }

private:
    glm::vec2 a_;
    glm::vec2 b_;
    glm::vec2 a_to_b_;
    float len_;
    std::unique_ptr<LineObstacleRenderer> renderer_;
};
