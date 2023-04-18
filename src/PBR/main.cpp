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
struct PostProcessData {
	glm::vec2 viewportSize;
};
glm::vec2 viewportSize{800, 600};

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
	glfwSetScrollCallback(window, [](GLFWwindow *w, double x, double y) {
		camera.Scroll(w, x, y);
	});
	Z::MyImGui::Init();

	auto vb = std::make_shared<Z::VertexBuffer>(quad, sizeof(quad));
	Z::BufferLayout layout{{Z::BufferLayout::Element{"pos", GL_FLOAT, 3, sizeof(float), false, 0},
	                        {"tex", GL_FLOAT, 2, sizeof(uint32_t), false, 0}}};
	vb->SetLayout(layout);
	auto ib = std::make_shared<Z::IndexBuffer>(indices, sizeof(indices));
	Z::VertexArray va;
	va.AddVertexBuffer(vb);
	va.SetIndexBuffer(ib);

	Z::Shader lighter{}, sample{},postProcess{};
	lighter.AddShader("PBR/vertex001.glsl", GL_VERTEX_SHADER);
	lighter.AddShader("PBR/fragment001.glsl", GL_FRAGMENT_SHADER);
	lighter.Link();
	sample.AddShader("PBR/vertex002.glsl", GL_VERTEX_SHADER);
	sample.AddShader("PBR/fragment002.glsl", GL_FRAGMENT_SHADER);
	sample.Link();
	postProcess.AddShader("PBR/vertex003.glsl", GL_VERTEX_SHADER);
	postProcess.AddShader("PBR/fragment003.glsl", GL_FRAGMENT_SHADER);
	postProcess.Link();

	Z::UniformBuffer ubo{sizeof(glm::mat4)}, fbo{&lightData, sizeof(LightData)};
	Z::UniformBuffer postProcessUbo{&postProcessData, sizeof(PostProcessData)};
	Z::AttachmentSpec attachments;
	attachments.width = 800;
	attachments.height = 600;
	attachments.attachments.push_back({GL_RGBA8, GL_COLOR_ATTACHMENT0});
	auto postProcessFrame=Z::FrameBuffer(attachments);
	attachments.attachments.pop_back();
	attachments.attachments.push_back({GL_RGB32F, GL_COLOR_ATTACHMENT0});
	attachments.attachments.push_back({GL_RGB32F, GL_COLOR_ATTACHMENT1});
	auto viewFrame = Z::FrameBuffer(attachments);
	attachments.attachments.pop_back();
	attachments.attachments.push_back({GL_RGB16, GL_COLOR_ATTACHMENT1});
	attachments.attachments.push_back({GL_RGB32F, GL_COLOR_ATTACHMENT2});
	attachments.attachments.push_back({GL_R16F, GL_COLOR_ATTACHMENT3});
	attachments.attachments.push_back({GL_R16F, GL_COLOR_ATTACHMENT4});
	attachments.attachments.push_back({GL_R16F, GL_COLOR_ATTACHMENT5});
	attachments.attachments.push_back({GL_R16F, GL_COLOR_ATTACHMENT6});
	attachments.attachments.push_back({GL_RG32I, GL_COLOR_ATTACHMENT7});
	attachments.attachments.push_back({GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT});
	auto fb = Z::FrameBuffer(attachments);

	std::vector<std::shared_ptr<Z::Model>> models;
	models.emplace_back(std::make_shared<Z::Model>("lighter/lighter.fbx"));
	//Z::Model model{"lighter/lighter.fbx"};

	glEnable(GL_DEPTH_TEST);
	auto walkControl = std::thread{Z::Camera::Walk, window};
	static bool viewPortHovered = false, viewPortFocused = false;
	static ImVec2 CursorPos{};
	static glm::ivec2 Index{-1, -1};
	static int lightIndex = 0;
	while (Z::Renderer::Running()) {
		Z::Timer::Update();
		Z::MyImGui::Begin();
		{
			ImGuiViewport *viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("DockSpace Demo", nullptr,
			             ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground |
			             ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
			             ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
			             ImGuiWindowFlags_NoMove);
			ImGui::DockSpace(ImGui::GetID("MyDockSpace"));
		}

		auto vp = camera.GetVPMatrix();
		ubo.SetData(&vp, sizeof(glm::mat4));
		ubo.Bind(7);
		postProcessUbo.SetData(&postProcessData, sizeof(PostProcessData));
		postProcessUbo.Bind(8);
		lightData.cameraPos = glm::vec4(camera.position, lightData.cameraPos.w);
		fbo.SetData(&lightData, sizeof(LightData));
		fbo.Bind(12);
		fb.Bind();
		Z::Renderer::SetClearValue({0.1f, 0.1f, 0.1f, .0f});
		fb.ClearAttachment(7, glm::ivec2{-1, -1});
		for (auto &model: models) {
			Z::Renderer::Draw(model, lighter);
		}
		if (viewPortFocused && viewPortHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Middle)) {
			CursorPos = ImGui::GetMousePos() - ImGui::GetWindowPos();
			CursorPos.y = viewportSize.y - CursorPos.y;
			Index = fb.ReadPixel<glm::ivec2>(7, CursorPos.x, CursorPos.y);
		}
		viewFrame.Bind();
		Z::Renderer::SetClearValue({0.f, 0.f, 0.f, 1.0f});
		fb.BindAttachment();
		sample.Bind();
		va.Draw();
		viewFrame.Unbind();
		Z::Renderer::SetClearValue({0.1f, 0.1f, 0.1f, 1.0f});
		postProcessFrame.Bind();
		postProcess.Bind();
		viewFrame.BindAttachment();
		va.Draw();
		postProcessFrame.Unbind();

		ImGui::Begin("##View", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
		ImGui::Image((void *)postProcessFrame.GetAttachment(0), ImVec2(viewportSize.x, viewportSize.y), ImVec2(0, 1),
		             ImVec2(1, 0));
		Z::Guizmo::Init(ImGui::GetWindowPos(), ImGui::GetWindowSize());
		viewPortHovered = ImGui::IsWindowHovered();
		viewPortFocused = ImGui::IsWindowFocused();
		auto viewSize = ImGui::GetWindowSize();
		if (viewSize.x != viewportSize.x || viewSize.y != viewportSize.y) {
			viewportSize = glm::vec2{viewSize.x, viewSize.y};
			postProcessData.viewportSize = viewportSize;
			attachments.width = viewportSize.x;
			attachments.height = viewportSize.y;
			viewFrame.Resize(viewportSize.x, viewportSize.y);
			fb.Resize(viewportSize.x, viewportSize.y);
			postProcessFrame.Resize(viewportSize.x, viewportSize.y);
			camera.aspect = viewportSize.x / viewportSize.y;
			camera.CalculateMatrix();
		}
		if (Index.x != -1 && Index.y != -1 && Index.x < models.size()) {
			Z::Guizmo::DrawGuizmo(camera.viewMatrix, camera.projectionMatrix, models[Index.x]->GetModelMatrix());
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
		ImGui::Text("Index: %d, %d", Index.x, Index.y);
		ImGui::Text("CursorPos: %.2f, %.2f", CursorPos.x, CursorPos.y);
		ImGui::SliderFloat("Bloom Threshold", &lightData.cameraPos[3], .5f, 3.f);
		ImGui::Combo("Light", &lightIndex, "Light0\0Light1\0Light2\0Light3\0Light4\0");
		ImGui::DragFloat4("LightPos", &lightData.lightPos[lightIndex][0], .1f);
		ImGui::DragFloat4("LightCol", &lightData.lightCol[lightIndex][0], .1f);
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
