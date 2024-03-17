//
// Created by z on 24-3-13.
//
#include <iostream>
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "AnimationModel.h"
#include "glm/glm/gtc/type_ptr.hpp"
#include "glm/glm/gtx/string_cast.hpp"

static Z::BufferLayout layout={
        Z::BufferLayout::Element{"Pos",GL_FLOAT,3,sizeof(float),false,0},
        Z::BufferLayout::Element{"Normal",GL_FLOAT,3,sizeof(float),false,sizeof(float)*3},
        Z::BufferLayout::Element{"Tangent",GL_FLOAT,3,sizeof(float),false,sizeof(float)*6},
        Z::BufferLayout::Element{"TexCoord",GL_FLOAT,2,sizeof(float),false,sizeof(float)*9},
        Z::BufferLayout::Element{"BoneIndex",GL_INT,4,sizeof(int),false,sizeof(float)*11},
        Z::BufferLayout::Element{"BoneWeight",GL_FLOAT,4,sizeof(float),false,sizeof(float)*11+sizeof(int)*4}
};

namespace Z {
    const std::string modelPath=Z_SOURCE_DIR"/Assets/Model/";
    AnimationModel::AnimationModel(const std::string &path) {
        LoadModel(modelPath + path);
    }

    void AnimationModel::ProcessNode(aiNode *node, const aiScene *scene) {
        for (uint32_t i = 0; i < node->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            ProcessMesh(mesh, scene);
        }
        for (uint32_t i = 0; i < node->mNumChildren; i++) {
            ProcessNode(node->mChildren[i], scene);
        }
    }


    void AnimationModel::ProcessMesh(aiMesh *mesh, const aiScene *scene) {
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
                vertex.tangent.x = mesh->mTangents[i].x;
                vertex.tangent.y = mesh->mTangents[i].y;
                vertex.tangent.z = mesh->mTangents[i].z;
            }
            vertices.push_back(vertex);
        }
        int BoneCounter=0;
        for(int i=0;i<mesh->mNumBones;++i){
            auto&bone=mesh->mBones[i];
            std::string boneName=bone->mName.C_Str();
            int boneID=BoneCounter;
            if(boneMap.find(boneName)==boneMap.end()){
                BoneInfo info{};
                info.offset= AssimpMatrixToGLM(mesh->mBones[i]->mOffsetMatrix);
                info.id=BoneCounter;
                boneMap[boneName]=info;
                ++BoneCounter;
            }else{
                boneID=boneMap[boneName].id;
            }
            for(int j=0;j<bone->mNumWeights;++j){
                auto vertexID=bone->mWeights[j];
                auto& vertex=vertices[vertexID.mVertexId];
                for(int k=0;k<4;++k){
                    if(vertex.bone_ids[k]<0){
                        vertex.bone_ids[k]=boneID;
                        vertex.bone_weight[k]=vertexID.mWeight;
                        break;
                    }
                }
            }
        }
        for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (uint32_t j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
    }

    void AnimationModel::LoadModel(const std::string &path) {
        Assimp::Importer importer{};
        auto scene=importer.ReadFile(path,aiProcess_CalcTangentSpace|aiProcess_Triangulate|aiProcess_FlipUVs);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }
        ProcessNode(scene->mRootNode,scene);
        animation = new Animation(scene,boneMap);
        animator=new Animator(animation);

        auto vertexBuffer=std::make_shared<Z::VertexBuffer>(vertices.data(),sizeof(Vertex),vertices.size());
        vertexBuffer->SetLayout(layout);
        auto indexBuffer=std::make_shared<IndexBuffer>(indices.data(),indices.size());
        vertexArray=std::make_shared<VertexArray>(vertexBuffer,indexBuffer);
        boneMatData=UniformBuffer(animator->GetMatrices().data(),animator->GetMatrices().size()*sizeof(glm::mat4));
    }

    void AnimationModel::Update(float deltaTime) {
        animator->Update(deltaTime);
        boneMatData.SetData(animator->GetMatrices().data(),animator->GetMatrices().size()*sizeof(glm::mat4));
    }

    void AnimationModel::Draw(std::shared_ptr<Shader> &shader) {
        shader->Bind();
        shader->SetUniform("model",&modelMatrix[0][0]);
        boneMatData.Bind(2);
        vertexArray->Draw();

    }

} // Z