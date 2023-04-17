//
// Created by 32725 on 2023/4/16.
//

#ifndef RENDERER003_OGL_CAMERA_H
#define RENDERER003_OGL_CAMERA_H
#include "glm/glm/glm.hpp"
#include "GLFW/glfw3.h"
namespace Z {
	struct Camera {
		glm::vec3 position,focus;
		float fov,aspect,near,far,distance;
		double lastX,lastY;

		glm::vec3 Right,Up=glm::vec3(0,1,0),Front=glm::vec3(-1,-1,-1);
		bool Resize=false;
		unsigned int width=800,height=600;

		static void Walk(GLFWwindow*);
		void Turn(GLFWwindow*,double x,double y);
		static void ReSize(GLFWwindow*,int width,int height);
		glm::mat4 GetVPMatrix() const;

	};

}

#endif //RENDERER003_OGL_CAMERA_H
