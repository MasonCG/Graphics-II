#version 450 core
layout(location=0) in vec2 texcoord;
layout(location=0) out vec4 color;
void main(){
    float d = distance(texcoord,vec2(0.5,0.5));
    if( d < 0.5 )
        color = vec4(1,1,1,1);
    else
        color = vec4(0,0,0,1);
}