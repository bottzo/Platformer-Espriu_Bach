#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "player.h"
#include "Enemy.h"
#include "j1Collisions.h"
#include "j1Pathfinding.h"
#include "j1Gui.h"
#include "j1MainMenu.h"
#include "j1Fonts.h"
#include "brofiler/Brofiler/Brofiler.h"

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
	Exit = false;
	exit.create("exit");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node&config)
{
	LOG("Loading Scene");
	bool ret = true;
	map_name.create(config.child("map_name").attribute("name").as_string());
	App->entities->Disable();
	App->map->Disable();
	App->collisions->Disable();
	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	App->map->Load(map_name.GetString());
	int w, h;
	uchar* data = NULL;
	if (App->map->CreateWalkabilityMap(w, h, &data))
		App->pathfinding->SetMap(w, h, data);
	RELEASE_ARRAY(data);
	App->entities->spawn_entities();
	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	if (App->input->reciving_text&&App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) {
		SDL_StopTextInput();
		App->input->reciving_text = false;
		check_console_input(App->input->input_text);
		App->input->input_text.Clear();
	}
	if (App->input->reciving_text&&App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN) {
		if (default_input_text != nullptr) {
			App->input->input_text.Cut(App->input->input_text.Length() - 2);
			App->gui->RemoveUiElement(default_input_text);
			default_input_text = App->gui->AddText(10, 0, App->input->input_text.GetString(), App->font->fonts[3], { 255,255,255,255 }, 42, false, false, App->scene->input_lable);
		}
	}
	return true;
}

void j1Scene::check_console_input(p2SString input) {
	if (input == exit) {
		Exit = true;
	}
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	BROFILER_CATEGORY("UpdateScene", Profiler::Color::HoneyDew);
	bool ret = true;
	if(App->map->IsEneabled())
		App->map->Draw();
	if (Exit) {
		ret = false;
		Exit = false;
	}
	return ret;
}

void j1Scene::ui_callback(UiElement*element) {
	if (element == input_lable) {
		//App->render->DrawQuad(default_input_text->GetScreenRect(), 255, 255, 255);
		App->gui->RemoveUiElement(default_input_text);
		SDL_StartTextInput();
		App->input->reciving_text = true;
	}
	if (element == Exit_button) {
		Exit = true;
	}
	if (element == Continue_button) {
		App->freeze = false;
		App->gui->RemoveUiElement(Settings_window);
		App->gui->RemoveUiElement(Exit_button);
		Settings_window = nullptr;
	}
}

bool j1Scene::positioncamera()
{
	App->render->camera.x = (-App->entities->GetPlayer()->position.x + ((App->win->width / 2) - (App->entities->GetPlayer()->sprite_tilesets.start->data->tile_width / 2)));
	if (App->render->camera.x > 0) {
		App->render->camera.x = 0;
	}
	App->render->camera.y = -(App->entities->GetPlayer()->position.y - App->win->height / 2);
	if (App->render->camera.y <= App->render->initial_camera_y)
		App->render->camera.y = App->render->initial_camera_y;

	//if (App->win->width / 2 < 0)App->render->camera.x = 0;
	//if (App->render->camera.y > App->render->initial_camera_y)App->render->camera.y = App->render->initial_camera_y;
	return true;
}
// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN&&!App->home->IsEneabled()) {
		if (Settings_window == nullptr) {
			Settings_window = App->gui->AddImage(-App->render->camera.x, -App->render->camera.y, { 0, 512, 483, 512 }, false, true);
			SceneButtonsFont = App->font->Load("fonts/open_sans/OpenSans-ExtraBoldItalic.ttf", 42);
			App->gui->AddText(170, 50, "PAUSE", SceneButtonsFont, { 0,0,255,255 }, 42, false, false, Settings_window);
			Exit_button = App->gui->AddButton(120, 230, { 642,169,229,69 }, { 0,113,229,69 }, { 411,169,229,69 }, true, false, Settings_window, this);
			App->gui->AddText(68, 2, "EXIT", SceneButtonsFont, { 0,0,255,255 }, 42, false, false, Exit_button);
			ContinueFont = App->font->Load("fonts/open_sans/OpenSans-ExtraBoldItalic.ttf", 36);
			Continue_button = App->gui->AddButton(120, 130, { 642,169,229,69 }, { 0,113,229,69 }, { 411,169,229,69 }, true, false, Settings_window, this);
			App->gui->AddText(23, 7, "CONTINUE", ContinueFont, { 0,0,255,255 }, 36, false, false, Continue_button);
		}
		App->freeze = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_DOWN && !App->home->IsEneabled()) {
		if (Console == nullptr) {
			if(SceneButtonsFont==nullptr)
				SceneButtonsFont = App->font->Load("fonts/open_sans/OpenSans-ExtraBoldItalic.ttf", 42);
			Console = App->gui->AddImage(-App->render->camera.x, -App->render->camera.y, { 0, 512, 483, 512 }, false, true);
			ConsoleTitle=App->gui->AddText(150, 50, "CONSOLE", SceneButtonsFont, { 0,0,255,255 }, 42, false, false, Console);
			input_lable = App->gui->AddImage(65, 100, { 488, 569, 344, 61 }, true, false, Console, this);
			default_input_text = App->gui->AddText(10, 0, "Type here", App->font->Load("fonts/open_sans/OpenSans-Light.ttf", 42), { 255,255,255,255 }, 42, false, false, input_lable);
		}
		else {
			Console->SetLocalPos(-App->render->camera.x, -App->render->camera.y);
			ConsoleTitle->SetLocalPos(150, 50);
			input_lable->SetLocalPos(65, 100);
		}
	}

	return ret;
}


// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
