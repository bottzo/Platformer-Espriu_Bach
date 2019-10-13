#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__
#include "j1Module.h"
#include "p2List.h"

class j1Player :public j1Module {
public:
	j1Player();
	~j1Player();
	bool Awake(pugi::xml_node&config);
	bool Load(const char* file_name);
	bool CleanUp();

private:
	pugi::xml_document player_doc;
	p2SString folder;
	bool player_loaded;
};
#endif // __j1RENDER_H__