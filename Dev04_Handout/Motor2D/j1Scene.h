#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"

struct SDL_Texture;
class player;
class flying_enemy;
class ground_enemy;
class UiElement;
struct _TTF_Font;

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

	// Called before quitting
	bool CleanUp();

	bool positioncamera();

	void ui_callback(UiElement*element);

	void check_console_input(p2SString input);

private:
	p2SString map_name;
public:
	int actualmap;
	p2List<p2SString*> map_names;
	UiElement*input_lable;
	UiElement*default_input_text;
	UiElement*Settings_window;
	UiElement*Exit_button;
	UiElement*Continue_button;
	_TTF_Font*SceneButtonsFont;
	_TTF_Font*ContinueFont;
	UiElement*ConsoleTitle;
	UiElement*Console;
	bool Exit;
	int CurrentMap;
	//console orders
	p2SString exit;
	p2SString change_map;
};

#endif // __j1SCENE_H__