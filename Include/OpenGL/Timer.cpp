//
// Created by 32725 on 2023/4/16.
//

#include "Timer.h"
namespace Z{
	float Timer::lastTime = 0;
	float Timer::deltaTime = 1E-2;
	float Timer::lastFlush = 0;
	void Timer::Update() {
		float time = glfwGetTime();
		deltaTime = time - lastTime;
		lastTime = time;
	}

	float Timer::GetDeltaTime() {
		return deltaTime;
	}

	float Timer::GetFPS() {
		static float fps = 1.f/deltaTime;
		if(lastTime - lastFlush > 1.f){
			fps = 1.f/deltaTime;
			lastFlush = lastTime;
		}
		return fps;
	}
}