# Voxel Engine (C++ / OpenGL)

![OpenGL](https://img.shields.io/badge/OpenGL-4.5-green.svg) ![C++](https://img.shields.io/badge/C%2B%2B-20-blue.svg)

A voxel engine written from scratch in C++ and Modern OpenGL. This engine features infinite terrain generation, dynamic chunk management, fundamental physics (AABB collision) and voxel interaction (raycasting).

It is designed as a foundational tech demo for voxel-based games, implementing core mechanics similar to *Minecraft* Alpha.

## Features

* **Procedural Generation**: Infinite 3D terrain generation using 3D Perlin Noise.
* **Chunk System**: Dynamic mesh generation with optimized face culling.
* **Infinite World**: Chunk loading and rendering based on player position.
* **Physics Engine**:
    * Gravity and jumping.
    * Axis-Separated AABB Collision Detection (sliding along walls).
    * Ghost Mode (Flying) vs. Walking Mode toggles.
* **Interaction**:
    * **Raycasting**: Precise block selection using the DDA (Digital Differential Analyzer) algorithm.
    * **Destruction**: Break blocks instantly with visual updates.
    * **Construction**: Place blocks on specific faces of existing voxels.
    * **Block Selection**: Hotbar system to swap block types.
* **Visuals**:
    * Texture Atlas support with correct UV mapping.
    * Wireframe Selection Box highlighting targeted blocks.

## Controls

| Key / Mouse | Action |
| :--- | :--- |
| **W / A / S / D** | Move Player |
| **Space** | Jump (Walk Mode) / Fly Up (Ghost Mode) |
| **Left Ctrl** | Fly Down (Ghost Mode only) |
| **Left Shift** | Sprint (2x Speed) |
| **C** | Toggle Mode (Ghost/Walk) |
| **Left Click** | Destroy Block |
| **Right Click** | Place Block |
| **1 / 2 / 3** | Select Block (Grass / Dirt / Stone) |
| **ESC** | Exit |

## Technology Stack

* **Language**: C++20
* **Graphics API**: OpenGL 4.5 (Core Profile)
* **Windowing**: [GLFW](https://www.glfw.org/)
* **Extension Loader**: [GLAD](https://glad.dav1d.de/)
* **Mathematics**: [GLM](https://github.com/g-truc/glm)
* **Texture Loading**: [stb_image](https://github.com/nothings/stb)
* **Noise Generation**: [stb_perlin](https://github.com/nothings/stb)

## Build Instructions

### Prerequisites
* **CMake** (3.14+)
* **C++ Compiler** (MSVC, GCC, or Clang) supporting C++20.

### Building
1.  Clone the repository:
    ```bash
    git clone https://github.com/ernerdgn/voxel-engine.git
    cd voxel-engine
    ```
2.  Create a build directory:
    ```bash
    mkdir build
    cd build
    ```
3.  Generate project files using CMake:
    ```bash
    cmake ..
    ```
4.  Compile:
    * **Windows**: Open the generated `.sln` file in Visual Studio or run `cmake --build .`
    * **Linux/Mac**: Run `make`

## Technical Deep Dive

### 1. The Physics System: AABB Collision
The engine uses **Axis-Aligned Bounding Box (AABB)** collision detection. To prevent moving through walls and allow moving diagonally against a wall, the engine employs **Axis-Separated Movement**.

**The Algorithm:**
Instead of moving the player directly to the target position, movement is splitted into three separate steps (X, Y, Z).

1.  **Apply Velocity on X axis.**
2.  **Check Collision:**
    * Defining the player's bounding box (Width: 0.3m, Height: 1.7m).
    * Sampling 12 points around the player model (Feet, Waist, Head).
    * If *any* of these points intersect a solid voxel (`BlockID > 0`), this considers as a collision.
3.  **Resolve X:** If a collision occurred, reset the X position to the previous safe value.
4.  **Repeat for Z axis.**
5.  **Repeat for Y axis (Gravity).**

### 2. Raycasting: The DDA Algorithm
To determine which block the player is looking at **Digital Differential Analyzer (DDA)** algorithm is used.

**The Concept:**
A 3D grid is composed of integer boundaries ($x=1, x=2, etc.$). The ray must cross these boundaries. DDA calculates the exact distance the ray must travel to cross the next X, Y or Z boundary and always "jumps" to the nearest one.

**The Math:**
Given a ray origin $\vec{O}$ and direction $\vec{D}$:

1.  **Step Direction ($step$)**:
    * If $D_x > 0 \Rightarrow step_x = +1$
    * If $D_x < 0 \Rightarrow step_x = -1$

2.  **Delta Distance ($\Delta dist$)**: The distance the ray travels to cover exactly 1 unit on a specific axis.
    $$ \Delta dist_x = \sqrt{1 + \frac{D_y^2}{D_x^2} + \frac{D_z^2}{D_x^2}} = \left| \frac{1}{D_x} \right| $$
    *(Simplified for normalized vectors)*

3.  **Side Distance ($sideDist$)**: The distance from the origin to the *first* boundary.
    $$ sideDist_x = (map_x + 1.0 - O_x) \times \Delta dist_x $$

4.  **The Loop**:
    Comparing $sideDist_x$, $sideDist_y$, and $sideDist_z$.
    * If $sideDist_x$ is the smallest, the ray hits an X-boundary first.
    * Increment map X by $step_x$.
    * Add $\Delta dist_x$ to $sideDist_x$.
    * Check map at new $(x, y, z)$. If solid, its a hit.

## TODO
* Ambient Occlusion (AO) for better depth perception.
* Save/load system (chunk serialization).
* Multithreaded chunk generation.
* Water and transparent blocks.