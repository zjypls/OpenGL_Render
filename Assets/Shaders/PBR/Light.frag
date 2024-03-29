#version 460
layout(location=0)out vec3 Fcolor;
layout(location=1)out vec3 bloom;
layout(location=0)in vec2 tex;

layout(binding = 0)uniform sampler2D Base;
layout(binding = 1)uniform sampler2D Normal;
layout(binding = 2)uniform sampler2D Wpos;
layout(binding = 3)uniform sampler2D Roughness;
layout(binding = 4)uniform sampler2D Metallic;
layout(binding = 5)uniform sampler2D AO;
layout(binding = 6)uniform sampler2D Depth;

layout(std140, binding=12)uniform Camera{
    vec4 cameraPos;
    vec3 lightPos[5];
    vec3 lightCol[5];
};
#define PI 3.1415926

float DistributionGGX(vec3 N, vec3 H, float roughness){
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness){
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness){
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 Frenel(vec3 F0, float costa, float r) {
    return F0 + (max(vec3(1.0 - r), F0) - F0) * pow(1.0 - costa, 5.0);
}


void main() {
    vec3 wpos=texture(Wpos, tex).rgb;
    vec3 viewDir = normalize(cameraPos.xyz - wpos);
    vec3 base=pow(texture(Base, tex).rgb, vec3(2.2f));
    float roughness=texture(Roughness, tex).r;
    float metallic = texture(Metallic, tex).r;
    float ao=texture(AO, tex).r;
    vec3 normal=texture(Normal, tex).rgb;

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, base, metallic);

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < 5; ++i)
    {
        vec3 L = normalize(lightPos[i] - wpos);
        vec3 H = normalize(viewDir + L);
        float distance = length(lightPos[i] - wpos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightCol[i] * attenuation;

        float NDF = DistributionGGX(normal, H, roughness);
        float G   = GeometrySmith(normal, viewDir, L, roughness);
        vec3 F    = Frenel(F0, max(dot(H, viewDir), 0.0), roughness);

        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;

        kD *= 1.0 - metallic;

        float NdotL = max(dot(normal, L), 0.0);

        Lo += (kD * base / PI + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.01) * base * ao;

    vec3 color = ambient + Lo;
    if (length(color)>cameraPos.w){
        bloom= color;
    } else {
        bloom=vec3(0.0);
    }
    Fcolor=color;
}
