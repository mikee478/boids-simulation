#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
    Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up);
    glm::vec3 GetPosition() const {return position_;}
    glm::mat4 GetViewMatrix();
    void MoveForward(float dt);
    void MoveBackward(float dt);
    void MoveRight(float dt);
    void MoveLeft(float dt);
    void MoveUp(float dt);
    void MoveDown(float dt);
    void Rotate(int delta_x, int delta_y);

private:
    void UpdateUpRightVectors();

    glm::vec3 position_;
    glm::vec3 forward_;
    glm::vec3 up_;
    glm::vec3 right_;
    const glm::vec3 world_up_;
    float yaw_;
    float pitch_;
    const float MIN_PITCH = glm::radians(-89.9f);
    const float MAX_PITCH = glm::radians(89.9f);
    glm::mat4 view_mat;
    bool view_mat_dirty;
    const float MOVE_SPEED = 15.0f;
    const float ROTATE_SPEED = 0.003f;
};