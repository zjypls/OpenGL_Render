#version 460
layout(location=0) out vec4 FragColor;
layout(location=0)in vec2 TexCoords;
vec2 uv=TexCoords*2-1;
struct BVHNode {
    vec4 Min;
    vec4 Max;
    ivec4 Count;
};
struct Ray{ vec3 origin;vec3 direction; };
layout(std140, binding=0) uniform BVHBuffer {
    BVHNode nodes[];
};
layout(std140,binding=1)uniform CameraData{
    vec4 cameraRight;
    vec4 cameraUp;
    vec4 cameraPosition;
    vec4 cameraDirection;
    vec4 data;
};

bool HitAABB(Ray ray, vec3 Min, vec3 Max, out float tmin, out float tmax) {
    vec3 invD = 1.0 / ray.direction;
    vec3 t0s = (Min - ray.origin) * invD;
    vec3 t1s = (Max - ray.origin) * invD;
    vec3 tsmaller = min(t0s, t1s);
    vec3 tbigger = max(t0s, t1s);
    tmin = max(max(tsmaller.x, tsmaller.y), tsmaller.z);
    tmax = min(min(tbigger.x, tbigger.y), tbigger.z);
    return tmax > max(tmin, 0.0);
}


void main() {
    Ray ray;
    ray.origin = cameraPosition.xyz;
    ray.direction = normalize(cameraRight.xyz * uv.x + cameraUp.xyz * uv.y + cameraDirection.xyz);
    float tmin,tmax;
    if (HitAABB(ray,nodes[0].Min.xyz,nodes[0].Max.xyz,tmin,tmax))
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    else
        FragColor = vec4(0.0, 1.0, 0.0, 1.0);
}
