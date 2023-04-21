//
// Created by 32725 on 2023/4/14.
//
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <cassert>
#include "FrameBuffer.h"
#include "stb/stb_image_write.h"
namespace Z {
	FrameBuffer::FrameBuffer(const AttachmentSpec &attachments)
			: width(attachments.width), height(attachments.height) {
		assert(!attachments.attachments.empty());
		glCreateFramebuffers(1, &id);
		glBindFramebuffer(GL_FRAMEBUFFER, id);
		for (int i = 0; i < attachments.attachments.size(); ++i) {
			uint32_t a = 0;
			glCreateTextures(GL_TEXTURE_2D, 1, &a);
			glTextureStorage2D(a, 1, attachments.attachments[i].format, width, height);
			glTextureParameteri(a, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(a, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureParameteri(a, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTextureParameteri(a, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachments.attachments[i].target, GL_TEXTURE_2D, a, 0);
			this->attachments.push_back({a, attachments.attachments[i].format, 0, attachments.attachments[i].target});
		}
		//check framebuffer status
		auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		assert(result == GL_FRAMEBUFFER_COMPLETE);

		if (this->attachments.empty()) {
			glDrawBuffer(GL_NONE);
		} else {
			std::vector<GLenum> buffers;
			for (int i = 0; i < this->attachments.size(); ++i) {
				if (this->attachments[i].target != GL_DEPTH_ATTACHMENT &&
				    this->attachments[i].target != GL_STENCIL_ATTACHMENT &&
					this->attachments[i].target != GL_DEPTH_STENCIL_ATTACHMENT)
					buffers.push_back(this->attachments[i].target);
			}
			glDrawBuffers(buffers.size(), buffers.data());
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::Bind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, id);
		glViewport(0, 0, width, height);
	}

	void FrameBuffer::Unbind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	uint32_t FrameBuffer::GetAttachment(uint32_t index) const {
		assert(index < attachments.size());
		return attachments[index].id;
	}

	void FrameBuffer::Resize(uint32_t width, uint32_t height) {
		assert(width > 0 && height > 0);
		this->width = width;
		this->height = height;
		for(auto &a : attachments)
			glDeleteTextures(1, &a.id);
		glDeleteFramebuffers(1, &id);
		glCreateFramebuffers(1, &id);
		glBindFramebuffer(GL_FRAMEBUFFER, id);
		for (auto &a : attachments) {
			glCreateTextures(GL_TEXTURE_2D, 1, &a.id);
			glTextureStorage2D(a.id, 1, a.format, width, height);
			glTextureParameteri(a.id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(a.id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureParameteri(a.id, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(a.id, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glFramebufferTexture2D(GL_FRAMEBUFFER, a.target, GL_TEXTURE_2D, a.id, 0);
		}
		//check framebuffer status
		auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		assert(result == GL_FRAMEBUFFER_COMPLETE);

		if(attachments.empty()){
			glDrawBuffer(GL_NONE);
		}else{
			std::vector<GLenum> buffers;
			for (int i = 0; i < attachments.size(); ++i) {
				if (attachments[i].target != GL_DEPTH_ATTACHMENT &&
				    attachments[i].target != GL_STENCIL_ATTACHMENT &&
					attachments[i].target != GL_DEPTH_STENCIL_ATTACHMENT)
					buffers.push_back(attachments[i].target);
			}
			glDrawBuffers(buffers.size(), buffers.data());
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::BindAttachment(uint32_t index) {
		for(int i = 0; i < attachments.size(); ++i){
			glBindTextureUnit(i+index, attachments[i].id);
		}
	}

	void FrameBuffer::ShotFrame(const std::string &path) {
		Bind();
		stbi_flip_vertically_on_write(true);
		std::vector<uint8_t> data(width*height*4);
		glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
		Unbind();
		stbi_write_png(path.c_str(), width, height, 4, data.data(), width*4);
	}
}