#include "Level.h"


Level::Level(std::string mapModel, std::string mapCollision, glm::vec3 mapPos, glm::vec3 mapRot, glm::vec3 mapScale)
{
    Model tempMap(mapModel);
    map = tempMap;
    position = mapPos;
    rotation = mapRot;
    scale = mapScale;
	collisionMap = initCollisionMap(mapCollision);
    model = glm::scale(model, scale);
    model = glm::translate(model, glm::vec3(position.x, position.y, position.z));

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