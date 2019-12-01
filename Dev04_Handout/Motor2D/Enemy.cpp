#include "Enemy.h"
#include "j1Map.h"
#include "j1App.h"
#include "j1Collisions.h"
#include "p2Log.h"


enemy::enemy() {
	switch (this->kind) {
	case Enemies::ground:
		this->type = Entity::Types::ground_enemy; break;
	case Enemies::air:
		this->type = Entity::Types::flying_enemy; break;
	}
}

ground_enemy::ground_enemy() {
	Load_Enemy(App->entities->ground_enemy_sprite.GetString());
	this->kind = Enemies::ground;
}

flying_enemy::flying_enemy() {
	Load_Enemy(App->entities->flying_enemy_sprite.GetString());
	this->kind = Enemies::air;
}

bool ground_enemy::Load_Enemy(const char* file_name) {
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

			LOG("Loading enemy animations");
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
			LOG("Loading enemy position and colliders");
			p2SString group_name; group_name.create("COLLAIDER_ENEMY");
			p2SString enemy_collider_string; enemy_collider_string.create("ground_enemy_collider");
			p2List_item<objectgroup*>*it = App->map->data.objectgroup.start;
			while (it != NULL) {
				if (it->data->name == group_name) {
					for (int i = 0; i < it->data->num_objects; ++i) {
						if (it->data->objects[i].name == enemy_collider_string) {
							enemy_collider = App->collisions->AddCollider(it->data->objects[i].rect, COLLIDER_ENEMY, App->entities);
							break;
						}
					}
				}
				it = it->next;
			}
			position.x = enemy_collider->rect.x;
			position.y = enemy_collider->rect.y;
		}
	}
	return ret;
}

bool flying_enemy::Load_Enemy(const char* file_name) {
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

			LOG("Loading enemy animations");
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
			LOG("Loading enemy position and colliders");
			p2SString group_name; group_name.create("COLLAIDER_ENEMY");
			p2SString enemy_collider_string; enemy_collider_string.create("flying_enemy_collider");
			p2List_item<objectgroup*>*it = App->map->data.objectgroup.start;
			while (it != NULL) {
				if (it->data->name == group_name) {
					for (int i = 0; i < it->data->num_objects; ++i) {
						if (it->data->objects[i].name == enemy_collider_string) {
							enemy_collider = App->collisions->AddCollider(it->data->objects[i].rect, COLLIDER_ENEMY, App->entities);
							break;
						}
					}
				}
				it = it->next;
			}
			position.x = enemy_collider->rect.x;
			position.y = enemy_collider->rect.y;
		}
	}
	return ret;
}

	/*#include "Application.h"
#include "Enemy.h"
#include "ModuleCollision.h"
#include "ModuleRender.h"

Enemy::Enemy(int x, int y) : position(x, y), collider(nullptr)
{}

Enemy::~Enemy()
{
	if (collider != nullptr)
		collider->to_delete = true;
}

const Collider* Enemy::GetCollider() const
{
	return collider;
}

void Enemy::Draw(SDL_Texture* sprites)
{
	if(collider != nullptr)
		collider->SetPos(position.x, position.y);

	App->render->Blit(sprites, position.x, position.y, &(animation->GetCurrentFrame()));
}**/
