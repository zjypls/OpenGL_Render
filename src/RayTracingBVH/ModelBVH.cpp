//
// Created by 32725 on 2023/4/22.
//

//the min thickness of the box,if less than this value,then add the SCALE_THICKNESS to it
#define MIN_THICKNESS 1E-5
#define SCALE_THICKNESS 5E-6
#include "ModelBVH.h"
int MinCount;
Triangle* OriginalBegin;
ModelBVH::ModelBVH(const std::string &path, int index,bool loadTex, const glm::vec3 &position) : Model(path, index,loadTex, position){
	for(uint32_t i=0;i<indices.size();i+=3){
		Triangle t{};
		t.v0.position=glm::vec4{vertices[indices[i]].position,1.f};
		t.v0.normal=glm::vec4{vertices[indices[i]].normal,1.f};
		t.v0.texCoord=glm::vec4{vertices[indices[i]].texCoord,0.f,0.f};

		t.v1.position=glm::vec4{vertices[indices[i+1]].position,1.f};
		t.v1.normal=glm::vec4{vertices[indices[i+1]].normal,1.f};
		t.v1.texCoord=glm::vec4{vertices[indices[i+1]].texCoord,0.f,0.f};

		t.v2.position=glm::vec4{vertices[indices[i+2]].position,1.f};
		t.v2.normal=glm::vec4{vertices[indices[i+2]].normal,1.f};
		t.v2.texCoord=glm::vec4{vertices[indices[i+2]].texCoord,0.f,0.f};

		AlignedTriangles.push_back(t);
	}
}

glm::vec3 GetMin(const Triangle& a){
	return glm::min(a.v0.position,glm::min(a.v1.position,a.v2.position));
}
glm::vec3 GetMax(const Triangle& a){
	return glm::max(a.v0.position,glm::max(a.v1.position,a.v2.position));
}

int GenerateBVH(Triangle* begin,Triangle* end,std::vector<BVH>&bvh){
	if(end-begin<=MinCount){
		BVH b{};
		auto min=glm::vec3{begin->v0.position};
		auto max=glm::vec3{begin->v0.position};
		for(auto i=begin;i!=end;++i){
			min=glm::min(min,GetMin(*i));
			max=glm::max(max,GetMax(*i));
		}
		for(int i=0;i<3;++i){
			if(fabs(max[i]-min[i])<MIN_THICKNESS){
				min[i]-=SCALE_THICKNESS;
				max[i]+=SCALE_THICKNESS;
			}
		}
		b.min=glm::vec4{min,0.f};
		b.max=glm::vec4{max,0.f};
		b.count=glm::ivec4{0,0,end-begin,begin-OriginalBegin};
		bvh.push_back(b);
		return bvh.size()-1;
	}
	auto min=glm::vec3(begin->v0.position);
	auto max=glm::vec3{begin->v0.position};
	for(auto i=begin;i!=end;++i){
		min=glm::min(min,GetMin(*i));
		max=glm::max(max,GetMax(*i));
	}
	auto size=max-min;
	auto maxIndex=0;
	for(auto i=1;i<3;++i){
		if(size[i]>size[maxIndex]){
			maxIndex=i;
		}
	}
	for(int i=0;i<3;++i){
		if(fabs(size[i])<MIN_THICKNESS){
			min[i]-=SCALE_THICKNESS;
			max[i]+=SCALE_THICKNESS;
		}
	}
	std::sort(begin,end,[maxIndex](const auto&lhs,const auto&rhs){
		return GetMin(lhs)[maxIndex]<GetMin(rhs)[maxIndex];
	});
	auto midIter=begin+(end-begin)/2;
	auto nowIndex=bvh.size();
	bvh.emplace_back();
	bvh[nowIndex].min=glm::vec4{min,0.f};
	bvh[nowIndex].max=glm::vec4{max,0.f};
	bvh[nowIndex].count.x=GenerateBVH(begin,midIter,bvh);
	bvh[nowIndex].count.y=GenerateBVH(midIter,end,bvh);
	bvh[nowIndex].count.z=-1;
	return nowIndex;
}

void ModelBVH::GenerateBVH(int minCount) {
	::OriginalBegin=AlignedTriangles.data();
	::MinCount=minCount;
	::GenerateBVH(AlignedTriangles.data(),AlignedTriangles.data()+AlignedTriangles.size(),bvh);
}

