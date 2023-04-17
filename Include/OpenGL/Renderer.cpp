//
// Created by 32725 on 2023/4/10.
//
#include "Renderer.h"

#include <cassert>

namespace Z {

	struct Triangle{
		vec3 p1;
		vec3 p2;
		vec3 p3;
	};
	struct RenderData{
		Triangle *RenderBuffer;
		Triangle* currentPtr;
	};

	GLFWwindow *Renderer::window = nullptr;
	RenderSpec Renderer::Spec;
	static RenderData *data = nullptr;
	void Renderer::Init(const RenderSpec&spec) {
		data= new RenderData();
		Spec = spec;
		data->RenderBuffer=new Triangle[Spec.maxBufferSize];
		int i = glfwInit();
		assert(i);
		window = glfwCreateWindow(spec.width, spec.height, spec.title, spec.fullScreen?glfwGetPrimaryMonitor(): nullptr, nullptr);
		assert(window);
		glfwMakeContextCurrent(window);
		int j = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
		assert(j);
		glfwSwapInterval(spec.vsync);
	}

}
