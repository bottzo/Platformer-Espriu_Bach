#include "Entity.h"
#include "player.h"
#include "j1Map.h"
#include "j1App.h"
#include "p2Log.h"
#include "j1Scene.h"
#include "J1Collisions.h"
#include "brofiler/Brofiler/Brofiler.h"

Entity::Entity(Types type) : type(type) { Load_Entity(App->entities->player_sprite.GetString()); }

Entity* EntityManager::CreateEntity(Entity::Types type) {
	//static_assert(Entity::Types::unknown == 3, "code needs update");
	Entity* ret = nullptr;
	switch (type) {
	//case Entity::Types::enemy: ret = new ground_enemy(); break;
	case Entity::Types::player: ret = new player(); break;
	}
	if (ret != nullptr)
		Entity_list.add(ret);
	return ret;
}

void EntityManager::DestroyEntity(Entity* entity) {

	p2List_item<Animation*>* item;
	item = entity->Animations.start;

	while (item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	entity->Animations.clear();
	p2List_item<TileSet*>* it;
	it = entity->sprite_tilesets.start;

	while (it != NULL)
	{
		RELEASE(it->data);
		it = it->next;
	}
	entity->sprite_tilesets.clear();
	entity->Entity_doc.reset();
}

bool EntityManager::Update(float dt)
{
	acumulated_ms += dt*1000.0f;
	if (acumulated_ms >= update_ms_cycle)
		do_logic = true;
	UpdateAll(dt,acumulated_ms/1000.0f, do_logic);
	if (do_logic == true) {
		acumulated_ms = 0.0f;
		do_logic = false;
	}
	return true;
}

bool EntityManager::UpdateAll(float s,float acumulated_s, bool do_logic) {
	BROFILER_CATEGORY("Update_all_entities", Profiler::Color::Aquamarine);
	santa_states state = App->scene->Player->current_santa_state(key_inputs);
	if (do_logic) {
		App->scene->Player->Updateposition(state,acumulated_s);
		App->scene->positioncamera();
	}
	App->scene->Player->Draw_player(state, s);
	return true;
}

EntityManager::EntityManager():j1Module () {
	name.create("Entities");
	acumulated_ms = 0.0f;
	do_logic = false;
}

EntityManager::~EntityManager() {

}

bool EntityManager::Awake(pugi::xml_node&config) {
	folder=config.child("folder").child_value();
	player_sprite.create(config.child("player_sprite").attribute("name").as_string());
	player_texture_offset.x = config.child("texture_offset").attribute("x").as_int();
	player_texture_offset.y = config.child("texture_offset").attribute("y").as_int();
	slide_texture_offset.x = config.child("slide_offset").attribute("x").as_int();
	slide_texture_offset.y = config.child("slide_offset").attribute("x").as_int();
	update_ms_cycle= config.child("logic_update_ms").attribute("ms").as_float();
	return true;
}

bool EntityManager::CleanUp() {
	key_inputs.Clear();
	p2List_item<Entity*>* item;
	item = Entity_list.start;

	while (item != NULL)
	{
		DestroyEntity(item->data);
		item = item->next;
	}
	return true;
}

bool Entity::Load_Entity(const char* file_name) {

	bool ret = true;
	p2SString tmp("%s%s", App->entities->folder.GetString(), file_name);

	pugi::xml_parse_result result = Entity_doc.load_file(tmp.GetString());

	if (result == NULL)
	{
		LOG("Could not loadplayer xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	if (ret == true)
	{
		bool ret = true;
		pugi::xml_node entity_node = Entity_doc.child("map").child("tileset");
		if (entity_node == NULL)
		{
			LOG("Error parsing player xml file: Cannot find 'tileset' tag.");
			ret = false;
		}
		else
		{
			pugi::xml_node tileset;
			for (tileset = entity_node; tileset && ret; tileset = tileset.next_sibling("tileset"))
			{
				TileSet* set = new TileSet();

				if (ret == true)
				{
					ret = App->map->LoadTilesetDetails(tileset, set);
				}

				if (ret == true)
				{
					ret = App->map->LoadTilesetImage(tileset, set, App->entities);
				}

				sprite_tilesets.add(set);
			}
			entity_node = entity_node.child("tile");
			LoadAnimations(entity_node);
		}
		return ret;
	}
}

SDL_Rect&Animation::GetCurrentFrame(float dt) {
	current_frame_time += dt;
	if (frames->duration < current_frame_time) {
		current_frame += 1;
		current_frame_time = 0.0f;
	}
	if (current_frame >= total_frames)
		current_frame = 0;
	return frames[current_frame].rect;
}

SDL_Rect&Animation::DoOneLoop(float dt) {
	current_frame_time += dt;
	if (frames->duration < current_frame_time) {
		current_frame += 1;
		current_frame_time = 0.0f;
	}
	if (current_frame >= total_frames)
		current_frame =total_frames-1;
	return frames[current_frame].rect;
}

void Entity::LoadAnimations(pugi::xml_node&node) {
	LOG("Loading player animations");
	for (node; node; node = node.next_sibling("tile")) {
		Animation*animation = new Animation();
		animation->name.create(node.child("properties").child("property").attribute("value").as_string());
		animation->total_frames = node.child("properties").child("property").next_sibling("property").attribute("value").as_uint();
		LOG("Loading %s animation with %d frames", animation->name.GetString(), animation->total_frames);
		animation->texture = sprite_tilesets.start->data->texture;
		pugi::xml_node frame_node = node.child("animation").child("frame");
		animation->frames = new Frame[animation->total_frames];
		for (int i = 0; i < animation->total_frames; frame_node = frame_node.next_sibling("frame"), ++i) {
			uint tileset_id = frame_node.attribute("tileid").as_uint();
			animation->frames[i].duration = frame_node.attribute("duration").as_float()/1000;//Dividir entre 1000 pk sigui canvi d'e frame de l0'animacio en cada segon (esta en milisegons en el tmx)
			animation->frames[i].rect = sprite_tilesets.start->data->TilesetRect(tileset_id + 1);//pk el +1??? pk la funcio tilesetrect conta el primer tile com si fos un 1 i no el zero
		}
		Animations.add(animation);
		LOG("Succesfully loaded %s animation", animation->name.GetString());
	}
}

void EntityManager::OnCollision(Collider*c1, Collider*c2) {
	if (c2->type == COLLIDER_DEATH && c1->type==COLLIDER_PLAYER1) {
		App->scene->Player->position.x = App->map->data.start->rect.x;
		App->scene->Player->position.y = App->map->data.start->rect.y;
	}
	if (c2->type == END_COLLIDER && c1->type == COLLIDER_PLAYER1) {
		App->map->ChangeMaps("Santa's mountains.tmx");
	}
}