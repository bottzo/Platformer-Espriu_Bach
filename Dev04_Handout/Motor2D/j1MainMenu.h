#ifndef __MAINMENU_H__
#define __MAINMENU_H__

#include "j1Module.h"

struct SDL_Texture;
struct _TTF_Font;
class UiElement;

class j1MainMenu : public j1Module
{
public:
	j1MainMenu();
	~j1MainMenu();

	bool Awake(pugi::xml_node&config);
	bool Start();
	bool Update(float dt);
	bool CleanUp();
	void ui_callback(UiElement*element);
private:
	SDL_Texture*background_texture;
	p2SString folder;
	p2SString BackgroundImagePath;
	UiElement*Play_button;
	_TTF_Font*MenuButtonsFont;
};

#endif // __MAINMENU_H__