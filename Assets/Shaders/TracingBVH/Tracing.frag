#version 460
layout(location=0) out vec4 FragColor;
layout(location=0)in vec2 TexCoords;
vec2 uv=TexCoords*2-1;
struct BVHNode { vec4 Min;vec4 Max;ivec4 Count; };
struct Ray{ vec3 origin;vec3 direction; };
struct VertexA{ vec4 position;vec4 normal;vec4 texCoord; };
struct Triangle{ VertexA v0, v1, v2; };

layout(std140, binding=0)uniform CameraData{ vec4 cameraRight, cameraUp, cameraPosition, cameraDirection, cameraData; };
layout(binding=0) uniform samplerBuffer BVHNodes;
layout(binding=1)uniform samplerBuffer Triangles;
layout(binding=2)uniform sampler2D base;
uniform int nums;

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
//u:v1,v:v2
bool rayTriangleIntersect(vec3 origin, vec3 direction, Triangle triangle, inout float to, out float u, out float v) {
    vec3 edge1 = triangle.v1.position.xyz - triangle.v0.position.xyz;
    vec3 edge2 = triangle.v2.position.xyz - triangle.v0.position.xyz;
    vec3 pvec = cross(direction, edge2);
    float det = dot(edge1, pvec);
    if (abs(det) < 0.000001) {
        return false;
    }
    float invDet = 1.0 / det;
    vec3 tvec = origin - triangle.v0.position.xyz;
    u = dot(tvec, pvec) * invDet;
    if (u < 0.0 || u > 1.0) {
        return false;
    }
    vec3 qvec = cross(tvec, edge1);
    v = dot(direction, qvec) * invDet;
    if (v < 0.0 || u + v > 1.0) {
        return false;
    }
    float t = dot(edge2, qvec) * invDet;
    if (t<0.001||t>to)return false;
    to=t;
    return true;
}


vec4 GetColor(Ray ray){
    int stack[64];
    int StackIndex=0;
    stack[StackIndex++]=0;
    vec4 Color=vec4(.3,.4,.9,1);
    float t=1000, u, v;
    while (StackIndex>0){
        int nodeIndex=stack[--StackIndex];
        BVHNode node;
        node.Min=texelFetch(BVHNodes, nodeIndex*3);
        node.Max=texelFetch(BVHNodes, nodeIndex*3+1);
        node.Count=floatBitsToInt(texelFetch(BVHNodes, nodeIndex*3+2));
        float tmin, tmax;
        if (HitAABB(ray, node.Min.xyz, node.Max.xyz, tmin, tmax)){
            if (node.Count.z!=-1){
                for (int i=0;i<node.Count.z;i++){
                    int index=node.Count.w+i;
                    Triangle triangle=Triangle(
                    VertexA(texelFetch(Triangles, index*9+0), texelFetch(Triangles, index*9+1), texelFetch(Triangles, index*9+2)),
                    VertexA(texelFetch(Triangles, index*9+3), texelFetch(Triangles, index*9+4), texelFetch(Triangles, index*9+5)),
                    VertexA(texelFetch(Triangles, index*9+6), texelFetch(Triangles, index*9+7), texelFetch(Triangles, index*9+8)));
                    if (rayTriangleIntersect(ray.origin, ray.direction, triangle, t, u, v)){
                        Color= texture(base,triangle.v0.texCoord.rg*(1-u-v)
                        +triangle.v1.texCoord.rg*u
                        +triangle.v2.texCoord.rg*v);
                    }
                }
            }else{
                stack[StackIndex++]=node.Count.y;
                stack[StackIndex++]=node.Count.x;
            }
        }
    }
    return Color;
}


void main() {
    Ray ray;
    ray.origin = cameraPosition.xyz;
    ray.direction = normalize(cameraRight.xyz * uv.x + cameraUp.xyz * uv.y + cameraDirection.xyz*cameraDirection.w);

    //debug
//    float t=1000, u, v;
//    for(int index=0;index<nums;++index){
//        Triangle triangle=Triangle(
//        VertexA(texelFetch(Triangles, index*9+0), texelFetch(Triangles, index*9+1), texelFetch(Triangles, index*9+2)),
//        VertexA(texelFetch(Triangles, index*9+3), texelFetch(Triangles, index*9+4), texelFetch(Triangles, index*9+5)),
//        VertexA(texelFetch(Triangles, index*9+6), texelFetch(Triangles, index*9+7), texelFetch(Triangles, index*9+8)));
//        if (rayTriangleIntersect(ray.origin, ray.direction, triangle, t, u, v)){
//            FragColor= triangle.v0.normal/**u
//            +triangle.v1.normal*v
//            +triangle.v2.normal*(1-u-v)*/;
//            //return;
//        }
//    }
//    if(t>900)
//    FragColor=vec4(0.5, 0.4, 0.1, 1.0);
    FragColor=GetColor(ray);
}
