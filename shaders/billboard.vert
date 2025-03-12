#version 450 core

#extension GL_GOOGLE_include_directive : enable

#include "importantconstants.h"
#include "pushconstants.txt"
#include "uniforms.txt"

layout(location=VS_INPUT_TEXCOORD) in vec2 texcoord;
layout(set=0,binding=BILLBOARD_TEXTURE_SLOT) uniform
                    textureBuffer billboardPositions;

layout(location=0) out vec2 v_texcoord;


void main(){

    //p holds center of billboard
    vec4 p = texelFetch( billboardPositions, gl_InstanceIndex );
    p.w = 1.0;

    //put in world space
    p = p * worldMatrix;
		
	// put into eye space
	p = p * viewMatrix;

	// textcoord in range 0...1
	//	multiple by 2 -> 0...2
	// subtract 1 -> -1...1
	p.xy += 2.0 * texcoord - vec2(1.0);

	p = p * projMatrix;
	gl_Position = p;
	v_texcoord = texcoord;


    //...
}