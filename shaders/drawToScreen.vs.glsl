#version 330

in vec3 in_position;

out vec2 texcoord[11];
out vec2 texcoord2[7];

void main(void)
{
    gl_Position = vec4(in_position.xy, 0, 1.0);
	vec2 center_texcoord = (in_position.xy) / 2.f + 0.5;
	float pixel_frags = 1.0/ 200;

	for(int i = -5; i <= 5; i++){
		float pixel = pixel_frags * i;
		texcoord[i+5] = center_texcoord + vec2(pixel, 0.0);
	}

	vec2 center_texcoord2 = (in_position.xy) / 2.f + 0.5;
	float pixel_frags2 = 1.0/ 11;

	for(int i = -3; i <= 3; i++){
		float pixel2 = pixel_frags2 * i;
		texcoord2[i+3] = center_texcoord2 + vec2(pixel2, 0.0);
	}
}
