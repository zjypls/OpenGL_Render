//
// Created by 32725 on 2023/4/22.
//

#ifndef OPENGL_RENDER_MODELBVH_H
#define OPENGL_RENDER_MODELBVH_H
#include "Model/Model.h"

struct BVH{
	glm::vec4 min,max;
	glm::ivec4 count;
};

class ModelBVH :public Z::Model{
public:
	ModelBVH(const std::string& path,int index=0,const glm::vec3&position=glm::vec3{0.f});
	void GenerateBVH(int minCount=2);
	const auto& GetBVH()const{
		return bvh;
	}
private:
	std::vector<BVH> bvh;
};


#endif //OPENGL_RENDER_MODELBVH_H
