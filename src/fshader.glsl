#version 150

flat in vec4 f_colour;
out vec4 out_colour;

void main() 
{

	// Inspired by this tutorial
	// https://www.desultoryquest.com/blog/drawing-anti-aliased-circular-points-using-opengl-slash-webgl/
	float r = 0.0, delta = 0.0, alpha = 1.0;
	vec2 cxy = 2.0 * gl_PointCoord - 1.0;
	r = dot(cxy, cxy);
	if (r > 1.0) {
		discard;
	}    
    
	out_colour = f_colour;
}
