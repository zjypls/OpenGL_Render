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
		explicit Model(const std::string& path,const glm::vec3&position=glm::vec3{0.f},int index=0);
		~Model();
		glm::vec3& GetOffset() { return offset; }
		glm::vec3& GetRotate() { return rotate; }
		glm::vec3& GetScale() { return scale; }
		glm::mat4& GetModelMatrix() { return modelMatrix; }
		auto begin()const { return vertexes.begin(); }
		auto end()const { return vertexes.end(); }
		auto size()const { return vertexes.size(); }
		auto begin() { return vertexes.begin(); }
		auto end() { return vertexes.end(); }
		void Draw(std::shared_ptr<Shader>&shader) const;
	private:
		void LoadTexture(const std::filesystem::path& path);
		static std::filesystem::path modelRootPath;
		void LoadModel(const std::string& path);
		void ProcessNode(aiNode* node, const aiScene* scene);
		void ProcessMesh(aiMesh* mesh, const aiScene* scene);
		glm::vec3 offset{},rotate{},scale{};
		glm::mat4 modelMatrix{1.f};
		int Index = 0;
		std::vector<std::shared_ptr<VertexArray>> vertexes;
		std::vector<std::shared_ptr<Texture>> textures;
	};

}

#endif //RENDERER003_OGL_MODEL_H
