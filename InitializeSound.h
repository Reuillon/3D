#pragma once
#include <AL\alc.h>
class InitializeSound
{
	public:
		static InitializeSound* get();
	private:
		InitializeSound();
		~InitializeSound();

		ALCdevice* p_ALCDevice;
		ALCcontext* p_ALCContext;
};

