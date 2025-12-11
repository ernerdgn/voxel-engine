#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"

class OutlineRenderer
{
public:
    OutlineRenderer();
    ~OutlineRenderer();

    void init(); // start

    void render(Shader& shader, glm::vec3 pos, glm::mat4 view, glm::mat4 projection);

private:
    unsigned int VAO, VBO;
};