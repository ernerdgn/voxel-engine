#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// default cam values
const float YAW = -90.0f; // pointing along -z axis
const float PITCH = 0.0f;
const float SPEED = 10.0f; // block per second
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:
    // cam attribs
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // euler angles
    float Yaw;
    float Pitch;

    // cam options
    float movement_speed;
    float mouse_sensitivity;
    float zoom;

    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) 
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed(SPEED), mouse_sensitivity(SENSITIVITY), zoom(ZOOM) 
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // returns view matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // input process from keyboard
    void ProcessKeyboard(int direction, float delta_time)
    {
        float velocity = movement_speed * delta_time;
        if (direction == 0) // FORWARD
            Position += Front * velocity;
        if (direction == 1) // BACKWARD
            Position -= Front * velocity;
        if (direction == 2) // LEFT
            Position -= Right * velocity;
        if (direction == 3) // RIGHT
            Position += Right * velocity;
        if (direction == 4) // UP (Space)
            Position += WorldUp * velocity;
        if (direction == 5) // DOWN (Ctrl)
            Position -= WorldUp * velocity;
    }

    // input process from mouse
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= mouse_sensitivity;
        yoffset *= mouse_sensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // bound pitch to prevent screen flip
        if (constrainPitch)
        {
            if (Pitch > 89.0f) Pitch = 89.0f;
            if (Pitch < -89.0f) Pitch = -89.0f;
        }

        updateCameraVectors();
    }

private:
    // Front calculator
    void updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // re-calculate the Right and Up
        Right = glm::normalize(glm::cross(Front, WorldUp));  
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};