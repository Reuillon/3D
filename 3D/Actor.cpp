#include "Actor.h"

Actor::Actor(std::string loadActorModel, std::string loadActorCollider, glm::vec3 setPosition, glm::vec3 setRotation)
{	
	Model tempModel(loadActorModel);
	actorModel = tempModel;
	actorCollider = initCollisionMap(loadActorCollider);
	actorPosition = setPosition;
	actorCollider[0].setTransform(glm::vec3(actorPosition.x - 1000.0f, actorPosition.y - 1000.0f, actorPosition.z - 1000.0f), glm::vec3(0.0));
	actorRotation = setRotation;
	model = glm::translate(model, glm::vec3(actorPosition.x, actorPosition.y, actorPosition.z));
	inverseMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
};
void Actor::drawActor(camera& c, Shader& shader)
{
	shader.setMat4("projection", c.projection);
	shader.setMat4("view", c.view);
	shader.setMat4("model", model);
	shader.setMat3("normalMatrix", inverseMatrix);
	actorModel.draw(shader);
}
void Actor::setPosition(glm::vec3 position)
{
	actorPosition = position;
	model = glm::mat4(1.0);
	actorCollider[0].setTransform(glm::vec3(actorPosition.x - 1000.0f, actorPosition.y - 1000.0f, actorPosition.z - 1000.0f), glm::vec3(0.0));
	model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
	inverseMatrix = glm::mat3(1.0);
	inverseMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
	
}