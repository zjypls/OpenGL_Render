//
// Created by 32725 on 2023/4/16.
//

#include "glad/glad/glad.h"
#include "Camera.h"
#include "OpenGL/Timer.h"
#include "glm/glm/gtc/matrix_transform.hpp"

namespace Z {
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
		float dx = x - lastX;
		float dy = y - lastY;
		//ToDo:change button 4 toRight
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
			Front = focus - position;
			Right = glm::normalize(glm::cross(Front, glm::vec3(0, 1, 0)));
			glm::vec3 up = glm::cross(Right, Front);
			glm::mat4 mat = glm::mat4(1);
			mat = glm::rotate(mat, glm::radians(-dx), up);
			mat = glm::rotate(mat, glm::radians(-dy), Right);
			Front = glm::normalize(glm::vec3(mat * glm::vec4(Front, 1)));
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
		glViewport(0, 0, width, height);
		camera->Resize = true;
		camera->width = width;
		camera->height = height;
		camera->CalculateMatrix();
	}

	void Camera::CalculateMatrix() {
		projectionMatrix = glm::perspective(glm::radians(fov), aspect, near, far);
		viewMatrix = glm::lookAt(position, focus, glm::vec3(0, 1, 0));
	}

	void Camera::Scroll(GLFWwindow *, double x, double y) {
		distance += y * Timer::GetDeltaTime()*20.f;
		glm::vec3 vec = glm::normalize(Front);
		position = focus - vec * distance;
		CalculateMatrix();
	}

}
