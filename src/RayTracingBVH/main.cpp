//
// Created by 32725 on 2023/4/21.
//

//define TEST_TRIANGLES_DATA to test the triangles data for debugging
//#define TEST_TRIANGLES_DATA

#include <iostream>
#include "OpenGL/Renderer.h"
#include "OpenGL/MyImGui.h"
#include "OpenGL/FrameBuffer.h"
#include "OpenGL/UniformBuffer.h"
#include "ModelBVH.h"
#include "OpenGL/Timer.h"
#include "Camera/Camera.h"

struct CameraData {
	glm::vec4 horizontal;
	glm::vec4 vertical;
	glm::vec4 origin;
	glm::vec4 direction;
	glm::vec4 data;
	glm::ivec4 data2;
};

glm::vec2 g_ViewPortSize{800, 600};

extern "C"{
	__declspec (dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

int main() {
	CameraData cameraData{glm::vec4{0, 0, -1, 0} * (g_ViewPortSize.x / 200.f),
	                      glm::vec4{0, 1, 0, 0} * (g_ViewPortSize.y / 200.f),
	                      glm::vec4{1.5, .1, .4, 0}, glm::vec4{-1, 0, 0, 10}, glm::vec4{g_ViewPortSize, 0, 0}};
	cameraData.horizontal =
			glm::vec4(glm::normalize(glm::cross(glm::vec3(cameraData.direction), glm::vec3(0, 1, 0))), 0) *g_ViewPortSize.x / 200.f;
	cameraData.vertical =
			glm::vec4(glm::normalize(glm::cross(glm::vec3(cameraData.horizontal), glm::vec3(cameraData.direction))),0) * g_ViewPortSize.y / 200.f;
	Z::RenderSpec spec;
	spec.title = "RayTracingBVH";

	Z::Renderer::Init(spec);
	Z::MyImGui::Init();

	auto attachmentsSpec = Z::AttachmentSpec{};
	attachmentsSpec.attachments.push_back({GL_RGBA8, GL_COLOR_ATTACHMENT0});
	#ifdef TEST_TRIANGLES_DATA
	attachmentsSpec.attachments.push_back({GL_DEPTH24_STENCIL8,GL_DEPTH_STENCIL_ATTACHMENT});
	#endif
	auto beforeFrame = Z::FrameBuffer(attachmentsSpec);

	auto model = std::make_shared<ModelBVH>("bunny/bunny.obj");
	model->GenerateBVH(3);

	auto bvh = model->GetBVH();
	auto bvhBuffer = Z::TextureBuffer(bvh.data(), bvh.size() * sizeof(BVH), GL_RGBA32F);
	bvhBuffer.Bind(0);

	auto cameraBuffer = Z::UniformBuffer(&cameraData, sizeof(CameraData));
	cameraBuffer.Bind(0);

	auto triangles = model->GetAlignedTriangles();
	auto modelBuffer = Z::TextureBuffer(triangles.data(), triangles.size() * sizeof(triangles[0]), GL_RGBA32F);
	modelBuffer.Bind(1);



#ifdef TEST_TRIANGLES_DATA
	Z::Camera camera{{1, 1, 1}, {0, 0, 0}, 45.f, 8.f / 6.f, 0.1f, 100.f};
	auto testShader=Z::Shader::Create({"TempTest/test.vert", "TempTest/test.frag"});
	auto vbTest=std::make_shared<Z::VertexBuffer>(triangles.data(),sizeof(triangles[0])/3,triangles.size()*3);
	vbTest->SetLayout({{"",GL_FLOAT,4,sizeof(float)},{"",GL_FLOAT,4,sizeof(float)},{"",GL_FLOAT,4,sizeof(float)}});
	auto vaTest=Z::VertexArray{};
	vaTest.AddVertexBuffer(vbTest);

//debug for BVH
//	auto vaTest=model->GetBVHVisualizeVAO();
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#else
	auto Quad = Z::Renderer::GetQuadVertexArray();
	auto TracingShader = Z::Shader::Create({"Common/Quad.vert", "TracingBVH/Tracing.frag"});
	TracingShader->Bind();
	TracingShader->SetUniform("nums", triangles.size());
#endif
	while (Z::Renderer::Running()) {
		Z::Timer::Update();
		Z::MyImGui::Begin();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		Z::MyImGui::BeginDockSpace(ImGui::GetMainViewport());

		beforeFrame.Bind();
		Z::Renderer::SetClearValue({0.2f, 0.3f, 0.3f, 1.0f});

		#ifdef TEST_TRIANGLES_DATA
		testShader->Bind();
//		vaTest.second->Bind();
		auto vp=camera.GetVPMatrix();
		testShader->SetUniform("vp",&vp[0][0]);
		vaTest.Draw();
		#else
		TracingShader->Bind();
		Quad->Draw();
		#endif
		Z::Renderer::BindDefaultFrameBuffer();

		ImGui::Begin("##View", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar);
		ImGui::Image((void *) (intptr_t) beforeFrame.GetAttachment(0), {g_ViewPortSize.x, g_ViewPortSize.y}, {0, 1},
		             {1, 0});
		if (auto size = ImGui::GetWindowSize();size.x != g_ViewPortSize.x || size.y != g_ViewPortSize.y) {
			g_ViewPortSize = {size.x, size.y};
			cameraData.horizontal = glm::vec4{0, 0, -1, 0} * (g_ViewPortSize.x / 200.f);
			cameraData.vertical = glm::vec4{0, 1, 0, 0} * (g_ViewPortSize.y / 200.f);
			cameraData.data = {g_ViewPortSize, 0, 0};
			cameraBuffer.SetData(&cameraData, sizeof(CameraData));
			beforeFrame.Resize(size.x, size.y);
		}
		ImGui::End();

		ImGui::PopStyleVar(3);

		ImGui::Begin("Setting");
		ImGui::Text("FPS: %.2f", Z::Timer::GetFPS());
		#ifdef TEST_TRIANGLES_DATA
		if (ImGui::DragFloat3("Camera Position", &camera.position.x, 0.01f)) {
			camera.CalculateMatrix();
		}
		#else
		if (ImGui::DragFloat3("Camera Position", &cameraData.origin.x, 0.01f)) {
			cameraBuffer.SetData(&cameraData, sizeof(CameraData));
		}
		#endif
		ImGui::End();

		Z::MyImGui::EndDockSpace();
		Z::MyImGui::End();

		Z::Renderer::SwapBuffers();
	}
	Z::MyImGui::Shutdown();
	Z::Renderer::Shutdown();
}