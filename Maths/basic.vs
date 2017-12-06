attribute vec2 a_Position;
attribute vec4 a_Color;

varying vec4 v_Color;

uniform vec4 myColor;


void main(void)
{
	v_Color = myColor;
	gl_Position = vec4(a_Position, 0.0, 1.);
}
