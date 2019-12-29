#ifndef __ENTITY_H__
#define __ENTITY_H__
#include "j1Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "p2Qeue.h"
#include "j1Render.h"

struct TileSet;
class SDL_Texture;
class player;
class ground_enemy;
class flying_enemy;

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
	uint current_frame;
	float current_frame_time=0.0f;
	SDL_Texture* texture;
	Frame* frames;
	SDL_Rect& GetCurrentFrame(float dt=1.0f);
	SDL_Rect& DoOneLoop(float dt=1.0f);
};

class Entity{
public:
	enum class Types
	{
		flying_enemy,
		ground_enemy,
		player,
		unknown
	};
public:
	Entity() {};
	virtual ~Entity();
	Entity(Types type);
	p2Point<float> position;
	p2Point<float> speed;
	p2List<TileSet*>sprite_tilesets;
	Types type;
protected:
	p2List<Animation*>Animations;
	pugi::xml_document Entity_doc;
	bool Load_Entity(const char* file_name);
};

class EntityManager :public j1Module {
public:
	EntityManager();
	virtual ~EntityManager();
	bool Awake(pugi::xml_node&config);
	bool Start();
	bool Update(float dt);
	bool UpdateAll(float dt, bool do_logic);
	bool CleanUp();
	void OnCollision(Collider*c1, Collider*c2);
	Entity* CreateEntity(Entity::Types type);
	void DestroyEntity(Entity* entity);
	player* GetPlayer() const;
	void update_enemies();
	void draw_enemies(float dt);
	void spawn_entities();
public:
	p2SString folder;
	p2SString player_sprite;
	p2SString flying_enemy_sprite;
	p2SString ground_enemy_sprite;
	iPoint player_texture_offset;
	iPoint slide_texture_offset;
	int ground_texture_offset;
	p2Qeue<inputs> key_inputs;
	float acumulated_ms;
	float update_ms_cycle;
	bool do_logic;
private:
	p2List<Entity*> Entity_list;
};
#endif // __ENTITY_H__