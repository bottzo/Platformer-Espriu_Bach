#include "ModuleFadeToBlack.h"
#include "j1MainMenu.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Gui.h"
#include "j1Fonts.h"
#include "j1Scene.h"
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
	TitlePath.create(config.child("TitleImage").attribute("name").as_string());
	return ret;
}

bool j1MainMenu::Start() {
	Exit = false;
	p2SString tmp("%s%s", folder.GetString(), BackgroundImagePath.GetString());
	p2SString tmp2("%s%s", folder.GetString(), TitlePath.GetString());
	background_texture = App->tex->Load(tmp.GetString());
	Title= App->tex->Load(tmp2.GetString());
	MenuButtonsFont = App->font->Load("fonts/open_sans/OpenSans-ExtraBoldItalic.ttf", 42);
	ContinueFont = App->font->Load("fonts/open_sans/OpenSans-ExtraBoldItalic.ttf", 36);
	Play_button = App->gui->AddButton(400, 430, { 642,169,229,69 }, { 0,113,229,69 }, { 411,169,229,69 }, true, false,nullptr,this);
	App->gui->AddText(68, 2, "PLAY", MenuButtonsFont, { 0,0,255,255 }, 42, false, false, Play_button);
	Continue_button = App->gui->AddButton(400, 530, { 642,169,229,69 }, { 0,113,229,69 }, { 411,169,229,69 }, true, false, nullptr, this);
	App->gui->AddText(23, 7, "CONTINUE", ContinueFont, { 0,0,255,255 }, 36, false, false, Continue_button);
	Exit_button = App->gui->AddButton(400, 630, { 642,169,229,69 }, { 0,113,229,69 }, { 411,169,229,69 }, true, false, nullptr, this);
	App->gui->AddText(68, 2, "EXIT", MenuButtonsFont, { 0,0,255,255 }, 42, false, false, Exit_button);
	return true;
}


bool j1MainMenu::Update(float dt) {
	App->render->Blit(background_texture,0,0);
	App->render->Blit(Title, 310, -60);
	bool ret = true;
	if (Exit) {
		ret = false;
		Exit = false;
	}
	return ret;
}
bool j1MainMenu::CleanUp() {
	App->tex->UnLoad(background_texture);
	App->tex->UnLoad(Title);
	App->gui->RemoveUiElement(Play_button);
	App->gui->RemoveUiElement(Continue_button);
	App->gui->RemoveUiElement(Exit_button);
	folder.Clear();
	BackgroundImagePath.Clear();
	TitlePath.Clear();
	App->font->fonts.clear();
	return true;
}

void j1MainMenu::ui_callback(UiElement*element) {
	if (element == Play_button) {
		App->fade->FadeToBlack(this, App->scene,1.5f);
	}
	if (element == Exit_button) {
		Exit = true;
	}
}