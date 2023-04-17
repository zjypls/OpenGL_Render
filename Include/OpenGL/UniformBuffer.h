//
// Created by 32725 on 2023/4/15.
//

#ifndef RENDERER003_OGL_UNIFORMBUFFER_H
#define RENDERER003_OGL_UNIFORMBUFFER_H
#include "glad/glad/glad.h"

namespace Z {
	class UniformBuffer {
	public:
		UniformBuffer();
		UniformBuffer(const void*data, size_t size);
		UniformBuffer( size_t size);
		void Bind(unsigned index) const;
		void SetData(const void*data, size_t size);
	private:
		unsigned id;
	};

}


#endif //RENDERER003_OGL_UNIFORMBUFFER_H
