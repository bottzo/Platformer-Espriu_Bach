#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "j1Module.h"
#include "p2DynArray.h"
#include "p2Point.h"
#include "j1App.h"
#include "SDL/include/SDL.h"


struct SDL_Texture;
struct _TTF_Font;

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
	UiElement(int x, int y, int w, int h, bool interactuable, bool draggeable, UiTypes uitype, UiElement* parent, j1Module* elementmodule);
	virtual ~UiElement();
	virtual void Update(int dx, int dy) = 0;
	virtual void Draw(SDL_Texture* atlas) = 0;
	const SDL_Rect GetScreenRect();
	const SDL_Rect GetLocalRect();
	const iPoint GetLocalPos();
	const iPoint GetScreenPos();
	void SetLocalPos(int x, int y);
	bool outofparent();
	bool draggable;
	bool interactuable;
	UiTypes type;
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
	void RemoveUiElement(UiElement*element);
	void Update_Ui();
	void Draw_Ui();
	//If the ui has a parent the x,y will be the local coordenates respect the parent
	UiElement*AddImage(int x, int y, SDL_Rect source_rect, bool interactuable = true, bool draggeable = false, UiElement* parent = nullptr, j1Module* elementmodule = nullptr);
	//If the ui has a parent the x,y will be the local coordenates respect the parent
	UiElement* AddText(int x, int y, const char*text, _TTF_Font*font = nullptr, SDL_Color color = { 255, 255, 255, 255 }, int size = 12, bool interactuable = false, bool draggeable = false, UiElement* parent = nullptr, j1Module* elementmodule = nullptr);
	//If the ui has a parent the x,y will be the local coordenates respect the parent
	UiElement* AddButton(int x, int y, SDL_Rect source_unhover, SDL_Rect source_hover, SDL_Rect source_click, bool interactuable = true, bool draggeable = false, UiElement* parent = nullptr, j1Module* elementmodule = nullptr);
	void DraggUiElements(UiElement*parent, int dx, int dy);
	UiElement* UiUnderMouse();
	bool MouseClick();
	UiElement*FocusNextElement(UiElement*current_element);
	UiElement*focusedUi;
	const SDL_Texture* GetAtlas() const;

private:
	p2List<UiElement*> UiElementList;
	SDL_Texture* atlas;
	p2SString atlas_file_name;
};

class UiImage :public UiElement {
public:
	UiImage(int x, int y, SDL_Rect source_rect, bool interactuable, bool draggeable, UiElement* parent, j1Module* elementmodule);
	void Draw(SDL_Texture* atlas)override;
	void Update(int dx, int dy)override;
	SDL_Rect atlas_rect;
};

enum class Button_state {
	unhovered,
	hovered,
	clicked
};

class UiButton :public UiElement {
public:
	UiButton(int x, int y, SDL_Rect source_unhover, SDL_Rect source_hover, SDL_Rect source_click, bool interactuable, bool draggeable, UiElement* parent, j1Module* elementmodule);
	void Draw(SDL_Texture* atlas)override;
	void Update(int dx, int dy)override;
	SDL_Rect unhover;
	SDL_Rect hover;
	SDL_Rect click;
	Button_state current_state;
};

class UiText :public UiElement {
public:
	UiText(int x, int y, const char*text, int size, SDL_Color color, bool interactuable, bool draggeable, _TTF_Font*font = nullptr, UiElement* parent = nullptr, j1Module* elementmodule = nullptr);
	void Draw(SDL_Texture* atlas)override;
	void Update(int dx, int dy)override;
	_TTF_Font*font_type;
	const char*message;
	SDL_Color color;
	SDL_Texture* texture;
};

#endif // __j1GUI_H__