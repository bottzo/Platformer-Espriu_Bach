#ifndef __ENTITY_H__
#define __ENTITY_H__
#include "j1Module.h"
#include "p2DynArray.h"
#include "p2Point.h"

class SDL_Texture;

/*class Entity :public j1Module{
public:
	enum class Types
	{
		enemy,
		player,
		unknown
	};
public:
	Entity(Types type);
	virtual ~Entity();
private:
	fPoint position;
	Entity::Types type;
	SDL_Texture* sprite;
};

struct entity_manager {
	p2DynArray<Entity*> Entities;
	Entity* CreateEntity(Entity::Types type);
	void DestroyEntity(Entity* entity);
	bool Update(float dt);
	bool UpdateAll(float dt, bool do_logic);
};*/

#endif // __ENTITY_H__