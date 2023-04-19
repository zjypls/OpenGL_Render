//
// Created by 32725 on 2023/4/14.
//

#ifndef RENDERER003_OGL_FRAMEBUFFER_H
#define RENDERER003_OGL_FRAMEBUFFER_H

#include "glad/glad//glad.h"
#include <vector>
#include "Texture.h"
#include "glm/glm/glm.hpp"

namespace Z {

	namespace Temp{
		template<class T>
		std::pair<GLenum,GLenum> GetFormat(){
			if(std::is_same_v<T,glm::ivec2>)
				return {GL_RG_INTEGER,GL_INT};
			if(std::is_same_v<T,glm::ivec3>)
				return {GL_RGB_INTEGER,GL_INT};
			if(std::is_same_v<T,glm::ivec4>)
				return {GL_RGBA_INTEGER,GL_INT};
			if(std::is_same_v<T,glm::vec2>)
				return {GL_RG,GL_FLOAT};
			if(std::is_same_v<T,glm::vec3>)
				return {GL_RGB,GL_FLOAT};
			if(std::is_same_v<T,glm::vec4>)
				return {GL_RGBA,GL_FLOAT};
			if(std::is_same_v<T,float>)
				return {GL_RED,GL_FLOAT};
			if(std::is_same_v<T,int>)
				return {GL_RED_INTEGER,GL_INT};
			if(std::is_same_v<T,unsigned int>)
				return {GL_RED_INTEGER,GL_UNSIGNED_INT};
		}
	}

	struct Attachment {
		GLenum format;
		GLenum target;
	};
	struct AttachmentSpec {
		uint32_t width=800, height=600;
		std::vector<Attachment> attachments;
	};

	class FrameBuffer {
	public:
		FrameBuffer(const AttachmentSpec &attachments);

		void Bind() const;

		void BindAttachment(uint32_t index=0);

		void Unbind() const;

		template<class T>
		void ClearAttachment(uint32_t index, const T& color){
			assert(index < attachments.size());
			auto [format,dataFormat] = Temp::GetFormat<T>();
			glClearTexImage(attachments[index].id, 0, format, dataFormat, (void*)&color);
		}

		uint32_t GetAttachment(uint32_t index) const;

		const uint32_t GetID() const { return id; }

		void Resize(uint32_t width, uint32_t height);

		template<class T>
		T ReadPixel(uint32_t attachmentIndex, uint32_t x, uint32_t y) {
			assert(attachmentIndex < attachments.size());
			glReadBuffer(attachments[attachmentIndex].target);
			T pixel{};
			auto [format,dataFormat] = Temp::GetFormat<T>();
			glReadPixels(x, y, 1, 1, format, dataFormat, (void*)&pixel);
			return pixel;
		}

	private:
		struct Attachment {
			uint32_t id;
			GLenum format;
			GLenum dataFormat;
			GLenum target;
		};
		uint32_t id, width, height;
		std::vector<Z::FrameBuffer::Attachment> attachments;
	};

}
#endif //RENDERER003_OGL_FRAMEBUFFER_H
