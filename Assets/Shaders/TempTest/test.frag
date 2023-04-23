#version 460
layout(location= 0) out vec4 color;
layout(location= 0) in vec4 normal;
void main() {
    color = vec4(normal.xyz, 1.0);
}
