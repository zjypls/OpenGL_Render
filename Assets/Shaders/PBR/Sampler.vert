#version 460
layout(location = 0) in vec3 aPos;
layout(location=1)in vec3 aNormal;
layout(location= 2)in vec2 aTexCoord;
layout(location=3)in vec3 aTangent;
layout(location=4)in vec3 aBitangent;
layout(location=5)in ivec2 Index;
layout(location=0)out vec2 TexCoord;
layout(location=1)out vec3 pos;
layout(location=2)out mat3 TBN;
layout(location=5)out flat ivec2 index;

layout(std140, binding=7) uniform Matrices {
    mat4 viewProjection;
};
uniform mat4 model;

void main() {
    gl_Position = viewProjection*model*vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    pos =(model *vec4( aPos,1.f)).xyz;
    TBN = mat3(aTangent, aBitangent, aNormal);
    index=Index;
}
