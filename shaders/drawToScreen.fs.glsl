#version 330

uniform sampler2D screen_texture;
uniform float time;
uniform int blur_state;

in vec2 texcoord[11];
in vec2 texcoord2[7];

layout(location = 0) out vec4 color;


void main()
{
	color = vec4(0.0);
//	if (blur_state == 2){
//		color += texture(screen_texture, texcoord2[0]) * 0.0713;
//		color += texture(screen_texture, texcoord2[1]) * 0.1315;
//		color += texture(screen_texture, texcoord2[2]) * 0.1898;
//		color += texture(screen_texture, texcoord2[3]) * 0.2146;
//		color += texture(screen_texture, texcoord2[4]) * 0.1898;
//		color += texture(screen_texture, texcoord2[5]) * 0.1315;
//		color += texture(screen_texture, texcoord2[6]) * 0.0713;
//	} else if(blur_state == 1) {
//		color += texture(screen_texture, texcoord[0]) * 0.0093;
//		color += texture(screen_texture, texcoord[1]) * 0.0280;
//		color += texture(screen_texture, texcoord[2]) * 0.0659;
//		color += texture(screen_texture, texcoord[3]) * 0.1217;
//		color += texture(screen_texture, texcoord[4]) * 0.1757;
//		color += texture(screen_texture, texcoord[5]) * 0.1985;
//		color += texture(screen_texture, texcoord[6]) * 0.1757;
//		color += texture(screen_texture, texcoord[7]) * 0.1217;
//		color += texture(screen_texture, texcoord[8]) * 0.0659;
//		color += texture(screen_texture, texcoord[9]) * 0.0280;
//		color += texture(screen_texture, texcoord[10]) * 0.0093;
//	} else {
		color = texture(screen_texture, texcoord[5]);
//}
	

	
	
}
// Guassian Blur distribution from https://observablehq.com/@jobleonard/gaussian-kernel-calculater
//  0: 0.009300040045324049
// 1: 0.028001560233780885
//2: 0.06598396774984912
//3: 0.12170274650962626
//4: 0.17571363439579307
//5: 0.19859610213125314
//6: 0.17571363439579307
//7: 0.12170274650962626
//8: 0.06598396774984912
//9: 0.028001560233780885
//10: 0.009300040045324049