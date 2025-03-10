#version 450 core

#extension GL_GOOGLE_include_directive : enable
#include "pushconstants.txt"
#include "uniforms.txt"

layout(location=VS_INPUT_POSITION) in vec3 position;

layout(location=VS_OUTPUT_WORLDPOS) out vec3 objpos;




void main(){
	objpos = position;
    vec4 p = vec4(position,1.0);
	p.xyz += eyePosition; //keep skybox centered around viewer
	
    p = p * viewProjMatrix;
    p.z = p.w; // put skybox at yon clip plane
			   // rendering it as the furthest object in the scene
	gl_Position = p;
	

}
