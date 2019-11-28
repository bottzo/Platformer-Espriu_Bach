#ifndef __ENTITY_H__
#define __ENTITY_H__
#include "j1Module.h"
#include "p2List.h"
#include "p2Point.h"

class SDL_Texture;

class Entity{
public:
	enum class Types
	{
		enemy,
		player,
		unknown
	};
public:
	Entity(Types type);
	//virtual ~Entity();
protected:
	fPoint position;
	Entity::Types type;
	SDL_Texture* sprite;
};

class EntityManager :public j1Module {
public:
	EntityManager();
	~EntityManager();
	bool Awake(pugi::xml_node&config);
	bool Update(float dt);
	bool UpdateAll(float dt, bool do_logic);
	bool CleanUp();
	Entity* CreateEntity(Entity::Types type);
	void DestroyEntity(Entity* entity);
private:
	p2List<Entity*> Entity_list;
protected:
	p2SString folder;
};
#endif // __ENTITY_H__