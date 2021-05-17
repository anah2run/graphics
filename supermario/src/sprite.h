#pragma once
static const char* sprite_crt_run_image_path = "../bin/textures/Biker_run.png";

std::vector<uint> sprite_indices = { 0, 2, 1, 1, 2, 3 };
std::vector<vertex> sprite_vertices = {
	{{-0.5f,-0.5f,0}, {0,0,1}, {0,0}},
	{{-0.5f, 0.5f, 0}, {0, 0, 1}, {0, 1}},
	{{0.5f,-0.5f,0}, {0,0,1}, {1,0}},
	{{0.5f, 0.5f, 0}, {0, 0, 1},{1, 1}}
};

std::vector<uint> circle_indices = { 0, 2, 1, 1, 2, 3 };
std::vector<vertex> create_circle_vertices(uint N)
{
	std::vector<vertex> v = { { vec3(0), vec3(0,1,0), vec2(0) } }; // origin
	for (uint k = 0; k <= N; k++)
	{
		float t = PI * 2.0f * k / float(N), c = cos(t), s = sin(t);
		v.push_back({ vec3(s,0,c), vec3(0,1,0), vec2(0) });
	}
	for (uint k = 0; k < N; k++)
	{
		circle_indices.push_back(0);	// the origin
		circle_indices.push_back(k + 1);
		circle_indices.push_back(k + 2);
	}
	return v;
}