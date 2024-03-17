#version 450 core

layout(location = 0 ) out vec4 frag;


layout(location=0)in OutData{
    vec3 pos;
    vec3 normal;
    vec3 tangent;
    vec2 uv;
}data;

layout(binding=3)uniform sampler2D diffuseMap;
layout(binding=4)uniform sampler2D normalMap;
void main(){
    frag=texture(diffuseMap,data.uv);
}