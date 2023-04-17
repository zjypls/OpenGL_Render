//
// Created by 32725 on 2023/4/14.
//

#ifndef RENDERER003_OGL_TEXTURE_H
#define RENDERER003_OGL_TEXTURE_H
#include "glad/glad//glad.h"
#include <filesystem>
namespace Z {
	class Texture {
	public:
		Texture(std::filesystem::path path);
		Texture(int width,int height,GLenum iFormat=GL_RGBA8);
		Texture(const uint32_t id,GLenum iFormat=GL_RGBA8,GLenum dataFormat=GL_RGBA);
		void Bind(uint32_t slot=0)const;
		void Unbind()const;
		void SetData(void* data,int size,int xOffset=0,int yOffset=0,GLenum dataFormat=GL_RGBA,GLenum dataType=GL_UNSIGNED_BYTE);
	private:
		friend class FrameBuffer;
		uint32_t id;
		int width,height;
		GLenum iFormat,dataFormat;
	};

}

#endif //RENDERER003_OGL_TEXTURE_H
