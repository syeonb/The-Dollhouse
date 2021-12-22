#version 330

// From Vertex Shader
in vec3 vcolor;
in vec2 vpos; // Distance from local origin
in float dist;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform int shadow_num;
uniform float is_mac_os;

// Output color
layout(location = 0) out vec4 color;

void main()
{
	vec3 colorA = vec3(0.01, 0.01, 0.01);
	//vec3 colorA = vec3(0, 1, 1);


	//color = vec4(0.0);
	if (is_mac_os == 1) {
		if(shadow_num == 5){
			color = vec4(colorA, 0.01);
		} else {
			color = vec4(colorA, 1.0/((dist-200)/75));
		}
	} else {
		if(shadow_num == 5){
			color = vec4(fcolor * vcolor, 0.01);
		} else {
			color = vec4(fcolor * vcolor, 1.0/((dist-200)/75));
		}
	}

	// Salmon mesh is contained in a 1x1 square
	//float radius = distance(vec2(0.0), vpos);
	
}