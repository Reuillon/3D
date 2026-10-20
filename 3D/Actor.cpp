#include "Actor.h"
#include "Global.h"
#include "Debug.h"


Actor::Actor(short int animLength, std::string loadActorModel, std::string loadActorCollider, glm::vec3 setPosition, glm::vec3 setRotation)
{	
	Model tempModel(loadActorModel);
	actorModel = tempModel;
	actorCollider = initCollisionMap(loadActorCollider);
	actorPosition = setPosition;
	for (int i = 0; i < actorCollider.size(); ++i)
	{
		actorCollider[i].setTransform(actorPosition, glm::vec3(0.0, actorRotation.y, 0.0));
	}
	model = glm::mat4(1.0);
	model = glm::translate(model, actorPosition);
	model = glm::rotate(model, actorRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, actorRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, actorRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	inverseMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
	
	if (animLength > 0)
	{
		animation_size = animLength;
		actorAnimation = std::make_unique<Animation[]>(animLength);
		for (int i = 0; i < animLength; ++i)
		{
			Animation temp;
			temp.initialize(loadActorModel, &actorModel, i);
			actorAnimation[i].initialize(loadActorModel, &actorModel, i);
		}
		actorAnimate.initialize(&actorAnimation[0]);
		actorAnimate.UpdateAnimation();
	}
	actorAnimate.loopAnim(false);
};
void Actor::drawActor(camera& c, Shader& shader)
{
	for (int i = 0; i < transforms.size(); ++i)
	{
		shader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
	}
	shader.setMat4("projection", c.projection);
	shader.setMat4("view", c.view);
	shader.setMat4("model", model);
	shader.setMat3("normalMatrix", inverseMatrix);
	actorModel.draw(shader);
}

void Actor::updateActor()
{
	if (animation_size > 0)
	{
		
		if (resetAnimation == true)
		{
			actorAnimate.ResetAnim();
			resetAnimation = false;
		}
		actorAnimate.PlayAnimation(&actorAnimation[0]);
		if (actorIsPaused)
		{
			actorAnimate.PauseAnim();
		}
		else
		{
			actorAnimate.UpdateAnimation();
		}
		transforms = actorAnimate.GetFinalBoneMatrices();
	}
	
}

void Actor::PauseAnimation()
{
	actorIsPaused = true;
}

void Actor::PlayAnimation()
{
	actorIsPaused = false;
}

void Actor::setTransform(glm::vec3 position, glm::vec3 rotation)
{
	actorPosition = position;
	actorRotation = rotation;
	model = glm::mat4(1.0);
	for (int i = 0; i< actorCollider.size() ; ++i)
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

