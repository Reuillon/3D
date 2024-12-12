#include "Animator.h"

Animator::Animator(Animation* animation) 
{
	m_CurrentTime = 0.0;
	m_CurrentAnimation = animation;
	isLoop = true;
	hasEnded = false;
	m_FinalBoneMatrices.reserve(100);

	for (int i = 0; i < 100; i++)
		m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
}

Animator::Animator(){}

void Animator::initialize(Animation* animation)
{
	m_CurrentTime = 0.0;
	m_CurrentAnimation = animation;
	isLoop = true;
	hasEnded = false;
	m_FinalBoneMatrices.reserve(100);

	for (int i = 0; i < 100; i++)
		m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
}


void Animator::loopAnim(bool loop)
{
	isLoop = loop;
}

float timerA = 0;

void Animator::UpdateAnimation(float dt) 
{
	m_DeltaTime = dt;
	if (m_CurrentAnimation)
	{
		if (isLoop)
		{
			hasEnded = false;
			m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
		}
		else
		{

			if (fTime <= (m_CurrentAnimation->GetDuration() - (1.0f)))
			{
				m_CurrentTime = (fTime += m_CurrentAnimation->GetTicksPerSecond() * dt);
				hasEnded = false;
			}
			else
			{
				hasEnded = true;
			}
		}
		m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
		CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
	}
}

bool Animator::finishedAnim()
{
	return hasEnded;
}

float Animator::getFrameTime()
{
	return m_CurrentTime;
}


void Animator::PauseAnim()
{

	if (m_CurrentAnimation)
	{
		m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
		CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
	}
}
void Animator::ResetAnim()
{

	if (m_CurrentAnimation)
	{
		fTime = 0;
		m_CurrentTime = 0;
		CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
	}
}

void Animator::PlayAnimation(Animation* pAnimation) 
{
	m_CurrentAnimation = pAnimation;
	
}


void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform) 
{
	std::string nodeName = node->name;
	glm::mat4 nodeTransform = node->transformation;

	Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

	if (Bone)
	{
		Bone->update(m_CurrentTime);
		nodeTransform = Bone->GetLocalTransform();
	}

	glm::mat4 globalTransformation = parentTransform * nodeTransform;

	auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int index = boneInfoMap[nodeName].id;
		glm::mat4 offset = boneInfoMap[nodeName].offset;
		m_FinalBoneMatrices[index] = globalTransformation * offset;
	}

	for (int i = 0; i < node->childrenCount; i++)
		CalculateBoneTransform(&node->children[i], globalTransformation);
}


std::vector<glm::mat4> Animator::GetFinalBoneMatrices() 
{
	return m_FinalBoneMatrices;
}