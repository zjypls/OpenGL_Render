#version 460
layout(location = 0)out vec4 BaseColor;
layout(location = 1)out vec3 Normal;
layout(location = 2)out vec3 wPos;
layout(location = 3)out float Roughness;
layout(location = 4)out float Metallic;
layout(location = 5)out float AO;
layout(location = 6)out float Depth;
layout(location = 7)out ivec2 Index;

layout(binding = 0)uniform sampler2D AOTex;
layout(binding = 1)uniform sampler2D Base;
layout(binding = 2)uniform sampler2D DepthTex;
layout(binding = 3)uniform sampler2D MetallicTex;
layout(binding = 4)uniform sampler2D NormalTex;
layout(binding = 5)uniform sampler2D RoughnessTex;

layout(location = 0) in vec2 aTex;
layout(location = 1) in vec3 pos;
layout(location = 2) in mat3 TBN;
layout(location = 5) in flat ivec2 index;


void main() {
    BaseColor = texture(Base, aTex);
    Normal =TBN*(texture(NormalTex, aTex).rgb*2-1);
    wPos = pos;
    Roughness = texture(RoughnessTex, aTex).r;
    Metallic = texture(MetallicTex, aTex).r;
    AO = texture(AOTex, aTex).r;
    Depth = texture(DepthTex, aTex).r;
    Index=index;
}
