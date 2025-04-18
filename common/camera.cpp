#include <common/camera.hpp>

Camera::Camera(const glm::vec3 Eye, const glm::vec3 Target)
{
    eye = Eye;
    target = Target;
}

void Camera::calculateMatrices(float deltaTime)
{
    calculateCameraVectors();
    if (!jumping) {
        eye.y = 0;
    }
    else {
        eye.y = jumpHeight * sin(3.14159f * jumpTimer);
        jumpTimer -= deltaTime;
        if (jumpTimer < 0) {
            jumping = false;
            jumpTimer = jumpTime;
        }
    }
    view = glm::lookAt(eye, eye + front, worldUp);
    projection = glm::perspective(fov, aspect, near, far);
}

void Camera::calculateCameraVectors()
{
    front = glm::vec3(cos(yaw) * cos(pitch), sin(pitch), sin(yaw) * cos(pitch));
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::cross(right, front);
}

void Camera::jump() {
    jumping = true;
}

void Camera::quaternionCamera(float deltaTime)
{
    //Jumping and Keep Player at Floor Level
    if (!jumping) {
        eye.y = 0;
    }
    else {
        eye.y = jumpHeight * sin(3.14159f * jumpTimer);
        jumpTimer -= deltaTime;
        if (jumpTimer < 0) {
            jumping = false;
            jumpTimer = jumpTime;
        }
    }

    // Calculate camera orientation quaternion from the Euler angles
    Quaternion newOrientation(-pitch, yaw);

    // Apply SLERP
    orientation = Maths::SLERP(orientation, newOrientation, 0.2f);

    // Calculate the view matrix
    view = orientation.matrix() * Maths::translate(-eye);

    if (thirdPerson) {
        view = Maths::translate(offset) * view;
    }

    // Calculate the projection matrix
    projection = glm::perspective(fov, aspect, near, far);

    // Calculate camera vectors from view matrix
    right = glm::vec3(view[0][0], view[1][0], view[2][0]);
    up = glm::vec3(view[0][1], view[1][1], view[2][1]);
    front = -glm::vec3(view[0][2], view[1][2], view[2][2]);
}