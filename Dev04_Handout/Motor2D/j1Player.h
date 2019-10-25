#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__
#include "j1Module.h"
#include "p2List.h"
#include "p2Qeue.h"
#include "SDL/include/SDL_rect.h"
#include "SDL/include/SDL_render.h"
#include "j1Map.h"

enum santa_states
{
	ST_UNKNOWN,

	ST_IDLE_RIGHT,
	ST_IDLE_LEFT,
	ST_WALK_FORWARD,
	ST_WALK_BACKWARD,
	ST_RUN_FORWRD,
	ST_RUN_BACKWARD,
	ST_JUMP_NEUTRAL,
	ST_JUMP_FORWARD,
	ST_JUMP_BACKWARD,
	ST_SLIDE_FORWARD,
	ST_SLIDE_BACKWARD
};

enum santa_inputs
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
	IN_SLIDE_FINISH
};

struct Frame {
	SDL_Rect rect;
	float duration;
};

struct PlayerAnimation {
	const char* name;
	uint total_frames;
	float current_frame;
	SDL_Texture* texture;
	Frame* frames;
	SDL_Rect& GetCurrentFrame();
};

class j1Player :public j1Module {
public:
	j1Player();
	~j1Player();
	bool Awake(pugi::xml_node&config);

	bool Load(const char* file_name);
	void LoadAnimations(pugi::xml_node&node);
	void j1Player::LoadPlayerPosition();
	void Draw_player(santa_states state);
	void Updateposition(santa_states state);
	bool CleanUp();
	p2Point<float> speed;
	p2Qeue<santa_inputs> key_inputs;//recordar que em falta borrarla de la memoria
	santa_states j1Player::current_santa_state(p2Qeue<santa_inputs>& inputs);
	//Collider* playercollider = nullptr;

private:
	pugi::xml_document player_doc;
	p2SString folder;
	bool player_loaded;
	p2List<TileSet*>sprite_tilesets;
	p2List<PlayerAnimation*>Animations;
	p2Point<float> position;
	friend class j1Map;
};
#endif // __j1RENDER_H__