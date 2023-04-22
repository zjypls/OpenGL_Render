//
// Created by 32725 on 2023/4/15.
//
#include "assimp/mesh.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <iostream>
#include "Model.h"
namespace Z{
	static std::string texturesName[6]{"ao","base","depth","meta","normal","rough"};

	//using namespace Assimp;
	std::filesystem::path Model::modelRootPath = std::filesystem::path(Z_MODULE_SOURCE_DIR);


	Model::Model(const std::string &path,int index,const glm::vec3&position):offset(position),Index(index) {
		LoadModel(path);
		auto dir=path.substr(0,path.rfind('/')+1)+"Textures/";
		for(std::string& name:texturesName){
			LoadTexture(modelRootPath.string()+dir+name+".png");
		}
	}

	void Model::LoadModel(const std::string &path) {
		//load model
		Assimp::Importer importer;
		auto scene = importer.ReadFile((modelRootPath/path).string(), aiProcess_Triangulate | aiProcess_FlipUVs);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return;
		}
		ProcessNode(scene->mRootNode, scene);
	}

	void Model::ProcessNode(aiNode *node, const aiScene *scene) {
		for (uint32_t i = 0; i < node->mNumMeshes; i++) {
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			ProcessMesh(mesh, scene);
		}
		for (uint32_t i = 0; i < node->mNumChildren; i++) {
			ProcessNode(node->mChildren[i], scene);
		}
	}

	void Model::ProcessMesh(aiMesh *mesh, const aiScene *scene) {
		int index = 0;
		for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
			Vertex vertex{};
			vertex.position.x = mesh->mVertices[i].x;
			vertex.position.y = mesh->mVertices[i].y;
			vertex.position.z = mesh->mVertices[i].z;
			if (mesh->mTextureCoords[0]) {
				vertex.texCoord.x = mesh->mTextureCoords[0][i].x;
				vertex.texCoord.y = mesh->mTextureCoords[0][i].y;
			}
			else {
				vertex.texCoord = { 0.0f,0.0f };
			}
			vertex.normal.x = mesh->mNormals[i].x;
			vertex.normal.y = mesh->mNormals[i].y;
			vertex.normal.z = mesh->mNormals[i].z;
			if (mesh->mTangents) {
				vertex.tangent.x = mesh->mTangents[i].x;
				vertex.tangent.y = mesh->mTangents[i].y;
				vertex.tangent.z = mesh->mTangents[i].z;
			}
			if (mesh->mBitangents) {
				vertex.bitangent.x = mesh->mBitangents[i].x;
				vertex.bitangent.y = mesh->mBitangents[i].y;
				vertex.bitangent.z = mesh->mBitangents[i].z;
			}
			vertex.Index= glm::ivec2(Index, index++);
			vertices.push_back(vertex);
		}
		for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (uint32_t j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}
//		auto vertexBuffer=std::make_shared<VertexBuffer>(vertices.data(), vertices.size() * sizeof(Vertex));
//		vertexBuffer->SetLayout(layout);
//		auto indexBuffer=std::make_shared<IndexBuffer>(indices.data(), indices.size());
//		auto vertexArray=std::make_shared<VertexArray>();
//		vertexArray->AddVertexBuffer(vertexBuffer);
//		vertexArray->SetIndexBuffer(indexBuffer);
//		vertexes.push_back(vertexArray);
	}

	Model::~Model() {
	}

//	void Model::Draw(std::shared_ptr<Shader>&shader) const {
//		shader->Bind();
//		shader->SetUniform("model",&modelMatrix[0][0]);
//		for(int i=0;i<textures.size();i++){
//			textures[i]->Bind(i);
//		}
//		for (auto& vertex : vertexes) {
//			vertex->Draw();
//		}
//	}

	void Model::LoadTexture(const std::filesystem::path& path) {
		auto texture = std::make_shared<Texture>(path.string());
		textures.push_back(texture);
	}
}
