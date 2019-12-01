#include "Entity.h"
#include "player.h"
#include "j1Map.h"
#include "j1App.h"
#include "p2Log.h"
#include "j1Scene.h"
#include "J1Collisions.h"
#include "Enemy.h"
#include "brofiler/Brofiler/Brofiler.h"

Entity::Entity(Types type) : type(type) {}

Entity* EntityManager::CreateEntity(Entity::Types type) {
	//static_assert(Entity::Types::unknown == 3, "code needs update");
	Entity* ret = nullptr;
	switch (type) {
	case Entity::Types::ground_enemy: ret = new ground_enemy(); break;
	case Entity::Types::flying_enemy: ret = new flying_enemy(); break;
	case Entity::Types::player: ret = new player(); break;
	}
	if (ret != nullptr)
		Entity_list.add(ret);
	return ret;
}

void EntityManager::DestroyEntity(Entity* entity) {
	if (Entity_list.find(entity) != -1)
		Entity_list.del(Entity_list.At(Entity_list.find(entity)));
	else
		LOG("Entity to delete not found");
}

player* EntityManager::GetPlayer() const {
	p2List_item<Entity*>item = Entity_list.start->data;
	while (item.data != nullptr) {
		if(item.data->type==Entity::Types::player)
			return (player*)item.data;
		item = item.next->data;
	}
	return nullptr;

}

bool EntityManager::Update(float dt)
{
	acumulated_ms += dt*1000.0f;
	if (acumulated_ms >= update_ms_cycle)
		do_logic = true;
	UpdateAll(dt, do_logic);
	if (do_logic == true) {
		acumulated_ms = 0.0f;
		do_logic = false;
	}
	return true;
}

bool EntityManager::UpdateAll(float dt, bool do_logic) {
	BROFILER_CATEGORY("Update_all_entities", Profiler::Color::Aquamarine);
	santa_states state = App->entities->GetPlayer()->current_santa_state(key_inputs);
	if (do_logic) {
		App->entities->GetPlayer()->Updateposition(state);
	}
	App->scene->positioncamera();
	App->entities->GetPlayer()->Draw_player(state,dt);
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
	flying_enemy_sprite.create(config.child("enemy_sprite").attribute("name").as_string());
	ground_enemy_sprite.create(config.child("enemy_sprite").attribute("name").as_string());
	player_texture_offset.x = config.child("texture_offset").attribute("x").as_int();
	player_texture_offset.y = config.child("texture_offset").attribute("y").as_int();
	slide_texture_offset.x = config.child("slide_offset").attribute("x").as_int();
	slide_texture_offset.y = config.child("slide_offset").attribute("x").as_int();
	update_ms_cycle= config.child("logic_update_ms").attribute("ms").as_float();
	return true;
}

bool EntityManager::CleanUp() {
	key_inputs.Clear();
	Entity_list.clear();
	return true;
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

void EntityManager::OnCollision(Collider*c1, Collider*c2) {
	if (c2->type == COLLIDER_DEATH && c1->type==COLLIDER_PLAYER1) {
		App->entities->GetPlayer()->position.x = App->map->data.start->rect.x;
		App->entities->GetPlayer()->position.y = App->map->data.start->rect.y;
	}
	if (c2->type == END_COLLIDER && c1->type == COLLIDER_PLAYER1) {
		App->map->ChangeMaps("Santa's mountains.tmx");
	}
}