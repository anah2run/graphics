#pragma once
#include "particle.h"
static const char* blocks_image_path = "../bin/textures/blocks.png";
static const char* blocks_opacity_image_path = "../bin/textures/blocks-opacity.png";
class Item;
static const uint MAP_WIDTH = 100;	// �� ����
static const uint MAP_HEIGHT = 50;	// �� ����
int new_map[MAP_WIDTH][MAP_HEIGHT] = {
		{1,1,1,1,1,1,1,1,1,1,1,1,1,},
		{1,1,1,},
		{1,1,1,}, //0
		{1,1,1,},
		{1,1,1,},
		{1,1,1,},
		{1,1,1,},
		{1,1,1,},
		{1,1,1,},
		{1,1,1,0,0,42,},
		{1,1,1,0,0,22,0,2},
		{1,1,1,0,0,32,},
		{1,1,1,},
		{1,1,1,},
		{1,1,1,},
		{1,1,1,},
		{0,0,0},//10
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{1,1,1,2,2,2,},
		{1,1,1,2,2,},
		{1,1,1,2,},
		{1,1,1,0,0,0,0,2},
		{1,1,1,0,0,0,0,2,0,2},
		{1,1,1,0,0,0,0,2},
		{1,1,1,},
		{1,1,1,2},//20
		{1,1,1,2,2},
		{1,1,1,2,2,2},
		{1,1,5},
		{1,1,5},
		{1,1,5},
		{1,1,1,2,2,2,},
		{1,1,1,2,2,},
		{1,1,1,2,},
		{1,1,1,},
		{1,1,1,},//30
		{1,1,1,},
		{0,0,0,0,0,2},
		{0,0,0,0,0,2},
		{0,0,0,0,0,2},
		{0,0,0,0,0,2},
		{1,1,1,},
		{1,1,1,},
		{0,0,0},
		{0,0,0},
		{1,1,1,},//40
		{0,0,0},
		{0,0,0},
		{1,1,1,},
		{1,1,1,1},
		{1,1,1,1,1},
		{1,1,1,1,1,1},
		{1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1},
		{1,1,1,},//50
		{1,1,1,},
		{1,1,1,},
		{1,1,1,},
		{1,1,1,},
		{1,1,1,},
};

struct BlockProp {
	int block_id = -1;
	int texture_id = 0;
	int max_hp = -1;
	bool destroy_bullet = true;
	int collid_dmg = 0;
};
static const BlockProp	block_array[] = {
//		id	tid	hp	destroy_bullet
	{	0,	0,	-1,	false	},		// air
	{	1,	1,	-1,	true	},		// stone
	{	2,	2,	5,	true	},		// wood
	{	3,	3,	-1,	false	},		// hole
	{	4,	4,	1,	false	},		// glass
	{	5,	5,	8,	true,	1	},		// spike
	{	6,	6,	2,	true	},		// item block
	{	7,	7,	-1,	true	},		// WIN block
};
class Block {
public:
	void swap_block(int i) {
		prop = &block_array[i];
		hp = prop->max_hp;
	}
	const BlockProp* prop;
	int x;
	int y;
	int hp;
	int item = -1; // when less than zero it gives no item
	Block() {
		swap_block(0);
	}
	Block(int i, int x, int y) {
		swap_block(i);
		this->x = x;
		this->y = y;
	}
	bool checkDestroy() {
		return  hp <= 0;
	}	
	void destroy() {
		swap_block(0);//change block to air
		particles_list.push_back(Particle(3, vec2(float(x),float(y)) + .5f));
		if (item > 0) {
			item_list.push_back(Item(item, vec2(x + .5f, y + .5f)));
			printf("item dropped!\n");
		}

	}
	void hit(int damage) {
		printf("hp: %d, damaged %d. now %d\n", hp, damage, hp - damage);
		hp -= damage;
		if (checkDestroy()) {
			destroy();
		}
	}
};
std::vector<uint> block_indices = {};
std::vector<vertex> create_block_vertices()
{
	vec2 tc[] = { {0.f,0.f},{0.1f,0.0f},{0.0f,0.1f},{0.1f,0.1f} };
	vec3 normal[] = { {0,0,-1},{0,0,1},{-1,0,0},{1,0,0}, {0,1,0}, {0,-1,0} };
	vec3 n_sample[] = {
		vec3(-1,-1,-1), vec3(1,-1,-1), vec3(-1,1,-1), vec3(1,1,-1), // front
		vec3(-1,-1,1), vec3(1,-1,1), vec3(-1,1,1), vec3(1,1,1),		// back
		vec3(-1,-1,-1), vec3(-1,-1,1), vec3(-1,1,-1), vec3(-1,1,1),	// left
		vec3(1,-1,1), vec3(1, - 1,-1), vec3(1,1,1), vec3(1,1,-1),	// right
		vec3(-1,1,1), vec3(1,1,1), vec3(-1,1,-1), vec3(1,1,-1),		// top
		vec3(-1,-1,-1), vec3(1,-1,-1), vec3(-1,-1,1), vec3(1,-1,1)	// bottom
	};
	std::vector<vertex> v = {};
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 4; j++) {
			v.push_back({ (n_sample[i * 4 + j] + vec3(1,1,0)) / 2, normal[i], tc[j] + vec2(0.1f * i,0) });
		}
		block_indices.push_back(i * 4);
		block_indices.push_back(i * 4 + 1);
		block_indices.push_back(i * 4 + 2);
		block_indices.push_back(i * 4 + 1);
		block_indices.push_back(i * 4 + 3);
		block_indices.push_back(i * 4 + 2);
	}
	return v;
}

class Map {
public:
	Block map[MAP_WIDTH][MAP_HEIGHT]; 
	float gravity = 30;
	Block* block(vec2 pos)
	{
		if (pos.x < 0 || pos.y < 0 || pos.x >= MAP_WIDTH || pos.y >= MAP_HEIGHT) return 0;
		return &map[int(pos.x)][int(pos.y)];
	}
	int	block_id(vec2 pos)
	{
		Block* bp = block(pos);
		if (bp == 0) return 0;
		return bp->prop->block_id;
	}
	Map() {
	}
	Map(int (*input)[MAP_HEIGHT]) {
		int temp;
		for (int i = 0; i < MAP_WIDTH; i++) {
			for (int j = 0; j < MAP_HEIGHT; j++) {
				temp = input[i][j];
				map[i][j] = Block(temp%10,i,j);
				if (temp / 10 > 0 ) map[i][j].item = temp/10;
			}
		}
	}
	int shadow_pos(vec2 pos) {
		int x = int(pos.x);
		if (x < 0 || x >= MAP_WIDTH) return 0;
		for (int y = std::min(int(pos.y), int(MAP_HEIGHT - 1)); y >= 0; y--) {
			if (map[x][y].prop->block_id > 0) {
				return y + 1;
			}
		}
		return -1;
	}
};