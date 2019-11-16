#pragma once
#ifndef __j1Collisions_H__
#define __j1Collisions_H__

#define MAX_COLLIDERS 100

#include "j1Module.h"

enum COLLIDER_TYPE
{
	COLLIDER_NONE = -1,
	START_COLLIDER,
	COLLIDER_WALL,
	COLLIDER_PLAYER1,
	COLLIDER_DEATH,
	COLLIDER_BACKGROUND,


	COLLIDER_MAX
};

struct Collider
{
	SDL_Rect rect;
	bool to_delete = false;
	bool active = true;
	COLLIDER_TYPE type;
	j1Module* callback = nullptr;
	Collider();

	Collider(SDL_Rect rectangle, COLLIDER_TYPE type, j1Module* callback = nullptr) :
		rect(rectangle),
		type(type),
		callback(callback)
	{}

	void SetPos(int x, int y)
	{
		rect.x = x;
		rect.y = y;
	}

	bool CheckCollision(const SDL_Rect& r) const;
};

class j1Collisions : public j1Module
{
public:

	j1Collisions();
	~j1Collisions();

	bool PreUpdate() override;
	bool Update(float dt);
	bool CleanUp() override;

	Collider* AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback = nullptr);
	void update_active_colliders();
	void DebugDraw();
	p2Point<float> origin_distance_player;
	int closest_xaxis_collider(santa_states state, bool orientation);
	int closest_yaxis_collider(santa_states state);
	bool on_the_way_x(int index);
	bool on_the_way_y(int index);

private:

	Collider* colliders[MAX_COLLIDERS];
	bool matrix[COLLIDER_MAX][COLLIDER_MAX];
	bool debug = false;
	
};

#endif // __ModuleCollision_H__