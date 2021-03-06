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
	bool PostUpdate();
	bool CleanUp();
	void ui_callback(UiElement*element);
private:
	int buttonsound;
	SDL_Texture*background_texture;
	SDL_Texture*Title;
	p2SString folder;
	p2SString BackgroundImagePath;
	p2SString TitlePath;
	UiElement*Play_button;
	UiElement*Continue_button;
	UiElement*Exit_button;
	_TTF_Font*MenuButtonsFont;
	_TTF_Font*ContinueFont;
	bool Exit;
};

#endif // __MAINMENU_H__