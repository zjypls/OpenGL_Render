//
// Created by 32725 on 2023/4/22.
//

//the min thickness of the box,if less than this value,then add the SCALE_THICKNESS to it
#define MIN_THICKNESS 1E-5
#define SCALE_THICKNESS 5E-6

#include "ModelBVH.h"

int MinCount;
Triangle *OriginalBegin;
ModelBVH *OriginalModel;

ModelBVH::ModelBVH(const std::string &path, int index, bool loadTex, const glm::vec3 &position) : Model(path, index,
                                                                                                        loadTex,
                                                                                                        position) {
	for (uint32_t i = 0; i < indices.size(); i += 3) {
		Triangle t{};
		t.v0.position = glm::vec4{vertices[indices[i]].position, 1.f};
		t.v0.normal = glm::vec4{vertices[indices[i]].normal, 1.f};
		t.v0.texCoord = glm::vec4{vertices[indices[i]].texCoord, 0.f, 0.f};

		t.v1.position = glm::vec4{vertices[indices[i + 1]].position, 1.f};
		t.v1.normal = glm::vec4{vertices[indices[i + 1]].normal, 1.f};
		t.v1.texCoord = glm::vec4{vertices[indices[i + 1]].texCoord, 0.f, 0.f};

		t.v2.position = glm::vec4{vertices[indices[i + 2]].position, 1.f};
		t.v2.normal = glm::vec4{vertices[indices[i + 2]].normal, 1.f};
		t.v2.texCoord = glm::vec4{vertices[indices[i + 2]].texCoord, 0.f, 0.f};

		AlignedTriangles.push_back(t);
	}
}

glm::vec3 GetMin(const Triangle &a) {
	return glm::min(a.v0.position, glm::min(a.v1.position, a.v2.position));
}

glm::vec3 GetMax(const Triangle &a) {
	return glm::max(a.v0.position, glm::max(a.v1.position, a.v2.position));
}

int GenerateBVH(Triangle *begin, Triangle *end, std::vector<BVH> &bvh) {
	auto min = glm::vec3(begin->v0.position);
	auto max = glm::vec3{begin->v0.position};
	for (auto i = begin; i != end; ++i) {
		min = glm::min(min, GetMin(*i));
		max = glm::max(max, GetMax(*i));
	}

	auto size = max - min;
	for (int i = 0; i < 3; ++i) {
		if (fabs(size[i]) < MIN_THICKNESS) {
			min[i] -= SCALE_THICKNESS;
			max[i] += SCALE_THICKNESS;
		}
	}
	OriginalModel->GetBVHVisualize().push_back({min, max});
	auto nowIndex = bvh.size();
	bvh.emplace_back();
	bvh[nowIndex].min = glm::vec4{min, 0.f};
	bvh[nowIndex].max = glm::vec4{max, 0.f};
	if (end - begin <= MinCount) {
		bvh[nowIndex].count = glm::ivec4{0, 0, end - begin, begin - OriginalBegin};
		return nowIndex;
	}
	auto maxIndex = 0;
	for (auto i = 1; i < 3; ++i) {
		if (size[i] > size[maxIndex]) {
			maxIndex = i;
		}
	}
	std::sort(begin, end, [maxIndex](const auto &lhs, const auto &rhs) {
		return GetMin(lhs)[maxIndex] < GetMin(rhs)[maxIndex];
	});
	auto midIter = begin + (end - begin) / 2;
	bvh[nowIndex].count.x = GenerateBVH(begin, midIter, bvh);
	bvh[nowIndex].count.y = GenerateBVH(midIter, end, bvh);
	bvh[nowIndex].count.z = -1;
	return nowIndex;
}

void ModelBVH::GenerateBVH(int minCount) {
	::OriginalBegin = AlignedTriangles.data();
	::MinCount = minCount;
	::OriginalModel = this;
	::GenerateBVH(AlignedTriangles.data(), AlignedTriangles.data() + AlignedTriangles.size(), bvh);
}

#ifdef BVH_VISUAL_INFO

std::pair<std::shared_ptr<Z::VertexArray>, std::shared_ptr<Z::Shader>> ModelBVH::GetBVHVisualizeVAO() {
	auto result = std::make_shared<Z::VertexArray>();
	std::vector<uint32_t> indices;
	std::vector<glm::vec3> BVHVertices;
	for (const auto &box: BVHVisualize) {
		BVHVertices.push_back(box.min);
		BVHVertices.emplace_back(box.min.x, box.min.y, box.max.z);
		BVHVertices.emplace_back(box.min.x, box.max.y, box.max.z);
		BVHVertices.emplace_back(box.min.x, box.max.y, box.min.z);
		BVHVertices.emplace_back(box.max.x, box.min.y, box.min.z);
		BVHVertices.emplace_back(box.max.x, box.min.y, box.max.z);
		BVHVertices.emplace_back(box.max);
		BVHVertices.emplace_back(box.max.x, box.max.y, box.min.z);
	}
	for (int i = 0; i < BVHVisualize.size(); ++i) {
		indices.push_back(i * 8);
		indices.push_back(i * 8 + 1);
//		indices.push_back(i * 8 + 3);
//		indices.push_back(i * 8 + 1);
		indices.push_back(i * 8 + 2);
		indices.push_back(i * 8 + 3);

		indices.push_back(i * 8 + 2);
		indices.push_back(i * 8 + 6);
		indices.push_back(i * 8 + 7);
//		indices.push_back(i * 8 + 2);
//		indices.push_back(i * 8 + 7);
		indices.push_back(i * 8 + 3);

		indices.push_back(i * 8 + 6);
		indices.push_back(i * 8 + 5);
		indices.push_back(i * 8 + 4);
//		indices.push_back(i * 8 + 6);
//		indices.push_back(i * 8 + 4);
		indices.push_back(i * 8 + 7);

		indices.push_back(i * 8 + 5);
		indices.push_back(i * 8 + 1);
		indices.push_back(i * 8);
//		indices.push_back(i * 8+5);
//		indices.push_back(i * 8);
		indices.push_back(i * 8 + 4);
	}
	result->AddVertexBuffer((float *) BVHVertices.data(), sizeof(BVHVertices[0]), BVHVertices.size(),
	                        {{"pos", GL_FLOAT, 3, sizeof(float)}});
	result->SetIndexBuffer(indices.data(), indices.size());

	const char *vertCode = R"(#version 460
	layout(location=0)in vec3 pos;
	uniform mat4 vp;
	void main() {
        gl_Position = vp*vec4(pos.xyz, 1.0);
	})";
	const char *fragCode = R"(#version 460
	layout(location=0)out vec4 color;
	void main() {
		color = vec4(1.0, 0.0, 0.0, 1.0);
	})";
	auto shader = std::make_shared<Z::Shader>();
	shader->AddShader(vertCode, GL_VERTEX_SHADER,false);
	shader->AddShader(fragCode, GL_FRAGMENT_SHADER,false);
	shader->Link();
	return {result, shader};
}

#endif

