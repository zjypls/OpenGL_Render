//
// Created by 32725 on 2023/4/17.
//
#include "imgui/imgui.h"
#include "Guizmo.h"
#include "ImGuizmo/ImGuizmo.h"
#include "OpenGL/Renderer.h"
#include "glm/glm/gtc/type_ptr.hpp"

namespace Z {
	static ImGuizmo::OPERATION currentOperation = ImGuizmo::TRANSLATE;

	void Guizmo::CheckOperation() {
		auto window = Renderer::GetWindow();
		if(glfwGetKey(window,GLFW_KEY_Y)==GLFW_PRESS)
			currentOperation = ImGuizmo::TRANSLATE;
		else if(glfwGetKey(window,GLFW_KEY_R)==GLFW_PRESS)
			currentOperation = ImGuizmo::ROTATE;
		else if(glfwGetKey(window,GLFW_KEY_S)==GLFW_PRESS)
			currentOperation = ImGuizmo::SCALE;
	}

	void
	Guizmo::DrawGuizmo(glm::mat4 &cameraView, glm::mat4 &cameraProjection, glm::mat4 &transform) {
		ImGuizmo::BeginFrame();
		auto window = Renderer::GetWindow();
		int width, height,x,y;
		glfwGetWindowSize(window, &width, &height);
		glfwGetWindowPos(window,&x,&y);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(x, y, width, height);
		ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), currentOperation, ImGuizmo::LOCAL,
							 glm::value_ptr(transform));
	}
}