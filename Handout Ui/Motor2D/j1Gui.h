#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "j1Module.h"
#include "p2DynArray.h"
#include "j1App.h"


struct SDL_Texture;
struct _TTF_Font;
struct SDL_Color;

#define CURSOR_WIDTH 2

// TODO 1: Create your structure of classes

enum class UiTypes {
	Unknown,
	Image,
	Text
};

class UiElement {
public:
	UiElement(int x, int y, UiTypes uitype, UiElement* parent, j1Module* elementmodule);
	virtual ~UiElement();
	//virtual void Update() = 0;
	virtual void Draw(SDL_Texture* atlas) = 0;
	iPoint position;
	UiTypes type;
	UiElement* parent;
	j1Module* Module;
};

// ---------------------------------------------------
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

	bool Update(float dt);

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// TODO 2: Create the factory methods
	// Gui creation functions
	void DeleteAllUiElements();
	void DeleteUiElement(UiElement*element);
	void Draw_Ui();
	//void Update_mouse();
	UiElement*AddImage(int x,int y,SDL_Rect source_rect,UiElement* parent=nullptr,j1Module* elementmodule=nullptr);
	UiElement* AddText(int x, int y, const char*text, SDL_Color color = { 255, 255, 255, 255 }, _TTF_Font*font = nullptr,UiElement* parent = nullptr, j1Module* elementmodule = nullptr);
	const SDL_Texture* GetAtlas() const;

private:
	p2List<UiElement*> UiElementList;
	SDL_Texture* atlas;
	p2SString atlas_file_name;
};

class UiImage :public UiElement {
public:
	UiImage(int x, int y, SDL_Rect source_rect, UiElement* parent, j1Module* elementmodule);
	void Draw(SDL_Texture* tex)override;
	SDL_Rect Image;
};

class UiText :public UiElement {
public:
	UiText(int x, int y,const char*text,SDL_Color color, _TTF_Font*font = nullptr, UiElement* parent=nullptr, j1Module* elementmodule=nullptr);
	void Draw(SDL_Texture* tex)override;
	_TTF_Font*font_type;
	const char*message;
	SDL_Color color;
};

#endif // __j1GUI_H__