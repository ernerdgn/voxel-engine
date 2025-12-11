#include "Physics.h"
#include <cmath>
#include <iostream>

Physics::Physics() : m_verticalVelocity(0.0f) {}

bool Physics::checkCollision(glm::vec3 pos, const VoxelWorld& world)
{
    float w = PLAYER_WIDTH;
    
    // check feet, waist and head
    std::vector<glm::vec3> checkPoints = {
        // feet
        pos + glm::vec3(-w, 0.1f, -w), pos + glm::vec3( w, 0.1f, -w),
        pos + glm::vec3(-w, 0.1f,  w), pos + glm::vec3( w, 0.1f,  w),
        // waist
        pos + glm::vec3(-w, 0.8f, -w), pos + glm::vec3( w, 0.8f, -w),
        pos + glm::vec3(-w, 0.8f,  w), pos + glm::vec3( w, 0.8f,  w),
        // head
        pos + glm::vec3(-w, PLAYER_HEIGHT - 0.2f, -w), pos + glm::vec3( w, PLAYER_HEIGHT - 0.2f, -w),
        pos + glm::vec3(-w, PLAYER_HEIGHT - 0.2f,  w), pos + glm::vec3( w, PLAYER_HEIGHT - 0.2f,  w)
    };

    for (const auto& p : checkPoints)
    {
        if (world.getBlock((int)floor(p.x), (int)floor(p.y), (int)floor(p.z)) > 0) return true;
    }
    return false;
}

void Physics::step(float deltaTime, VoxelWorld& world, Camera& camera)
{
    if (camera.flying_mode) return;

    // gravity, hi newton!
    m_verticalVelocity -= GRAVITY * deltaTime;
    
    // move
    glm::vec3 nextPos = camera.Position;
    nextPos.y += m_verticalVelocity * deltaTime;

    // floor/ceiling collision
    if (checkCollision(glm::vec3(nextPos.x, nextPos.y - PLAYER_HEIGHT, nextPos.z), world))
    {
        if (m_verticalVelocity < 0) m_verticalVelocity = 0; // landed
        else if (m_verticalVelocity > 0) m_verticalVelocity = 0; // head hit, ouch
    } 
    else
    {
        camera.Position = nextPos;
    }

    // if falling from the world
    if (camera.Position.y < -50)
    {
        camera.Position = glm::vec3(16, 60, 16);
        m_verticalVelocity = 0;
    }
}

void Physics::move(glm::vec3 direction, float speed, VoxelWorld& world, Camera& camera)
{
    if (camera.flying_mode) return;
    if (glm::length(direction) == 0.0f) return;

    // get velo
    glm::vec3 moveStep = glm::normalize(direction) * speed;

    glm::vec3 originalPos = camera.Position;

    // x move
    camera.Position.x += moveStep.x;
    glm::vec3 feetPos = camera.Position; 
    feetPos.y -= PLAYER_HEIGHT;
    
    if (checkCollision(feetPos, world))
    {
        camera.Position.x = originalPos.x; // back x
    }

    // z move
    camera.Position.z += moveStep.z;
    feetPos = camera.Position; 
    feetPos.y -= PLAYER_HEIGHT;
    
    if (checkCollision(feetPos, world))
    {
        camera.Position.z = originalPos.z; // back z
    }
}

void Physics::jump(VoxelWorld& world, Camera& camera)
{
    if (camera.flying_mode) return;

    // jump if feets on ground
    if (checkCollision(glm::vec3(camera.Position.x, camera.Position.y - PLAYER_HEIGHT - 0.1f, camera.Position.z), world))
    {
        if (abs(m_verticalVelocity) < 0.1f) m_verticalVelocity = JUMP_FORCE;
    }
}

RaycastResult Physics::raycast(glm::vec3 origin, glm::vec3 direction, float maxDist, const VoxelWorld& world)
{
    int x = (int)floor(origin.x);
    int y = (int)floor(origin.y);
    int z = (int)floor(origin.z);

    int stepX = (direction.x > 0) ? 1 : -1;
    int stepY = (direction.y > 0) ? 1 : -1;
    int stepZ = (direction.z > 0) ? 1 : -1;

    float deltaDistX = (direction.x == 0) ? 1e30 : std::abs(1.0f / direction.x);
    float deltaDistY = (direction.y == 0) ? 1e30 : std::abs(1.0f / direction.y);
    float deltaDistZ = (direction.z == 0) ? 1e30 : std::abs(1.0f / direction.z);

    float sideDistX = (direction.x < 0) ? (origin.x - x) * deltaDistX : (x + 1.0f - origin.x) * deltaDistX;
    float sideDistY = (direction.y < 0) ? (origin.y - y) * deltaDistY : (y + 1.0f - origin.y) * deltaDistY;
    float sideDistZ = (direction.z < 0) ? (origin.z - z) * deltaDistZ : (z + 1.0f - origin.z) * deltaDistZ;

    int lastAxis = -1;
    float dist = 0.0f;
    
    while (dist < maxDist)
    {
        if (sideDistX < sideDistY && sideDistX < sideDistZ)
        {
            sideDistX += deltaDistX;
            x += stepX;
            dist = sideDistX - deltaDistX;
            lastAxis = 0;
        }
        else if (sideDistY < sideDistZ) 
        {
            sideDistY += deltaDistY;
            y += stepY;
            dist = sideDistY - deltaDistY;
            lastAxis = 1;
        }
        else 
        {
            sideDistZ += deltaDistZ;
            z += stepZ;
            dist = sideDistZ - deltaDistZ;
            lastAxis = 2;
        }

        if (world.getBlock(x, y, z) > 0)
        {
            glm::vec3 normal(0.0f);
            if (lastAxis == 0) normal.x = (float)-stepX;
            if (lastAxis == 1) normal.y = (float)-stepY;
            if (lastAxis == 2) normal.z = (float)-stepZ;
            return { true, x, y, z, normal };
        }
    }
    return { false, 0, 0, 0, glm::vec3(0.0f) };
}