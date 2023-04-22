//
// Created by 32725 on 2023/4/22.
//

#include "ModelPBR.h"
static Z::BufferLayout layout = {
		Z::BufferLayout::Element{"Pos",GL_FLOAT,3,sizeof(float),false,0},
		Z::BufferLayout::Element{"Normal",GL_FLOAT,3,sizeof(float),false,sizeof(float)*3},
		Z::BufferLayout::Element{"TexCoord",GL_FLOAT,2,sizeof(float),false,sizeof(float)*6},
		Z::BufferLayout::Element{"Tangent",GL_FLOAT,3,sizeof(float),false,sizeof(float)*8},
		Z::BufferLayout::Element{"Bitangent",GL_FLOAT,3,sizeof(float),false,sizeof(float)*11},
		Z::BufferLayout::Element{"Index",GL_INT,2,sizeof(int),false,sizeof(float)*14}
};
ModelPBR::ModelPBR(const std::string &path, int index, const glm::vec3 &position) : Model(path, index, position) {
	GenerateVertexArray();
}

void ModelPBR::GenerateVertexArray() {
	auto vertexBuffer=std::make_shared<Z::VertexBuffer>(vertices.data(),vertices.size()*sizeof(Z::Model::Vertex));
	auto indexBuffer=std::make_shared<Z::IndexBuffer>(indices.data(),indices.size());
	vertexBuffer->SetLayout(layout);
	vertexArray=std::make_shared<Z::VertexArray>(vertexBuffer,indexBuffer);
}

void ModelPBR::Draw(std::shared_ptr<Z::Shader> &shader) const {
	shader->Bind();
	shader->SetUniform("model",&modelMatrix[0][0]);
	for(int i=0;i<textures.size();i++){
		textures[i]->Bind(i);
	}
	vertexArray->Draw();
}
