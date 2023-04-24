#version 460
#define LAMBERT 1
#define MIRROR 2
#define GLASS 3
#define META 4
#define LIGHT 5
#define FOG 6
#define PI 3.141592
struct Ray{ vec3 origin;vec3 direction; };
struct Sphere{ vec4 center;vec4 color;ivec4 info; };
struct Plane{ vec4 v0, v1, v2, v3, minV, maxV;vec4 normal;vec4 color;ivec4 info; };
//Info.x:type,Info.y:eta,Info.z:glossy ,info.w:texID
//if type==FOG,Info.y: fog thickness
struct HitRecord{ float t;vec3 p;vec3 normal;vec4 color;ivec4 Info;float RefrectRatio;uint bou; };
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
vec3 toNormalHemisphere(vec3 v, vec3 N) {
    vec3 helper = vec3(1, 0, 0);
    if (abs(N.x)>0.999) helper = vec3(0, 0, 1);
    vec3 tangent = normalize(cross(N, helper));
    vec3 bitangent = normalize(cross(N, tangent));
    return normalize(v.x * tangent + v.y * bitangent + v.z * N);
}

vec3 GetRandomVec(vec3 iNormal){
    return normalize(vec3(Random()*2-1, Random()*2-1, Random()*2-1)+iNormal);
}

// 1 ~ 8 维的 sobol 生成矩阵

const uint V[8*32] = {
2147483648u, 1073741824u, 536870912u, 268435456u, 134217728u, 67108864u, 33554432u, 16777216u, 8388608u, 4194304u, 2097152u, 1048576u, 524288u, 262144u, 131072u, 65536u, 32768u, 16384u, 8192u, 4096u, 2048u, 1024u, 512u, 256u, 128u, 64u, 32u, 16u, 8u, 4u, 2u, 1u,
2147483648u, 3221225472u, 2684354560u, 4026531840u, 2281701376u, 3422552064u, 2852126720u, 4278190080u,
2155872256u, 3233808384u, 2694840320u, 4042260480u, 2290614272u, 3435921408u, 2863267840u, 4294901760u,
2147516416u, 3221274624u, 2684395520u, 4026593280u, 2281736192u, 3422604288u, 2852170240u, 4278255360u,
2155905152u, 3233857728u, 2694881440u, 4042322160u, 2290649224u, 3435973836u, 2863311530u, 4294967295u,
2147483648u, 3221225472u, 1610612736u, 2415919104u, 3892314112u, 1543503872u, 2382364672u, 3305111552u,
1753219072u, 2629828608u, 3999268864u, 1435500544u, 2154299392u, 3231449088u, 1626210304u, 2421489664u,
3900735488u, 1556135936u, 2388680704u, 3314585600u, 1751705600u, 2627492864u, 4008611328u, 1431684352u,
2147543168u, 3221249216u, 1610649184u, 2415969680u, 3892340840u, 1543543964u, 2382425838u, 3305133397u,
2147483648u, 3221225472u, 536870912u, 1342177280u, 4160749568u, 1946157056u, 2717908992u, 2466250752u,
3632267264u, 624951296u, 1507852288u, 3872391168u, 2013790208u, 3020685312u, 2181169152u, 3271884800u,
546275328u, 1363623936u, 4226424832u, 1977167872u, 2693105664u, 2437829632u, 3689389568u, 635137280u,
1484783744u, 3846176960u, 2044723232u, 3067084880u, 2148008184u, 3222012020u, 537002146u, 1342505107u,
2147483648u, 1073741824u, 536870912u, 2952790016u, 4160749568u, 3690987520u, 2046820352u, 2634022912u,
1518338048u, 801112064u, 2707423232u, 4038066176u, 3666345984u, 1875116032u, 2170683392u, 1085997056u,
579305472u, 3016343552u, 4217741312u, 3719483392u, 2013407232u, 2617981952u, 1510979072u, 755882752u,
2726789248u, 4090085440u, 3680870432u, 1840435376u, 2147625208u, 1074478300u, 537900666u, 2953698205u,
2147483648u, 1073741824u, 1610612736u, 805306368u, 2818572288u, 335544320u, 2113929216u, 3472883712u,
2290089984u, 3829399552u, 3059744768u, 1127219200u, 3089629184u, 4199809024u, 3567124480u, 1891565568u,
394297344u, 3988799488u, 920674304u, 4193267712u, 2950604800u, 3977188352u, 3250028032u, 129093376u,
2231568512u, 2963678272u, 4281226848u, 432124720u, 803643432u, 1633613396u, 2672665246u, 3170194367u,
2147483648u, 3221225472u, 2684354560u, 3489660928u, 1476395008u, 2483027968u, 1040187392u, 3808428032u,
3196059648u, 599785472u, 505413632u, 4077912064u, 1182269440u, 1736704000u, 2017853440u, 2221342720u,
3329785856u, 2810494976u, 3628507136u, 1416089600u, 2658719744u, 864310272u, 3863387648u, 3076993792u,
553150080u, 272922560u, 4167467040u, 1148698640u, 1719673080u, 2009075780u, 2149644390u, 3222291575u,
2147483648u, 1073741824u, 2684354560u, 1342177280u, 2281701376u, 1946157056u, 436207616u, 2566914048u,
2625634304u, 3208642560u, 2720006144u, 2098200576u, 111673344u, 2354315264u, 3464626176u, 4027383808u,
2886631424u, 3770826752u, 1691164672u, 3357462528u, 1993345024u, 3752330240u, 873073152u, 2870150400u,
1700563072u, 87021376u, 1097028000u, 1222351248u, 1560027592u, 2977959924u, 23268898u, 437609937u
};

// 格林码
uint grayCode(uint i) { return i ^ (i>>1); }

// 生成第 d 维度的第 i 个 sobol 数
float sobol(uint d, uint i) {
    uint result = 0;
    uint offset = d * 32;
    for (uint j = 0;i!=0; i >>= 1, j++){
        if ((i & 1u)!=0){
            result ^= V[j+offset];
        }
    }

    return float(result) * (1.0f/float(0xFFFFFFFFU));
}

// 生成第 i 帧的第 b 次反弹需要的二维随机向量
vec2 sobolVec2(uint i, uint b) {
    float u = sobol(b*2, grayCode(i));
    float v = sobol(b*2+1, grayCode(i));
    return vec2(u, v);
}

vec2 CranleyPattersonRotation(vec2 p) {
    float u = Random();
    float v = Random();

    p.x += u;
    if (p.x>1) p.x -= 1;
    if (p.x<0) p.x += 1;

    p.y += v;
    if (p.y>1) p.y -= 1;
    if (p.y<0) p.y += 1;

    return p;
}

vec3 SampleHemisphere(vec2 UV, vec3 normal) {
    vec2 rUV = CranleyPattersonRotation(UV);
    float z = rUV.x;
    float r = max(0, sqrt(1.0 - z*z));
    float phi = 2.0 * PI * UV.y;
    return toNormalHemisphere(vec3(r * cos(phi), r * sin(phi), z), normal);
}

vec3 SampleCosineHemisphere(vec2 UV, vec3 N) {

    vec2 rUV = CranleyPattersonRotation(UV);
    // 均匀采样 xy 圆盘然后投影到 z 半球
    float r = sqrt(rUV.x);
    float theta = rUV.y * 2.0 * PI;
    float x = r * cos(theta);
    float y = r * sin(theta);
    float z = sqrt(1.0 - x*x - y*y);

    // 从 z 半球投影到法向半球
    vec3 L = toNormalHemisphere(vec3(x, y, z), N);
    return L;
}

// Christophe Schlick approximation
float reflectance(float cosine, float ref_idx) {
    float r0 = (1-ref_idx) / (1+ref_idx);
    r0 = r0*r0;
    return r0 + (1-r0)*pow((1 - cosine), 5.0);
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
            if (sphere.info.x==FOG){
                float len=discriminant/(2*a);
                float tl=(sphere.info.y/100.f)*Random();
                if (tl>len){
                    return;
                } else {
                    record.t=t+tl;
                    record.color=sphere.color;
                    record.Info=sphere.info;
                }
            } else {
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
    vec2 sobolUV=sobolVec2(control.y, record.bou);
    switch (record.Info.x){
        case LAMBERT:
        ray.direction=SampleCosineHemisphere(sobolUV, record.normal);//GetRandomVec(record.normal);
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
        if (reflectance(cosi, eta)>Random()){
            ray.direction=reflect(ray.direction, record.normal)+/*GetRandomVec(record.normal)*/SampleCosineHemisphere(sobolUV, record.normal)*(record.Info.z/100.f);
            ray.origin=record.p;
        } else {
            ray.direction=refract(ray.direction, record.normal, eta)+/*GetRandomVec(record.normal)*/SampleCosineHemisphere(sobolUV, record.normal)*(record.Info.z/100.f);//normalize(eta*ray.direction+(eta*cosi-sqrt(k))*n1);
            ray.origin=record.p;
        }
        record.RefrectRatio=.97;
        break;
        case META:
        ray.direction=reflect(ray.direction, record.normal)+/*GetRandomVec(record.normal)*/SampleCosineHemisphere(sobolUV, record.normal)*(record.Info.z/100.f);//add a vector to make a glossy effect
        ray.origin=record.p;
        record.RefrectRatio=.85;
        break;
        case FOG:
        ray.origin=ray.origin+ray.direction*record.t;
        ray.direction=SampleCosineHemisphere(sobolUV, ray.direction);//GetRandomVec(ray.direction);
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
    record.bou=0;
    vec4 Color=vec4(1, 1, 1, 1);
    while (HitWorld(ray, record)){
        if (--depth<0)
        return vec4(0, 0, 0, 0);
        if (record.Info.x==LIGHT){
            return record.color*Color;
        }
        record.bou;
        record.t=1E3;
        ProcessResult(ray, record);
        Color*=record.color*record.RefrectRatio;
    }

    vec3 direction=normalize(ray.direction);
    float t=0.5+0.5*direction.y;
    return mix(vec4(1, 1, 1, 1), vec4(0.5, 0.7, 1, 1), t)*Color;
}
#define SAMPLES 2
#define MULTIS (SAMPLES*SAMPLES*4)
#define dSize sqrt(SAMPLES)
void main() {

    //    if (control.y>7000){
    //        color=texture(otherFrame, texcoord);
    //        return;
    //    }

    vec2 dsize=1.f/control.zw/dSize;
    vec4 temp=vec4(0, 0, 0, 0);
    //Todo: optimize
    Ray ray;
    for (int i=-SAMPLES;i<SAMPLES;++i){
        for (int j=-SAMPLES;j<SAMPLES;++j){
            vec2 UV=uv+vec2(i*dsize.x, j*dsize.y);
            getRay(UV, ray);
            temp+=GetColor(ray);
        }
    }
    color= temp/MULTIS;
    //Todo: optimize
    if (gamaMode!=0){
        color.rgb/=color.rgb+vec3(1, 1, 1);
        color.rgb=pow(color.rgb, vec3(1.f/2.2f));
    }
    vec3 before=texture(otherFrame, texcoord).rgb;
    color.rgb = color.rgb*(1.f/control.y)+texture(otherFrame, texcoord).rgb*((control.y-1.f)/control.y);
    color.a=1;
}
