//
// Created by 32725 on 2023/4/15.
//

#ifndef RENDERER003_OGL_MODEL_H
#define RENDERER003_OGL_MODEL_H

#include <filesystem>
#include "OpenGL/VertexArray.h"
#include "OpenGL/Texture.h"

extern "C"{
	struct aiNode;
	struct aiScene;
	struct aiMesh;
};

namespace Z{
	class Model {
	public:
		explicit Model(const std::string& path,int index=0,const glm::vec3&position=glm::vec3{0.f});
		~Model();
		glm::vec3& GetOffset() { return offset; }
		glm::vec3& GetRotate() { return rotate; }
		glm::vec3& GetScale() { return scale; }
		glm::mat4& GetModelMatrix() { return modelMatrix; }
		virtual void Draw(std::shared_ptr<Shader>&shader) const{}//Todo:change

		struct Vertex{
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 texCoord;
			glm::vec3 tangent;
			glm::vec3 bitangent;
			glm::ivec2 Index;
		};
	protected:
		void LoadTexture(const std::filesystem::path& path);
		static std::filesystem::path modelRootPath;
		void LoadModel(const std::string& path);
		void ProcessNode(aiNode* node, const aiScene* scene);
		void ProcessMesh(aiMesh* mesh, const aiScene* scene);
		glm::vec3 offset{},rotate{},scale{};
		glm::mat4 modelMatrix{1.f};
		int Index = 0;
		std::vector<std::shared_ptr<Texture>> textures;
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
	};

}

#endif //RENDERER003_OGL_MODEL_H
