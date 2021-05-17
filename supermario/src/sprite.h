#pragma once
static const char* sprite_crt_run_image_path = "../bin/textures/Biker_run.png";
static const char* sprite_crt_idle_image_path = "../bin/textures/Biker_idle.png";
static const char* sprite_crt_jump_image_path = "../bin/textures/Biker_jump.png";
static const char* sprite_enemy_run_image_path = "../bin/textures/Cyborg_run.png";
static const char* sprite_enemy_idle_image_path = "../bin/textures/Cyborg_idle.png";
static const char* sprite_enemy_jump_image_path = "../bin/textures/Cyborg_jump.png";

static const char* sprite_heart_image_path = "../bin/textures/heart.png";

std::vector<uint> sprite_indices = { 0, 2, 1, 1, 2, 3 };
std::vector<vertex> sprite_vertices = {
	{{-0.5f,-0.5f,0}, {0,0,1}, {0,0}},
	{{-0.5f, 0.5f, 0}, {0, 0, 1}, {0, 1}},
	{{0.5f,-0.5f,0}, {0,0,1}, {1,0}},
	{{0.5f, 0.5f, 0}, {0, 0, 1},{1, 1}}
};

std::vector<uint> circle_indices;
std::vector<vertex> create_circle_vertices(uint N)
{
	std::vector<vertex> v = { { {0,0,0}, {0,1,0}, {0,0} } }; // origin
	for (uint k = 0; k <= N; k++)
	{
		float t = PI * 2.0f * k / float(N), c = cos(t), s = sin(t);
		v.push_back({ {c,0,s}, {0,1,0},{0,0} });
	}
	for (uint k = 0; k < N; k++)
	{
		
		circle_indices.push_back(0);	// the origin
		circle_indices.push_back(k + 2);	
		circle_indices.push_back(k + 1);
		
	}
	return v;
}