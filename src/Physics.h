#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "VoxelWorld.h"
#include "Camera.h"

struct RaycastResult {
    bool hit;
    int x, y, z;
    glm::vec3 normal;
};

class Physics
{
public:
    // Constants
    const float GRAVITY = 18.0f;
    const float JUMP_FORCE = 8.0f;
    const float PLAYER_HEIGHT = 1.7f;
    const float PLAYER_WIDTH = 0.3f;

    Physics();

    // update
    void step(float deltaTime, VoxelWorld& world, Camera& camera);

    // horizontal move
    void move(glm::vec3 direction, float speed, VoxelWorld& world, Camera& camera);

    // action
    void jump(VoxelWorld& world, Camera& camera);
    
    RaycastResult raycast(glm::vec3 origin, glm::vec3 direction, float maxDist, const VoxelWorld& world);

private:
    float m_verticalVelocity;

    // aabb, funny name
    bool checkCollision(glm::vec3 pos, const VoxelWorld& world);
};