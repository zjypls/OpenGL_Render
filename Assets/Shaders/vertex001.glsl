#version 460
layout(location = 0) in vec3 aPos;
layout(location=1)in vec3 aNormal;
layout(location= 2)in vec2 aTexCoord;
layout(location=3)in vec3 aTangent;
layout(location=4)in vec3 aBitangent;
layout(location=0)out vec2 TexCoord;
layout(location=1)out vec3 pos;
layout(location=2)out mat3 TBN;

layout(std140, binding=7) uniform Matrices {
    mat4 viewProjection;
};


void main() {
    gl_Position = viewProjection*vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    pos = aPos;
    TBN = mat3(aTangent, aBitangent, aNormal);
}
