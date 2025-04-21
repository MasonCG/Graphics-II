#version 450 core
#extension GL_GOOGLE_include_directive : enable

#include "pushconstants.txt"
#include "uniforms.txt"

#define PI 3.14159265358979323




layout(location=VS_OUTPUT_WORLDPOS) in vec3 worldPosition;


layout(location=0) out vec4 color;

void main(){

	float d = distance(eyePosition, worldPosition);
	
	d = (d-hitherYon[0]) / hitherYon[2];
	
	color = vec4(d,d,d,1);
}
