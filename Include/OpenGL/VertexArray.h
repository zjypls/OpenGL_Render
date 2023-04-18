//
// Created by 32725 on 2023/4/15.
//

#ifndef RENDERER003_OGL_VERTEXARRAY_H
#define RENDERER003_OGL_VERTEXARRAY_H
#include <vector>
#include <string>
#include <memory>
#include "glad/glad/glad.h"
#include "glm/glm/glm.hpp"
#include "Shader.h"
namespace Z {
	class BufferLayout {
	public:
		struct Element {
			std::string name;
			uint32_t type;
			uint32_t count;
			uint32_t size;
			bool normalized=false;
			uint32_t offset;
		};
		BufferLayout() = default;
		BufferLayout(const std::initializer_list<Element>& elements);
		~BufferLayout() = default;
		const std::vector<Element>& GetElements() const { return elements; }
		uint32_t GetStride() const { return stride; }
	private:
		std::vector<Element> elements;
		uint32_t stride = 0;
	};
	class VertexBuffer{
	public:
		VertexBuffer(const void* data, uint32_t size);
		~VertexBuffer(){glDeleteBuffers(1,&id);}
		void Bind() const{glBindBuffer(GL_ARRAY_BUFFER,id);}
		void Unbind() const{glBindBuffer(GL_ARRAY_BUFFER,0);}
		const BufferLayout& GetLayout() const { return layout; }
		void SetLayout(const BufferLayout& bufferLayout) { layout = bufferLayout; }
	private:
		uint32_t id;
		BufferLayout layout;
	};

	class IndexBuffer {
	public:
		IndexBuffer(const uint32_t* data, uint32_t count);
		~IndexBuffer(){glDeleteBuffers(1,&id);}

		IndexBuffer();

		void Bind() const{glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,id);}
		void Unbind() const{glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);}
		uint32_t GetCount() const { return count; }
	private:
		uint32_t id, count;
	};

	class VertexArray {
	public:
		VertexArray();
		void Bind() const{glBindVertexArray(id);vertexBuffers[0]->Bind();indexBuffer->Bind();}
		void Unbind() const{glBindVertexArray(0);}
		void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vb);
		void AddVertexBuffer(const float* data, uint32_t size, const BufferLayout& layout);
		void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& ib){indexBuffer = ib;}
		void SetIndexBuffer(const uint32_t* data, uint32_t count);
		const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const { return vertexBuffers; }
		const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return indexBuffer; }

		void Draw()const;

	private:
		uint32_t id;
		std::vector<std::shared_ptr<VertexBuffer>> vertexBuffers;
		std::shared_ptr<IndexBuffer> indexBuffer;
	};

}


#endif //RENDERER003_OGL_VERTEXARRAY_H
