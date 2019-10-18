#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__
#include "j1Module.h"
#include "p2List.h"
#include "SDL/include/SDL_rect.h"
#include "SDL/include/SDL_render.h"
#include "j1Map.h"

struct Frame {
	SDL_Rect rect;
	float duration;
};

struct PlayerAnimation {
	const char* name;
	uint total_frames;
	SDL_Texture* texture;
	Frame* frames;
};

class j1Player :public j1Module {
public:
	j1Player();
	~j1Player();
	bool Awake(pugi::xml_node&config);
	bool Load(const char* file_name);
	void LoadAnimations(pugi::xml_node&node);
	bool CleanUp();

private:
	pugi::xml_document player_doc;
	p2SString folder;
	bool player_loaded;
	p2List<TileSet*>sprite_tilesets;
	p2List<PlayerAnimation*>Animations;
	friend class j1Map;
};
#endif // __j1RENDER_H__