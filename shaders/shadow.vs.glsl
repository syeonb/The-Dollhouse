#version 330

// Input attributes
in vec3 in_position;
in vec3 in_color;

out vec3 vcolor;
out vec2 vpos;
out float dist;

// Application data
uniform vec2 player_position;
uniform vec2 shadow_size;
uniform vec2 shadow_pos;
uniform int shadow_num;
uniform mat3 transform;
uniform mat3 projection;

void main()
{

	vpos = in_position.xy; // local coordinated before transform
	vcolor = in_color;


	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	

	vec2 f = vec2(0, 0);

	if(shadow_num < 4){
		f = vec2((0.74 + 0.8 * shadow_num)*600, 0) - vec2(player_position.x, 0);
	}
	
	



	vec2 shadow_vertex =  shadow_pos + vec2((in_position.x) * shadow_size.x /2 , (in_position.y) * shadow_size.y /2);


	vec2 dir = vec2(shadow_vertex.x - f.x, shadow_vertex.y - f.y);
	float total_normalization = sqrt(dir.x*dir.x + dir.y*dir.y);
	dir = dir * 1/total_normalization;

	dist = distance(player_position.xy, vec2(f.x, f.y));
	vec2 new_pos = pos.xy;


	if(shadow_num == 4){
		vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
		gl_Position = vec4(pos.xy, in_position.z, 1.0);
	}
	else if(shadow_vertex.y > player_position.y &&  abs(dist) < 800 && in_position.y > 0){
		float scale_shadow = distance(player_position.xy, vec2(f.x, f.y + 100));
		shadow_vertex = shadow_vertex + dir * scale_shadow/10;
		new_pos = vec2(shadow_vertex.x/600 - 1, 1 - shadow_vertex.y/400);
		gl_Position = vec4(new_pos.xy, 0, 1.0);  //double distance

	 }
	 else {
		vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
		gl_Position = vec4(pos.xy, in_position.z, 1.0);
	}



}

