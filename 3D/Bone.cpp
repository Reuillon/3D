#include "Bone.h"

void Bone::update(float animationTime) 
{
	glm::mat4 translation = InterpolatePosition(animationTime);
	glm::mat4 rotation = InterpolateRotation(animationTime);
	glm::mat4 scale = InterpolateScale(animationTime);
	m_LocalTransform = translation * rotation * scale;
}

glm::mat4 Bone::GetLocalTransform() 
{
	return m_LocalTransform;
}
std::string Bone::GetBoneName() const
{
	return m_Name;
}
int Bone::GetBoneID() 
{
	return m_ID;
}

int Bone::GetPositionIndex(float animationTime) 
{
	for (int index = 0; index < m_NumPositions - 1; ++index)
	{
		if (animationTime < m_Positions[index + 1].timeStamp)
			return index;
	}
	assert(0);
}

int Bone::GetRotationIndex(float animationTime) 
{
	for (int index = 0; index < m_NumRotations - 1; ++index)
	{
		if (animationTime < m_Rotations[index + 1].timeStamp)
			return index;
	}
	assert(0);
}
int Bone::GetScaleIndex(float animationTime) 
{
	for (int index = 0; index < m_NumScalings - 1; ++index)
	{
		if (animationTime < m_Scales[index + 1].timeStamp)
			return index;
	}
	assert(0);
}

float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) 
{
	float scaleFactor = 0.0f;
	float midWayLength = animationTime - lastTimeStamp;
	float framesDiff = nextTimeStamp - lastTimeStamp;
	scaleFactor = midWayLength / framesDiff;
	return scaleFactor;
}
glm::mat4 Bone::InterpolatePosition(float animationTime) 
{
	if (1 == m_NumPositions)
		return glm::translate(glm::mat4(1.0f), m_Positions[0].position);

	int p0Index = GetPositionIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp,
		m_Positions[p1Index].timeStamp, animationTime);
	glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].position, m_Positions[p1Index].position
		, scaleFactor);
	return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Bone::InterpolateRotation(float animationTime) 
{
	if (1 == m_NumRotations)
	{
		auto rotation = glm::normalize(m_Rotations[0].orientation);
		return glm::toMat4(rotation);
	}

	int p0Index = GetRotationIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = GetScaleFactor(m_Rotations[p0Index].timeStamp,
		m_Rotations[p1Index].timeStamp, animationTime);
	glm::quat finalRotation = glm::slerp(m_Rotations[p0Index].orientation, m_Rotations[p1Index].orientation
		, scaleFactor);
	finalRotation = glm::normalize(finalRotation);
	return glm::toMat4(finalRotation);
}
glm::mat4 Bone::InterpolateScale(float animationTime) 
{
	if (1 == m_NumScalings)
	{
		return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);
	}
	int p0Index = GetScaleIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = GetScaleFactor(m_Scales[p0Index].timeStamp,	m_Scales[p1Index].timeStamp, animationTime);
	glm::vec3 finalScale = glm::mix(m_Scales[p0Index].scale, m_Scales[p1Index].scale, scaleFactor);
	return glm::scale(glm::mat4(1.0f), finalScale);
}