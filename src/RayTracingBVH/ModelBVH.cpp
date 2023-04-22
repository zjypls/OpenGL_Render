//
// Created by 32725 on 2023/4/22.
//

#include "ModelBVH.h"
int MinCount;
ModelBVH::ModelBVH(const std::string &path, int index, const glm::vec3 &position) : Model(path, index, position){

}

int GenerateBVH(Z::Model::Vertex* begin,Z::Model::Vertex* end,std::vector<BVH>&bvh){
	if(end-begin<=MinCount){
		BVH b{};
		b.min=glm::vec4{begin->position,0.f};
		b.max=glm::vec4{(end-1)->position,0.f};
		b.count=glm::ivec4{0,0,end-begin,0};
		bvh.push_back(b);
		return bvh.size()-1;
	}
	auto min=glm::vec3{std::numeric_limits<float>::max()};
	auto max=glm::vec3{std::numeric_limits<float>::min()};
	for(auto i=begin;i!=end;++i){
		min=glm::min(min,i->position);
		max=glm::max(max,i->position);
	}
	auto size=max-min;
	auto maxIndex=0;
	for(auto i=1;i<3;++i){
		if(size[i]>size[maxIndex]){
			maxIndex=i;
		}
	}
	std::sort(begin,end,[maxIndex](const auto&lhs,const auto&rhs){
		return lhs.position[maxIndex]<rhs.position[maxIndex];
	});
	auto midIter=begin+(end-begin)/2;
	auto nowIndex=bvh.size();
	bvh.emplace_back();
	bvh[nowIndex].min=glm::vec4{min,0.f};
	bvh[nowIndex].max=glm::vec4{max,0.f};
	bvh[nowIndex].count.x=GenerateBVH(begin,midIter,bvh);
	bvh[nowIndex].count.y=GenerateBVH(midIter,end,bvh);
	bvh[nowIndex].count.z=-1;
	return bvh.size()-1;
}

void ModelBVH::GenerateBVH(int minCount) {
	::MinCount=minCount;
	::GenerateBVH(vertices.data(),vertices.data()+vertices.size(),bvh);
}

