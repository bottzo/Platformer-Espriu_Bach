#include "Entity.h"

/*Entity* entity_manager::CreateEntity(Entity::Types type) {
	//static_assert(Entity::Types::unknown == 2, "code needs update");
	Entity* ret = nullptr;
	switch (type) {
	case Entity::Types::enemy: ret = new enemy(); break;
	case Entity::Types::player: ret = new player(); break;
	}	if (ret != nullptr)
		Entities.PushBack(ret);
	return ret;
}

void entity_manager::DestroyEntity(Entity* entity) {

}

bool entity_manager::Update(float dt)
{
	accumulated_time += dt;
	if (accumulated_time >= update_ms_cycle)
		do_logic = true;
	UpdateAll(dt, do_logic);
	if (do_logic == true) {
		accumulated_time = 0.0f;
		do_logic = false;
	}
	return true;
}

Entity::Entity(Types type) : type(type) {}*/