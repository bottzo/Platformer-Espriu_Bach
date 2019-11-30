#ifndef __ENTITY_H__
#define __ENTITY_H__
#include "j1Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "p2Qeue.h"
#include "j1Render.h"

struct TileSet;
class SDL_Texture;

enum inputs
{
	IN_LEFT_DOWN,
	IN_LEFT_UP,
	IN_RIGHT_DOWN,
	IN_RIGHT_UP,
	IN_LEFT_AND_RIGHT,
	IN_JUMP,
	IN_SLIDE_UP,
	IN_SLIDE_DOWN,
	IN_JUMP_AND_SLIDE,
	IN_JUMP_FINISH,
	IN_SLIDE_FINISH,
};

struct Frame {
	SDL_Rect rect;
	float duration;
};

struct Animation {
	p2SString name;
	uint total_frames;
	float current_frame;
	SDL_Texture* texture;
	Frame* frames;
	SDL_Rect& GetCurrentFrame();
	SDL_Rect& DoOneLoop();
};

class Entity{
public:
	enum class Types
	{
		ground_enemy,
		flying_enemy,
		player,
		unknown
	};
public:
	Entity(Types type);
	bool Load_Entity(const char* file_name);
	void LoadAnimations(pugi::xml_node&node);
public:
	p2List<TileSet*>sprite_tilesets;
	p2List<Animation*>Animations;
	pugi::xml_document Entity_doc;
	p2Point<float> position;
	Entity::Types type;
};

class EntityManager :public j1Module {
public:
	EntityManager();
	~EntityManager();
	bool Awake(pugi::xml_node&config);
	bool Update(float dt);
	bool UpdateAll(float dt, bool do_logic);
	bool CleanUp();
	void OnCollision(Collider*c1, Collider*c2);
	Entity* CreateEntity(Entity::Types type);
	void DestroyEntity(Entity* entity);
public:
	p2SString folder;
	p2SString player_sprite;
	iPoint player_texture_offset;
	iPoint slide_texture_offset;
	p2Qeue<inputs> key_inputs;
	float acumulated_time;
	float update_ms_cycle;
	bool do_logic;
private:
	p2List<Entity*> Entity_list;
};
#endif // __ENTITY_H__