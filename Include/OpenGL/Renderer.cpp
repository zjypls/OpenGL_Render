//
// Created by 32725 on 2023/4/10.
//
#include "Renderer.h"
#include "VertexArray.h"
#include <cassert>

const float QuadVertexes[]{
		-1.0f, -1.0f, .0f, 0.0f, 0.0f,
		1.0f, -1.0f, .0f, 1.0f, 0.0f,
		1.0f, 1.0f, .0f, 1.0f, 1.0f,
		-1.0f, 1.0f, .0f, 0.0f, 1.0f
};
const uint32_t QuadIndices[]{
		0, 1, 2,
		2, 3, 0
};

Z::BufferLayout QuadDefaultLayout{{Z::BufferLayout::Element{"pos", GL_FLOAT, 3, sizeof(float), false, 0},
                                   {"tex", GL_FLOAT, 2, sizeof(uint32_t), false, 0}}};

namespace Z {

	GLFWwindow *Renderer::window = nullptr;
	RenderSpec Renderer::Spec;

	void Renderer::Init(const RenderSpec &spec) {
		Spec = spec;
		int i = glfwInit();
		assert(i);
		window = glfwCreateWindow(spec.width, spec.height, spec.title,
		                          spec.fullScreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
		assert(window);
		glfwMakeContextCurrent(window);
		int j = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
		assert(j);
		glfwSwapInterval(spec.vsync);
		glEnable(GL_DEPTH_TEST);
	}

	glm::vec2 Renderer::GetWindowSize() {
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		return glm::vec2(width, height);
	}

	std::shared_ptr<VertexArray> Renderer::GetQuadVertexArray() {
		auto vertexBuffer = std::make_shared<VertexBuffer>(QuadVertexes, sizeof(float)*5, sizeof(QuadVertexes) / sizeof(float)/5);
		vertexBuffer->SetLayout(QuadDefaultLayout);
		auto indexBuffer = std::make_shared<IndexBuffer>(QuadIndices, sizeof(QuadIndices));
		return std::make_shared<VertexArray>(vertexBuffer,indexBuffer);
	}

	bool Renderer::Running() {
		return !glfwWindowShouldClose(window);
	}

	void Renderer::BindDefaultFrameBuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}

	void Renderer::Shutdown(){
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void Renderer::SwapBuffers()  { glfwSwapBuffers(window);glfwPollEvents(); }

	void Renderer::SetClearValue(const vec4 &value) {
		glClearColor(value.r,value.g,value.b,value.a);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	}
}
