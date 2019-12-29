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

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	BROFILER_CATEGORY("UpdateScene", Profiler::Color::HoneyDew);
	if(App->map->IsEneabled())
		App->map->Draw();
	return true;
}

void j1Scene::ui_callback(UiElement*element) {
	if (element == input_lable) {
		//App->render->DrawQuad(default_input_text->GetScreenRect(), 255, 255, 255);
		App->gui->RemoveUiElement(default_input_text);
		SDL_StartTextInput();
		App->input->reciving_text = true;
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
		if(Settings_window==nullptr)
			Settings_window = App->gui->AddImage(-App->render->camera.x, -App->render->camera.y, { 0, 512, 483, 512 }, false, true);
		App->freeze = true;
	}
	return ret;
}


// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
