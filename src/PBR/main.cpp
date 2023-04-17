#include "OpenGL/Renderer.h"
#include "OpenGL/Texture.h"
#include "OpenGL/FrameBuffer.h"
#include "OpenGL/Shader.h"
#include "OpenGL/VertexArray.h"
#include "Model/Model.h"
#include "Camera/Camera.h"
#include "OpenGL/UniformBuffer.h"
#include "OpenGL/Timer.h"
#include "OpenGL/MyImGui.h"
#include <Windows.h>
#include <thread>
#include <iostream>

extern "C"
{
__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

const float quad[20]{
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f
};
const uint32_t indices[6]{
		0, 1, 2,
		2, 3, 0
};

Z::Camera camera{
		{1.5, -1.3, .6},
		{0, 0, 0},
		45.f, 8.f / 6.f, 0.1f, 100.f, glm::sqrt(4.2f), 0, 0
};
struct LightData {
	glm::vec4 cameraPos;
	glm::vec4 lightPos[5], lightCol[5];
};

int main() {
	LightData lightData{
			{1.5,               -1.3,              0.6,              0},
			{{1,  1,  0, 0}, {-10, -10, 10, 0}, {-10, 10, -10, 0}, {-10, -10, 10, 0}, {10,  -10, 10,  0}},
			{{17, 17, 17,  1}, {0,  0, 0, 1}, {0,  0, 0, 1}, {0,  0,  0, 1}, {0, 0, 0, 1}}};
	Z::RenderSpec spec;
	spec.width = 800;
	spec.height = 600;
	spec.title = "PBR";
	spec.maxBufferSize = 1024;
	spec.fullScreen = false;
	spec.vsync = false;

	Z::Renderer::Init(spec);
	auto window = Z::Renderer::GetWindow();
	glfwSetWindowUserPointer(window, &camera);
	glfwGetCursorPos(window, &camera.lastX, &camera.lastY);
	glfwSetCursorPosCallback(window, [](GLFWwindow *w, double x, double y) {
		camera.Turn(w, x, y);
	});
	glfwSetFramebufferSizeCallback(window, Z::Camera::ReSize);
	Z::MyImGui::Init();

	auto vb = std::make_shared<Z::VertexBuffer>(quad, sizeof(quad));
	Z::BufferLayout layout{{Z::BufferLayout::Element{"pos", GL_FLOAT, 3, sizeof(float), false, 0},
	                        {"tex", GL_FLOAT, 2, sizeof(uint32_t), false, 0}}};
	vb->SetLayout(layout);
	auto ib = std::make_shared<Z::IndexBuffer>(indices, sizeof(indices));
	Z::VertexArray va;
	va.AddVertexBuffer(vb);
	va.SetIndexBuffer(ib);

	Z::Shader lighter{}, sample{};
	lighter.AddShader("PBR/vertex001.glsl", GL_VERTEX_SHADER);
	lighter.AddShader("PBR/fragment001.glsl", GL_FRAGMENT_SHADER);
	lighter.Link();
	sample.AddShader("PBR/vertex002.glsl", GL_VERTEX_SHADER);
	sample.AddShader("PBR/fragment002.glsl", GL_FRAGMENT_SHADER);
	sample.Link();

	Z::UniformBuffer ubo{sizeof(glm::mat4)}, fbo{&lightData, sizeof(LightData)};
	Z::AttachmentSpec attachments;
	attachments.width = 800;
	attachments.height = 600;
	attachments.attachments.push_back({GL_RGBA8, GL_COLOR_ATTACHMENT0});
	attachments.attachments.push_back({GL_RGB16, GL_COLOR_ATTACHMENT1});
	attachments.attachments.push_back({GL_RGB32F, GL_COLOR_ATTACHMENT2});
	attachments.attachments.push_back({GL_R16F, GL_COLOR_ATTACHMENT3});
	attachments.attachments.push_back({GL_R16F, GL_COLOR_ATTACHMENT4});
	attachments.attachments.push_back({GL_R16F, GL_COLOR_ATTACHMENT5});
	attachments.attachments.push_back({GL_R16F, GL_COLOR_ATTACHMENT6});
	attachments.attachments.push_back({GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT});
	auto fb = Z::FrameBuffer(attachments);

	Z::Model model{"lighter/lighter.fbx"};

	glEnable(GL_DEPTH_TEST);
	auto walkControl = std::thread{Z::Camera::Walk, window};
	while (Z::Renderer::Running()) {
		Z::MyImGui::Begin();
		Z::Timer::Update();
		auto vp = camera.GetVPMatrix();
		ubo.SetData(&vp, sizeof(glm::mat4));
		ubo.Bind(7);
		lightData.cameraPos = glm::vec4(camera.position, 0.f);
		fbo.SetData(&lightData, sizeof(LightData));
		fbo.Bind(12);
		fb.Bind();
		Z::Renderer::SetClearValue({0.2f, 0.3f, 0.3f, 1.0f});
		Z::Renderer::Draw(model, lighter);
		fb.Unbind();

		Z::Renderer::SetClearValue({0.f, 0.f, 0.f, 1.0f});
		sample.Bind();
		for (int i = 0; i < 7; i++)
			glBindTextureUnit(i, fb.GetAttachment(i));

		va.Draw();

		ImGui::Begin("Status");
		static float frame = 1 / Z::Timer::GetDeltaTime();
		if (Z::Timer::GetFlushTime() > 1.f) {
			frame = 1 / Z::Timer::GetDeltaTime();
			Z::Timer::Flush();
		}
		static int lightIndex = 0;
		ImGui::Text("FPS: %.2f", frame);
		ImGui::Text("CameraPos: %.2f, %.2f, %.2f", camera.position.x, camera.position.y, camera.position.z);
		ImGui::Combo("Light", &lightIndex, "Light0\0Light1\0Light2\0Light3\0Light4\0");
		ImGui::DragFloat4("LightPos", &lightData.lightPos[lightIndex][0]);
		ImGui::DragFloat4("LightCol", &lightData.lightCol[lightIndex][0]);

		ImGui::End();
		Z::MyImGui::End();

		Z::Renderer::SwapBuffers();
		if (camera.Resize) {
			camera.Resize = false;
			fb.Resize(camera.width, camera.height);
		}
	}
	if (walkControl.joinable())
		walkControl.join();
	Z::MyImGui::Shutdown();
	Z::Renderer::Shutdown();
}
