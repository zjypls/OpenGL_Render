//
// Created by 32725 on 2023/4/15.
//

#include "VertexArray.h"

namespace Z {

	BufferLayout::BufferLayout(const std::initializer_list<Element> &elements) {
		this->elements = elements;
		uint32_t offset = 0;
		for (auto &element: this->elements) {
			element.offset = offset;
			offset += element.size * element.count;
			stride += element.size * element.count;
		}
	}

	VertexBuffer::VertexBuffer(const void *data, uint32_t size) {
		glCreateBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}

	IndexBuffer::IndexBuffer(const uint32_t *data, uint32_t count) {
		this->count = count;
		glCreateBuffers(1, &id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, GL_STATIC_DRAW);
	}

	IndexBuffer::IndexBuffer() {
		glCreateBuffers(1, &id);
	}

	VertexArray::VertexArray() {
		glCreateVertexArrays(1, &id);
	}

	void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer> &vb) {
		glBindVertexArray(id);
		vb->Bind();
		const auto &layout = vb->GetLayout();
		int index = 0;
		for (const auto &element: layout.GetElements()) {
			glEnableVertexAttribArray(index);
			if (element.type == GL_FLOAT) {
				glVertexAttribPointer(index, element.count, element.type, element.normalized, layout.GetStride(),
				                      (const void *) element.offset);
			} else if (element.type == GL_INT) {
				glVertexAttribIPointer(index, element.count, element.type, layout.GetStride(),
				                       (const void *) element.offset);
			}
			index++;
			vertexBuffers.push_back(vb);
		}
	}

	void VertexArray::Draw() const {
		Bind();
		glDrawElements(GL_TRIANGLES, indexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);
	}
}
