//
// Created by z on 24-3-13.
//

#ifndef OPENGL_RENDER_ANIMATIONMODEL_H
#define OPENGL_RENDER_ANIMATIONMODEL_H
#include <vector>
#include "glm/glm/glm.hpp"
#include "OpenGL/Texture.h"
#include "OpenGL/Shader.h"
#include "Animation.h"
#include "OpenGL/VertexArray.h"
#include "OpenGL/UniformBuffer.h"

namespace Z {

    class AnimationModel {
    public:
        struct Vertex{
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec3 tangent;
            glm::vec2 texCoord;
            glm::ivec4 bone_ids{-1};
            glm::vec4 bone_weight{0};
        };
        AnimationModel(const std::string& path);
        void Update(float deltaTime);
        auto& GetMatrices(){return animator->GetMatrices();}

        void Draw(std::shared_ptr<Shader>&shader);

    private:
        void ProcessNode(aiNode* node,const aiScene* scene);
        void ProcessMesh(aiMesh* mesh,const aiScene* scene);
        void LoadModel(const std::string& path);

        Animation* animation;
        Animator* animator;
        std::map<std::string,BoneInfo> boneMap;
        UniformBuffer boneMatData;
        std::shared_ptr<VertexArray> vertexArray;
        glm::vec3 offset{},rotate{},scale{};
        glm::mat4 modelMatrix=glm::scale(glm::mat4{1.f},glm::vec3{1});
        std::vector<std::shared_ptr<Texture>> textures;
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };

} // Z

#endif //OPENGL_RENDER_ANIMATIONMODEL_H
