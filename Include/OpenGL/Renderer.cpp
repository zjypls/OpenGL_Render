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

	struct Triangle {
		vec3 p1;
		vec3 p2;
		vec3 p3;
	};
	struct RenderData {
		Triangle *RenderBuffer;
		Triangle *currentPtr;
	};

	GLFWwindow *Renderer::window = nullptr;
	RenderSpec Renderer::Spec;
	static RenderData *data = nullptr;

	void Renderer::Init(const RenderSpec &spec) {
		data = new RenderData();
		Spec = spec;
		data->RenderBuffer = new Triangle[Spec.maxBufferSize];
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
		auto vertexBuffer = std::make_shared<VertexBuffer>(QuadVertexes, sizeof(QuadVertexes));
		vertexBuffer->SetLayout(QuadDefaultLayout);
		auto indexBuffer = std::make_shared<IndexBuffer>(QuadIndices, sizeof(QuadIndices));
		return std::make_shared<VertexArray>(std::initializer_list<std::shared_ptr<VertexBuffer>>{vertexBuffer}, indexBuffer);
	}

}
