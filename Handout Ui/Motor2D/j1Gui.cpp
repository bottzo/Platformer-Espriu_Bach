#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Gui.h"

j1Gui::j1Gui() : j1Module()
{
	name.create("gui");
}

// Destructor
j1Gui::~j1Gui()
{}

// Called before render is available
bool j1Gui::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlas_file_name = conf.child("atlas").attribute("file").as_string("");

	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{
	atlas = App->tex->Load(atlas_file_name.GetString());

	return true;
}

// Update all guis
bool j1Gui::PreUpdate()
{
	return true;
}

bool j1Gui::Update(float dt) {
	Update_Ui();
	Draw_Ui();
	return true;
}

// Called after all Updates
bool j1Gui::PostUpdate()
{
	return true;
}

// Called before quitting
bool j1Gui::CleanUp()
{
	LOG("Freeing GUI");
	DeleteAllUiElements();
	return true;
}

// const getter for atlas
const SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}

void j1Gui::DeleteAllUiElements() {
	UiElementList.clear();
}

void j1Gui::DeleteUiElement(UiElement* element) {
	if (UiElementList.find(element) != -1)
		UiElementList .del(UiElementList.At(UiElementList.find(element)));
	else
		LOG("UiElement to delete not found");
}

void j1Gui::Update_Ui() {
	for (int i = 0; i < UiElementList.count(); ++i) {
		UiElementList[i]->Update();
	}
}

void j1Gui::Draw_Ui() {
	for (int i = 0; i < UiElementList.count(); ++i) {
		UiElementList[i]->Draw(atlas);
	}
}

UiElement* j1Gui::AddImage(int x, int y, SDL_Rect source_rect, UiElement* parent, j1Module* elementmodule) {
	UiElement* Image = new UiImage(x,y,source_rect,parent,elementmodule);
	UiElementList.add(Image);
	return Image;
}

UiElement* j1Gui::AddText(int x, int y, const char*text, SDL_Color color, _TTF_Font*font, UiElement* parent, j1Module* elementmodule) {
	UiElement* Text = new UiText(x, y, text, color, font, parent, elementmodule);
	UiElementList.add(Text);
	return Text;
}

UiElement* j1Gui::AddButton(int x, int y, SDL_Rect source_unhover, SDL_Rect source_hover, SDL_Rect source_click, UiElement* parent, j1Module* elementmodule) {
	UiElement* Button = new UiButton(x, y,source_unhover,source_hover,source_click, parent, elementmodule);
	UiElementList.add(Button);
	return Button;
}


UiElement::UiElement(int x, int y, UiTypes uitype, UiElement* parent, j1Module* elementmodule) : type(uitype), parent(parent), Module(elementmodule) { position.x = x; position.y=y; };

UiElement::~UiElement() {};

UiImage::UiImage(int x, int y,SDL_Rect source_rect, UiElement* parent, j1Module* elementmodule):UiElement(x,y,UiTypes::Image,parent,elementmodule),Image(source_rect) {}

void UiImage::Update() {
	//fer que la imatge es mogui amb la camera
}

void UiImage::Draw(SDL_Texture* atlas) {
	App->render->Blit(atlas,position.x,position.y,&Image);
}

UiText::UiText(int x, int y, const char*text, SDL_Color color, _TTF_Font*font, UiElement* parent, j1Module* elementmodule) : UiElement(x, y, UiTypes::Text, parent, elementmodule), font_type(font), message(text), color(color) {}

void UiText::Draw(SDL_Texture* atlas) {
	App->render->Blit(App->font->Print(message,color, font_type),position.x,position.y);
}

void UiText::Update() {
	//fer que el text es mogui amb la camera
}

UiButton::UiButton(int x, int y, SDL_Rect source_unhover, SDL_Rect source_hover, SDL_Rect source_click, UiElement* parent, j1Module* elementmodule) :UiElement(x, y, UiTypes::Button, parent, elementmodule), unhover(source_unhover),hover(source_hover),click(source_click),current_state(Button_state::unhovered) {}

void UiButton::Update() {
	int x, y;
	App->input->GetMousePosition(x,y);
	if (position.x < x && x< position.x + unhover.w && position.y < y && y< position.y + unhover.h) {
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
			current_state = Button_state::clicked;
		else
			current_state = Button_state::hovered;
	}
	else {
		current_state = Button_state::unhovered;
	}
}

void UiButton::Draw(SDL_Texture*atlas) {
	switch (current_state) {
	case Button_state::unhovered:
		App->render->Blit(atlas, position.x, position.y, &unhover);
		break;
	case Button_state::hovered:
		App->render->Blit(atlas, position.x, position.y, &hover);
		break;
	case Button_state::clicked:
		App->render->Blit(atlas, position.x, position.y, &click);
		break;
	}
}