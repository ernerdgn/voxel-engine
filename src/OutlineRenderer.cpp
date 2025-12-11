#include "OutlineRenderer.h"

// Define the data inside the CPP so it doesn't pollute the header
static float verticesBox[] = {
    0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,  0.0f, 1.0f, 1.0f,  0.0f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 1.0f
};

OutlineRenderer::OutlineRenderer() : VAO(0), VBO(0) {}

OutlineRenderer::~OutlineRenderer()
{
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
}

void OutlineRenderer::init()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesBox), verticesBox, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void OutlineRenderer::render(Shader& shader, glm::vec3 pos, glm::mat4 view, glm::mat4 projection)
{
    glLineWidth(3.0f); 
    shader.use();
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::scale(model, glm::vec3(1.002f));
    model = glm::translate(model, glm::vec3(-0.001f)); 

    shader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 24); 
    glLineWidth(1.0f);
}