#pragma once
#include <array>
#include <cstdint>
#include <vector>

constexpr int CHUNK_SIZE = 32;
constexpr int CHUNK_VOLUME = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

// type (0 = air, 1 = dirt)
using BlockID = uint8_t;

class Chunk
{
public:
    Chunk();
    ~Chunk();

    void setBlock(int x, int y, int z, BlockID type);

    BlockID getBlock(int x, int y, int z) const;

    // mesh
    void updateMesh();
    void render();

private:
    std::array<BlockID, CHUNK_VOLUME> m_blocks;

    unsigned int m_VAO = 0; // vertex array obj
    unsigned int m_VBO = 0; // vertex buffer obj
    size_t m_vertex_count = 0;

    // convert x,y,z to index = x + (z * size) + (y * size * size)
    inline int getIndex(int x, int y, int z) const
    {
        return x + (z * CHUNK_SIZE) + (y * CHUNK_SIZE * CHUNK_SIZE);
    }

    bool isSolid(int x, int y, int z) const
    {
        if (x < 0 || x >= CHUNK_SIZE ||
            y < 0 || y >= CHUNK_SIZE ||
            z < 0 || z >= CHUNK_SIZE)
        {
            return false; 
        }

        return m_blocks[getIndex(x, y, z)] > 0;
    }
};