#version 460
layout(location=0)out vec4 color;
layout(location=0)in vec2 tex;
layout(binding=0)uniform sampler2D originalColor;
layout(binding=1)uniform sampler2D bloomTex;

layout(std140, binding= 9)uniform PostProcessData{
    vec2 frameSize;
};

// 9*9 gaussian blur
//have multiplier 1000
const float gauss[9][9] = {
{ 1.79106e-05, 0.000593119, 0.00722567, 0.0323832, 0.0533908, 0.0323832, 0.00722567, 0.000593119, 1.79106e-05 },
{ 0.000593119, 0.0196414, 0.239281, 1.07238, 1.76806, 1.07238, 0.239281, 0.0196414, 0.000593119 },
{ 0.00722567, 0.239281, 2.91504, 13.0643, 21.5394, 13.0643, 2.91504, 0.239281, 0.00722567 },
{ 0.0323832, 1.07238, 13.0643, 58.5502, 96.5329, 58.5502, 13.0643, 1.07238, 0.0323832 },
{ 0.0533908, 1.76806, 21.5394, 96.5329, 159.156, 96.5329, 21.5394, 1.76806, 0.0533908 },
{ 0.0323832, 1.07238, 13.0643, 58.5502, 96.5329, 58.5502, 13.0643, 1.07238, 0.0323832 },
{ 0.00722567, 0.239281, 2.91504, 13.0643, 21.5394, 13.0643, 2.91504, 0.239281, 0.00722567 },
{ 0.000593119, 0.0196414, 0.239281, 1.07238, 1.76806, 1.07238, 0.239281, 0.0196414, 0.000593119 },
{ 1.79106e-05, 0.000593119, 0.00722567, 0.0323832, 0.0533908, 0.0323832, 0.00722567, 0.000593119, 1.79106e-05 },
};

void main() {
    vec3 sourceColor = texture(originalColor, tex).rgb;
    vec2 stepSize=1.f/textureSize(bloomTex, 0);
    vec3 bloom=vec3(0.f);
    //Todo : seperate vertical and horizontal blur to reduce calculation
    for (int i=-4;i<5;++i){
        for (int j=-4;j<5;++j){
            vec2 tTex=tex+vec2(i, j)*stepSize;
            bloom+=texture(bloomTex, tTex).rgb*gauss[i+4][j+4];
        }
    }

    bloom=bloom/1000.f+sourceColor;
    color=vec4(pow(bloom/(bloom+1.f), vec3(1/2.2f)), 1.f);
}
