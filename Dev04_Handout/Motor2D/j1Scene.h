#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"

struct SDL_Texture;

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake(pugi::xml_node&config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	bool PositionCamera();
	// Called before quitting
	bool CleanUp();

private:
	p2SString map_name;
	
public:
	int actualmap;
	santa_states state;
	p2SString player_sprite;
	p2List<p2SString*> map_names;
};

#endif // __j1SCENE_H__