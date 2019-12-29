#include "Entity.h"
#include "player.h"
#include "j1Map.h"
#include "j1App.h"
#include "p2Log.h"
#include "j1Scene.h"
#include "J1Collisions.h"
#include "Enemy.h"
#include "j1Window.h"
#include "brofiler/Brofiler/Brofiler.h"

Entity::Entity(Types type) : type(type) {}

Entity::~Entity(){}

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
	p2List_item<Entity*>*item = Entity_list.start;
	while (item->data != nullptr) {
		if(item->data->type==Entity::Types::player)
			return (player*)item->data;
		item = item->next;
	}
	return nullptr;
}

bool EntityManager::Start() {
	App->collisions->Enable();
	App->map->Enable();
	return true;
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

void EntityManager::spawn_entities() {
	p2SString enemy_group_name; enemy_group_name.create("COLLIDER_ENEMY");
	p2SString ground_collider_string; ground_collider_string.create("ground_enemy_collider");
	p2SString flying_collider_string; flying_collider_string.create("flying_enemy_collider");
	p2SString player_group_name; player_group_name.create("COLLAIDER_PLAYER");
	p2SString player_string; player_string.create("player_collider");
	p2SString slide_collider_name; slide_collider_name.create("slide_collider");
	p2List_item<objectgroup*>*it = App->map->data.objectgroup.start;
	while (it != NULL) {
		if (it->data->name == enemy_group_name) {
			enemy*current=nullptr;
			for (int i = 0; i < it->data->num_objects; ++i) {
				if (it->data->objects[i].name == ground_collider_string) {
					current=(enemy*)CreateEntity(Entity::Types::ground_enemy);
					current->enemy_collider = App->collisions->AddCollider(it->data->objects[i].rect, COLLIDER_ENEMY, App->entities);
					current->position.x = current->enemy_collider->rect.x - ground_texture_offset;
					current->position.y = current->enemy_collider->rect.y;
				}
				else if(it->data->objects[i].name == flying_collider_string) {
					current = (enemy*)CreateEntity(Entity::Types::flying_enemy);
					current->enemy_collider = App->collisions->AddCollider(it->data->objects[i].rect, COLLIDER_ENEMY, App->entities);
					current->position.x = current->enemy_collider->rect.x;
					current->position.y = current->enemy_collider->rect.y;
				}
			}
		}
		else if (it->data->name == player_group_name) {
			player*ptr=nullptr;
			for (int i = 0; i < it->data->num_objects; ++i) {
				if (it->data->objects[i].name == player_string) {
					if(ptr==nullptr)
						ptr=(player*)CreateEntity(Entity::Types::player);
					ptr->player_collider = App->collisions->AddCollider(it->data->objects[i].rect, COLLIDER_PLAYER1, App->entities);
					ptr->position.x = App->map->data.start->rect.x;
					ptr->position.y = App->map->data.start->rect.y;
				}
				else if (it->data->objects[i].name == slide_collider_name) {
					if (ptr == nullptr)
						ptr = (player*)CreateEntity(Entity::Types::player);
					ptr->slide_collider = App->collisions->AddCollider(it->data->objects[i].rect, COLLIDER_PLAYER1, App->entities);
					ptr->slide_collider->active = false;
				}
			}
		}
		it = it->next;
	}
}

bool Entity::Load_Entity(const char* file_name) {
	bool ret = true;
	p2SString tmp("%s%s", App->entities->folder.GetString(), file_name);

	pugi::xml_parse_result result = Entity_doc.load_file(tmp.GetString());

	if (result == NULL)
	{
		LOG("Could not load entity xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	if (ret == true)
	{
		pugi::xml_node entity_node = Entity_doc.child("map").child("tileset");
		if (entity_node == NULL)
		{
			LOG("Error parsing entity xml file: Cannot find 'tileset' tag.");
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

			LOG("Loading entity animations");
			for (entity_node; entity_node; entity_node = entity_node.next_sibling("tile")) {
				Animation*animation = new Animation();
				animation->name.create(entity_node.child("properties").child("property").attribute("value").as_string());
				animation->total_frames = entity_node.child("properties").child("property").next_sibling("property").attribute("value").as_uint();
				LOG("Loading %s animation with %d frames", animation->name.GetString(), animation->total_frames);
				animation->texture = sprite_tilesets.start->data->texture;
				pugi::xml_node frame_node = entity_node.child("animation").child("frame");
				animation->frames = new Frame[animation->total_frames];
				for (int i = 0; i < animation->total_frames; frame_node = frame_node.next_sibling("frame"), ++i) {
					uint tileset_id = frame_node.attribute("tileid").as_uint();
					animation->frames[i].duration = frame_node.attribute("duration").as_float() / 1000;//Dividir entre 1000 pk sigui canvi d'e frame de l0'animacio en cada segon (esta en milisegons en el tmx)
					animation->frames[i].rect = sprite_tilesets.start->data->TilesetRect(tileset_id + 1);//pk el +1??? pk la funcio tilesetrect conta el primer tile com si fos un 1 i no el zero
				}
				Animations.add(animation);
				LOG("Succesfully loaded %s animation", animation->name.GetString());
			}
		}
	}
	return ret;
}

void EntityManager::update_enemies() {
	p2List_item<Entity*>*item = Entity_list.start;
	enemy*current = nullptr;
	while (item != NULL) {
		if (item->data->type != Entity::Types::flying_enemy && item->data->type != Entity::Types::ground_enemy) {
			item = item->next;
			continue;
		}
		current = (enemy*)item->data;
		if (abs(current->position.x - GetPlayer()->position.x)<App->win->width) {
			current->move();
		}
		item = item->next;
	}
}

void EntityManager::draw_enemies(float dt) {
	p2List_item<Entity*>*item = Entity_list.start;
	enemy*current = nullptr;
	while (item != NULL) {
		if (item->data->type != Entity::Types::flying_enemy && item->data->type != Entity::Types::ground_enemy) {
			item = item->next;
			continue;
		}
		current = (enemy*)item->data;
		if (abs(current->position.x - GetPlayer()->position.x) < App->win->width) {
			current->Draw_Enemy(dt);
		}
		item = item->next;
	}
}

bool EntityManager::UpdateAll(float dt, bool do_logic) {
	BROFILER_CATEGORY("Update_all_entities", Profiler::Color::Aquamarine);
	santa_states state = GetPlayer()->current_santa_state(key_inputs);
	if (do_logic) {
		GetPlayer()->Updateposition(state);
		update_enemies();
	}
	App->scene->positioncamera();
	GetPlayer()->Draw_player(state,dt);
	draw_enemies(dt);
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
	flying_enemy_sprite.create(config.child("flying_enemy_sprite").attribute("name").as_string());
	ground_enemy_sprite.create(config.child("ground_enemy_sprite").attribute("name").as_string());
	player_texture_offset.x = config.child("texture_offset").attribute("x").as_int();
	player_texture_offset.y = config.child("texture_offset").attribute("y").as_int();
	slide_texture_offset.x = config.child("slide_offset").attribute("x").as_int();
	slide_texture_offset.y = config.child("slide_offset").attribute("x").as_int();
	ground_texture_offset = config.child("ground_texture_offset").attribute("x").as_int();
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
	/*if (c1->type == COLLIDER_ENEMY && c2->type == COLLIDER_WALL) {
		if (App->scene->penguin->enemy_collider == c1) {
			App->scene->penguin->speed.y = 0;
			App->scene->penguin->on_the_ground = true;
		}
	}*/
	if (c1->type == COLLIDER_PLAYER1 && c2->type == COLLIDER_ENEMY) {
		App->entities->GetPlayer()->position.x = App->map->data.start->rect.x;
		App->entities->GetPlayer()->position.y = App->map->data.start->rect.y;
		/*if (App->scene->penguin != nullptr) {
			if (App->scene->penguin->enemy_collider == c2) {
				App->entities->DestroyEntity(App->entities->FindGroundEnemy(App->scene->penguin));
				delete App->scene->penguin;
				App->scene->penguin = nullptr;
				c2->to_delete = true;
			}
		}
		if ( App->scene->bee != nullptr) {
			if (App->scene->bee->enemy_collider == c2) {
				App->entities->DestroyEntity(App->entities->FindFlyingEnemy(App->scene->bee));
				delete App->scene->bee;
				App->scene->bee = nullptr;
				c2->to_delete = true;
			}
		}*/
	}
}