#ifndef GL_ES
#version 110
#endif

uniform float u_time;
varying vec4 v_Color;


void main(void)
{
	gl_FragColor= v_Color;
	//gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
