//
// Created by 32725 on 2023/4/17.
//
#include "OpenGL/Renderer.h"
#include "Camera/Camera.h"
#include "OpenGL/FrameBuffer.h"

const float quadVertexes[]{
		-1.0f, -1.0f, .0f, 0.0f, 0.0f,
		1.0f, -1.0f, .0f, 1.0f, 0.0f,
		1.0f, 1.0f, .0f, 1.0f, 1.0f,
		-1.0f, 1.0f, .0f, 0.0f, 1.0f
};
const uint32_t quadIndices[]{
		0, 1, 2,
		2, 3, 0
};

int main() {
	Z::RenderSpec spec{};
	spec.width = 800;
	spec.height = 600;
	spec.title = "Ray Tracing in One Weekend";
	spec.maxBufferSize = 1000000;
	spec.vsync = false;
	Z::Renderer::Init(spec);
	Z::AttachmentSpec attachments{};
	attachments.attachments.push_back({GL_RGBA8, GL_COLOR_ATTACHMENT0});

	Z::VertexArray quad{};
	Z::BufferLayout layout{
			{{"a_Position", GL_FLOAT, 3, sizeof(float), false, 0},
			 {"Tex", GL_FLOAT, 2, sizeof(float), false, sizeof(float) * 3}}};
	quad.AddVertexBuffer(quadVertexes, sizeof(quadVertexes), layout);
	quad.SetIndexBuffer(quadIndices, sizeof(quadIndices));

	while (Z::Renderer::Running()) {
		Z::Renderer::SetClearValue({0.0f, 0.0f, 0.0f, 1.0f});
		Z::Renderer::SwapBuffers();
	}
	Z::Renderer::Shutdown();
}