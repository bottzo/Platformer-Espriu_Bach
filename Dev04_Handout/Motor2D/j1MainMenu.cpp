#include "ModuleFadeToBlack.h"
#include "j1MainMenu.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Gui.h"
#include "j1Fonts.h"
#include "j1Collisions.h"
#include "Entity.h"
#include "j1Map.h"
#include "p2Log.h"

j1MainMenu::j1MainMenu() : j1Module() {
	name.create("MainMenu");
}
j1MainMenu::~j1MainMenu() {

}

bool j1MainMenu::Awake(pugi::xml_node&config) {
	LOG("Loading Main Menu");
	bool ret = true;
	folder.create(config.child("folder").child_value());
	BackgroundImagePath.create(config.child("backgroundImage").attribute("name").as_string());
	return ret;
}

bool j1MainMenu::Start() {
	p2SString tmp("%s%s", folder.GetString(), BackgroundImagePath.GetString());
	background_texture = App->tex->Load(tmp.GetString());
	MenuButtonsFont = App->font->Load("fonts/open_sans/OpenSans-ExtraBoldItalic.ttf", 42);
	Play_button = App->gui->AddButton(400, 430, { 642,169,229,69 }, { 0,113,229,69 }, { 411,169,229,69 }, true, false,nullptr,this);
	App->gui->AddText(68, 2, "PLAY", MenuButtonsFont, { 0,0,255,255 }, 42, false, false, Play_button);
	return true;
}


bool j1MainMenu::Update(float dt) {
	App->render->Blit(background_texture,0,0);
	return true;
}
bool j1MainMenu::CleanUp() {
	App->tex->UnLoad(background_texture);
	return true;
}

void j1MainMenu::ui_callback(UiElement*element) {
	if (element == Play_button) {
		App->entities->Enable();
		App->collisions->Enable();
		App->fade->FadeToBlack(this, App->map);
	}
}