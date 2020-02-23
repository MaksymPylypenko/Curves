#version 150

in vec4 vPosition;
in vec4 vColor;
flat out vec4 f_colour;


void main()
{
	gl_Position = vPosition;	
	f_colour = vColor; 	
}