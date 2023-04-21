#include "OpenGL/Renderer.h"
#include "OpenGL/FrameBuffer.h"
#include "OpenGL/Shader.h"
#include "OpenGL/VertexArray.h"
#include "Model/Model.h"
#include "Camera/Camera.h"
#include "OpenGL/UniformBuffer.h"
#include "OpenGL/Timer.h"
#include "OpenGL/MyImGui.h"
#include "OpenGL/Guizmo.h"
#include <thread>
#include <iostream>

extern "C"
{
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
//__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
Z::Camera camera{
		{1.5, -1.3, .6},
		{0, 0, 0},
		45.f, 8.f / 6.f, 0.1f, 100.f, glm::sqrt(4.2f), 0, 0
};
struct LightData {
	//use vec4 to align for glsl
	glm::vec4 cameraPos;
	glm::vec4 lightPos[5], lightCol[5];
};
struct PostProcessData {
	glm::vec2 viewportSize;
};
glm::vec2 g_viewportSize{800, 600};

ImVec2 operator-(const ImVec2 &a, const ImVec2 &b) {
	return {a.x - b.x, a.y - b.y};
}

int main() {
	camera.Front = camera.focus - camera.position;
	camera.Right = glm::normalize(glm::cross(camera.Front, glm::vec3(0, 1, 0)));
	LightData lightData{
			{1.5,             -1.3,              0.6,               1.5},
			{{1,  1,  0,  0}, {-10, -10, 10, 0}, {-10, 10, -10, 0}, {-10, -10, 10, 0}, {10, -10, 10, 0}},
			{{17, 17, 17, 1}, {0,   0,   0,  1}, {0,   0,  0,   1}, {0,   0,   0,  1}, {0,  0,   0,  1}}};
	PostProcessData postProcessData{{800, 600}};
	Z::RenderSpec spec{};
	spec.title = "PBR";

	Z::Renderer::Init(spec);
	auto window = Z::Renderer::GetWindow();
	glfwSetWindowUserPointer(window, &camera);
	glfwGetCursorPos(window, &camera.lastX, &camera.lastY);
	glfwSetCursorPosCallback(window, [](GLFWwindow *w, double x, double y) {
		camera.Turn(w, x, y);
	});
	glfwSetScrollCallback(window, [](GLFWwindow *w, double x, double y) {
		camera.Scroll(w, x, y);
	});
	Z::MyImGui::Init();

	auto QuadArray = Z::Renderer::GetQuadVertexArray();
	auto SamplerShader = Z::Shader::Create({"PBR/Sampler.vert", "PBR/Sampler.frag"});
	auto LightComputeShader = Z::Shader::Create({"PBR/Light.vert", "PBR/Light.frag"});
	auto PostProcessShader = Z::Shader::Create({"PBR/PostProcess.vert", "PBR/PostProcess.frag"});


	Z::UniformBuffer CameraDataUniformBuffer{sizeof(glm::mat4)}, LightDataUniformBuffer{&lightData, sizeof(LightData)};
	Z::UniformBuffer postProcessUbo{&postProcessData, sizeof(PostProcessData)};
	Z::AttachmentSpec attachmentsSpec;
	attachmentsSpec.width = 800;
	attachmentsSpec.height = 600;
	attachmentsSpec.attachments.push_back({GL_RGBA8, GL_COLOR_ATTACHMENT0});
	auto postProcessFrame = Z::FrameBuffer(attachmentsSpec);
	attachmentsSpec.attachments.pop_back();
	attachmentsSpec.attachments.push_back({GL_RGB32F, GL_COLOR_ATTACHMENT0});
	attachmentsSpec.attachments.push_back({GL_RGB32F, GL_COLOR_ATTACHMENT1});
	auto LightViewFrame = Z::FrameBuffer(attachmentsSpec);
	attachmentsSpec.attachments.pop_back();
	attachmentsSpec.attachments.push_back({GL_RGB16, GL_COLOR_ATTACHMENT1});
	attachmentsSpec.attachments.push_back({GL_RGB32F, GL_COLOR_ATTACHMENT2});
	attachmentsSpec.attachments.push_back({GL_R16F, GL_COLOR_ATTACHMENT3});
	attachmentsSpec.attachments.push_back({GL_R16F, GL_COLOR_ATTACHMENT4});
	attachmentsSpec.attachments.push_back({GL_R16F, GL_COLOR_ATTACHMENT5});
	attachmentsSpec.attachments.push_back({GL_R16F, GL_COLOR_ATTACHMENT6});
	attachmentsSpec.attachments.push_back({GL_RG32I, GL_COLOR_ATTACHMENT7});
	attachmentsSpec.attachments.push_back({GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT});
	auto firstSampleFrame = Z::FrameBuffer(attachmentsSpec);

	std::vector<std::shared_ptr<Z::Model>> models;
	models.emplace_back(std::make_shared<Z::Model>("lighter/lighter.fbx"));

	auto walkControl = std::thread{Z::Camera::Walk, window};
	static bool viewPortHovered = false, viewPortFocused = false;
	static ImVec2 CursorPos{};
	static glm::ivec2 ModelIndex{-1, -1};
	static int CurrentLightIndex = 0;
	while (Z::Renderer::Running()) {
		Z::Timer::Update();
		Z::MyImGui::Begin();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		Z::MyImGui::BeginDockSpace(viewport);

		auto ViewProjectionMat = camera.GetVPMatrix();
		CameraDataUniformBuffer.SetData(&ViewProjectionMat, sizeof(glm::mat4));
		CameraDataUniformBuffer.Bind(7);
		postProcessUbo.SetData(&postProcessData, sizeof(PostProcessData));
		postProcessUbo.Bind(8);
		lightData.cameraPos = glm::vec4(camera.position, lightData.cameraPos.w);
		LightDataUniformBuffer.SetData(&lightData, sizeof(LightData));
		LightDataUniformBuffer.Bind(12);
		firstSampleFrame.Bind();
		Z::Renderer::SetClearValue({0.1f, 0.1f, 0.1f, .0f});
		firstSampleFrame.ClearAttachment(7, glm::ivec2{-1, -1});
		for (auto &model: models) {
			Z::Renderer::Draw(model, SamplerShader);
		}
		if (viewPortFocused && viewPortHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Middle)) {
			CursorPos = ImGui::GetMousePos() - ImGui::GetWindowPos();
			CursorPos.y = g_viewportSize.y - CursorPos.y;
			ModelIndex = firstSampleFrame.ReadPixel<glm::ivec2>(7, CursorPos.x, CursorPos.y);
		}
		LightViewFrame.Bind();
		Z::Renderer::SetClearValue({0.f, 0.f, 0.f, 1.0f});
		firstSampleFrame.BindAttachment();
		LightComputeShader->Bind();
		QuadArray->Draw();
		LightViewFrame.Unbind();
		Z::Renderer::SetClearValue({0.1f, 0.1f, 0.1f, 1.0f});
		postProcessFrame.Bind();
		PostProcessShader->Bind();
		LightViewFrame.BindAttachment();
		QuadArray->Draw();
		postProcessFrame.Unbind();

		ImGui::Begin("##View", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
		ImGui::Image((void *) postProcessFrame.GetAttachment(0), ImVec2(g_viewportSize.x, g_viewportSize.y),
		             ImVec2(0, 1),ImVec2(1, 0));
		Z::Guizmo::Init(ImGui::GetWindowPos(), ImGui::GetWindowSize());
		viewPortHovered = ImGui::IsWindowHovered();
		viewPortFocused = ImGui::IsWindowFocused();
		auto viewSize = ImGui::GetWindowSize();
		if (viewSize.x != g_viewportSize.x || viewSize.y != g_viewportSize.y) {
			g_viewportSize = glm::vec2{viewSize.x, viewSize.y};
			postProcessData.viewportSize = g_viewportSize;
			attachmentsSpec.width = g_viewportSize.x;
			attachmentsSpec.height = g_viewportSize.y;
			LightViewFrame.Resize(g_viewportSize.x, g_viewportSize.y);
			firstSampleFrame.Resize(g_viewportSize.x, g_viewportSize.y);
			postProcessFrame.Resize(g_viewportSize.x, g_viewportSize.y);
			camera.aspect = g_viewportSize.x / g_viewportSize.y;
			camera.CalculateMatrix();
		}
		if (ModelIndex.x != -1 && ModelIndex.y != -1 && ModelIndex.x < models.size()) {
			Z::Guizmo::DrawGuizmo(camera.viewMatrix, camera.projectionMatrix, models[ModelIndex.x]->GetModelMatrix());
		}

		ImGui::PopStyleVar(3);

		ImGui::End();

		ImGui::Begin("Status");
		static float frame = 1 / Z::Timer::GetDeltaTime();
		if (Z::Timer::GetFlushTime() > 1.f) {
			frame = 1 / Z::Timer::GetDeltaTime();
			Z::Timer::Flush();
		}
		ImGui::Text("FPS: %.2f", frame);
		ImGui::Text("Index: %d, %d", ModelIndex.x, ModelIndex.y);
		ImGui::Text("CursorPos: %.2f, %.2f", CursorPos.x, CursorPos.y);
		ImGui::SliderFloat("Bloom Threshold", &lightData.cameraPos[3], .5f, 3.f);
		ImGui::Combo("Light", &CurrentLightIndex, "Light0\0Light1\0Light2\0Light3\0Light4\0");
		ImGui::DragFloat4("LightPos", &lightData.lightPos[CurrentLightIndex][0], .1f);
		ImGui::DragFloat4("LightCol", &lightData.lightCol[CurrentLightIndex][0], .1f);
		if(ImGui::Button("Screen Shot")){
			postProcessFrame.ShotFrame("PBR_Shot.png");
		}
		ImGui::Text("Right Mouse Button to rotate camera");
		ImGui::Text("Middle Mouse Button to pick model and show guizmo");
		ImGui::Text("Scroll to change focus distance");

		ImGui::End();
		ImGui::End();
		Z::MyImGui::End();

		Z::Renderer::SwapBuffers();
	}
	if (walkControl.joinable())
		walkControl.join();
	Z::MyImGui::Shutdown();
	Z::Renderer::Shutdown();
}
