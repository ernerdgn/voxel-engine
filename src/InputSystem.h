#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "VoxelWorld.h"
#include "Physics.h"
#include "Camera.h"

class InputSystem
{
public:
    InputSystem();

    // glfw callbacks
    void setupCallbacks(GLFWwindow* window);

    void processInput(GLFWwindow* window, float deltaTime, VoxelWorld& world, Physics& physics, Camera& camera);

    // getter
    int getSelectedBlockID() const { return m_selectedBlockID; }

private:
    // interaction
    int m_selectedBlockID;
    bool m_isLeftMousePressed;
    bool m_isRightMousePressed;
    bool m_isCPressed;

    // mouse look
    bool m_firstMouse;
    float m_lastX;
    float m_lastY;

    // callback logic
    void onMouseMovement(double xpos, double ypos);

    // static wrapper for GLFW (C-style function pointers)
    static void mouse_callback_dispatch(GLFWwindow* window, double xpos, double ypos);
};