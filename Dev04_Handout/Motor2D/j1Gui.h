#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "j1Module.h"
#include "j1Textures.h"

#define CURSOR_WIDTH 2

// TODO 1: Create your structure of classes

// ---------------------------------------------------
class SDL_Rect;
class SDL_Texture;
class p2SString;
class elements;
class j1Gui : public j1Module
{
public:

	j1Gui();

	// Destructor
	virtual ~j1Gui();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// TODO 2: Create the factory methods
	// Gui creation functions
	bool Deleteelements(elements &elements);
	bool DeleteAllelements();

	elements* AddImage(int posx, int posy, SDL_Rect rect, j1Module* callback = nullptr, elements* parent = nullptr);
	elements* AddButton(int posx, int posy, SDL_Rect button_rect, SDL_Rect focus, SDL_Rect pressed, j1Module* callback = nullptr, elements* parent = nullptr);
	elements* AddLabel(int posx, int posy, p2SString text, int size = 20, uint32 width = 0, elements* parent = nullptr);
	elements* AddBox(int x, int y, SDL_Rect normal, SDL_Rect no_normal, SDL_Rect tickpressed, SDL_Rect tickfocus, SDL_Rect ticknofocus, j1Module* callback = nullptr, elements* parent = nullptr);
	elements* mouse(int x, int y);

	const SDL_Texture* GetAtlas() const;
	

private:

	SDL_Texture* atlas;
	p2SString atlas_file_name;
};

#endif // __j1GUI_H__