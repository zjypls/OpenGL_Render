#version 460

struct Ray{ vec3 origin;vec3 direction; };
layout(location=0)out vec4 color;
layout(location=0)in vec2 texcoord;
layout(binding=0)uniform sampler2D BefFrame;
layout(binding=0) uniform Data{ vec4 horizon;vec4 vertical;vec4 origin;vec4 direction; };

vec2 uv=texcoord*2-1;

void getRay(out Ray ray){
    ray.origin=origin.xyz;
    ray.direction=direction.xyz+uv.x*horizon.xyz+uv.y*vertical.xyz;
}


vec4 GetColor(Ray ray){
    vec3 direction=normalize(ray.direction);
    float t=0.5+0.5*direction.y;
    return mix(vec4(1, 1, 1, 1), vec4(0.5, 0.7, 1, 1), t);
}
void main() {
    Ray ray;
    getRay(ray);
    color=GetColor(ray);
}
