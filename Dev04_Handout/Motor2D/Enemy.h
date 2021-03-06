/*#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "p2Point.h"
#include "Animation.h"

struct SDL_Texture;
struct Collider;

class Enemy
{
protected:
	Animation* animation;
	Collider* collider;

public:
	iPoint position;

public:
	Enemy(int x, int y);
	virtual ~Enemy();

	const Collider* GetCollider() const;

	virtual void Move() {};
	virtual void Draw(SDL_Texture* sprites);
};

#endif // __ENEMY_H__*/
#ifndef __enemy_H__
#define __enemy_H__
#include "Entity.h"

class enemy :public Entity {
public:
	enum class Enemies {
		ground,
		air,
		unknown
	};
public:
	enemy();
	virtual ~enemy();
	void Draw_Enemy(float dt);
	virtual void move() {};
	p2Point<float> speed;
	Collider*enemy_collider;
	p2Point<float>distance;
	Enemies kind;
	int count = 0;
};

class ground_enemy :public enemy {
public:
	ground_enemy();
	~ground_enemy() {};
	void move()override;
	bool on_the_ground;
};

class flying_enemy :public enemy {
public:
	flying_enemy();
	~flying_enemy() {};
	void move()override;
};
#endif // __enemy_H__