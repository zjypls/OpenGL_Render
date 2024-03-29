//
// Created by 32725 on 2023/4/16.
//

#ifndef RENDERER003_OGL_CAMERA_H
#define RENDERER003_OGL_CAMERA_H
#include "glm/glm/glm.hpp"
#include "GLFW/glfw3.h"
#include "glm/glm/gtc/matrix_transform.hpp"
namespace Z {
	struct Camera {
		glm::vec3 position,focus;
		float fov=60,aspect=1,near=0.1,far=1000,distance=3;
		double lastX,lastY;

		glm::vec3 Right{-1,0,0},Up=glm::vec3(0,1,0),Front=glm::vec3(-1,-1,-1);
		bool Resize=false;
		unsigned int width=800,height=600;
		glm::mat4 viewMatrix=glm::lookAt(position, focus, glm::vec3(0, 1, 0));
		glm::mat4 projectionMatrix=glm::perspective(glm::radians(fov), aspect, near, far);

		static void Walk(GLFWwindow*);
		void Turn(GLFWwindow*,double x,double y);
		static void ReSize(GLFWwindow*,int width,int height);
		void Scroll(GLFWwindow*,double x,double y);
		glm::mat4 GetVPMatrix() const;
		void CalculateMatrix();

	};

}

#endif //RENDERER003_OGL_CAMERA_H
