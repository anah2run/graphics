class Enemy;
static const uint MAP_WIDTH = 100;	// �� ����
static const uint MAP_HEIGHT = 50;	// �� ����

static const std::vector<vec2> new_map1_enemies = {
	{24, 6},
	{25, 6},
	{27, 6},
	{28, 6},
	{39, 5},
	{39, 9},
	{48, 4},
	{35, 7},
	{67, 3},
	{67, 5},
	{67, 7},
	{85, 5},
	{88, 7},
	{91, 5},
	{94, 7}
};
static const std::vector<Item> new_map1_items = {
	Item(1,vec2(32.5,12)),
};
static const int new_map1[MAP_WIDTH][MAP_HEIGHT] = {
		{3,3,3,3,3,3,3,3,3,3,3,3}, //0
		{1,1,1,},
		{1,1,1,},
		{1,1,1,},
		{1,1,1,},
		{1,1,1,},
		{1,1,1,0,0},
		{1,1,1,0,0,2},
		{1,1,1,0,0,26,0,0,12},
		{1,1,1,0,0,2},
		{1,1,1,0,0,2},//10
		{1,1,1},
		{3,3,3,3,0,0,0,0,3},
		{3,3,3,3,0,0,0,0,3},
		{3,3,3,3,0,0,0,0,3},
		{3,3,3,3,0,0,0,0,3},
		{3,3,3,3,3,3,3,3,3},
		{3,3,3,2,2,2,2,2,2},
		{3,3,3},
		{3,3,3},
		{3,3,3},//20
		{3,3,3,3},
		{3,3,3,3,3},
		{3,3,3,3,3,3,3},
		{3,3,3,35,4},
		{3,3,3,5,4},
		{3,3,3,3,4},
		{3,3,3,5,4},
		{3,3,3,55,4},
		{3,3,3,3,3,3,3},
		{3,3,3,3,3},//30
		{3,3,3,3},
		{3,3,3},
		{3,3,3},
		{3,3,3,4,3,3,3,3,3,3},
		{3,3,3,0,0,3,0,0,0,3},
		{3,3,3,0,0,3,0,0,0,3},
		{3,3,3,3,4,3,4,3,4,3},
		{3,3,3,3,0,0,0,3},
		{3,3,3,3,0,0,0,3},
		{3,3,3,3,3,3,3,3},//40
		{3,3,3,2,2,2,2,2},
		{3,3,3},
		{3,3,3},
		{3,3,3},
		{3,3,3,0,3},
		{3,3,3},
		{3,3,3,0,0,3},
		{3,3,3},
		{3,3,3,0,0,0,3},
		{3,3,3},//50
		{3,3,3,0,0,0,0,3},
		{3,3,3},
		{3,3,3,0,0,0,0,0,3},
		{3,3,3,0,0,0,0,0,3},
		{3,3,3,3,3,3,3,3,3},
		{3,3,3},
		{3,3,3,4,2,2,2,2,2,2,2,2,2},
		{3,3,3},
		{3,3,3,4,4,4,4,4,4},
		{3,3,3,4,4,4,4,4,4},//60
		{3,3,3,4,4,4,4,4,4},
		{3,3,3,4,4,4,4,4,4},
		{3,3,3,0,3,3,3,3,3},
		{3,3,3,0,3,0,0,0,3},
		{3,3,3,0,3,0,3,0,3},
		{3,3,3,0,3,0,3,0,3},
		{3,3,3,0,3,0,3,0,3},
		{3,3,3,0,3,0,3,0,3},
		{3,3,3,0,3,0,3,0,3},
		{3,3,3,0,0,0,3,0,3},//70
		{3,3,3,3,3,3,3,0,3},
		{3,3,3},
		{3,3,3},
		{3,3,3,0,3,0,4,0,3},
		{3,3,3},
		{3,3,3,0,4,0,3,0,4},
		{3,3,3},
		{3,3,3,0,3,0,4,0,3},
		{3,3,3},
		{3,3,3,0,4,0,3,0,4},//80
		{3,3,3},
		{3,3,3,0,3,0,4,0,3},
		{3,3,3},
		{3,3,3,4,3,3,3,3,3,3},
		{3,3,3,0,3,0,0,0,3},
		{3,3,3,0,3,0,0,0,3},
		{3,3,3,0,0,0,3},
		{3,3,3,0,0,0,3},
		{3,3,3,0,0,0,3},
		{3,3,3,0,3},//90
		{3,3,3,0,3,},
		{3,3,3,0,3,0,0,0,0,3},
		{3,3,3,0,0,0,3,0,0,3},
		{3,3,3,0,0,0,3,0,0,3},
		{3,3,3,0,0,0,3},
		{3,3,3,3,3,3,3,3,3,3},
		{3,3,3},
		{3,3,3,0,7},
		{3,3,3,3,3,3,3,3,3,3,3,3,3},
};

static const std::vector<vec2> new_map2_enemies = {
	{22, 3},
	{38, 6},
	{33, 6},
	{12, 3},
	{50.5f, 4},
	{52, 4},
	{63,4},
	{88,7},
	{89,8},
	{90,9},
};
static const std::vector<Item> new_map2_items = {
	Item(5,vec2(50.5f,14)),
	Item(5,vec2(97.5f,10)),
	Item(5,vec2(97.5f,9)),
	Item(5,vec2(97.5f,8)),
};
static const int new_map2[MAP_WIDTH][MAP_HEIGHT] = {
		{1,1,1,1,1,1,1,1,1,1,1,1,1,},
		{1,1,1,},
		{1,1,1,}, 
		{1,1,1,},
		{1,1,1,},
		{1,1,1,},
		{1,1,1,},
		{1,1,1,},
		{1,1,1,},
		{1,1,1,0,0,2,},
		{1,1,1,0,0,26,0,52},
		{1,1,1,0,0,2,},
		{1,1,1,},
		{1,1,1,},
		{1,1,1,},
		{1,1,1,},
		{1,1,1,},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{1,1,1,2,2,2,},
		{1,1,1,2,2,},
		{1,1,1,2,},
		{1,1,1,0,0,0,0,2},
		{1,1,1,0,0,0,0,2,0,36},
		{1,1,1,0,0,0,0,2},
		{1,1,1,},
		{1,1,1,2},
		{1,1,1,2,2},
		{1,1,1,2,2,2},
		{1,1,5},
		{1,1,5},
		{1,1,5},
		{1,1,1,2,2,2,},
		{1,1,1,2,2,},
		{1,1,1,2,},
		{1,1,1,},
		{1,1,1,},
		{1,1,1,},
		{0,0,0,0,0,2},
		{0,0,0,0,0,2},
		{0,0,0,0,0,2},
		{0,0,0,0,0,16},
		{1,1,1},
		{1,1,1,3,0,0,0,0},
		{0,0,0,3,0,0,0,1},
		{0,0,0,3,0,0,0,1},
		{3,3,3,3,0,0,0,1,0,0,0},
		{0,0,0,3,0,0,0,0,0,0,0},
		{0,0,0,3,0,0,0,0,0,0,1},
		{0,0,0,3,0,0,0,0,0,0,1},
		{3,3,3,3,0,0,0,0,0,0,1},
		{0,0,0,3},
		{0,0,0,3},
		{1,1,1,3,0,0,0,1},
		{1,1,1},
		{1,1,1},
		{1,1,1},
		{1,1,1,1,1,1,1,1},
		{1,1,1,56,0,3,0,4},
		{1,1,1,36,0,3,0,4},
		{1,1,1,56,0,3,0,4},
		{1,1,1,0,3,0,0,4},
		{1,1,1,0,0,0,0,4},
		{1,1,1,3,0,0,0,4},
		{1,1,1,1,1,1,1,1},
		{1,1,1,2,2},
		{1,1,1,2},
		{1,1,1,},
		{1,1,1,0,0,0,1},
		{1,1,1,0,0,0,1,0,0,16},
		{1,1,1,0,0,0,1},
		{1,1,1,},
		{1,1,1,},
		{1,1,1,},
		{1,1,1,},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{1,1,1,2,2,2,},
		{1,1,1,2,2,},
		{1,1,1,2,},
		{1,1,1,},
		{1,1,1,},
		{1,1,1,},
		{1,1,1,},
		{1,1,1,1},
		{1,1,1,1,1},
		{1,1,1,1,1,1},
		{1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1},
		{5,5,5,},
		{5,5,5,},
		{5,5,5,},
		{5,5,5,},
		{5,5,5,7,},
		{5,5,5,},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,},
};
