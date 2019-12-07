#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"

struct SDL_Texture;
class player;
class flying_enemy;
class ground_enemy;

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

	bool positioncamera();
	// Called before quitting
	bool CleanUp();

private:
	p2SString map_name;
public:
	ground_enemy*penguin;
	flying_enemy* bee;
	int actualmap;
	p2List<p2SString*> map_names;
};

#endif // __j1SCENE_H__