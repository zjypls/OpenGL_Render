#version 460
layout(location=0)in vec4 pos;
layout(location=1)in vec4 normal;
layout(location=2)in vec4 color;

layout(location=0)out vec4 nColor;

uniform mat4 vp;

void main() {
    gl_Position = vp*vec4(pos.xyz, 1.0);
    nColor = normal;
}
