#version 450 core

layout(location = 0 ) in vec3 pos;
layout(location = 1 ) in vec3 normal;
layout(location = 2 ) in vec3 tangent;
layout(location = 3 ) in vec2 uv;
layout(location = 4 ) in ivec4 bone_index;
layout(location = 5 ) in vec4 bone_weights;

layout(binding=0) uniform CameraData{
    mat4 pv;
}camera;
uniform mat4 model;
layout(binding=2) uniform AnimModel{
    mat4 trans[52];
} animModel;

layout(location=0)out OutData{
    vec3 pos;
    vec3 normal;
    vec3 tangent;
    vec2 uv;
}voData;


void main(){
    vec4 localPos=vec4(0);
    vec4 alignPos=vec4(pos,1);
    if(bone_index.x>=0){
        localPos+= animModel.trans[bone_index.x]*alignPos * bone_weights.x;
    }
    if(bone_index.y>=0){
        localPos+= animModel.trans[bone_index.y]*alignPos  * bone_weights.y;
    }
    if(bone_index.z>=0){
        localPos+= animModel.trans[bone_index.z]*alignPos  * bone_weights.z;
    }
    if(bone_index.w>=0){
        localPos+= animModel.trans[bone_index.w]*alignPos * bone_weights.w;
    }
    gl_Position=camera.pv*model*localPos;
    voData.pos=pos;
    voData.normal=normal;
    voData.tangent=tangent;
    voData.uv=uv;
}


