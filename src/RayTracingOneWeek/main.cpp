//
// Created by 32725 on 2023/4/17.
//

#include "OpenGL/Renderer.h"
#include "Camera/Camera.h"
#include "OpenGL/FrameBuffer.h"
#include "OpenGL/MyImGui.h"
#include "OpenGL/Timer.h"
#include "OpenGL/UniformBuffer.h"

struct CameraData {
	glm::vec4 horizon;
	glm::vec4 vertical;
	glm::vec4 origin;
	glm::vec4 direction;
};

Z::Camera camera{{0, 0, 0}, {0, 0, -1}, 90, 800.0f / 600.0f, 0.1f, 100.0f, 1};
glm::vec2 g_viewportSize{800, 600};


int main() {
	CameraData cameraData{
			glm::vec4{0, 0, -1, 0} * (g_viewportSize.x / 200.f),
			glm::vec4{0, 1, 0, 0} * (g_viewportSize.y / 200.f),
			{-1, 0, 0, 0},
			{1,  0, 0, 1}};
	Z::RenderSpec spec{};
	spec.title = "Ray Tracing in One Weekend";
	Z::Renderer::Init(spec);
	Z::MyImGui::Init();
	auto RayShader = Z::Shader{};
	RayShader.AddShader("RayTracingOneWeek/Quad.vert", GL_VERTEX_SHADER);
	RayShader.AddShader("RayTracingOneWeek/Tracing.frag", GL_FRAGMENT_SHADER);
	RayShader.Link();
	Z::AttachmentSpec attachments{};
	attachments.attachments.push_back({GL_RGBA8, GL_COLOR_ATTACHMENT0});

	auto QuadArray = Z::Renderer::GetQuadVertexArray();
	auto attachmentsSpec = Z::AttachmentSpec();
	attachmentsSpec.attachments.push_back({GL_RGBA8, GL_COLOR_ATTACHMENT0});
	auto beforeFrame = Z::FrameBuffer(attachmentsSpec);
	auto cameraDataBuffer = Z::UniformBuffer(&cameraData, sizeof(CameraData));


	while (Z::Renderer::Running()) {
		Z::Timer::Update();
		Z::Renderer::SetClearValue({0.0f, 0.0f, 0.0f, 1.0f});
		Z::MyImGui::Begin();
		beforeFrame.Bind();
		cameraDataBuffer.Bind(0);

		RayShader.Bind();
		QuadArray->Draw();
		beforeFrame.Unbind();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGuiViewport *viewport = ImGui::GetMainViewport();
		Z::MyImGui::BeginDockSpace(viewport);
		ImGui::Begin("##viewport", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
		ImGui::Image((void *) beforeFrame.GetAttachment(0), ImVec2(g_viewportSize.x, g_viewportSize.y), ImVec2(0, 1), ImVec2(1, 0));
		if (auto viewSize = ImGui::GetWindowSize();viewSize.x != g_viewportSize.x || viewSize.y != g_viewportSize.y) {
			g_viewportSize = {viewSize.x, viewSize.y};
			beforeFrame.Resize(g_viewportSize.x, g_viewportSize.y);
			cameraData.horizon = {0, 0, -1, g_viewportSize.x / 200.f};
			cameraData.vertical = {0, 1, 0, g_viewportSize.y / 200.f};
			camera.width = g_viewportSize.x;
			camera.height = g_viewportSize.y;
			camera.aspect = g_viewportSize.x / g_viewportSize.y;
			cameraDataBuffer.SetData(&cameraData, sizeof(CameraData));
		}
		ImGui::End();
		ImGui::PopStyleVar(3);


		Z::MyImGui::EndDockSpace();
		Z::MyImGui::End();

		Z::Renderer::SwapBuffers();
	}
	Z::Renderer::Shutdown();
}