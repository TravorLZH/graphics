#version 330 core
layout(location=0) in vec3 pos;
uniform float time;

void main()
{
	mat2 m;
	float k=0.85+0.15*cos(4*time);
	m[0]=k*vec2(cos(time),sin(time));
	m[1]=k*vec2(-sin(time),cos(time));
	gl_Position=vec4(pos.xy,0,1);
}
