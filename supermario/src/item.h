#pragma once
class Map;
class Character;

class Item {
public:
	int item_code;
	vec2 position;
	float theta = 0;
	Item() {

	}
	Item(int code, vec2 pos) {
		item_code = code;
		position = pos;
	}
};
std::list<Item> item_list = {Item(1, vec2(10.5f,7.5f))};