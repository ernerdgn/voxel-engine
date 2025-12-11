cmake_minimum_required(VERSION 3.14)
project(VoxelEngineBurden VERSION 0.1 LANGUAGES CXX C)

# settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# dependencies
include(FetchContent)
# 1-GLFW for window and input
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.8
)
FetchContent_MakeAvailable(glfw)

# 2-GLAD for opengl load
add_library(glad "${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/src/glad.c")
target_include_directories(glad PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include")

# 3-GLM for math
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        0af55ccecd98d4e5a8d1fad7de25ba429d60e863 #refs/tags/1.0.1
)
FetchContent_MakeAvailable(glm)

# 4- stb_image for texture loading
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(stb)

# 5-exe
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/Chunk.cpp 
    src/Chunk.h
    src/Shader.h
    src/Camera.h
    src/stb_impl.cpp
    src/VoxelWorld.h
    src/VoxelWorld.cpp
    src/Physics.h
    src/Physics.cpp
    src/InputSystem.h
    src/InputSystem.cpp
    src/OutlineRenderer.h
    src/OutlineRenderer.cpp
    src/Texture.h
    src/Texture.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE ${stb_SOURCE_DIR})

# 6-link
target_link_libraries(${PROJECT_NAME} PRIVATE glfw glad glm::glm)

# # 7-get assets to build
# add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#     "${CMAKE_CURRENT_SOURCE_DIR}/assets"
#     "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
# )