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
	Text,
	Button
};

class UiElement {
public:
	UiElement(int x, int y, int w,int h, UiTypes uitype, UiElement* parent, j1Module* elementmodule);
	virtual ~UiElement();
	virtual void Update() = 0;
	virtual void Draw(SDL_Texture* atlas) = 0;
	const SDL_Rect GetScreenRect();
	const SDL_Rect GetLocalRect();
	const iPoint GetLocalPos();
	const iPoint GetScreenPos();
	void SetLocalPos(int x, int y);
	UiTypes type;
protected:
	UiElement* parent;
	j1Module* Module;
private:
	SDL_Rect ui_rect;
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
	void Update_Ui();
	void Draw_Ui();
	UiElement*AddImage(int x,int y,SDL_Rect source_rect,UiElement* parent=nullptr,j1Module* elementmodule=nullptr);
	UiElement* AddText(int x, int y, const char*text, _TTF_Font*font = nullptr, SDL_Color color = { 255, 255, 255, 255 }, int size = 12, UiElement* parent = nullptr, j1Module* elementmodule = nullptr);
	UiElement* AddButton(int x, int y, SDL_Rect source_unhover, SDL_Rect source_hover, SDL_Rect source_click, UiElement* parent=nullptr, j1Module* elementmodule=nullptr);
	const SDL_Texture* GetAtlas() const;

private:
	p2List<UiElement*> UiElementList;
	SDL_Texture* atlas;
	p2SString atlas_file_name;
};

class UiImage :public UiElement {
public:
	UiImage(int x, int y, SDL_Rect source_rect, UiElement* parent, j1Module* elementmodule);
	void Draw(SDL_Texture* atlas)override;
	void Update()override;
	SDL_Rect atlas_rect;
};

enum class Button_state{
	unhovered,
	hovered,
	clicked
};

class UiButton :public UiElement {
public:
	UiButton(int x, int y, SDL_Rect source_unhover, SDL_Rect source_hover, SDL_Rect source_click,UiElement* parent, j1Module* elementmodule);
	void Draw(SDL_Texture* atlas)override;
	void Update()override;
	SDL_Rect unhover;
	SDL_Rect hover;
	SDL_Rect click;
	Button_state current_state;
};

class UiText :public UiElement {
public:
	UiText(int x, int y,const char*text,int size,SDL_Color color, _TTF_Font*font = nullptr, UiElement* parent=nullptr, j1Module* elementmodule=nullptr);
	void Draw(SDL_Texture* atlas)override;
	void Update()override;
	_TTF_Font*font_type;
	const char*message;
	SDL_Color color;
};

#endif // __j1GUI_H__