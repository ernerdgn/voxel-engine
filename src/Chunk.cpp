#include "Chunk.h"
#include <cstring> // memset
#include "CubeData.h" // vertex data
#include <glad/glad.h>
#include <vector>

const float ATLAS_WIDTH_PX = 256.0f;
const float ATLAS_HEIGHT_PX = 256.0f;
const float TILE_SIZE_PX = 16.0f;
const int ATLAS_ROWS = 16;
const int ATLAS_COLS = 16;

void getTextureIndex(BlockID blockID, int face_index, int &row, int &col)
{
    // face index map: 0=front, 1=back, 2=left, 3=right, 4=top, 5=bottom
    switch (blockID)
    {
        case 1: // grass
            if (face_index == 4) { row = 12; col = 12; }  // top
            else if (face_index == 5) { row = 0; col = 2; }  // bottom
            else { row = 0; col = 3; }  // sides
            break;
        case 2: // dirt
            row = 0; col = 2;
            break;
        case 3: // stone
            row = 0; col = 1;
            break;
        default: // error texture
            row = 9; col = 9;
            break;
    }
}

Chunk::Chunk()
{
    m_blocks.fill(0); // fill with air
}

Chunk::~Chunk()
{
    if (m_VAO != 0) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO != 0) glDeleteBuffers(1, &m_VBO);
}

void Chunk::setBlock(int x, int y, int z, BlockID type)
{
    if (x >= 0 && x < CHUNK_SIZE && 
        y >= 0 && y < CHUNK_SIZE && 
        z >= 0 && z < CHUNK_SIZE)
    {
        m_blocks[getIndex(x, y, z)] = type;
    }
}

BlockID Chunk::getBlock(int x, int y, int z) const
{
    if (x < 0 || x >= CHUNK_SIZE || 
        y < 0 || y >= CHUNK_SIZE || 
        z < 0 || z >= CHUNK_SIZE) // if out of bounds...
    {
        return 0; // ...return air
    }
    return m_blocks[getIndex(x, y, z)];
}

void Chunk::updateMesh() {
    std::vector<float> vertices;

    // normalized UV size of one single tile
    // 128 / 778 = 0.1645...
    float texStepX = TILE_SIZE_PX / ATLAS_WIDTH_PX;
    // 128 / 1948 = 0.0657...
    float texStepY = TILE_SIZE_PX / ATLAS_HEIGHT_PX;

    int neighbors[6][3] = {
        { 0,  0,  1}, // front--(Z+)
        { 0,  0, -1}, // back---(Z-)
        {-1,  0,  0}, // left---(X-)
        { 1,  0,  0}, // right--(X+)
        { 0,  1,  0}, // top----(Y+)
        { 0, -1,  0}  // bottom-(Y-)
    };

    // loop every block in the chunk
    for (int y = 0; y < CHUNK_SIZE; y++) 
    {
        for (int z = 0; z < CHUNK_SIZE; z++)
        {
            for (int x = 0; x < CHUNK_SIZE; x++)
            {
                BlockID block = getBlock(x, y, z);

                if (block > 0)  // if block is not air
                {
                    // faces
                    for (int f = 0; f < 6; f++)
                    {
                        
                        // neighbor check
                        int nx = x + neighbors[f][0];
                        int ny = y + neighbors[f][1];
                        int nz = z + neighbors[f][2];

                        // culling, drawing faces if neigbour is empty/air
                        if (!isSolid(nx, ny, nz))
                        {
                            int pngRow, pngCol;
                            // get texture (TODO: Different textures for different faces for spesific blocks.)
                            getTextureIndex(block, f, pngRow, pngCol);

                            // opengl wants us to flip
                            int glRow = (ATLAS_ROWS - 1) - pngRow;

                            // adding vertices for the face
                            for (int v = 0; v < 6; v++)
                            {
                                // f = face (0-5), v = vertex (0-5)
                                int iterator = (f * 6) + v; 

                                float vx = cubeVertices[iterator * 5 + 0];
                                float vy = cubeVertices[iterator * 5 + 1];
                                float vz = cubeVertices[iterator * 5 + 2];
                                float localU = cubeVertices[iterator * 5 + 3];
                                float localV = cubeVertices[iterator * 5 + 4];

                                // atlas UV calculation
                                float finalU = (pngCol * texStepX) + (localU * texStepX);
                                float finalV = (glRow * texStepY) + (localV * texStepY);

                                vertices.push_back(vx + x);
                                vertices.push_back(vy + y);
                                vertices.push_back(vz + z);
                                vertices.push_back(finalU);
                                vertices.push_back(finalV);

                            }
                        }
                    }
                }
            }
        }
    }

    m_vertex_count = vertices.size() / 5; // 5 floats per vertex

    // safety check
    if (vertices.empty()) return;

    // create buffers if dont exist
    if (m_VAO == 0)
    {
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
    }

    glBindVertexArray(m_VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    // send data
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    int stride = 5 * sizeof(float);

    // ATTRIB 0: POSITION (3 floats)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    // ATTRIB 1: TEXTURE COORDS (2 floats)
    // offset = void*(3*float)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Unbind to be safe
}

void Chunk::render()
{
    if (m_vertex_count > 0)
    {
        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, m_vertex_count);
        glBindVertexArray(0);
    }
}