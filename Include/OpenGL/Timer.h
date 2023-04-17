//
// Created by 32725 on 2023/4/16.
//

#ifndef RENDERER003_OGL_TIMER_H
#define RENDERER003_OGL_TIMER_H
#include "GLFW/glfw3.h"

namespace Z {
	struct Timer {
		static float lastTime,deltaTime,lastFlush;
		static void Update();
		static float GetDeltaTime();
		static void Flush(){
			lastFlush = glfwGetTime();
		}
		static float GetFlushTime(){
			return glfwGetTime() - lastFlush;
		}
	};

}


#endif //RENDERER003_OGL_TIMER_H
