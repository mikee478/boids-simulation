#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/polar_coordinates.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 forward, glm::vec3 world_up)
    : position_(position), forward_(glm::normalize(forward)), 
    world_up_(glm::normalize(world_up)), view_mat_dirty(true)
{
    glm::vec3 polar = glm::polar(forward_);
    pitch_ = polar[0];
    yaw_ = polar[1] + glm::half_pi<float>();
    UpdateUpRightVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
    if(view_mat_dirty)
    {
        view_mat = glm::lookAt(position_, position_ + forward_, up_);
        view_mat_dirty = false;
    }
    return view_mat;
}

void Camera::MoveForward(float dt)
{
    position_ += forward_ * dt * MOVE_SPEED;
    view_mat_dirty = true;
}

void Camera::MoveBackward(float dt)
{
    position_ -= forward_ * dt * MOVE_SPEED;
    view_mat_dirty = true;
}

void Camera::MoveRight(float dt)
{
    position_ += right_ * dt * MOVE_SPEED;
    view_mat_dirty = true;
}

void Camera::MoveLeft(float dt)
{
    position_ -= right_ * dt * MOVE_SPEED;
    view_mat_dirty = true;
}

void Camera::MoveUp(float dt)
{
    position_ += world_up_ * dt * MOVE_SPEED;
    view_mat_dirty = true;
}

void Camera::MoveDown(float dt)
{
    position_ -= world_up_ * dt * MOVE_SPEED;
    view_mat_dirty = true;
}

void Camera::Rotate(int delta_x, int delta_y)
{
    if(delta_x == 0 && delta_y == 0)
        return;

    yaw_ += delta_x * ROTATE_SPEED;
    pitch_ = glm::clamp(pitch_ + delta_y * ROTATE_SPEED, MIN_PITCH, MAX_PITCH);

    forward_.x = cos(yaw_) * cos(pitch_);
    forward_.y = sin(pitch_);
    forward_.z = sin(yaw_) * cos(pitch_);

    UpdateUpRightVectors();
}

void Camera::UpdateUpRightVectors()
{
    right_ = glm::cross(forward_, world_up_);
    up_ = glm::cross(right_, forward_);
    view_mat_dirty = true;
}