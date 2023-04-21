//
// Created by 32725 on 2023/4/15.
//

#ifndef RENDERER003_OGL_SHADER_H
#define RENDERER003_OGL_SHADER_H
#include <string>
#include <initializer_list>
#include "glad/glad/glad.h"
#include "glm/glm/glm.hpp"

namespace Z {
	class Shader {
	public:
		Shader(){id = glCreateProgram();}
		~Shader();

		static std::shared_ptr<Shader> Create(const std::initializer_list<std::string>&src,
											  const std::initializer_list<GLenum>&types={GL_VERTEX_SHADER,GL_FRAGMENT_SHADER});

		void AddShader(const std::string& source,GLenum type,bool isFile=true);

		void Bind() const{
			glUseProgram(id);
		}

		void Link(){
			glLinkProgram(id);
		}

		void Unbind() const{
			glUseProgram(0);
		}

		void SetUniform(const std::string&name,const float* matrix);
		void SetUniform(const std::string&name,const int value);

	private:
		uint32_t id;
	};

}


#endif //RENDERER003_OGL_SHADER_H
