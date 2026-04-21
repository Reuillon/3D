#include "Actor.h"
#include "Debug.h"


Actor::Actor(std::string loadActorModel, std::string loadActorCollider, glm::vec3 setPosition, glm::vec3 setRotation)
{	
	Model tempModel(loadActorModel);
	actorModel = tempModel;
	actorCollider = initCollisionMap(loadActorCollider);
	actorPosition = setPosition;
	for (int i = 0; i < actorCollider.size(); i++)
	{
		actorCollider[i].setTransform(actorPosition, glm::vec3(0.0, actorRotation.y, 0.0));
	}
	model = glm::mat4(1.0);
	model = glm::translate(model, actorPosition);
	model = glm::rotate(model, actorRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, actorRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, actorRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
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

void Actor::setTransform(glm::vec3 position, glm::vec3 rotation)
{
	actorPosition = position;
	actorRotation = rotation;
	model = glm::mat4(1.0);
	for (int i = 0; i< actorCollider.size() ;i++)
	{
		actorCollider[i].setTransform(actorPosition, actorRotation);
	}
	model = glm::translate(model, actorPosition);
	model = glm::rotate(model, actorRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, actorRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, actorRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	
	inverseMatrix = glm::mat3(1.0);
	inverseMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
}

