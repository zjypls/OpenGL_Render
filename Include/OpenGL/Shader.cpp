//
// Created by 32725 on 2023/4/15.
//
#include <filesystem>
#include <fstream>
#include <iostream>
#include "Shader.h"

namespace Z {

	namespace Temp{
		std::string ReadFile(std::filesystem::path path){
			std::ifstream stream(path,std::ios::in|std::ios::binary|std::ios::ate);
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

}