#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1PathFinding.h"
#include "j1Gui.h"
#include "j1Scene.h"
#include "j1Fonts.h"

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	if(App->map->Load("iso_walk.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;
		if(App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}

	debug_tex = App->tex->Load("maps/path2.png");
	// TODO 3: Create the banner (rect {485, 829, 328, 103}) as a UI element
	// TODO 4: Create the text "Hello World" as a UI element

	UiElement*window = App->gui->AddImage(350, 130, { 0, 512, 483, 512 },false,true);
	UiElement*Image = App->gui->AddImage(65, 100, { 485, 829, 328, 103 },false,false,window);
	UiElement*title = App->gui->AddText(140, 25, "SpaceShip", App->font->Load("fonts/open_sans/OpenSans-Light.ttf",48), { 0,255,255,255 }, 48,false,false,window);
	UiElement*Button=App->gui->AddButton(120, 230, { 642,169,229,69 }, { 0,113,229,69 }, { 411,169,229,69 },true,false,window);
	UiElement*text = App->gui->AddText(100, 23, "Button", App->font->Load("fonts/open_sans/OpenSans-Light.ttf"), { 0,255,255,255 },12,false,false,Button);
	input_lable = App->gui->AddImage(65, 300, { 488, 569, 344, 61 }, true, false, window,this);
	default_input_text = App->gui->AddText(10, 0, "Your name", App->font->Load("fonts/open_sans/OpenSans-Light.ttf",42), { 255,255,255,255 }, 42, false, false, input_lable);

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

// Called each loop iteration
bool j1Scene::PreUpdate()
{

	// debug pathfing ------------------
	static iPoint origin;
	static bool origin_selected = false;

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);

	if(App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if(origin_selected == true)
		{
			App->pathfinding->CreatePath(origin, p);
			origin_selected = false;
		}
		else
		{
			origin = p;
			origin_selected = true;
		}
	}

	if (App->input->reciving_text&&App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) {
		SDL_StopTextInput();
		App->input->reciving_text = false;
	}
	if (App->input->reciving_text&&App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN) {
		if (default_input_text != nullptr) {
			App->input->input_text.Cut(App->input->input_text.Length()-2);
			App->gui->RemoveUiElement(default_input_text);
			default_input_text = App->gui->AddText(10, 0, App->input->input_text.GetString(), App->font->fonts[3], { 255,255,255,255 }, 42, false, false, App->scene->input_lable);
		}
	}
	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	// Gui ---
	
	// -------
	if(App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN&& !App->input->reciving_text)
		App->LoadGame("save_game.xml");

	if(App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN && !App->input->reciving_text)
		App->SaveGame("save_game.xml");

	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += floor(200.0f * dt);

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= floor(200.0f * dt);

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += floor(200.0f * dt);

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= floor(200.0f * dt);

	App->map->Draw();
	App->gui->Draw_Ui();

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint map_coordinates = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y);
	p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d Tile:%d,%d",
					App->map->data.width, App->map->data.height,
					App->map->data.tile_width, App->map->data.tile_height,
					App->map->data.tilesets.count(),
					map_coordinates.x, map_coordinates.y);

	//App->win->SetTitle(title.GetString());

	// Debug pathfinding ------------------------------
	//int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);
	p = App->map->MapToWorld(p.x, p.y);

	App->render->Blit(debug_tex, p.x, p.y);

	const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();

	for(uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = App->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		App->render->Blit(debug_tex, pos.x, pos.y);
	}
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
