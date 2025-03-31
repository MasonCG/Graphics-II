#version 450 core
#extension GL_GOOGLE_include_directive : enable

#include "pushconstants.txt"
#include "uniforms.txt"

#define PI 3.14159265358979323



layout(location=VS_OUTPUT_TEXCOORD) in vec2 texcoord;
layout(location=VS_OUTPUT_NORMAL) in vec3 normal;
layout(location=VS_OUTPUT_WORLDPOS) in vec3 worldPosition;
layout(location=VS_OUTPUT_TANGENT) in vec4 tangent;
layout(location=VS_OUTPUT_TEXCOORD2) in vec2 texcoord2;

layout(set=0,binding=NEAREST_SAMPLER_SLOT) uniform sampler nearestSampler;
layout(set=0,binding=LINEAR_SAMPLER_SLOT) uniform sampler linearSampler;
layout(set=0,binding=MIPMAP_SAMPLER_SLOT) uniform sampler mipSampler;
layout(set=0,binding=BASE_TEXTURE_SLOT) uniform texture2DArray baseTexture;
layout(set=0,binding=EMISSIVE_TEXTURE_SLOT) uniform texture2DArray emitTexture;
layout(set=0,binding=NORMAL_TEXTURE_SLOT) uniform texture2DArray normalTexture;
layout(set=0,binding=METALLICROUGHNESS_TEXTURE_SLOT) uniform texture2DArray metallicRoughnessTexture;
layout(set=0,binding=ENVMAP_TEXTURE_SLOT) uniform textureCube environmentMap;



layout(location=0) out vec4 color;



#define AMBIENT_ABOVE vec3(0.3,0.3,0.3)
#define AMBIENT_BELOW vec3(0.1,0.1,0.1)


void computeLightContribution(int i, vec3 N, vec3 V, float roughness, float metalicity, vec3 d, vec3 F0, vec3 one_minus_F0,
                            out vec3 diff, out vec3 spec){

    vec3 lightPosition = lightPositionAndDirectionalFlag[i].xyz;
    float positional = lightPositionAndDirectionalFlag[i].w;
    vec3 lightColor = lightColorAndIntensity[i].xyz;
    float intensity = lightColorAndIntensity[i].w;
    vec3 spotDir = spotDirection[i].xyz ;
    float cosSpotInnerAngle = cosSpotAngles[i].x;
    float cosSpotOuterAngle = cosSpotAngles[i].y;

    vec3 L = normalize( lightPosition - worldPosition );

    vec3 R = reflect(-L,N);
	vec3 H = L+V;
	H = normalize(H);
	
	//theta angles
	float cos_NL = dot(N, L);
	float sin_NL = pow(1 - pow(cos_NL, 2), 0.5);
	float cos_VH = dot(V, H);
	float cos_NH = dot(N, H);
	
	float cos_NV = dot(N, V);
	cos_NV = clamp(cos_NV, 0.0, 1.0);
	
	// clamping theta angles
	cos_NL = clamp(cos_NL, 0.0, 1.0);
	sin_NL = clamp(sin_NL, 0.0, 1.0);
	cos_VH = clamp(cos_VH, 0.0, 1.0);
	cos_NH = clamp(cos_NH, 0.0, 1.0);
	
	vec3 F = F0 + one_minus_F0 * pow(1.0 - cos_VH,5.0);
	
	float roughness2 = roughness*roughness;
	
	float disc1 = max(0.0,
		roughness2 + (1-roughness2)*(cos_NV * cos_NV)
	);
	
	float disc2 = max(0.0,
		roughness2 + (1-roughness2)*(cos_NL * cos_NL)
	);
	
	float tmp = roughness / (1.0 + cos_NH * cos_NH* (roughness2-1) );
	float Dis = 1.0/PI * tmp*tmp;
	
	float denom = max (0.0001,
		cos_NL * sqrt(disc1) + 
		cos_NV * sqrt(disc2)
	);
	
	float Vis = 1.0 / (2.0 * denom);
	
	vec3 sp = cos_NL * F * Vis * Dis; 
	diff = cos_NL * ( vec3(1.0)-F) * d/PI;
	

    float D = distance( lightPosition, worldPosition );
    float A = 1.0/( D*(attenuation[2] * D   +
                       attenuation[1]     ) +
                       attenuation[0]     );
    A = clamp(A, 0.0, 1.0);

    diff *= A;
    sp *= A;

    float cosineTheta = dot(-L,spotDir);
    float spotAttenuation = smoothstep(
                    cosSpotOuterAngle,
                    cosSpotInnerAngle,
                    cosineTheta);
    diff *= spotAttenuation;
    sp *= spotAttenuation;

    diff = diff * lightColor * intensity;
    spec = sp * lightColor * intensity;
}

vec3 doBumpMapping(vec3 b, vec3 N)
{

    if( tangent.w == 0.0 )
        return N;
		

    vec3 T = tangent.xyz;
	T = T - (dot(T, N)*N);
	T = normalize(T);
	
	vec3 B = cross(N, T);
	B = normalize(B);
	B = B * tangent.w;
	
	vec3 beta = 2.0*(b.rgb - vec3(0.5));
	
	beta = beta * normalFactor;
	
	mat3 mapping = mat3(T.x, B.x, N.x, T.y, B.y, N.y, T.z, B.z, N.z);
	
	N = beta * mapping;

    return N;       //bump mapped normal
}


void main(){


	if( doingReflections == 1 ){
        if( dot(vec4(worldPosition,1.0),reflectionPlane) < 0 ){
            discard;
            return;
        }
    }


	vec3 b = texture(sampler2DArray(normalTexture, mipSampler),
                    vec3(texcoord2,0.0) ).xyz;

    vec3 N = normalize(normal);
    N = doBumpMapping(b.xyz, N);
	
	N = (vec4(N,0) * worldMatrix).xyz;
	N = normalize(N);

    vec3 tc = vec3(texcoord,0.0);
    vec4 c = texture(
        sampler2DArray(baseTexture,mipSampler), tc
    );
    c = c * baseColorFactor;
	


    float mappedY = 0.5 * (N.y+1.0);
    vec3 ambient = c.rgb * mix( AMBIENT_BELOW, AMBIENT_ABOVE, mappedY );

    vec3 totaldp = vec3(0.0);
    vec3 totalsp = vec3(0.0);
	
	
	
	
	// working on relflecting amd metalicity
	
	
	
	vec3 V = normalize(eyePosition - worldPosition);
	
	
	vec3 MR = texture(sampler2DArray(metallicRoughnessTexture, mipSampler),
				vec3(texcoord2,0.0) ).xyz;
	
	float metalicity = MR.z;
	float roughness = MR.y;
	metalicity *= metallicFactor;
	roughness *= roughnessFactor;
	

	vec3 reflectedView = reflect(-V,N);


   vec3 reflectionColor = texture(
    samplerCube(environmentMap, mipSampler),
    reflectedView,
    roughness*8.0
	).rgb;
	
	vec3 d = mix(0.96*c.rgb, vec3(0), metalicity);
	vec3 F0 = mix(vec3(0.04) , c.rgb , metalicity); 
	vec3 one_minus_F0 = vec3(1.0)-F0;


    for(int i=0;i<MAX_LIGHTS;++i){
        vec3 dp, sp;
        computeLightContribution(i,N, V, roughness, metalicity, d, F0, one_minus_F0, dp,sp);

        totaldp += dp ;
        totalsp += sp ;
    }
	
	
    vec3 ec = texture(
        sampler2DArray(emitTexture,mipSampler), tc
    ).rgb;
    ec = ec * emissiveFactor;
	
	


    c.rgb = c.rgb * (ambient + totaldp) + totalsp ;
    c.rgb += ec.rgb;
    c.rgb = clamp(c.rgb, vec3(0.0), vec3(1.0));
	
    color = c;
	
	if (doingReflections != 2){
		color.rgb += pow(1.0-roughness,4.0) * metalicity * reflectionColor;
	}

	
	if( doingReflections == 2 )
		color.a *= 0.85;




}
