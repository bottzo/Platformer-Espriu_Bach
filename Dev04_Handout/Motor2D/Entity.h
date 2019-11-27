#ifndef __ENTITY_H__
#define __ENTITY_H__
#include "j1Module.h"
#include "p2List.h"
#include "p2Point.h"

class SDL_Texture;

class Entity :public j1Module{
public:
	enum class Types
	{
		enemy,
		player,
		unknown
	};
public:
	bool Update(float dt);
	bool UpdateAll(float dt, bool do_logic);

	Entity(Types type);
	//virtual ~Entity();
	Entity* CreateEntity(Entity::Types type);
	void DestroyEntity(Entity* entity);
protected:
	fPoint position;
	Entity::Types type;
	SDL_Texture* sprite;
private:
	p2List<Entity*> Entities;
};
#endif // __ENTITY_H__