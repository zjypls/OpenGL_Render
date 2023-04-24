//
// Created by 32725 on 2023/4/22.
//

#ifndef OPENGL_RENDER_MODELBVH_H
#define OPENGL_RENDER_MODELBVH_H
#define BVH_VISUAL_INFO
#include "Model/Model.h"

struct BVH {
	glm::vec4 min, max;
	glm::ivec4 count;
};
struct Box{
	glm::vec3 min, max;
};
struct VertexA {
	glm::vec4 position;
	glm::vec4 normal;
	glm::vec4 texCoord;
};
struct Triangle {
	VertexA v0, v1, v2;
};

class ModelBVH : public Z::Model {
public:
	ModelBVH(const std::string &path, int index = 0, bool loadTex = false, const glm::vec3 &position = glm::vec3{0.f});

	void GenerateBVH(int minCount = 3);

	const auto &GetBVH() const {
		return bvh;
	}

	const auto &GetAlignedTriangles() const {
		return AlignedTriangles;
	}

	#ifdef BVH_VISUAL_INFO
	std::pair<std::shared_ptr<Z::VertexArray>,std::shared_ptr<Z::Shader>> GetBVHVisualizeVAO();
	auto& GetBVHVisualize(){
		return BVHVisualize;
	}
private:
	std::vector<Box> BVHVisualize;
	#endif
private:
	std::vector<BVH> bvh;
	std::vector<Triangle> AlignedTriangles;
};


#endif //OPENGL_RENDER_MODELBVH_H
