#version 330 core
out vec3 color;
uniform float time;
uniform vec2 pt;

void main()
{
	mat2 tr;
	vec2 p=gl_PointCoord+vec2(0.5,0.5);
	mat3 m;
	m[0]=vec3(cos(1.5*time)/4+0.75,0,0);
	m[1]=vec3(0,0.2*sin(time)+0.8,0);
	m[2]=vec3(0,0,sin(time)*0.1+0.8);
	color=m*vec3(p*distance(gl_PointCoord,pt)*1.5,sin(time)*0.1+0.8);
}
