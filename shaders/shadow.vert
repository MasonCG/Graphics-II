#version 450 core

#extension GL_GOOGLE_include_directive : enable
#include "pushconstants.txt"
#include "uniforms.txt"

layout(location=VS_INPUT_POSITION) in vec3 position;


layout(location=VS_OUTPUT_WORLDPOS) out vec3 v_worldpos;




void main(){
    vec4 p = vec4(position,1.0);
    p = p * worldMatrix;
    v_worldpos = p.xyz;
	
	p = p * viewProjMatrix;
    gl_Position = p;
	
}
