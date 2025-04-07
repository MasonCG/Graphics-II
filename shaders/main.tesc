#version 450 core

#extension GL_GOOGLE_include_directive : enable

#include "importantconstants.h"
#include "pushconstants.txt"
#include "uniforms.txt"

layout(vertices=3) out;

layout(location=VS_OUTPUT_TEXCOORD) in vec2 texcoord[];
layout(location=VS_OUTPUT_TEXCOORD2) in vec2 texcoord2[];
layout(location=VS_OUTPUT_NORMAL) in vec3 normal[];
layout(location=VS_OUTPUT_WORLDPOS) in vec3 worldpos[];
layout(location=VS_OUTPUT_TANGENT) in vec4 tangent[];

layout(location=VS_OUTPUT_TEXCOORD) out vec2 t_texcoord[];
layout(location=VS_OUTPUT_TEXCOORD2) out vec2 t_texcoord2[];
layout(location=VS_OUTPUT_NORMAL) out vec3 t_normal[];
layout(location=VS_OUTPUT_WORLDPOS) out vec3 t_worldpos[];
layout(location=VS_OUTPUT_TANGENT) out vec4 t_tangent[];


void main(){
    gl_TessLevelOuter[0] = tesslevel;
	gl_TessLevelOuter[1] = tesslevel;
	gl_TessLevelOuter[2] = tesslevel;
	gl_TessLevelInner[0] = tesslevel;
    t_texcoord[gl_InvocationID]  = texcoord[gl_InvocationID] ;
    t_texcoord2[gl_InvocationID] = texcoord2[gl_InvocationID];
    t_normal[gl_InvocationID]    = normal[gl_InvocationID]   ;
    t_worldpos[gl_InvocationID]  = worldpos[gl_InvocationID] ;
    t_tangent[gl_InvocationID]   = tangent[gl_InvocationID]  ;
}