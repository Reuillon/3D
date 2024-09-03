#ifndef ANIMATOR_H
#define ANIMATOR_H
#include "glm/glm.hpp"
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include "Animation.h"
#include "Bone.h"

class Animator
{
	public:
		Animator(Animation* animation);
		void UpdateAnimation(float dt);
		void PlayAnimation(Animation* pAnimation);
		void loopAnim(bool loop);
		void PauseAnim();
		void ResetAnim();
		float getFrameTime();
		bool finishedAnim();
		void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
		std::vector<glm::mat4> GetFinalBoneMatrices();
	private:
		bool isLoop = false;
		bool hasEnded;
		float fTime = 0;
		std::vector<glm::mat4> m_FinalBoneMatrices;
		Animation* m_CurrentAnimation;
		float m_CurrentTime;
		float m_DeltaTime;
};
#endif
