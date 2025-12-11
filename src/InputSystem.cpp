#include "InputSystem.h"
#include <iostream>

// global pointer for static callback to find cam instance
static InputSystem* g_InputSystem = nullptr;
static Camera* g_CameraPtr = nullptr; 

InputSystem::InputSystem() 
    : m_selectedBlockID(1), 
      m_isLeftMousePressed(false), 
      m_isRightMousePressed(false), 
      m_isCPressed(false),
      m_firstMouse(true),
      m_lastX(1280.0f / 2.0f),
      m_lastY(720.0f / 2.0f)
{
    g_InputSystem = this;
}

void InputSystem::setupCallbacks(GLFWwindow* window)
{
    // set wrapper as callback
    glfwSetCursorPosCallback(window, mouse_callback_dispatch);
}

void InputSystem::mouse_callback_dispatch(GLFWwindow* window, double xpos, double ypos)
{
    if (g_InputSystem) {
        g_InputSystem->onMouseMovement(xpos, ypos);
    }
}

// mouse logic
void InputSystem::onMouseMovement(double xposIn, double yposIn) {
    if (!g_CameraPtr) return; // Safety check

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (m_firstMouse) {
        m_lastX = xpos;
        m_lastY = ypos;
        m_firstMouse = false;
    }

    float xoffset = xpos - m_lastX;
    float yoffset = m_lastY - ypos; // reversed y

    m_lastX = xpos;
    m_lastY = ypos;

    g_CameraPtr->ProcessMouseMovement(xoffset, yoffset);
}

void InputSystem::processInput(GLFWwindow* window, float deltaTime, VoxelWorld& world, Physics& physics, Camera& camera)
{
    // update global cam
    g_CameraPtr = &camera;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // MODE TOGGLE
    // c
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !m_isCPressed)
    {
        m_isCPressed = true;
        camera.flying_mode = !camera.flying_mode;
        std::cout << "Mode: " << (camera.flying_mode ? "FLYING" : "WALKING") << std::endl;
    }
    else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE)
    {
        m_isCPressed = false;
    }

    // HOTBAR
    // 1,2,3
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) m_selectedBlockID = 1;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) m_selectedBlockID = 2;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) m_selectedBlockID = 3;

    // MOVE MF
    // wasd, space, ctrl, shift
    float velocity = 10.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) velocity *= 2.0f;

    if (camera.flying_mode)
    {
        // flying
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(0, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(1, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(2, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(3, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camera.ProcessKeyboard(4, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) camera.ProcessKeyboard(5, deltaTime);
    }
    else
    {
        // walking
        glm::vec3 front = glm::normalize(glm::vec3(camera.Front.x, 0.0f, camera.Front.z));
        glm::vec3 right = glm::normalize(glm::vec3(camera.Right.x, 0.0f, camera.Right.z));
        glm::vec3 moveDir(0.0f);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) moveDir += front;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) moveDir -= front;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) moveDir -= right;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) moveDir += right;

        physics.move(moveDir, velocity, world, camera);

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            physics.jump(world, camera);
        }
    }

    // MOUSE CLICKS
    // GLFW_MOUSE_BUTTON_LEFT
    int stateLeft = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (stateLeft == GLFW_PRESS && !m_isLeftMousePressed)
    {
        m_isLeftMousePressed = true;
        RaycastResult result = physics.raycast(camera.Position, camera.Front, 8.0f, world);
        if (result.hit)
        {
            world.setBlock(result.x, result.y, result.z, 0); // air
        }
    }
    else if (stateLeft == GLFW_RELEASE)
    {
        m_isLeftMousePressed = false;
    }

    // GLFW_MOUSE_BUTTON_RIGHT
    int stateRight = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    if (stateRight == GLFW_PRESS && !m_isRightMousePressed)
    {
        m_isRightMousePressed = true;
        RaycastResult result = physics.raycast(camera.Position, camera.Front, 8.0f, world);
        if (result.hit)
        {
            int newX = result.x + (int)result.normal.x;
            int newY = result.y + (int)result.normal.y;
            int newZ = result.z + (int)result.normal.z;

            // dont place inside
            float dist = glm::distance(camera.Position, glm::vec3(newX + 0.5f, newY + 0.5f, newZ + 0.5f));
            if (dist > 1.5f)
            {
                world.setBlock(newX, newY, newZ, m_selectedBlockID);
            }
        }
    }
    else if (stateRight == GLFW_RELEASE) {
        m_isRightMousePressed = false;
    }
}