#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include <vector>
#include "VoxelWorld.h"
#include "Physics.h"
#include "InputSystem.h"
#include "Texture.h"
#include "OutlineRenderer.h"

// force gpu
extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

// globals
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
Camera camera(glm::vec3(16.0f, 20.0f, 40.0f));
// time
float delta_time = 0.0f;
float last_frame = 0.0f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool first_mouse = true;

// glfw callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }

// bridge for cam global
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn); 
    float ypos = static_cast<float>(yposIn);
    if (first_mouse) { lastX = xpos; lastY = ypos; first_mouse = false; }
    camera.ProcessMouseMovement(xpos - lastX, lastY - ypos);
    lastX = xpos; lastY = ypos;
}

int main()
{
    // window creation
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Voxel Engine Burden | Dev Build", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        std::cout << "failed to create GLFW window" << std::endl;
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;
    glEnable(GL_DEPTH_TEST);

    // shader setup
    Shader shader("../assets/shaders/chunk.vert", "../assets/shaders/chunk.frag");
    Shader outlineShader("../assets/shaders/outline.vert", "../assets/shaders/outline.frag");

    // system inits
    VoxelWorld world;
    Physics physics;
    InputSystem input;
    input.setupCallbacks(window);

    // init rendering helpers
    OutlineRenderer outlineRenderer;
    outlineRenderer.init();

    Texture atlas;
    atlas.load("../assets/textures/texture_atlas.png");

    // shader uniform config
    shader.use(); 
    shader.setInt("TEXTURE", 0);

    // loop
    while (!glfwWindowShouldClose(window))
    {
        float current_frame = static_cast<float>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        // logic
        input.processInput(window, delta_time, world, physics, camera);
        physics.step(delta_time, world, camera);

        // clear render
        glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // bind text
        atlas.bind(0);

        // render
        shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shader.setMat4("projection", projection);
        shader.setMat4("view", camera.GetViewMatrix());

        world.render(shader, camera.Position, 8);

        // outline
        RaycastResult target = physics.raycast(camera.Position, camera.Front, 8.0f, world);
        if (target.hit)
        {
            outlineRenderer.render(outlineShader, glm::vec3(target.x, target.y, target.z), camera.GetViewMatrix(), projection);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}