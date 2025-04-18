#pragma once

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <common/maths.hpp>

class Camera
{
public:
    // Projection parameters
    float fov = Maths::radians(45.0f);
    float aspect = 1024.0f / 768.0f;
    float near = 0.2f;
    float far = 100.0f;
    float jumpTime = 1.0f;
    float jumpTimer = jumpTime;
    float jumpHeight = 2.5f;
    bool jumping = false;
    bool thirdPerson = true;
    glm::vec3 offset = glm::vec3(0.0f, 0.0f, -1.0f);
    Quaternion orientation = Quaternion(pitch, yaw);


    // Camera vectors
    glm::vec3 eye;
    glm::vec3 target;
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);

    //Camera Angles
    float yaw = 0.0f;
    float pitch = 0.0f;
    float roll = 0.0f;

    // Transformation matrices
    glm::mat4 view;
    glm::mat4 projection;

    // Constructor
    Camera(const glm::vec3 eye, const glm::vec3 target);

    // Methods
    void calculateMatrices(float deltaTime);

    void calculateCameraVectors();

    void jump();

    void quaternionCamera(float deltaTime);
};
