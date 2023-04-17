#version 460
layout(location = 0)out vec4 BaseColor;
layout(location = 1)out vec3 Normal;
layout(location = 2)out vec3 wPos;
layout(location = 3)out vec4 Roughness;
layout(location = 4)out vec4 Metallic;
layout(location = 5)out vec4 AO;
layout(location =  6)out float Depth;

layout(binding = 0)uniform sampler2D AOTex;
layout(binding = 1)uniform sampler2D Base;
layout(binding = 2)uniform sampler2D DepthTex;
layout(binding = 3)uniform sampler2D MetallicTex;
layout(binding = 4)uniform sampler2D NormalTex;
layout(binding = 5)uniform sampler2D RoughnessTex;

layout(location = 0) in vec2 aTex;
layout(location = 1) in vec3 pos;
layout(location = 2) in mat3 TBN;


void main() {
    BaseColor = texture(Base, aTex);
    Normal =texture(NormalTex, aTex).rgb;
    wPos = pos;
    Roughness.rgb = TBN[0];
    Metallic.rgb = TBN[1];
    AO.rgb = TBN[2];
    Roughness.a = texture(RoughnessTex, aTex).r;
    Metallic.a = texture(MetallicTex, aTex).r;
    AO.a = texture(AOTex, aTex).r;
    Depth = texture(DepthTex, aTex).r;
}
