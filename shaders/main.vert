#version 450 core

#extension GL_GOOGLE_include_directive : enable
#include "pushconstants.txt"
#include "uniforms.txt"

layout(location=VS_INPUT_POSITION) in vec3 position;
layout(location=VS_INPUT_TEXCOORD) in vec2 texcoord;
layout(location=VS_INPUT_NORMAL) in vec3 normal;
layout(location=VS_INPUT_TANGENT) in vec4 tangent;
layout(location=VS_INPUT_TEXCOORD2) in vec2 texcoord2;



layout(location=VS_OUTPUT_TEXCOORD) out vec2 v_texcoord;
layout(location=VS_OUTPUT_NORMAL) out vec3 v_normal;
layout(location=VS_OUTPUT_WORLDPOS) out vec3 v_worldpos;
layout(location=VS_OUTPUT_TANGENT) out vec4 v_tangent;
layout(location=VS_OUTPUT_TEXCOORD2) out vec2 v_texcoord2;



void main(){
    vec4 p = vec4(position,1.0);
    p = p * worldMatrix;
    v_worldpos = p.xyz;
	
	// for shadows
	if (doingShadow != 0){
		p = p * flattenMatrix;
	}
	
    p = p * viewProjMatrix;
    gl_Position = p;
    v_texcoord=texcoord;
    //vec4 N = vec4(normal,0.0);
    //N = N * worldMatrix;
    v_normal = normal; //v_normal = N.xyz;
	
	v_tangent = tangent;
	v_texcoord2 = texcoord2;
	
}
