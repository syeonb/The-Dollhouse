#version 330

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat3 transform;
uniform mat3 projection;
uniform float frame;
uniform float animation_type;
uniform float frame_width;
uniform float frame_height;

// for advanced geometry shader
uniform int is_squished;
uniform vec2 enemy_position;
uniform float squish_countdown;

void main()
{
	texcoord = in_texcoord;
	texcoord.y += frame_height * animation_type;
	texcoord.x += frame_width * frame;
	float result_x = in_position.x;

	if (is_squished == 1) {
		// increment in_position to the right if it is to top left of enemy_position
		if (in_position.x < 0 && in_position.y < 0) {
			result_x = result_x + abs(sin((50 - squish_countdown) * (3.14 / 50))) * 100/600;
			//result_x = result_x + 150;
		}
	}

	vec3 pos = projection * transform * vec3(result_x, in_position.y, 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}
