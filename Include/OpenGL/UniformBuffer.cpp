//
// Created by 32725 on 2023/4/15.
//

#include "UniformBuffer.h"

namespace Z{

	UniformBuffer::UniformBuffer() {
		glCreateBuffers(1, &id);
	}

	UniformBuffer::UniformBuffer(const void *data, size_t size) {
		glCreateBuffers(1, &id);
		glBindBuffer(GL_UNIFORM_BUFFER, id);
		glBufferData(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_COPY);
	}

	void UniformBuffer::Bind(unsigned int index) const {
		glBindBufferBase(GL_UNIFORM_BUFFER, index, id);
	}

	void UniformBuffer::SetData(const void *data, size_t size) {
		glBindBuffer(GL_UNIFORM_BUFFER, id);
		glBufferData(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_COPY);
	}

	UniformBuffer::UniformBuffer(size_t size) {
		glCreateBuffers(1, &id);
		glBindBuffer(GL_UNIFORM_BUFFER, id);
		glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_COPY);
	}
}