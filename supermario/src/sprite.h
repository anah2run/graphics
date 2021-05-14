#pragma once

std::vector<vertex> sprite_vertices = {
	{{-0.5f,-0.5f,0}, {0,0,1}, {0,0}},
	{{-0.5f, 0.5f, 0}, {0, 0, 1}, {0, 1}},
	{{0.5f,-0.5f,0}, {0,0,1}, {1,0}},
	{{0.5f, 0.5f, 0}, {0, 0, 1},{1, 1}}
};
std::vector<uint> sprite_indices = {0, 2, 1, 1, 2, 3 };