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
#include "brofiler/Brofiler/Brofiler.h"

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
	Player = nullptr;
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
	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	Player = (player*)App->entities->CreateEntity(Entity::Types::player);
	App->map->Load(map_name.GetString());
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
	BROFILER_CATEGORY("UpdateScene", Profiler::Color::HoneyDew);
	if(App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		App->LoadGame();

	if(App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		App->SaveGame();

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y -= 100*dt;
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y += 100*dt;
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x -= 100*dt;
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x += 100*dt;
	
	/*state = Player->current_santa_state(Player->key_inputs);
	Player->Updateposition(state);
	App->map->Draw();
	Player->Draw_player(state);*/

	/*p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
					App->map->data.width, App->map->data.height,
					App->map->data.tile_width, App->map->data.tile_height,
					App->map->data.tilesets.count());

	App->win->SetTitle(title.GetString());*/
	App->map->Draw();
	return true;
}

bool j1Scene::positioncamera()
{
	App->render->camera.x = -Player->position.x + ((App->win->width / 2) - (Player->sprite_tilesets.start->data->tile_width / 2));
	if (App->render->camera.x > 0) {
		App->render->camera.x = 0;
	}
	App->render->camera.y = -(Player->position.y - App->win->height / 2);
	if (App->render->camera.y <= App->render->initial_camera_y)
		App->render->camera.y = App->render->initial_camera_y;

	//if (App->win->width / 2 < 0)App->render->camera.x = 0;
	//if (App->render->camera.y > App->render->initial_camera_y)App->render->camera.y = App->render->initial_camera_y;
	return true;
}
// Called each loop iteration
bool j1Scene::PostUpdate()
{
	BROFILER_CATEGORY("ScenePostUpdate", Profiler::Color::Pink);
	bool ret = true;

	//positioncamera();

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
