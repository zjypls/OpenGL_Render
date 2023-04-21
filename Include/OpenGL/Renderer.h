//
// Created by 32725 on 2023/4/10.
//

#ifndef RENDERER003_OGL_RENDERER_H
#define RENDERER003_OGL_RENDERER_H

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm/glm.hpp"
#include "Model/Model.h"

namespace Z {
	using namespace glm;
		struct RenderSpec{
			int width=800;
			int height=600;
			const char* title;

			bool fullScreen=false;
			bool vsync=false;
		};
	class Renderer {
		static GLFWwindow *window;
		static RenderSpec Spec;
	public:
		static void Init(const RenderSpec&spec);
		static bool Running() { return !glfwWindowShouldClose(window); }
		static auto GetWindow() { return window; }
		static void BindDefaultFrameBuffer() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

		static void Draw(const std::shared_ptr<Model>& model,std::shared_ptr<Shader>&shader){model->Draw(shader);}
		static void Draw(const std::shared_ptr<VertexArray>& vertexArray,Shader&shader){shader.Bind();vertexArray->Draw();}

		static void Shutdown() {
			glfwDestroyWindow(window);
			glfwTerminate();
		}
		static std::shared_ptr<VertexArray> GetQuadVertexArray();
		static void SwapBuffers() { glfwSwapBuffers(window);glfwPollEvents(); }

		static void SetClearValue(const vec4&value) { glClearColor(value.x,value.y,value.z,value.w); glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); }

		static glm::vec2 GetWindowSize();
	};

}


#endif //RENDERER003_OGL_RENDERER_H
