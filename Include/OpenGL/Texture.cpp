//
// Created by 32725 on 2023/4/14.
//
#define STB_IMAGE_IMPLEMENTATION
#include <cassert>
#include "Texture.h"
#include "stb/stb_image.h"
namespace Z {
	std::filesystem::path RootDir={Z_TEXTURE_PATH};
	namespace Temp{
		GLenum GetDataFormat(int cal){
			switch(cal){
				case 1:return GL_RED;
				case 2:return GL_RG;
				case 3:return GL_RGB;
				case 4:return GL_RGBA;
				default:assert(false);
			}
		}
		GLenum GetInternalFormat(int cal){
			switch(cal){
				case 1:return GL_R8;
				case 2:return GL_RG8;
				case 3:return GL_RGB8;
				case 4:return GL_RGBA8;
				default:assert(false);
			}
		}
	}

	Texture::Texture(std::filesystem::path path) {
		int wid,hig,cal;
		auto pixels=stbi_load((RootDir/path).string().c_str(),&wid,&hig,&cal,0);
		assert(pixels);
		width=wid,height=hig;
		dataFormat=Temp::GetDataFormat(cal);
		iFormat=Temp::GetInternalFormat(cal);
		glCreateTextures(GL_TEXTURE_2D,1,&id);
		glBindTexture(GL_TEXTURE_2D,id);
		glTexImage2D(GL_TEXTURE_2D,0,iFormat,wid,hig,0,dataFormat,GL_UNSIGNED_BYTE,pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		stbi_image_free(pixels);
	}

	Texture::Texture(int width, int height, GLenum iFormat) {
		assert(width>0&&height>0);
		this->width=width;
		this->height=height;
		this->iFormat=iFormat;
		glCreateTextures(GL_TEXTURE_2D,1,&id);
		glTextureStorage2D(id,1,iFormat,width,height);
		glTextureParameteri(id,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTextureParameteri(id,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTextureParameteri(id,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTextureParameteri(id,GL_TEXTURE_WRAP_T,GL_REPEAT);
	}

	void Texture::Bind(uint32_t slot) const {
		glBindTextureUnit(slot,id);
	}

	void Texture::Unbind() const {
		glBindTextureUnit(0,0);

	}

	void Texture::SetData(void *data, int size, int xOffset, int yOffset, GLenum dataFormat, GLenum dataType) {
		glTextureSubImage2D(id,0,xOffset,yOffset,width,height,dataFormat,dataType,data);
	}

	Texture::Texture(const uint32_t id,GLenum iFormat,GLenum dataFormat):iFormat(iFormat),dataFormat(dataFormat) {
		this->id=id;
		glGetTextureLevelParameteriv(id,0,GL_TEXTURE_WIDTH,&width);
		glGetTextureLevelParameteriv(id,0,GL_TEXTURE_HEIGHT,&height);
	}
}
