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

	VertexBuffer::VertexBuffer(const void *data, uint32_t size,uint32_t count):count(count){
		glCreateBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, size*count, data, GL_STATIC_DRAW);
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
		}
		vertexBuffers.push_back(vb);
	}

	void VertexArray::Draw() const {
		Bind();
		if(indexBuffer)
		glDrawElements(GL_TRIANGLES, indexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);
		else
		glDrawArrays(GL_TRIANGLES,0,vertexBuffers[0]->count);
	}

	void VertexArray::AddVertexBuffer(const float *data, uint32_t size,uint32_t count, const BufferLayout &layout) {
		auto vb = std::make_shared<VertexBuffer>(data,count, size);
		vb->SetLayout(layout);
		AddVertexBuffer(vb);
	}

	void VertexArray::SetIndexBuffer(const uint32_t *data, uint32_t count) {
		indexBuffer = std::make_shared<IndexBuffer>(data, count);
	}

	VertexArray::VertexArray(std::shared_ptr<VertexBuffer> &vertexBuffer,
	                         const std::shared_ptr<IndexBuffer> &indexBuffer) {
		glCreateVertexArrays(1, &id);
		AddVertexBuffer(vertexBuffer);
		SetIndexBuffer(indexBuffer);
	}
}
