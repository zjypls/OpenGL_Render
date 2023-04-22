//
// Created by 32725 on 2023/4/21.
//
#include <iostream>
#include "OpenGL/Renderer.h"
#include "OpenGL/MyImGui.h"
#include "OpenGL/FrameBuffer.h"
#include "OpenGL/UniformBuffer.h"
#include "ModelBVH.h"

struct CameraData{
	glm::vec4 horizontal;
	glm::vec4 vertical;
	glm::vec4 origin;
	glm::vec4 direction;
	glm::vec4 data;
};

int main(){
	CameraData cameraData{
		glm::vec4{0,0,-4,0},
		glm::vec4{0,3,0,0},
		glm::vec4{1,0,0,0},
		glm::vec4{-1,0,0,2},
		glm::vec4{800,600,0,0}
	};
	Z::RenderSpec spec;
	spec.title="RayTracingBVH";

	Z::Renderer::Init(spec);
	Z::MyImGui::Init();

	auto attachmentsSpec=Z::AttachmentSpec{};
	attachmentsSpec.attachments.push_back({GL_RGBA8,GL_COLOR_ATTACHMENT0});
	auto beforeFrame=Z::FrameBuffer(attachmentsSpec);

	auto model=std::make_shared<ModelBVH>("lighter/lighter.fbx");
	model->GenerateBVH(2);

	auto bvh=model->GetBVH();
	auto bvhBuffer =Z::UniformBuffer(bvh.data(),bvh.size()*sizeof(BVH));
	bvhBuffer.Bind(0);
	auto cameraBuffer=Z::UniformBuffer(&cameraData,sizeof(CameraData));
	cameraBuffer.Bind(1);


	auto Quad=Z::Renderer::GetQuadVertexArray();
	auto TracingShader=Z::Shader::Create({"Common/Quad.vert", "TracingBVH/Tracing.frag"});


	while(Z::Renderer::Running()){
		Z::Renderer::SetClearValue({0.2f,0.3f,0.3f,1.0f});

		TracingShader->Bind();
		Quad->Draw();

		Z::Renderer::SwapBuffers();
	}
	Z::MyImGui::Shutdown();
	Z::Renderer::Shutdown();
}