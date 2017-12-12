#ifndef GL_ES
#version 110
#endif

attribute vec2 a_Position;
attribute vec4 a_Color;

uniform float u_time;

varying vec4 v_Color;

void main(void)
{
	
	v_Color=a_Color* sqrt(cos(u_time)*cos(u_time));
	gl_Position = vec4(a_Position.x*cos(u_time),a_Position.y*sin(u_time),0.0f, 1.0f);
	//gl_Position = vec4(r*cos(u_time),r*sin(u_time),0.0f, 1.0f);
	gl_PointSize=32.0;
}
