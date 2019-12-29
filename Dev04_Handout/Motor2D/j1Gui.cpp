#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1MainMenu.h"
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
	focusedUi = nullptr;
	return true;
}

// Update all guis
bool j1Gui::PreUpdate()
{
	return true;
}

bool j1Gui::Update(float dt) {
	if (MouseClick() && UiUnderMouse() != nullptr && UiUnderMouse()->interactuable) {
		focusedUi = UiUnderMouse();
		if (focusedUi->Module != nullptr) {
			focusedUi->Module->ui_callback(focusedUi);
		}
	}
	else if (MouseClick() && UiUnderMouse() == nullptr)
		focusedUi = nullptr;
	if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN)
		focusedUi = FocusNextElement(focusedUi);
	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		if (focusedUi != nullptr&&focusedUi->Module != nullptr) {
			focusedUi->Module->ui_callback(focusedUi);
		}
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

void j1Gui::RemoveUiElement(UiElement* element) {
	if (UiElementList.find(element) != -1) {
		for (int i = 0; i < UiElementList.count(); ++i) {
			if (UiElementList[i]->parent == element)
				RemoveUiElement(UiElementList[i]);
		}
		UiElementList.del(UiElementList.At(UiElementList.find(element)));
	}
	else
		LOG("UiElement to delete not found");
}

void j1Gui::Update_Ui() {
	int dx, dy;
	App->input->GetMouseMotion(dx, dy);
	for (int i = 0; i < UiElementList.count(); ++i) {
		UiElementList[i]->Update(dx, dy);
	}
}

void j1Gui::Draw_Ui() {
	for (int i = 0; i < UiElementList.count(); ++i) {
		UiElementList[i]->Draw(atlas);
	}
}

UiElement* j1Gui::UiUnderMouse() {
	int x, y;
	App->input->GetMousePosition(x, y);
	if (!App->home->IsEneabled()) {
		x -= App->render->camera.x;
		y -= App->render->camera.y;
	}
	UiElement*Element = nullptr;
	for (int i = 0; i < UiElementList.count(); ++i) {
		if (UiElementList[i]->GetScreenPos().x < x && x < UiElementList[i]->GetScreenPos().x + UiElementList[i]->GetScreenRect().w && UiElementList[i]->GetScreenPos().y < y && y < UiElementList[i]->GetScreenPos().y + UiElementList[i]->GetScreenRect().h && (UiElementList[i]->interactuable || UiElementList[i]->draggable))
			Element = UiElementList[i];
	}
	return Element;
}

bool j1Gui::MouseClick() {
	return (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT);
}

void j1Gui::DraggUiElements(UiElement*parent, int dx, int dy) {
	for (int i = 0; i < UiElementList.count(); ++i) {
		if (UiElementList[i]->parent == parent) {
			UiElementList[i]->SetLocalPos(UiElementList[i]->GetLocalPos().x + dx, UiElementList[i]->GetLocalPos().y + dy);
			DraggUiElements(UiElementList[i], dx, dy);
		}
	}
}

UiElement*j1Gui::FocusNextElement(UiElement*current_element) {
	if (current_element == nullptr) {
		for (int i = 0; i < UiElementList.count(); ++i) {
			if (UiElementList[i]->interactuable)
				return UiElementList[i];
		}
	}
	else {
		bool iteration = true;
		for (int i = UiElementList.find(current_element) + 1; i < UiElementList.count(); ++i) {
			if (UiElementList[i]->interactuable)
				return UiElementList[i];
			if (i == UiElementList.count() - 1 && iteration) {
				iteration = false;
				i = 0;
				if (UiElementList[i]->interactuable)
					return UiElementList[i];
			}
		}
	}
	return nullptr;
}

UiElement* j1Gui::AddImage(int x, int y, SDL_Rect source_rect, bool interactuable, bool draggeable, UiElement* parent, j1Module* elementmodule) {
	UiElement* Image = new UiImage(x, y, source_rect, interactuable, draggeable, parent, elementmodule);
	UiElementList.add(Image);
	return Image;
}

UiElement* j1Gui::AddText(int x, int y, const char*text, _TTF_Font*font, SDL_Color color, int size, bool interactuable, bool draggeable, UiElement* parent, j1Module* elementmodule) {
	UiElement* Text = new UiText(x, y, text, size, color, interactuable, draggeable, font, parent, elementmodule);
	UiElementList.add(Text);
	return Text;
}

UiElement* j1Gui::AddButton(int x, int y, SDL_Rect source_unhover, SDL_Rect source_hover, SDL_Rect source_click, bool interactuable, bool draggeable, UiElement* parent, j1Module* elementmodule) {
	UiElement* Button = new UiButton(x, y, source_unhover, source_hover, source_click, interactuable, draggeable, parent, elementmodule);
	UiElementList.add(Button);
	return Button;
}


UiElement::UiElement(int x, int y, int w, int h, bool interactuable, bool draggeable, UiTypes uitype, UiElement* parent, j1Module* elementmodule) : type(uitype), parent(parent), Module(elementmodule), ui_rect({ x,y,w,h }), interactuable(interactuable), draggable(draggeable) { if (parent != nullptr)SetLocalPos(x, y); }

UiElement::~UiElement() {};

const iPoint UiElement::GetScreenPos() {
	iPoint position(ui_rect.x, ui_rect.y);
	return position;
}

const iPoint UiElement::GetLocalPos() {
	if (parent == nullptr) {
		iPoint position(ui_rect.x, ui_rect.y);
		return position;
	}
	else {
		iPoint position(ui_rect.x - parent->GetScreenPos().x, ui_rect.y - parent->GetScreenPos().y);
		return position;
	}
}

const SDL_Rect UiElement::GetScreenRect() {
	return ui_rect;
}

const SDL_Rect UiElement::GetLocalRect() {
	return { GetLocalPos().x,GetLocalPos().y,ui_rect.w,ui_rect.h };
}

void UiElement::SetLocalPos(int x, int y) {
	if (parent == nullptr) {
		ui_rect.x = x; ui_rect.y = y;
	}
	else {
		ui_rect.x = parent->GetScreenPos().x + x;
		ui_rect.y = parent->GetScreenPos().y + y;
	}
}bool UiElement::outofparent() {
	return (GetScreenPos().x + GetScreenRect().w*0.5f<parent->GetScreenPos().x || GetScreenPos().x>parent->GetScreenPos().x + parent->GetScreenRect().w - GetScreenRect().w*0.5 || GetScreenPos().y + GetScreenRect().h*0.5f<parent->GetScreenPos().y || GetScreenPos().y>parent->GetScreenPos().y + parent->GetScreenRect().h - GetScreenRect().h*0.5f);
}



UiImage::UiImage(int x, int y, SDL_Rect source_rect, bool interactuable, bool draggeable, UiElement* parent, j1Module* elementmodule) :UiElement(x, y, source_rect.w, source_rect.h, interactuable, draggeable, UiTypes::Image, parent, elementmodule), atlas_rect(source_rect) {}

void UiImage::Update(int dx, int dy) {
	//fer que la imatge es mogui amb la camera
	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN && App->gui->focusedUi == this)
		App->gui->focusedUi = nullptr;
	if (App->gui->MouseClick() && App->gui->focusedUi == this)
		App->gui->focusedUi = nullptr;
	if (draggable && App->gui->MouseClick() && App->gui->UiUnderMouse() == this && dx != 0 && dy != 0) {
		SetLocalPos(GetLocalPos().x + dx, GetLocalPos().y + dy);
		App->gui->DraggUiElements(this, dx, dy);
	}
}

void UiImage::Draw(SDL_Texture* atlas) {
	if (parent == nullptr || !outofparent())
		App->render->Blit(atlas, GetScreenPos().x, GetScreenPos().y, &atlas_rect);
}

UiText::UiText(int x, int y, const char*text, int size, SDL_Color color, bool interactuable, bool draggeable, _TTF_Font*font, UiElement* parent, j1Module* elementmodule) : UiElement(x, y, size, size, interactuable, draggeable, UiTypes::Text, parent, elementmodule), font_type(font), message(text), color(color), texture(App->font->Print(message, color, font_type)) {}

void UiText::Draw(SDL_Texture* atlas) {
	if (parent == nullptr || !outofparent())
		App->render->Blit(texture, GetScreenPos().x, GetScreenPos().y);
}

void UiText::Update(int dx, int dy) {
	//fer que el text es mogui amb la camera
	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN && App->gui->focusedUi == this)
		App->gui->focusedUi = nullptr;
	if (App->gui->MouseClick() && App->gui->focusedUi == this)
		App->gui->focusedUi = nullptr;
	if (draggable && App->gui->MouseClick() && App->gui->UiUnderMouse() == this && dx != 0 && dy != 0) {
		SetLocalPos(GetLocalPos().x + dx, GetLocalPos().y + dy);
		App->gui->DraggUiElements(this, dx, dy);
	}
}

UiButton::UiButton(int x, int y, SDL_Rect source_unhover, SDL_Rect source_hover, SDL_Rect source_click, bool interactuable, bool draggeable, UiElement* parent, j1Module* elementmodule) :UiElement(x, y, source_unhover.w, source_unhover.h, interactuable, draggeable, UiTypes::Button, parent, elementmodule), unhover(source_unhover), hover(source_hover), click(source_click), current_state(Button_state::unhovered) {}

void UiButton::Update(int dx, int dy) {
	if (App->gui->focusedUi == this)
		current_state = Button_state::hovered;
	else if (App->gui->UiUnderMouse() == this)
		current_state = Button_state::hovered;
	else
		current_state = Button_state::unhovered;
	if (App->gui->MouseClick() && App->gui->focusedUi == this) {
		current_state = Button_state::clicked;
		App->gui->focusedUi = nullptr;
	}
	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN && App->gui->focusedUi == this) {
		current_state = Button_state::clicked;
		App->gui->focusedUi = nullptr;
	}
	if (draggable && App->gui->MouseClick() && App->gui->UiUnderMouse() == this && dx != 0 && dy != 0) {
		SetLocalPos(GetLocalPos().x + dx, GetLocalPos().y + dy);
		App->gui->DraggUiElements(this, dx, dy);
	}
}

void UiButton::Draw(SDL_Texture*atlas) {
	if (parent == nullptr || !outofparent()) {
		switch (current_state) {
		case Button_state::unhovered:
			App->render->Blit(atlas, GetScreenPos().x, GetScreenPos().y, &unhover);
			break;
		case Button_state::hovered:
			App->render->Blit(atlas, GetScreenPos().x, GetScreenPos().y, &hover);
			break;
		case Button_state::clicked:
			App->render->Blit(atlas, GetScreenPos().x, GetScreenPos().y, &click);
			break;
		}
	}
}