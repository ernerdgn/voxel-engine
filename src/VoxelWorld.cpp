#include "VoxelWorld.h"
#include <iostream>
#include <cmath>

VoxelWorld::VoxelWorld()
{
    m_chunks.resize(WORLD_SIZE * WORLD_SIZE);
    generateTerrain();
}

void VoxelWorld::generateTerrain()
{
    float frequency = 0.05f;
    float amplitude = 10.0f;
    int sea_level = 4;

    std::cout << "generating " << WORLD_SIZE << "x" << WORLD_SIZE << " world..." << std::endl;

    for(int cx = 0; cx < WORLD_SIZE; cx++)
    {
        for(int cz = 0; cz < WORLD_SIZE; cz++) {
            Chunk& currentChunk = m_chunks[cx + cz * WORLD_SIZE];
            
            for(int x = 0; x < CHUNK_SIZE; x++)
            {
                for(int z = 0; z < CHUNK_SIZE; z++)
                {
                    int worldX = (cx * CHUNK_SIZE) + x;
                    int worldZ = (cz * CHUNK_SIZE) + z;

                    float noise_value = stb_perlin_noise3(worldX * frequency, worldZ * frequency, 0.0f, 0, 0, 0);
                    int height = sea_level + (int)((noise_value + 1.0f) * 0.5f * amplitude);

                    if (height >= CHUNK_SIZE) height = CHUNK_SIZE - 1;
                    if (height < 0) height = 0;

                    for(int y = 0; y <= height; y++) {
                        if (y == height) currentChunk.setBlock(x, y, z, 1);
                        else if (y > height - 3) currentChunk.setBlock(x, y, z, 2);
                        else currentChunk.setBlock(x, y, z, 3);
                    }
                }
            }
            currentChunk.updateMesh();
        }
    }
    std::cout << "generation complete!" << std::endl;
}

int VoxelWorld::getBlock(int x, int y, int z) const
{
    if (y < 0 || y >= CHUNK_SIZE) return 0;

    int chunkX = x / CHUNK_SIZE;
    int chunkZ = z / CHUNK_SIZE;

    if (x < 0) chunkX = (x - CHUNK_SIZE + 1) / CHUNK_SIZE;
    if (z < 0) chunkZ = (z - CHUNK_SIZE + 1) / CHUNK_SIZE;

    if (chunkX < 0 || chunkX >= WORLD_SIZE || chunkZ < 0 || chunkZ >= WORLD_SIZE) return 0;

    int localX = x % CHUNK_SIZE;
    int localZ = z % CHUNK_SIZE;
    if (localX < 0) localX += CHUNK_SIZE;
    if (localZ < 0) localZ += CHUNK_SIZE;

    int chunkIndex = chunkX + (chunkZ * WORLD_SIZE);
    return m_chunks[chunkIndex].getBlock(localX, y, localZ);
}

void VoxelWorld::setBlock(int x, int y, int z, int type)
{
    if (y < 0 || y >= CHUNK_SIZE) return;

    int chunkX = x / CHUNK_SIZE;
    int chunkZ = z / CHUNK_SIZE;

    if (x < 0) chunkX = (x - CHUNK_SIZE + 1) / CHUNK_SIZE;
    if (z < 0) chunkZ = (z - CHUNK_SIZE + 1) / CHUNK_SIZE;

    if (chunkX < 0 || chunkX >= WORLD_SIZE || chunkZ < 0 || chunkZ >= WORLD_SIZE) return;

    int localX = x % CHUNK_SIZE;
    int localZ = z % CHUNK_SIZE;
    if (localX < 0) localX += CHUNK_SIZE;
    if (localZ < 0) localZ += CHUNK_SIZE;

    int chunkIndex = chunkX + (chunkZ * WORLD_SIZE);
    m_chunks[chunkIndex].setBlock(localX, y, localZ, type);
    m_chunks[chunkIndex].updateMesh();

    // neighbor
    if (localX == 0 && chunkX > 0) 
        m_chunks[(chunkX - 1) + (chunkZ * WORLD_SIZE)].updateMesh();
    else if (localX == CHUNK_SIZE - 1 && chunkX < WORLD_SIZE - 1) 
        m_chunks[(chunkX + 1) + (chunkZ * WORLD_SIZE)].updateMesh();

    if (localZ == 0 && chunkZ > 0) 
        m_chunks[chunkX + ((chunkZ - 1) * WORLD_SIZE)].updateMesh();
    else if (localZ == CHUNK_SIZE - 1 && chunkZ < WORLD_SIZE - 1) 
        m_chunks[chunkX + ((chunkZ + 1) * WORLD_SIZE)].updateMesh();
}

void VoxelWorld::render(Shader& shader, glm::vec3 playerPos, int render_distance)
{
    int playerChunkX = (int)(playerPos.x / CHUNK_SIZE);
    int playerChunkZ = (int)(playerPos.z / CHUNK_SIZE);

    for(int cx = 0; cx < WORLD_SIZE; cx++)
    {
        for(int cz = 0; cz < WORLD_SIZE; cz++)
        {
            int dist = sqrt(pow(cx - playerChunkX, 2) + pow(cz - playerChunkZ, 2));
            if (dist < render_distance)
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(cx * CHUNK_SIZE, 0.0f, cz * CHUNK_SIZE));
                shader.setMat4("model", model);
                m_chunks[cx + cz * WORLD_SIZE].render();
            }
        }
    }
}