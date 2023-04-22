//
// Created by 32725 on 2023/4/21.
//
#include "OpenGL/Renderer.h"
#include "OpenGL/MyImGui.h"
#include "OpenGL/FrameBuffer.h"


int main(){
	Z::RenderSpec spec;
	spec.title="RayTracingBVH";

	Z::Renderer::Init(spec);
	Z::MyImGui::Init();

	auto attachmentsSpec=Z::AttachmentSpec{};
	attachmentsSpec.attachments.push_back({GL_RGBA8,GL_COLOR_ATTACHMENT0});
	auto beforeFrame=Z::FrameBuffer(attachmentsSpec);


	while(Z::Renderer::Running()){
		Z::Renderer::SetClearValue({0.2f,0.3f,0.3f,1.0f});



		Z::Renderer::SwapBuffers();
	}
	Z::MyImGui::Shutdown();
	Z::Renderer::Shutdown();
}