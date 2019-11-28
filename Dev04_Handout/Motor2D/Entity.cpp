#include "Entity.h"
#include "player.h"

Entity::Entity(Types type) : type(type){}

Entity* EntityManager::CreateEntity(Entity::Types type) {
	//static_assert(Entity::Types::unknown == 2, "code needs update");
	Entity* ret = nullptr;
	switch (type) {
	//case Entity::Types::enemy: ret = new enemy(); break;
	case Entity::Types::player: ret = new player(); break;
	}
	if (ret != nullptr)
		Entity_list.add(ret);
	return ret;
}

void EntityManager::DestroyEntity(Entity* entity) {

}

bool EntityManager::Update(float dt)
{
	/*accumulated_time += dt;
	if (accumulated_time >= update_ms_cycle)
		do_logic = true;
	UpdateAll(dt, do_logic);
	if (do_logic == true) {
		accumulated_time = 0.0f;
		do_logic = false;
	}*/
	return true;
}

bool EntityManager::UpdateAll(float dt, bool do_logic) {

	return true;
}

Entity::Entity(Types type) : type(type) {}

EntityManager::EntityManager():j1Module () {
	name.create("Entities");
}

EntityManager::~EntityManager() {

}

bool EntityManager::Awake(pugi::xml_node&config) {
	folder.create(config.child("folder").child_value());
	/*player_texture_offset.x = config.child("texture_offset").attribute("x").as_int();
	player_texture_offset.y = config.child("texture_offset").attribute("y").as_int();
	slide_texture_offset.x = config.child("slide_offset").attribute("x").as_int();
	slide_texture_offset.y = config.child("slide_offset").attribute("x").as_int();*/
	return true;
}

bool EntityManager::CleanUp() {

	return true;
}