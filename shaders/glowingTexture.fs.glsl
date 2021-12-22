#version 330

uniform sampler2D screen_texture;
uniform float time;
uniform int blur_state;
uniform vec2 player_position;

in vec2 fragWorldPos;
in vec2 texcoord[11];
in vec2 texcoord2[7];
uniform int bg_sprite;

layout(location = 0) out vec4 color;


void main()
{
    // -1 < fragWorldPos.y < 1
    // -1 < fragWorldPos.x < 1
    color = vec4(0.0);
    if (blur_state == 2){
        color += texture(screen_texture, texcoord2[0]) * 0.0713;
        color += texture(screen_texture, texcoord2[1]) * 0.1315;
        color += texture(screen_texture, texcoord2[2]) * 0.1898;
        color += texture(screen_texture, texcoord2[3]) * 0.2146;
        color += texture(screen_texture, texcoord2[4]) * 0.1898;
        color += texture(screen_texture, texcoord2[5]) * 0.1315;
        color += texture(screen_texture, texcoord2[6]) * 0.0713;
    } else if (blur_state == 1) {
        color += texture(screen_texture, texcoord[0]) * 0.0093;
        color += texture(screen_texture, texcoord[1]) * 0.0280;
        color += texture(screen_texture, texcoord[2]) * 0.0659;
        color += texture(screen_texture, texcoord[3]) * 0.1217;
        color += texture(screen_texture, texcoord[4]) * 0.1757;
        color += texture(screen_texture, texcoord[5]) * 0.1985;
        color += texture(screen_texture, texcoord[6]) * 0.1757;
        color += texture(screen_texture, texcoord[7]) * 0.1217;
        color += texture(screen_texture, texcoord[8]) * 0.0659;
        color += texture(screen_texture, texcoord[9]) * 0.0280;
        color += texture(screen_texture, texcoord[10]) * 0.0093;
    } else {
        color = texture(screen_texture, texcoord[5]);
    }
    // Ambient glow for attic and hallway
    if (bg_sprite == 12){
        color.x -= 0.4;
        color.y -= 0.4;
        color.z -= 0.4;
        if (distance(vec2(player_position.x / 600 - 1, -player_position.y / 400 + 1), fragWorldPos) < 0.3) {
            color.xyz += (0.3 - distance(vec2(player_position.x / 600 - 1, -player_position.y / 400 + 1), fragWorldPos));
        }
    }
    if (bg_sprite == 48){
        color.x -= 0.4;
        color.y -= 0.4;
        color.z -= 0.4;
        if (distance(vec2(player_position.x / 600 - 1, -player_position.y / 400 + 1), fragWorldPos) < 0.3) {
            color.xyz += (0.3 - distance(vec2(player_position.x / 600 - 1, -player_position.y / 400 + 1), fragWorldPos));
        }
        vec2 originVector = vec2(-0.7, 0.77);
        vec2 yellowStar = vec2(-0.705, 0.93);
        vec2 redLightOrigin = vec2(-0.76, 0.8);
        vec2 firePlace = vec2(-0.1, 0.62);
        if (distance(fragWorldPos, yellowStar) < 0.2){
            color.xyz += (1 - (distance(yellowStar, fragWorldPos))) / (distance(yellowStar, fragWorldPos) * 100);
        }
        if (distance(fragWorldPos, firePlace) < 1){
            color.xyz += (1 - (distance(firePlace, fragWorldPos))) / (distance(firePlace, fragWorldPos) * 100)
            * (abs(sin(time/10))) * vec3(1, 1, 0);
        }
        // green lights
        if (distance(fragWorldPos, originVector) < 0.1){
            color.xyz += (1 - (distance(originVector, fragWorldPos) * (abs(sin(time/5) * 10)))) * vec3(0, 1, 0)
            / (distance(originVector, fragWorldPos) * 100);
        }
        originVector -= vec2(0.12, 0.15);
        if (distance(fragWorldPos, originVector) < 0.1) {
            color.xyz += (1 - (distance(originVector, fragWorldPos) * (abs(sin(time/5) * 10)))) * vec3(0, 1, 0)
            / (distance(originVector, fragWorldPos) * 100);
        }
        originVector -= vec2(-0.175, 0.055);
        if (distance(fragWorldPos, originVector) < 0.1) {
            color.xyz += (1 - (distance(originVector, fragWorldPos) * (abs(sin(time/5) * 10)))) * vec3(0, 1, 0)
            / (distance(originVector, fragWorldPos) * 100);
        }
        originVector -= vec2(-0.12, 0.17);
        if (distance(fragWorldPos, originVector) < 0.1) {
            color.xyz += (1 - (distance(originVector, fragWorldPos) * (abs(sin(time/5) * 10)))) * vec3(0, 1, 0)
            / (distance(originVector, fragWorldPos) * 100);
        }
        originVector -= vec2(-0.01, 0.15);
        if (distance(fragWorldPos, originVector) < 0.1) {
            color.xyz += (1 - (distance(originVector, fragWorldPos) * (abs(sin(time/5) * 10)))) * vec3(0, 1, 0)
            / (distance(originVector, fragWorldPos) * 100);
        }
        originVector -= vec2(0.19, 0.07);
        if (distance(fragWorldPos, originVector) < 0.1) {
            color.xyz += (1 - (distance(originVector, fragWorldPos) * (abs(sin(time/5) * 10)))) * vec3(0, 1, 0)
            / (distance(originVector, fragWorldPos) * 100);
        }
        originVector -= vec2(0.01, -0.2);
        if (distance(fragWorldPos, originVector) < 0.1) {
            color.xyz += (1 - (distance(originVector, fragWorldPos) * (abs(sin(time/5) * 10)))) * vec3(0, 1, 0)
            / (distance(originVector, fragWorldPos) * 100);
        }
        originVector -= vec2(0.2, 0.11);
        if (distance(fragWorldPos, originVector) < 0.1) {
            color.xyz += (1 - (distance(originVector, fragWorldPos) * (abs(sin(time/5) * 10)))) * vec3(0, 1, 0)
            / (distance(originVector, fragWorldPos) * 100);
        }
        // red lights
        if (distance(fragWorldPos, redLightOrigin) < 0.1) {
            color.xyz += (1.2 - (distance(redLightOrigin, fragWorldPos) * (10 - abs(sin(time/5) * 10)))) * vec3(1, 0, 0)
            / (distance(redLightOrigin, fragWorldPos) * 100);
        }
        redLightOrigin -= vec2(-0.15, 0.045);
        if (distance(fragWorldPos, redLightOrigin) < 0.1) {
            color.xyz += (1.2 - (distance(redLightOrigin, fragWorldPos) * (10 - abs(sin(time/5) * 10)))) * vec3(1, 0, 0)
            / (distance(redLightOrigin, fragWorldPos) * 100);
        }
        redLightOrigin -= vec2(-0.05, 0.16);
        if (distance(fragWorldPos, redLightOrigin) < 0.1) {
            color.xyz += (1.2 - (distance(redLightOrigin, fragWorldPos) * (10 - abs(sin(time/5) * 10)))) * vec3(1, 0, 0)
            / (distance(redLightOrigin, fragWorldPos) * 100);
        }
        redLightOrigin -= vec2(0.18, 0.025);
        if (distance(fragWorldPos, redLightOrigin) < 0.1) {
            color.xyz += (1.2 - (distance(redLightOrigin, fragWorldPos) * (10 - abs(sin(time/5) * 10)))) * vec3(1, 0, 0)
            / (distance(redLightOrigin, fragWorldPos) * 100);
        }
        redLightOrigin -= vec2(0.09, 0.145);
        if (distance(fragWorldPos, redLightOrigin) < 0.1) {
            color.xyz += (1.2 - (distance(redLightOrigin, fragWorldPos) * (10 - abs(sin(time/5) * 10)))) * vec3(1, 0, 0)
            / (distance(redLightOrigin, fragWorldPos) * 100);
        }
        redLightOrigin -= vec2(-0.01, 0.21);
        if (distance(fragWorldPos, redLightOrigin) < 0.1) {
            color.xyz += (1.2 - (distance(redLightOrigin, fragWorldPos) * (10 - abs(sin(time/5) * 10)))) * vec3(1, 0, 0)
            / (distance(redLightOrigin, fragWorldPos) * 100);
        }
        redLightOrigin -= vec2(-0.21, 0);
        if (distance(fragWorldPos, redLightOrigin) < 0.1) {
            color.xyz += (1.2 - (distance(redLightOrigin, fragWorldPos) * (10 - abs(sin(time/5) * 10)))) * vec3(1, 0, 0)
            / (distance(redLightOrigin, fragWorldPos) * 100);
        }
        redLightOrigin -= vec2(0.015, -0.19);
        if (distance(fragWorldPos, redLightOrigin) < 0.1) {
            color.xyz += (1.2 - (distance(redLightOrigin, fragWorldPos) * (10 - abs(sin(time/5) * 10)))) * vec3(1, 0, 0)
            / (distance(redLightOrigin, fragWorldPos) * 100);
        }
    }
    if (bg_sprite == 47) {
        color.x -= 0.4;
        color.y -= 0.4;
        color.z -= 0.4;
        if (distance(vec2(player_position.x / 600 - 1, -player_position.y / 400 + 1), fragWorldPos) < 0.3) {
            color.xyz += (0.3 - distance(vec2(player_position.x / 600 - 1, -player_position.y / 400 + 1), fragWorldPos));
        }

        vec2 f = -vec2(player_position.x / 600 - 1, 0) + vec2(-1.26, 1.0);
        if (distance(f, fragWorldPos) < 1.0 && fragWorldPos.y < 0.86
        && (f.y - fragWorldPos.y) / 2.8 > (f.x - fragWorldPos.x)
        && (f.y - fragWorldPos.y) / 2.8 > (fragWorldPos.x - f.x)){
            color.xyz += 0.1 / (distance(f, fragWorldPos)) * vec3(1, 1, 0);
            if (abs(sin(time/10)) > 0.999) {
                color.xyz -= 0.2 * abs(sin(time/20));
            }
        }
        f.x += 0.8;
        if (distance(f, fragWorldPos) < 1.0 && fragWorldPos.y < 0.86
        && (f.y - fragWorldPos.y) / 2.8 > (f.x - fragWorldPos.x)
        && (f.y - fragWorldPos.y) / 2.8 > (fragWorldPos.x - f.x)){
            color.xyz += 0.1 / (distance(f, fragWorldPos)) * vec3(1, 1, 0);
            if (abs(sin(time/10)) < 0.90 && abs(sin(time/10)) > 0.89) {
                color.xyz -= 0.2 * abs(sin(time/20));
            }
        }
        f.x += 0.8;
        if (distance(f, fragWorldPos) < 1.0 && fragWorldPos.y < 0.86
        && (f.y - fragWorldPos.y) / 2.8 > (f.x - fragWorldPos.x)
        && (f.y - fragWorldPos.y) / 2.8 > (fragWorldPos.x - f.x)){
            color.xyz += 0.1 / (distance(f, fragWorldPos)) * vec3(1, 1, 0);
            if (abs(sin(time/10)) < 0.72 && abs(sin(time/10)) > 0.70) {
                color.xyz -= 0.2 * abs(sin(time/20));
            }
        }

        f.x += 0.8;
        if (distance(f, fragWorldPos) < 1.0 && fragWorldPos.y < 0.86
        && (f.y - fragWorldPos.y) / 2.8 > (f.x - fragWorldPos.x)
        && (f.y - fragWorldPos.y) / 2.8 > (fragWorldPos.x - f.x)){
            color.xyz += 0.1 / (distance(f, fragWorldPos)) * vec3(1, 1, 0);
            if (abs(sin(time/10)) < 0.30 && abs(sin(time/10)) > 0.28) {
                color.xyz -= 0.2 * abs(sin(time/20));
            }
        }
    }
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