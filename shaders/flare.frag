#version 450 core
#extension GL_GOOGLE_include_directive : enable

#include "importantconstants.h"
#include "pushconstants.txt"


layout(location=0) in vec2 texcoord;
layout(set=0,binding=SUNFBO_TEXTURE_SLOT) uniform texture2DArray sunfbo;


layout(location=0) out vec4 color;

layout(set=0,binding=NEAREST_SAMPLER_SLOT) uniform sampler texSampler;
layout(set=0,binding=BASE_TEXTURE_SLOT) uniform texture2DArray baseColorTexture;

void main(){
    vec4 c = texture(
        sampler2DArray(
            baseColorTexture,texSampler
        ),
        vec3(texcoord,0)
    );

    int numMips = textureQueryLevels(sampler2DArray(sunfbo, texSampler));
    vec4 sunVisibility = texelFetch(
        sampler2DArray(sunfbo, texSampler),
        ivec3(0,0,0),
        numMips-1
    );

    color = c;
    color.a *= sunVisibility.r;     //monochrome texture; choose any channel
    color *= baseColorFactor;

}