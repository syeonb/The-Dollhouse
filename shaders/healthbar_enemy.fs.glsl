#version 330

// From Vertex Shader
in vec3 vcolor;
in vec2 vpos;

// Application data
uniform vec3 fcolor;
uniform float enemy_health_pct;

// Output color
layout(location = 0) out vec4 color;

void main()
{
	if (vpos.x < enemy_health_pct - 0.5) {
		color = vec4(0.7412, 0.0235, 0.1059, 1.0);
	} else {
		color = vec4(0.172, 0.132, 0.152, 1.0);
	}
}