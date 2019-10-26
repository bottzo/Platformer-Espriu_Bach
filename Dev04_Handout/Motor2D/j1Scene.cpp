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
#include "j1Player.h"

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
	player_sprite.create(config.child("player_sprite").attribute("name").as_string());
	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	App->map->Load(map_name.GetString());
	App->player->Load(player_sprite.GetString());
	
	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	if(App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		App->LoadGame();

	if(App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		App->SaveGame();

	if(App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		App->render->camera.y -= 30;

	if(App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		App->render->camera.y += 30;

	if(App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		App->render->camera.x += 15;

	if(App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		App->render->camera.x -= 15;

	App->player->Updateposition(App->player->current_santa_state(App->player->key_inputs));
	App->map->Draw();
	App->player->Draw_player(App->player->current_santa_state(App->player->key_inputs));

	p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
					App->map->data.width, App->map->data.height,
					App->map->data.tile_width, App->map->data.tile_height,
					App->map->data.tilesets.count());

	App->win->SetTitle(title.GetString());
	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
