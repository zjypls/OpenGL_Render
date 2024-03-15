//
// Created by z on 24-3-13.
//
#include <thread>
#include <cstdlib>
#include <iostream>
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
    Z::RenderSpec spec{};
    spec.title="Animation";
    spec.width=800;
    spec.height=600;

    Z::Renderer::Init(spec);
    while(Z::Renderer::Running()){
        Z::Timer::Update();
        Z::Renderer::SetClearValue({0.1,0.1,0.1,0});
        Z::Renderer::SwapBuffers();
    }
    Z::Renderer::Shutdown();

}