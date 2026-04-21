#include "Level.h"


Level::Level(std::string mapModel, std::string mapCollision, glm::vec3 mapPos, glm::vec3 mapRot, glm::vec3 mapScale)
{
    Model tempMap(mapModel);
    map = tempMap;
    position = mapPos;
    rotation = mapRot;
    scale = mapScale;
	collisionMap = initCollisionMap(mapCollision);
    for (int i = 0; i < collisionMap.size(); i++)
    {
        collisionMap[i].setTransform(position, rotation, scale);
    }
    model = glm::scale(model, scale);
    model = glm::translate(model, position);
    model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    inverseMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
};

void Level::mapRender(camera& c, Shader& shader)
{
    shader.setMat4("projection", c.projection);
    shader.setMat4("view", c.view);
    shader.setMat4("model", model);
    shader.setMat3("normalMatrix", inverseMatrix);
    map.draw(shader);
}