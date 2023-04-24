#version 460
#define LAMBERT 1
#define MIRROR 2
#define GLASS 3
#define META 4
#define LIGHT 5
#define FOG 6

struct Ray{ vec3 origin;vec3 direction; };
struct Sphere{ vec4 center;vec4 color;ivec4 info; };
struct Plane{ vec4 v0, v1, v2, v3, minV, maxV;vec4 normal;vec4 color;ivec4 info; };
//Info.x:type,Info.y:eta,Info.z:glossy ,info.w:texID
//if type==FOG,Info.y: fog thickness
struct HitRecord{ float t;vec3 p;vec3 normal;vec4 color;ivec4 Info;float RefrectRatio; };
layout(location=0)out vec4 color;
layout(location=0)in vec2 texcoord;
layout(binding=0)uniform sampler2D otherFrame;
layout(binding=1)uniform sampler2D Textures[6];
layout(binding=0) uniform Data{ vec4 horizon;vec4 vertical;vec4 origin;vec4 direction;ivec4 control; };
layout(binding=1)uniform World{
    Sphere spheres[7];
    Plane planes[2];
    vec4 count;
};

uniform int gamaMode;


vec2 uv=texcoord*2-1;
uint wseed=uint((gl_FragCoord.x+gl_FragCoord.y*control.z)*control.y);
int depth=control.x;

uint whash(){
    wseed = (wseed ^ uint(61)) ^ (wseed >> uint(16));
    wseed *= uint(9);
    wseed = wseed ^ (wseed >> uint(4));
    wseed *= uint(0x27d4eb2d);
    wseed = wseed ^ (wseed >> uint(15));
    return wseed;
}
float Random(){
    return float(whash())/4294967296.0;
}
vec3 GetRandomVec(vec3 iNormal){
    return vec3(Random()*2-1, Random()*2-1, Random()*2-1)+iNormal;
}
// Christophe Schlick approximation
float reflectance(float cosine, float ref_idx) {
    float r0 = (1-ref_idx) / (1+ref_idx);
    r0 = r0*r0;
    return r0 + (1-r0)*pow((1 - cosine),5.0);
}

void getRay(vec2 UV, out Ray ray){
    ray.origin=origin.xyz;
    ray.direction=normalize(direction.xyz*direction.w+UV.x*horizon.xyz+UV.y*vertical.xyz);
}

void HitSphere(in Sphere sphere, inout Ray ray, inout HitRecord record){
    vec3 oc=ray.origin-sphere.center.xyz;
    float a=dot(ray.direction, ray.direction);
    float b=dot(oc, ray.direction);
    float c=dot(oc, oc)-sphere.center.w*sphere.center.w;
    float discriminant=b*b-a*c;
    if (discriminant>0){
        discriminant=sqrt(discriminant);
        float t=(-b-discriminant)/a;
        if (t<record.t&&t>0.001){
            if(sphere.info.x==FOG){
                float len=discriminant/(2*a);
                float tl=(sphere.info.y/100.f)*Random();
                if(tl>len){
                    return;
                }else{
                    record.t=t+tl;
                    record.color=sphere.color;
                    record.Info=sphere.info;
                }
            }else{
                record.t=t;
                record.p=ray.origin+t*ray.direction;
                record.normal=(record.p-sphere.center.xyz)/sphere.center.w;
                record.color=sphere.color;
                record.Info=sphere.info;
            }
        }
    }
}

void HitPlane(in Plane plane, in Ray ray, inout HitRecord record){
    float t0=dot(plane.normal.xyz, ray.direction);
    if (abs(t0)<1E-4)return;
    float t= dot(plane.normal.xyz, plane.v0.xyz-ray.origin)/t0;
    if (t>record.t||t<0.001)return;
    vec3 p=ray.origin+t*ray.direction;
    if (p.x<plane.minV.x||p.x>plane.maxV.x||p.y<plane.minV.y||p.y>plane.maxV.y||p.z<plane.minV.z||p.z>plane.maxV.z)return;
    record.t=t;
    record.p=p;
    record.normal=plane.normal.xyz;
    record.color=plane.color;
    record.Info=plane.info;
}

bool HitWorld(inout Ray ray, inout HitRecord record){
    for (int i=0;i<count.x;i++){
        HitSphere(spheres[i], ray, record);
    }
    return record.t<1E2;
}

void ProcessResult(inout Ray ray, inout HitRecord record){
    switch (record.Info.x){
        case LAMBERT:
        ray.direction=GetRandomVec(record.normal);
        ray.origin=record.p;
        record.RefrectRatio=.65;
        break;
        case MIRROR:
        ray.direction=reflect(ray.direction, record.normal);//add a vector to make a glossy effect
        ray.origin=record.p;
        record.RefrectRatio=.97;
        break;
        case GLASS:
        float n=100.f/record.Info.y;
        float cosi=dot(ray.direction, record.normal);
        float etai=1, etat=n;
        vec3 n1=record.normal;
        if (cosi<0){
            cosi=-cosi;
        } else {
            etai=n;
            etat=1;
            n1=-record.normal;
        }
        float eta=etai/etat;
        float k=1-eta*eta*(1-cosi*cosi);
        if (k<0 || reflectance(cosi, eta)>Random() ){
            ray.direction=reflect(ray.direction, record.normal);
            ray.origin=record.p;
        } else {
            ray.direction=refract(ray.direction,record.normal,eta);//normalize(eta*ray.direction+(eta*cosi-sqrt(k))*n1);
            ray.origin=record.p;
        }
        record.RefrectRatio=.97;
        break;
        case META:
        ray.direction=reflect(ray.direction, record.normal)+GetRandomVec(record.normal)*(record.Info.z/100.f);//add a vector to make a glossy effect
        ray.origin=record.p;
        record.RefrectRatio=.85;
        break;
        case FOG:
        ray.origin=ray.origin+ray.direction*record.t;
        ray.direction=GetRandomVec(ray.direction);
        record.RefrectRatio=.55;
        break;
    }
    if (record.Info.w!=0){
        vec2 texC=vec2(atan(record.normal.x, record.normal.z)/(2*3.1415926)+.5, acos(record.normal.y)/3.1415926);
        record.color=texture(Textures[record.Info.w-1], texC);
    }
}

vec4 GetColor(Ray ray){
    depth=control.x;
    HitRecord record;
    record.t=1E3;
    vec4 Color=vec4(1, 1, 1, 1);
    while (HitWorld(ray, record)){
        if (--depth<0)
        return vec4(0, 0, 0, 0);
        if (record.Info.x==LIGHT){
            return record.color*Color;
        }
        record.t=1E3;
        ProcessResult(ray, record);
        Color*=record.color*record.RefrectRatio;
    }

    vec3 direction=normalize(ray.direction);
    float t=0.5+0.5*direction.y;
    return mix(vec4(1, 1, 1, 1), vec4(0.5, 0.7, 1, 1), t)*Color;
}
void main() {

//    if (control.y>700){
//        color=texture(otherFrame, texcoord);
//        return;
//    }

    vec2 dsize=1.f/control.zw;
    vec4 temp=vec4(0, 0, 0, 0);
    //Todo: optimize
    for (int i=-1;i<1;++i){
        for (int j=-1;j<1;++j){
            vec2 UV=uv+vec2(i*dsize.x, j*dsize.y);
            Ray ray;
            getRay(UV, ray);
            temp+=GetColor(ray);
        }
    }
    color= temp/4.f;
    //Todo: optimize
    if (gamaMode!=0){
        color.rgb/=color.rgb+vec3(1, 1, 1);
        color.rgb=pow(color.rgb, vec3(1.f/2.2f));
    }
    color.rgb = color.rgb*(1.f/control.y)+texture(otherFrame, texcoord).rgb*((control.y-1.f)/control.y);
    color.a=1;
}
