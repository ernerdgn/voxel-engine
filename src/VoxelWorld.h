#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Chunk.h"
#include "Shader.h"
#include "stb_perlin.h"

class VoxelWorld
{
public:
    // settings
    const int WORLD_SIZE = 5;
    const int CHUNK_SIZE = 32;

    VoxelWorld();

    int getBlock(int x, int y, int z) const;
    void setBlock(int x, int y, int z, int type);

    void render(Shader& shader, glm::vec3 playerPos, int render_distance);

    const std::vector<Chunk>& getChunks() const { return m_chunks; }

private:
    std::vector<Chunk> m_chunks;

    void generateTerrain();
};