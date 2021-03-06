#ifndef __player_H__
#define __player_H__
#include "Entity.h"
#include "p2Qeue.h"

enum santa_states
{
	ST_UNKNOWN,

	ST_IDLE_RIGHT,
	ST_IDLE_LEFT,
	ST_WALK_FORWARD,
	ST_WALK_BACKWARD,
	ST_RUN_FORWRD,
	ST_RUN_BACKWARD,
	ST_JUMP,
	ST_SLIDE_FORWARD,
	ST_SLIDE_BACKWARD,
};

class player:public Entity {
public:
	player();
	~player();
	void Draw_player(santa_states state,float dt);
	void Updateposition(santa_states state);
	santa_states player::current_santa_state(p2Qeue<inputs>& input);
	Collider*player_collider;
	Collider*slide_collider;
	bool looking_right = true;
	float slide_timer;
	bool move_in_air;
	bool start_jump = true;
	bool start_slide;
	p2Point<float>distance;
};
#endif // __player_H__