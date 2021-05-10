#pragma once
#include "map.h"
#include <cmath>

class Hitbox {
public:
	vec4	box_rec = { -0.3f, -0.5f, 0.3f, 0.5f };	// rectangle margin
	Hitbox() {
	}
	Hitbox(float l, float b, float r, float t) {
		box_rec = vec4(l, b, r, t);
	}
	bool collid(vec2 pos) {
		bool left_check = box_rec.x < pos.x&& box_rec.y < pos.y;
		bool right_check = pos.x < box_rec.z&& pos.y < box_rec.a;
		return  left_check && right_check;
	}
	float width() {
		return box_rec.z - box_rec.x;
	}
	float height() {
		return box_rec.a - box_rec.y;
	}
};
class Character {
public:
	Character() {
	}
	Character(Map* p, vec2 pos) {
		mapp = p;
		position = pos;
	}
	
	float	invinc_time = 1.5f;	// 피격 시 무적 시간
	float	max_speed = 5.0f;	// 최대 이동속도
	float	max_jump = 3.5f;	// 최대 점프
	int	max_hp = 3;				// 최대 체력
	Map* mapp;					// 맵 포인터

	vec2	direction;			// 가고자 하는 방향
	Hitbox	hitbox = Hitbox();	// 히트 박스
	vec2	position;			// 위치	
	int		hp = max_hp;		// 현재 체력
	float	invinc_t = 0;		// 무적 타이머
	bool	is_jump = true;		// 점프 상태
	int		stand_block = 0;	// 서 있는 블록

	// function
	bool	hit(int damage);
	bool	heal(int amount);
	void	move_right();
	void	move_left();
	void	jump();
	bool	checkJump();
	bool	checkDeath();
	void	physics(float t, bool);
	void	update(float t, bool moving);
private:
	vec2	velocity = vec2(0);
	vec2	acceration = vec2(0);
};

inline bool Character::hit(int damage) {
	if (invinc_t <= 0) {
		printf("player took %d damage! hp: %d\n", damage, hp);
		hp -= damage;
		invinc_t = invinc_time;
		checkDeath();
		return true;
	}
	return false;
}
inline bool Character::heal(int amount) {
	bool flag = hp <= max_hp;
	hp = std::min(hp + amount, max_hp);
	return flag;
}
inline bool Character::checkDeath() {
	return hp <= 0;
}

inline bool Character::checkJump() {
	vec2 stand_block_pos = position - vec2(0, 1);
	//printf("check jump : %f %f \n",stand_block_pos.x,position.y);
	stand_block = mapp->block_id(stand_block_pos);
	return stand_block == 0;
}
inline void Character::physics(float t, bool moving)
{
	vec2 temp_pos = position;
	if (is_jump) {
		acceration.x *= 0.8f;
		acceration.y = -mapp->gravity;
		stand_block = 0;
	}
	else {
		is_jump = checkJump();
		acceration.y = 0;
		velocity.y = 0;
		if (!moving) acceration.x = -velocity.x * 10;
		else acceration.x = 0;
	}
	velocity += acceration * t;
	position += velocity * t + acceration * powf(t, 2);
	if (position.x - temp_pos.x >= 0) { //캐릭터가 오른쪽으로 움직임
		int temp_pos_x = int(position.x + hitbox.box_rec.z);
		for (int i = int(temp_pos.x + hitbox.box_rec.z); i <= temp_pos_x; i++)
		{
			if (mapp->block_id(vec2(float(i), temp_pos.y)) > 0) {
				position.x = i - hitbox.box_rec.z;
				break;
			}
		}
	}
	if (position.x - temp_pos.x <= 0) { //캐릭터가 왼쪽으로 움직임
		int temp_pos_x = int(position.x + hitbox.box_rec.x);
		for (int i = int(temp_pos.x + hitbox.box_rec.x); i >= temp_pos_x; i--)
		{
			if (mapp->block_id(vec2(float(i), temp_pos.y)) > 0) {
				position.x = i + 1 - hitbox.box_rec.x;
				break;
			}
		}
	}
	if(is_jump)
	{
		if (position.y - temp_pos.y > 0) { //캐릭터가 위쪽으로 움직임
			int temp_pos_y = int(position.y + hitbox.box_rec.a);
			for (int i = int(temp_pos.y + hitbox.box_rec.a); i <= temp_pos_y; i++)
			{
				Block* bp = mapp->block(vec2(position.x, float(i)));
				if (block_array[bp->block_id].max_hp > 0) {
					bp->hit(1);
				}
				if (bp->block_id > 0) {
					position.y = i - hitbox.box_rec.a;
					velocity.y = 0; // 천장 부딫힘
					break;
				}
			}
		}
		else {
			int temp_pos_y = int(position.y + hitbox.box_rec.y);
			for (int i = int(temp_pos.y + hitbox.box_rec.y); i >= temp_pos_y; i--)
			{
				if (mapp->block_id(vec2(position.x, float(i))) > 0) {
					position.y = i + 1 - hitbox.box_rec.y;
					is_jump = false;
					break;
				}
			}
		}
	}
}

inline void Character::update(float t, bool moving)
{
	invinc_t = std::max(invinc_t - t, 0.0f);
	physics(t, moving);
}


inline void Character::move_right()
{
	if (is_jump) {
		if (velocity.x < max_speed/2)
		{
			acceration.x = max_speed*5;
		}
	}
	else
	{
		velocity.x = max_speed;
	}
}

inline void Character::move_left()
{
	if (is_jump ) {
		if (velocity.x > -max_speed/2)
		{
			acceration.x = -max_speed*5;
		}
	}
	else
	velocity.x = -max_speed;
}

inline void Character::jump()
{
	if (!is_jump) {
		float jv = sqrt(2 * max_jump * mapp->gravity);
		velocity.y = jv;
		is_jump = true;
	}
}

class Enemy : public Character
{
public:
	int ai_level = 1;
	float invinc_t = 0;
	Enemy(Map* p, vec2 pos) {
		mapp = p;
		position = pos;
	
	}
	void update(float t, bool b) {
		physics(t,b);
		float x = position.x;
		int y = int(position.y);
		move();
	}
	void move() {
		if (direction == 1) {
			move_right();
		}
		else {
			move_left();
		}
	}
	

};
class Player : public Character 
{

};