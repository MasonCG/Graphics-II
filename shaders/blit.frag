#version 450 core
#extension GL_GOOGLE_include_directive : enable

#include "importantconstants.h"

layout(location=0) in vec2 texcoord;

layout(location=0) out vec4 color;

layout(set=0,binding=NEAREST_SAMPLER_SLOT) uniform sampler texSampler;
layout(set=0,binding=BASE_TEXTURE_SLOT) uniform texture2DArray baseColorTexture;

void main(){


	vec4 c = vec4(0);
	vec2 p1 = texcoord;
	float weight = 1.0;
	float totalweight = 0.0;
	int numsteps = 5;
	float stepsize = 0.05;
	vec2 blur_center = vec2(0.5, 0.5);
	
	vec2 ray = p1-blur_center;
	ray *= stepsize;


	for (int i = 0; i < numsteps; i++){
		vec4 fetched = texture(
			sampler2DArray(
				baseColorTexture,texSampler
			),
			vec3(p1,0)
		);
		
		fetched *= weight;
		c += fetched;
		totalweight += weight;
		weight *= 0.9;
		p1 += ray;
		
	}
	
	c /= totalweight;
	
	color = c;
	color.a = 1.0;

}