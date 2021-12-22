#version 330

// From Vertex Shader
in vec3 vcolor;
in vec2 vpos;

// Application data
uniform vec3 fcolor;
uniform float player_health_pct;
uniform float player_mana_pct;
uniform float is_mac_os;

// Output color
layout(location = 0) out vec4 color;

void main()
{
	if (is_mac_os == 1 && vpos.y < 0.5){
		color = vec4(0.7412, 0.0235, 0.1059, 1.0);
	}
	if (is_mac_os == 0 && vpos.y < 0 || is_mac_os == 1 && vpos.y < 0.5) {
		if (vpos.x < player_health_pct - 0.5) {
			color = vec4(0.7412, 0.0235, 0.1059, 1.0);
		} else {
			color = vec4(0.172, 0.132, 0.152, 1.0);
		}
	} else {
		if (vpos.x < player_mana_pct - 0.5) {
			color = vec4(0.0666, 0.17254, 0.851, 1.0);
		} else {
			color = vec4(0.172, 0.132, 0.152, 1.0);
		}
	}
}