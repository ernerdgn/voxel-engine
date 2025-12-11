#pragma once

// Format: x, y, z, u, v
// Coordinates: 0.0 to 1.0 (Fixes alignment with Raycast/Outline)
const float cubeVertices[] = {
    // Front Face (Z=1)
    0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

    // Back Face (Z=0)
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

    // Left Face (X=0)
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

    // Right Face (X=1)
    1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

    // Top Face (Y=1)
    0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 1.0f, 0.0f, 0.0f,

    // Bottom Face (Y=0)
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // <--- Corrected this vertex!
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f
};