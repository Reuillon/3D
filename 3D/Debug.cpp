#include "Debug.h"
//DEBUG TOOLS  //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////


void Debug::initializeGrid(float size)
{
    int gridSize = 50;
    for (int i = 0; i < gridSize; i++)
    {
        for (int j = 0; j < gridSize; j++)
        {
            if (i < gridSize - 1)
            {
                grid.insert(grid.end(), { (float)(0.0 + ((i - (gridSize / 2)) * size)),-2.0,(float)(0.0 + ((j - (gridSize / 2)) * size)), (float)(0.0 + ((i - (gridSize / 2)) * size)),-2.0, (float)(-size + ((j - (gridSize / 2)) * size)) });
            }
            if (j < gridSize - 1)
            {
                grid.insert(grid.end(), { (float)(0.0 + ((i - (gridSize / 2)) * size)),-2.0,(float)(0.0 + ((j - (gridSize / 2)) * size)) , (float)(-size + ((i - (gridSize / 2)) * size)),-2.0,(float)(0.0 + ((j - (gridSize / 2)) * size)) });
            }
        }
    }
}

void Debug::debugControls(GLFWwindow* window, float deltaTime)
{
    //CONTROLS VARIOUS BEHAVIOURS
    if (glfwGetKey(window, GLFW_KEY_UP))
    {
        //y += 0.001f;
        yC += 10 * deltaTime;
        yVal += 0.2 * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN))
    {
        //y -= 0.001f;
        yC -= 10 * deltaTime;
        yVal -= 0.2 * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT))
    {
        // x += 0.001f;
        xC += 10 * deltaTime;
        xVal += 0.2 * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT))
    {
        // x -= 0.001f;
        xC -= 10 * deltaTime;
        xVal -= 0.2 * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_E))
    {
        // z += 0.001f;
        zC += 10 * deltaTime;
        zVal += 0.2 * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_F))
    {
        // z -= 0.001f;
        zC -= 10 * deltaTime;
        zVal -= 0.2 * deltaTime;
    }
}
void Debug::drawGrid(camera c)
{
    defaultShader.use();
    defaultShader.setVec4("outColor", glm::vec4(0.75f));
    defaultShader.setMat4("projection", c.projection);
    defaultShader.setMat4("view", c.view);
    defaultShader.setMat4("model", gridPos);
    gridPos = glm::translate(glm::mat4(1.0f), glm::vec3(((int)c.cameraPos.x / 4) * 4, -5.0, ((int)c.cameraPos.z / 4) * 4));

    if (lineVAO == 0)
    {
        glGenVertexArrays(1, &lineVAO);
        glGenBuffers(1, &lineVBO);
    }
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, grid.size() * sizeof(unsigned int), &grid[0], GL_STATIC_DRAW);
    glBindVertexArray(lineVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(lineVAO);

    glLineWidth(2);
    glDrawArrays(GL_LINES, 0, grid.size() / 3);
    glBindVertexArray(0);
}
void Debug::drawCollider(MeshCollider& collider, camera c)
{
    std::vector<float> colliderMesh;
    //DRAWS COLLIDER MESHES(USED FOR DEBUGGING)
    for (int i = 0; i < collider.vertices.size(); i++)
    {
        for (int j = i + 1; j < collider.vertices.size(); j++)
        {
            colliderMesh.push_back(collider.vertices[i].x);
            colliderMesh.push_back(collider.vertices[i].y);
            colliderMesh.push_back(collider.vertices[i].z);
            colliderMesh.push_back(collider.vertices[j].x);
            colliderMesh.push_back(collider.vertices[j].y);
            colliderMesh.push_back(collider.vertices[j].z);
        }
    }
    defaultShader.use();
    defaultShader.setVec4("outColor", glm::vec4(collider.color, 1.0));
    defaultShader.setMat4("projection", c.projection);
    defaultShader.setMat4("view", c.view);
    defaultShader.setMat4("model", glm::mat4(1.0f));

    if (lineVAO == 0)
    {
        glGenVertexArrays(1, &lineVAO);
        glGenBuffers(1, &lineVBO);
    }
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, colliderMesh.size() * sizeof(unsigned int), &colliderMesh[0], GL_STATIC_DRAW);
    glBindVertexArray(lineVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(lineVAO);

    glLineWidth(2);
    glDrawArrays(GL_LINES, 0, colliderMesh.size() / 3);
    glBindVertexArray(0);
}
void Debug::drawLine(camera c, glm::vec3 origin, glm::vec3 pos, glm::vec4 color)
{
    defaultShader.use();
    defaultShader.setVec4("outColor", color);
    defaultShader.setMat4("projection", c.projection);
    defaultShader.setMat4("view", c.view);
    defaultShader.setMat4("model", glm::mat4(1.0f));
    float vertices[] =
    {
    origin.x, origin.y, origin.z,
    pos.x, pos.y, pos.z
    };

    if (lineVAO == 0)
    {
        glGenVertexArrays(1, &lineVAO);
        glGenBuffers(1, &lineVBO);
    }
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(lineVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(lineVAO);

    glLineWidth(3.0);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}