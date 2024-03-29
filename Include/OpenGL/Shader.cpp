//
// Created by 32725 on 2023/4/15.
//
#include <filesystem>
#include <fstream>
#include <iostream>
#include "Shader.h"

namespace Z {
	std::filesystem::path ShaderPath{Z_SHADER_PATH};
	namespace Temp{
		std::string ReadFile(const std::filesystem::path& path){
			std::ifstream stream(ShaderPath/path,std::ios::in|std::ios::binary|std::ios::ate);
			assert(stream.is_open());
			auto size=stream.tellg();
			std::string result(size,' ');
			stream.seekg(0,std::ios::beg);
			stream.read(result.data(),size);
			stream.close();
			return result;
		}
	}

	Shader::~Shader() {
		glDeleteProgram(id);
	}

	void Shader::AddShader(const std::string &source, GLenum type, bool isFile) {
		auto src=source;
		if(isFile){
			src=Temp::ReadFile(source);
		}
		auto shader=glCreateShader(type);
		const char* cstr=src.c_str();
		glShaderSource(shader,1,&cstr,nullptr);
		glCompileShader(shader);
		int success;
		glGetShaderiv(shader,GL_COMPILE_STATUS,&success);
		if(!success){
			char infoLog[1024];
			glGetShaderInfoLog(shader,1024,nullptr,infoLog);
			std::cout<<infoLog<<std::endl;
			assert(false);
		}
		glAttachShader(id,shader);
		glDeleteShader(shader);
	}

	void Shader::SetUniform(const std::string &name,const float* matrix) {
		auto location=glGetUniformLocation(id,name.c_str());
		glUniformMatrix4fv(location,1,GL_FALSE,matrix);
	}

	void Shader::SetUniform(const std::string &name, const int value) {
		auto location=glGetUniformLocation(id,name.c_str());
		glUniform1i(location,value);
	}

	std::shared_ptr<Shader>
	Shader::Create(const std::initializer_list<std::string> &src, const std::initializer_list<GLenum> &types) {
		assert(src.size()<=types.size());
		auto shader=std::make_shared<Shader>();
		auto srcIt=src.begin();
		auto typeIt=types.begin();
		for(;srcIt!=src.end();++srcIt,++typeIt){
			shader->AddShader(*srcIt,*typeIt);
		}
		shader->Link();
        GLint success;
        glGetProgramiv(shader->id, GL_LINK_STATUS, &success);
        if (!success) {
            GLchar infoLog[512];
            glGetProgramInfoLog(shader->id, 512, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR\n" << infoLog << std::endl;
        	assert(false);
        }
		return shader;
	}

}