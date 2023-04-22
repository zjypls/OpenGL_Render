//
// Created by 32725 on 2023/4/22.
//

#ifndef OPENGL_RENDER_MODELPBR_H
#define OPENGL_RENDER_MODELPBR_H

#include "Model/Model.h"

class ModelPBR : public Z::Model {
	std::shared_ptr<Z::VertexArray> vertexArray;
	void GenerateVertexArray();
public:
	ModelPBR(const std::string& path,int index=0,const glm::vec3&position=glm::vec3{0.f});
	void Draw(std::shared_ptr<Z::Shader>&shader) const override;
};


#endif //OPENGL_RENDER_MODELPBR_H
