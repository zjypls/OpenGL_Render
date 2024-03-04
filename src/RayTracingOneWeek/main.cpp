//
// Created by 32725 on 2023/4/17.
//
#include "Objects.h"
#include "OpenGL/Renderer.h"
#include "Camera/Camera.h"
#include "OpenGL/FrameBuffer.h"
#include "OpenGL/MyImGui.h"
#include "OpenGL/Timer.h"
#include "OpenGL/UniformBuffer.h"
#include "OpenGL/Guizmo.h"

struct CameraData {
	glm::vec4 horizon, vertical, origin, direction;
	glm::ivec4 control;
};

Z::Camera camera{{12, 2, 0}, {0, 0, 0}, 90, 800.0f / 600.0f, 0.1f, 100.0f, 1};
glm::vec2 g_viewportSize{800, 600};
World world{
		{Sphere{{-.5, 1,  -1.7, 1},{.8,  .5, .3,   1},{LAMBERT,0,0,1}},
		Sphere{{-.8, 1,  1.3, 1},{.7,  .1, .8,  1},{MIRROR,   0,  0,   0}},
		Sphere{{.8, 1.2, -.3, .7},{.9, .9, .9,  1},{FOG,  40, 0,   0}},
		Sphere{{1.3, 0.5, -1.1, .5},{.9, .9, .9,  1},{GLASS,  50, 0,   0}},
		Sphere{{.8, 4.7, -.3, 1.2},{8., 13., 5.,  1},{LIGHT,  0, 0,   0}},
		Sphere{{1.8, .48, .8, .5},{.7, .1, .9,  1},{META,  70, 20,   0}},
		Sphere{{-1, -100, 0,  100},{.9, .9,.9, 1}}},
		{Face{{1,  -1,     1,  0},{1,  -1,     -1, 0},{-1, -1,     -1, 0},{-1, -1,     1,  0},{-1, -1.005, -1, 0},{1,  -.995,  1,  0},{0,  1,      0,  0},{.8, .5,     .3, 1}},
		  Face{{1,  1.5,   1,  0},{1,  1.5,   -1, 0},{-1, 1.5,   -1, 0},{-1, 1.5,   1,  0},{-1, 1.495, -1, 0},{1,  1.505, 1,  0},{0,  1,     0,  0},{.9, .7,    .1, 1}}}
};
#ifdef Z_PLATFORM_WIN32
	extern "C"
	{
	// http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
	// The following line is to favor the high performance NVIDIA GPU if there are multiple GPUs
	// Has to be .exe module to be correctly detected.
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	}
#endif

void SetEnv(){
#ifdef Z_PLATFORM_LINUX
		putenv("__NV_PRIME_RENDER_OFFLOAD=1");
		putenv("__GLX_VENDOR_LIBRARY_NAME=nvidia");
#endif
}


int main() {
	SetEnv();
	CameraData cameraData{glm::vec4{0, 0, -1, 0} * (g_viewportSize.x / 200.f),
	                      glm::vec4{0, 1, 0, 0} * (g_viewportSize.y / 200.f),
	                      {20, 2, 0, 0}, {-1, 0, 0, 20}, {10.f, 1, 800, 600}};
	Z::RenderSpec spec{};
	spec.title = "Ray Tracing in One Weekend";
	Z::Renderer::Init(spec);
	Z::MyImGui::Init("RayTracingOneWeek.ini\0");
	auto RayShader=Z::Shader::Create({"Common/Quad.vert", "RayTracingOneWeek/TracingTest.frag"});
	RayShader->Bind();
	RayShader->SetUniform("gamaMode", 0);

	auto attachmentsSpec = Z::AttachmentSpec();
	attachmentsSpec.attachments.push_back({GL_RGBA8, GL_COLOR_ATTACHMENT0});
	auto beforeFrame = Z::FrameBuffer(attachmentsSpec);
	auto afterFrame = Z::FrameBuffer(attachmentsSpec);

	auto QuadArray = Z::Renderer::GetQuadVertexArray();
	auto cameraDataBuffer = Z::UniformBuffer(&cameraData, sizeof(CameraData));
	auto worldDataBuffer = Z::UniformBuffer(&world, sizeof(World));

	uint32_t frameCount = 1;
	auto jupiter = std::make_shared<Z::Texture>("jupiter.jpg");
	jupiter->Bind(1);
	bool gamaCorrection = false;
	bool needResize = false;
	while (Z::Renderer::Running()) {
		Z::Timer::Update();

		cameraData.control.y = frameCount;
		cameraDataBuffer.SetData(&cameraData, sizeof(CameraData));
		if (frameCount % 2) {
			beforeFrame.Bind();
			afterFrame.BindAttachment();
		} else {
			afterFrame.Bind();
			beforeFrame.BindAttachment();
		}
		Z::Renderer::SetClearValue({0.0f, 0.0f, 0.0f, 1.0f});
		Z::MyImGui::Begin();
		cameraDataBuffer.Bind(0);
		worldDataBuffer.Bind(1);
		RayShader->Bind();
		QuadArray->Draw();
		Z::Renderer::BindDefaultFrameBuffer();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGuiViewport *viewport = ImGui::GetMainViewport();
		Z::MyImGui::BeginDockSpace(viewport);
		ImGui::Begin("##viewport", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
		ImGui::Image((void *) (frameCount % 2 ? beforeFrame.GetAttachment(0) : afterFrame.GetAttachment(0)),
		             ImVec2(g_viewportSize.x, g_viewportSize.y), ImVec2(0, 1),ImVec2(1, 0));
		auto viewSize = ImGui::GetWindowSize();
		if (viewSize.x != g_viewportSize.x || viewSize.y != g_viewportSize.y) {
			needResize = true;
		}

		ImGui::End();
		ImGui::Begin("Setting");
		ImGui::Text("FPS: %.2f", Z::Timer::GetFPS());
		if (ImGui::SliderFloat("Distance", &cameraData.direction.w, .5, 20)) {
			frameCount = 0;
			cameraDataBuffer.SetData(&cameraData, sizeof(CameraData));
		}
		if (ImGui::DragFloat3("Origin", &cameraData.origin.x, 0.1, -100, 100)) {
			frameCount = 0;
			cameraDataBuffer.SetData(&cameraData, sizeof(CameraData));
		}
		if (ImGui::SliderInt("Depth", &cameraData.control.x, 1, 50)) {
			frameCount = 0;
			cameraDataBuffer.SetData(&cameraData, sizeof(CameraData));
		}
		if(ImGui::Button("Screen Shot")){
			frameCount%2?beforeFrame.ShotFrame("shots.png"):afterFrame.ShotFrame("shots.png");
		}
		if(ImGui::Checkbox("Gama Correction",&gamaCorrection)){
			frameCount = 0;
			RayShader->SetUniform("gamaMode",gamaCorrection);
		}
		ImGui::End();
		ImGui::PopStyleVar(3);

		Z::MyImGui::EndDockSpace();
		Z::MyImGui::End();

		Z::Renderer::SwapBuffers();
		if (needResize) {
			needResize = false;
			frameCount = 0;
			g_viewportSize = { viewSize.x, viewSize.y };
			beforeFrame.Resize(g_viewportSize.x, g_viewportSize.y);
			afterFrame.Resize(g_viewportSize.x, g_viewportSize.y);
			cameraData.horizon = glm::vec4{ 0, 0, -1, 0 } *g_viewportSize.x / 200.f;
			cameraData.vertical = glm::vec4{ 0, 1, 0, 0 } *g_viewportSize.y / 200.f;
			cameraData.control.z = g_viewportSize.x;
			cameraData.control.w = g_viewportSize.y;
			camera.width = g_viewportSize.x;
			camera.height = g_viewportSize.y;
			camera.aspect = g_viewportSize.x / g_viewportSize.y;
			cameraDataBuffer.SetData(&cameraData, sizeof(CameraData));
		}
		++frameCount;
	}
	Z::Renderer::Shutdown();
}
