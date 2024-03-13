//
// Created by 32725 on 2023/4/16.
//
#include "glm/glm/gtx/quaternion.hpp"
#include "Camera.h"
#include "OpenGL/Timer.h"
#include "glm/glm/gtc/matrix_transform.hpp"
//#include "glm/gtx/quaternion.hpp"

namespace Z {
	constexpr glm::vec3 g_Up=glm::vec3(0,1,0);
    constexpr float rotateScale=1E-2f;
	void Camera::Walk(GLFWwindow *window) {
		auto camera = (Camera *) glfwGetWindowUserPointer(window);
		while (!glfwWindowShouldClose(window)) {
			glm::vec3 vec{0, 0, 0};
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
				vec += camera->Front;
			} else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
				vec -= camera->Front;
			}
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
				vec -= camera->Right;
			} else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
				vec += camera->Right;
			}
			if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
				vec += glm::vec3(0, 1, 0);
			} else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
				vec += glm::vec3(0, -1, 0);
			}
			if (vec != glm::vec3(0, 0, 0)) {
				vec *= Timer::GetDeltaTime() * 5E-4;
				camera->position += vec;
				camera->focus += vec;
				camera->CalculateMatrix();
			}
		}
	}

	void Camera::Turn(GLFWwindow *window, double x, double y) {
		float dx = -(x - lastX)*rotateScale;
		float dy = -(y - lastY)*rotateScale;
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
			Front = focus - position;
            auto rotation=glm::angleAxis(dx,g_Up)*glm::angleAxis(dy,Right);
			Front = glm::normalize(rotation*Front);
            Up=rotation*Up;
            Right=glm::normalize(glm::cross(Front,Up));
			position = focus - Front * distance;
			CalculateMatrix();
		}
		lastX = x;
		lastY = y;
	}

	glm::mat4 Camera::GetVPMatrix() const {
		return projectionMatrix * viewMatrix;
	}

	void Camera::ReSize(GLFWwindow *window, int width, int height) {
		auto camera = (Camera *) glfwGetWindowUserPointer(window);
		camera->aspect = (float) width / height;
		camera->Resize = true;
		camera->width = width;
		camera->height = height;
		camera->CalculateMatrix();
	}

	void Camera::CalculateMatrix() {
		projectionMatrix = glm::perspective(glm::radians(fov), aspect, near, far);
		viewMatrix = glm::lookAt(position, focus, Up);
	}

	void Camera::Scroll(GLFWwindow *, double x, double y) {
		distance += y * Timer::GetDeltaTime()*20.f;
		glm::vec3 vec = glm::normalize(Front);
		position = focus - vec * distance;
		CalculateMatrix();
	}

}
