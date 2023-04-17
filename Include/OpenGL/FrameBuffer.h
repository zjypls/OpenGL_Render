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

	struct Attachment {
		GLenum format;
		GLenum target;
	};
	struct AttachmentSpec {
		uint32_t width, height;
		std::vector<Attachment> attachments;
	};

	class FrameBuffer {
	public:
		FrameBuffer(const AttachmentSpec &attachments);

		void Bind() const;

		void Unbind() const;

		uint32_t GetAttachment(uint32_t index) const;

		const uint32_t GetID() const { return id; }

		void Resize(uint32_t width, uint32_t height);

		template<class T>
		T ReadPixel(uint32_t attachmentIndex, uint32_t x, uint32_t y) {
			assert(attachmentIndex < attachments.size());
			glReadBuffer(attachments[attachmentIndex].target);
			T pixel;
			glReadPixels(x, y, 1, 1, attachments[attachmentIndex].format, attachments[attachmentIndex].dataFormat, &pixel);
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
