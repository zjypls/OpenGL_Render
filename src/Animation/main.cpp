//
// Created by z on 24-3-13.
//
#include <thread>
#include <cstdlib>
#include <iostream>
#include "glm/glm/gtx/string_cast.hpp"
#include "OpenGL/Renderer.h"
#include "OpenGL/FrameBuffer.h"
#include "OpenGL/Shader.h"
#include "OpenGL/VertexArray.h"
#include "AnimationModel.h"
#include "Camera/Camera.h"
#include "OpenGL/UniformBuffer.h"
#include "OpenGL/Timer.h"
#include "OpenGL/MyImGui.h"
#include "OpenGL/Guizmo.h"
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

int main(){
    SetEnv();
    glm::vec3 pos{0,1,2.5};
    Z::Camera camera{pos,pos+glm::vec3{0,0,-1},60,1,0.1,1000,1};
    camera.Front=camera.focus-camera.position;
    camera.Right=glm::normalize(glm::cross(camera.Front,camera.Up));
    Z::RenderSpec spec{};
    spec.title="Animation";
    spec.width=800;
    spec.height=600;
    spec.vsync=true;

    Z::Renderer::Init(spec);
    auto window=Z::Renderer::GetWindow();
    glfwSetWindowUserPointer(window,&camera);
    glfwGetCursorPos(window,&camera.lastX,&camera.lastY);
    glfwSetCursorPosCallback(window,[](auto windows,auto x,auto y){
        auto cam= (Z::Camera*)glfwGetWindowUserPointer(windows);
        cam->Turn(windows,x,y);
    });
    glfwSetScrollCallback(window,[](auto window,auto x,auto y){
        auto cam=(Z::Camera*) glfwGetWindowUserPointer(window);
        cam->Scroll(window,x,y);
    });
    Z::MyImGui::Init("Animation.ini\0");

    auto model = new Z::AnimationModel ("animation/dancing_vampire.dae");
    auto sampleShader=Z::Shader::Create({"Animation/animation.vert","Animation/animation.frag"});

    auto normalTex=new Z::Texture("dancing_vampire/normal.png");
    auto diffuseTex=new Z::Texture("dancing_vampire/diffuse.png");
    auto specularTex=new Z::Texture("dancing_vampire/specular.png");

    auto vpMat=camera.GetVPMatrix();
    Z::UniformBuffer cameraData{&vpMat,sizeof(glm::mat4)};
    auto walkControl=std::thread{Z::Camera::Walk,window};
    normalTex->Bind(4);
    diffuseTex->Bind(3);
    while(Z::Renderer::Running()){
        Z::Timer::Update();
        Z::Renderer::SetClearValue({0.1,0.1,0.1,0});

        vpMat=camera.GetVPMatrix();


        cameraData.SetData(&vpMat[0][0],sizeof(glm::mat4));

        cameraData.Bind(0);
        model->Draw(sampleShader);
        Z::Renderer::SwapBuffers();
        model->Update(Z::Timer::deltaTime);
    }
    if(walkControl.joinable()){
        walkControl.join();
    }
    Z::MyImGui::Shutdown();
    delete normalTex;
    delete diffuseTex;
    delete specularTex;
    delete model;
    sampleShader= nullptr;
    Z::Renderer::Shutdown();

}