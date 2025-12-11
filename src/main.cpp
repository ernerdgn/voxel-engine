#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Chunk.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include "stb_image.h"
#include "stb_perlin.h"
#include <vector>

// force external gpu
extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

struct RaycastResult
{
    bool hit;
    int x, y, z;
};

float verticesBox[] = {
    0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
    
    0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,  0.0f, 1.0f, 1.0f,  0.0f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
    
    0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 1.0f
};

unsigned int boxVAO = 0, boxVBO = 0;

void initOutline()
{
    glGenVertexArrays(1, &boxVAO);
    glGenBuffers(1, &boxVBO);
    glBindVertexArray(boxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesBox), verticesBox, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void drawOutline(Shader& shader, int x, int y, int z, glm::mat4 view, glm::mat4 proj)
{
    glLineWidth(9.0f); 

    shader.use();
    shader.setMat4("view", view);
    shader.setMat4("projection", proj);
    
    glm::mat4 model = glm::mat4(1.0f);
    
    model = glm::translate(model, glm::vec3(x, y, z));
    
    model = glm::scale(model, glm::vec3(1.002f));
    
    model = glm::translate(model, glm::vec3(-0.001f)); 

    shader.setMat4("model", model);

    glBindVertexArray(boxVAO);
    glDrawArrays(GL_LINES, 0, 24); 
    
    glLineWidth(1.0f);
}

// global
const unsigned int WIDTH = 1280;
const unsigned int HEIGHT = 720;
float cam_speed_coefficient = 1.0f;
const int WORLD_SIZE = 5;
bool is_left_mouse_button_pressed = false;

// cam setup
Camera camera(glm::vec3(16.0f, 20.0f, 40.0f));
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool first_mouse = true;

// time
float delta_time = 0.0f;
float last_frame = 0.0f;

// CALLBACKS
// resize callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// mosue callback
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (first_mouse) {
        lastX = xpos;
        lastY = ypos;
        first_mouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

bool setBlockGlobal(int x, int y, int z, int type, std::vector<Chunk>& chunks)
{
    if (y < 0 || y >= CHUNK_SIZE) return false;

    int chunkX = x / CHUNK_SIZE;
    int chunkZ = z / CHUNK_SIZE;

    // negative pos handling
    if (x < 0) chunkX = (x - CHUNK_SIZE + 1) / CHUNK_SIZE;
    if (z < 0) chunkZ = (z - CHUNK_SIZE + 1) / CHUNK_SIZE;

    // check bounds
    if (chunkX < 0 || chunkX >= WORLD_SIZE || chunkZ < 0 || chunkZ >= WORLD_SIZE) return false;

    // local pos
    int localX = x % CHUNK_SIZE;
    int localZ = z % CHUNK_SIZE;
    if (localX < 0) localX += CHUNK_SIZE;
    if (localZ < 0) localZ += CHUNK_SIZE;

    // DEBUG
    int chunkIndex = chunkX + (chunkZ * WORLD_SIZE);
    
    std::cout << "Click at Global(" << x << "," << z << ") -> Chunk(" << chunkX << "," << chunkZ << ")" 
              << " Index calculated: " << chunkIndex 
              << " (Limit: " << chunks.size() << ")" << std::endl;
    // DEBUG

    //int chunkIndex = chunkX + (chunkZ * WORLD_SIZE);
    //chunks[chunkIndex].setBlock(localX, y, z, type);
    chunks[chunkIndex].setBlock(localX, y, localZ, type);
    chunks[chunkIndex].updateMesh();

    // neighbor chunk update if destroyed block on the side
    // update left
    if (localX == 0 && chunkX > 0) {
        int leftNeighbor = (chunkX - 1) + (chunkZ * WORLD_SIZE);
        chunks[leftNeighbor].updateMesh();
    }
    // update right
    else if (localX == CHUNK_SIZE - 1 && chunkX < WORLD_SIZE - 1) {
        int rightNeighbor = (chunkX + 1) + (chunkZ * WORLD_SIZE);
        chunks[rightNeighbor].updateMesh();
    }

    // update back
    if (localZ == 0 && chunkZ > 0) {
        int backNeighbor = chunkX + ((chunkZ - 1) * WORLD_SIZE);
        chunks[backNeighbor].updateMesh();
    }
    // update front
    else if (localZ == CHUNK_SIZE - 1 && chunkZ < WORLD_SIZE - 1) {
        int frontNeighbor = chunkX + ((chunkZ + 1) * WORLD_SIZE);
        chunks[frontNeighbor].updateMesh();
    }

    return true;
}

int getBlockGlobal(int x, int y, int z, const std::vector<Chunk>& chunks)
{
    if (y < 0 || y >= CHUNK_SIZE) return 0; // air

    int chunkX = x / CHUNK_SIZE;
    int chunkZ = z / CHUNK_SIZE;
    
    // negative coords handling
    if (x < 0) chunkX = (x - CHUNK_SIZE + 1) / CHUNK_SIZE;
    if (z < 0) chunkZ = (z - CHUNK_SIZE + 1) / CHUNK_SIZE;

    if (chunkX < 0 || chunkX >= WORLD_SIZE || chunkZ < 0 || chunkZ >= WORLD_SIZE) return 0;

    int localX = x % CHUNK_SIZE;
    int localZ = z % CHUNK_SIZE;
    if (localX < 0) localX += CHUNK_SIZE;
    if (localZ < 0) localZ += CHUNK_SIZE;

    int chunkIndex = chunkX + (chunkZ * WORLD_SIZE);
    //return chunks[chunkIndex].getBlock(localX, y, z);
    return chunks[chunkIndex].getBlock(localX, y, localZ);
}

RaycastResult raycast(glm::vec3 origin, glm::vec3 direction, float maxDist, const std::vector<Chunk>& chunks)
{
    // start voxel
    int x = (int)floor(origin.x);
    int y = (int)floor(origin.y);
    int z = (int)floor(origin.z);

    // dir steps
    int stepX = (direction.x > 0) ? 1 : -1;
    int stepY = (direction.y > 0) ? 1 : -1;
    int stepZ = (direction.z > 0) ? 1 : -1;

    // ray length
    float delta_distX = (direction.x == 0) ? 1e30 : std::abs(1.0f / direction.x);
    float delta_distY = (direction.y == 0) ? 1e30 : std::abs(1.0f / direction.y);
    float delta_distZ = (direction.z == 0) ? 1e30 : std::abs(1.0f / direction.z);

    // init side dist
    float side_distX, side_distY, side_distZ;

    if (direction.x < 0) side_distX = (origin.x - x) * delta_distX;
    else                 side_distX = (x + 1.0f - origin.x) * delta_distX;

    if (direction.y < 0) side_distY = (origin.y - y) * delta_distY;
    else                 side_distY = (y + 1.0f - origin.y) * delta_distY;

    if (direction.z < 0) side_distZ = (origin.z - z) * delta_distZ;
    else                 side_distZ = (z + 1.0f - origin.z) * delta_distZ;

    // digital differential analyzer, cool name
    float dist = 0.0f;
    while (dist < maxDist)
    {
        // Walk along the shortest path
        if (side_distX < side_distY && side_distX < side_distZ)
        {
            side_distX += delta_distX;
            x += stepX;
            dist = side_distX - delta_distX; // TODO: simplify
        } 
        else if (side_distY < side_distZ)
        {
            side_distY += delta_distY;
            y += stepY;
            dist = side_distY - delta_distY;
        } 
        else
        {
            side_distZ += delta_distZ;
            z += stepZ;
            dist = side_distZ - delta_distZ;
        }

        // if hit a block
        if (getBlockGlobal(x, y, z, chunks) > 0)
        {
            return { true, x, y, z };
        }
    }

    return { false, 0, 0, 0 }; // miss
}

// input process
void processInput(GLFWwindow *window, std::vector<Chunk>& chunks)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)  // esc to close
        glfwSetWindowShouldClose(window, true);

    // WASD - 0=fwd, 1=back, 2=left, 3=right, 4=up, 5=down
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(0, delta_time * cam_speed_coefficient);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(1, delta_time * cam_speed_coefficient);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(2, delta_time * cam_speed_coefficient);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(3, delta_time * cam_speed_coefficient);

    // fly up/down - space/ctrl
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(4, delta_time * cam_speed_coefficient);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.ProcessKeyboard(5, delta_time * cam_speed_coefficient);

    // move speed - shift
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cam_speed_coefficient = 3.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        cam_speed_coefficient = 1.0f;

    // mouse clicks
    // GLFW_MOUSE_BUTTON_LEFT
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (state == GLFW_PRESS && !is_left_mouse_button_pressed)
    {
        is_left_mouse_button_pressed = true; // lock until release

        // ray cast
        RaycastResult result = raycast(camera.Position, camera.Front, 8.0f, chunks);
        
        // if hit delete block
        if (result.hit) {
            std::cout << "breaking block at: " << result.x << " " << result.y << " " << result.z << std::endl;
            setBlockGlobal(result.x, result.y, result.z, 0, chunks); // 0 = Air
        }
    }
    else if (state == GLFW_RELEASE)
    {
        is_left_mouse_button_pressed = false; // release
    }
}

unsigned int loadTexture(const char *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrChannels;
    // flip textures vertically because opengl expects 0.0 on bottom
    stbi_set_flip_vertically_on_load(true); 
    
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format;
        if (nrChannels == 1) format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // nearest neighor filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        stbi_image_free(data);
    }
    
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        std::cout << "STB Error: " << stbi_failure_reason() << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

int main()
{
    // glfw init
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // config opengl
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // window creation
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Burden Voxel Engine | Dev Build", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // mouse capture (hide cursor, lock to center)
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad init
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // depth testing
    glEnable(GL_DEPTH_TEST);

    // compile shaders
    Shader shader ("../assets/shaders/chunk.vert", "../assets/shaders/chunk.frag");

    initOutline();
    Shader outlineShader("../assets/shaders/outline.vert", "../assets/shaders/outline.frag");

    // test chunk
    std::vector<Chunk> chunks(WORLD_SIZE * WORLD_SIZE);

    // terrain config
    float frequency = 0.05f;
    float amplitude = 10.0f;
    int sea_level = 4;

    std::cout << WORLD_SIZE << "^2 test world is generating..." << std::endl;

    // loop chunks
    for(int cx = 0; cx < WORLD_SIZE; cx++) 
    {
        for(int cz = 0; cz < WORLD_SIZE; cz++) 
        {
            Chunk& currentChunk = chunks[cx + cz * WORLD_SIZE];
            // loop blocks
            for(int x = 0; x < CHUNK_SIZE; x++) 
            {
                for(int z = 0; z < CHUNK_SIZE; z++) 
                {
                    // calculate world pos
                    // Chunk 0's X range: 0-31
                    // Chunk 1's X range: 32-63
                    int worldX = (cx * CHUNK_SIZE) + x;
                    int worldZ = (cz * CHUNK_SIZE) + z;

                    // generate noise according to world pos
                    float noise_value = stb_perlin_noise3(worldX * frequency, worldZ * frequency, 0.0f, 0, 0, 0);
                    int height = sea_level + (int)((noise_value + 1.0f) * 0.5f * amplitude);

                    // safety bound
                    if (height >= CHUNK_SIZE) height = CHUNK_SIZE - 1;
                    if (height < 0) height = 0;

                    // fill
                    for(int y = 0; y <= height; y++)
                    {
                        if (y == height) 
                            currentChunk.setBlock(x, y, z, 1); // grass
                        else if (y > height - 3) 
                            currentChunk.setBlock(x, y, z, 2); // dirt
                        else 
                            currentChunk.setBlock(x, y, z, 3); // stone
                    }
                }
            }
            
            // generate mesh
            currentChunk.updateMesh();
        }
    }

    std::cout << "generation complete" << std::endl;

    // chunks radius
    int render_distance = 8;

    // debug - gpu info
    std::cout << "OpenGL Info:" << std::endl;
    std::cout << "  Vendor:   " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "  Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "  Version:  " << glGetString(GL_VERSION) << std::endl;

    unsigned int textureAtlas = loadTexture("../assets/textures/texture_atlas.png");
    // ../assets/textures/texture_atlas.png
    shader.use();
    shader.setInt("ourTexture", 0);

    // loop
    while (!glfwWindowShouldClose(window))
    {
        // time
        float current_frame = static_cast<float>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        // input
        processInput(window, chunks);

        // clear view
        glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureAtlas);

        // activate shaders
        shader.use();

        RaycastResult target = raycast(camera.Position, camera.Front, 8.0f, chunks);

        // CAM
        // proj
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        shader.setMat4("projection", projection);

        // view
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("view", view);

        // render world
        int playerChunkX = (int)(camera.Position.x / CHUNK_SIZE);
        int playerChunkZ = (int)(camera.Position.z / CHUNK_SIZE);

        for(int cx = 0; cx < WORLD_SIZE; cx++) 
        {
            for(int cz = 0; cz < WORLD_SIZE; cz++) 
            {
                // render distance check
                int dist = sqrt(pow(cx - playerChunkX, 2) + pow(cz - playerChunkZ, 2));
                
                if (dist < render_distance) 
                {
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(cx * CHUNK_SIZE, 0.0f, cz * CHUNK_SIZE));
                    shader.setMat4("model", model);

                    // render from vector
                    chunks[cx + cz * WORLD_SIZE].render();
                }
            }
        }

        if (target.hit)
        {
            drawOutline(outlineShader, target.x, target.y, target.z, view, projection);
        }

        // swap
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // clean
    glfwTerminate();
    return 0;
}