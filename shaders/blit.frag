#version 450 core
#extension GL_GOOGLE_include_directive : enable

#include "importantconstants.h"

layout(location=0) in vec2 texcoord;

layout(location=0) out vec4 color;

layout(set=0,binding=NEAREST_SAMPLER_SLOT) uniform sampler texSampler;
layout(set=0,binding=BASE_TEXTURE_SLOT) uniform texture2DArray baseColorTexture;



vec3 hsv2rgb(vec3 hsv)
{
	float h = hsv[0];
	float s = hsv[1];
	float v = hsv[2];
	float r,g,b;
	if(s == 0.0)
		r=g=b=v;
	else{
		h=h/60.0;
		float ipart = floor(h);
		float fpart = h-ipart;
		float A=v*(1.0-s);
		float B=v*(1.0-s*fpart);
		float C=v*(1.0-s*(1.0-fpart));
		if(ipart == 0.0){
			r=v; g=C; b=A;
		} else if( ipart==1.0){
			r=B; g=v; b=A;
		} else if( ipart==2.0 ){
			r=A; g=v; b=C;
		} else if( ipart==3.0){
			r=A; g=B; b=v;
		} else if( ipart==4.0 ){
			r=C; g=A; b=v;
		} else {
			r=v; g=A; b=B;
		}
	}
	return vec3(r,g,b);
	}

vec3 rgb2hsv(vec3 color)
{
	float r=color.r;
	float g=color.g;
	float b=color.b;
	float mx = max(r,max(g,b));
	float mn = min(r,min(g,b));
	float h,s,v;

	v = mx;    //value
	if(mx != 0.0 ){
		s = (mx-mn)/mx;    //saturation
		if(r == mx)
			h = (g-b)/(mx-mn);
		else if(g == mx)
			h = 2.0+(b-r)/(mx-mn);
		else
			h = 4.0+(r-g)/(mx-mn);
		h *= 60.0;
		if( h < 0.0 )
			h += 360.0;
	} else {
		h=s=0.0;
	}
	
	return vec3(h,s,v);
}
	
#define deltaSaturation -0.25;

void main(){


	
	/*
   ivec2 coord = ivec2(gl_FragCoord.xy);
    mat3 C = mat3( 0,  1, 0,
                   1, -4, 1,
                   0,  1, 0 );
    vec4 total = vec4(0);
    for(int i=0;i<10;++i){
        for(int j=0;j<10;++j){
            ivec3 c = ivec3(coord,0);
            c.x += i;
            c.y += j;
            total += C[i][j] * texelFetch(
                sampler2DArray(baseColorTexture,texSampler),
                c,
                0
            );
        }
    }
	
    color = vec4(1) - 10*total;
    color.a = 1.0;
	
	ivec2 tsize = textureSize(sampler2DArray(baseColorTexture,texSampler),0).xy;
    ivec2 T = ivec2(round(texcoord * tsize));

    //ivec2 I = ivec2(gl_FragCoord.xy);
    int rem = T.y % 5;
    T.y -= rem;
    vec4 c = texelFetch(
        sampler2DArray(baseColorTexture,texSampler),
        ivec3( T, 0 ),
        0                   //mip level
    );

    if( rem != 1 )
        c.rgb *=0.50;
    color += c;
	
	*/
	
	vec4 c = texture(
			sampler2DArray(
				baseColorTexture,texSampler
			),
			vec3(texcoord,0)
		);
		
	ivec2 coord = ivec2(gl_FragCoord.xy);
		
		color = c;
		
}