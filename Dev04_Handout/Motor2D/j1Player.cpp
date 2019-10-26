#include "j1Player.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Map.h"
#include "j1Collisions.h"
j1Player::j1Player():j1Module(),player_loaded(false) {
	name.create("player");
}

j1Player::~j1Player() {

}

bool j1Player::Awake(pugi::xml_node&config) {

		LOG("Loading Player Parser");
		bool ret = true;

		folder.create(config.child("folder").child_value());

		return ret;
}

SDL_Rect&PlayerAnimation::GetCurrentFrame() {
	current_frame += frames->duration;
	if (current_frame >= total_frames)
		current_frame = 0;
	return frames[(int)current_frame].rect;
}

void j1Player::LoadAnimations(pugi::xml_node&node) {
	LOG("Loading player animations");
	for (node; node; node = node.next_sibling("tile")) {
		PlayerAnimation*animation = new PlayerAnimation();
		animation->name = node.child("properties").child("property").attribute("value").as_string();
		animation->total_frames = node.child("properties").child("property").next_sibling("property").attribute("value").as_uint();
		LOG("Loading %s animation with %d frames", animation->name,animation->total_frames);
		animation->texture = sprite_tilesets.start->data->texture;
		pugi::xml_node frame_node = node.child("animation").child("frame");
		animation->frames = new Frame[animation->total_frames];
		for (int i = 0;i<animation->total_frames; frame_node = frame_node.next_sibling("frame"),++i) {
			uint tileset_id = frame_node.attribute("tileid").as_uint();
			animation->frames[i].duration = frame_node.attribute("duration").as_float()/100;//Dividir entre 1000 pk sigui canvi d'e frame de l0'animacio en cada segon (esta en milisegons en el tmx)
			animation->frames[i].rect = sprite_tilesets.start->data->TilesetRect(tileset_id+1);//pk el +1??? pk la funcio tilesetrect conta el primer tile com si fos un 1 i no el zero
		}
		Animations.add(animation);
		LOG("Succesfully loaded %s animation", animation->name);
	}
}

void j1Player::Updateposition(santa_states state) {
	//speed.y +=App->map->data.gravity;
	switch (state) {
	case ST_IDLE_RIGHT:
		speed.x = 0;
		break;
	case ST_IDLE_LEFT:
		speed.x = 0;
		break;
	case ST_WALK_FORWARD:
		speed.x += 2;
		if (speed.x >= 25) {
			speed.x = 25;
		}
		break;
	case ST_WALK_BACKWARD:
		speed.x -= 2;
		if (speed.x <= -25) {
			speed.x = -25;
		}
		break;
	}
	position.x += speed.x;
	position.y += speed.y;
	player_collider->SetPos(position.x-(collider_player_offset_x), position.y-(collider_player_offset_y));
	slide_collider->SetPos(position.x - (collider_player_offset_x), position.y - (collider_player_offset_y-(player_collider->rect.h-slide_collider->rect.h)));
}

void j1Player::Draw_player(santa_states state) {
	switch (state) {
	case ST_IDLE_RIGHT:
		App->render->Blit(Animations.start->data->texture, position.x, position.y, &Animations.start->data->GetCurrentFrame());
		break;
	case ST_IDLE_LEFT:
		App->render->Blit(Animations.start->data->texture, position.x, position.y, &Animations.start->data->GetCurrentFrame(),SDL_FLIP_HORIZONTAL, sprite_tilesets.start->data);
		break;
	case ST_WALK_FORWARD:
		App->render->Blit(Animations.start->data->texture, position.x, position.y, &Animations.start->next->data->GetCurrentFrame());
		break;
	case ST_WALK_BACKWARD:
		App->render->Blit(Animations.start->data->texture, position.x, position.y, &Animations.start->next->data->GetCurrentFrame(),SDL_FLIP_HORIZONTAL,sprite_tilesets.start->data);
	}

	
}


void j1Player::change_player_collider(santa_states state) {

}

santa_states j1Player::current_santa_state(p2Qeue<santa_inputs>& inputs)
{
	static santa_states state = ST_IDLE_RIGHT;
	santa_inputs last_input;

	while (inputs.Pop(last_input))
	{
		switch (state)
		{
		case ST_IDLE_RIGHT:
		{
			switch (last_input)
			{
			case IN_RIGHT_DOWN: state = ST_WALK_FORWARD; break;
			case IN_LEFT_DOWN: state = ST_WALK_BACKWARD; break;
			case IN_JUMP: state = ST_JUMP_NEUTRAL; /*jump_timer = SDL_GetTicks();*/  break;
			//case IN_SLIDE_DOWN: state = ST_SLIDE_(cap a on?); break;
			}
		}
		break;

		case ST_IDLE_LEFT:
		{
			switch (last_input)
			{
			case IN_RIGHT_DOWN: state = ST_WALK_FORWARD; break;
			case IN_LEFT_DOWN: state = ST_WALK_BACKWARD; break;
			case IN_JUMP: state = ST_JUMP_NEUTRAL; /*jump_timer = SDL_GetTicks();*/  break;
				//case IN_SLIDE_DOWN: state = ST_SLIDE_(cap a on?); break;
			}
		}
		break;

		case ST_WALK_FORWARD:
		{
			switch (last_input)
			{
			case IN_RIGHT_UP: state = ST_IDLE_RIGHT; break;
			case IN_LEFT_AND_RIGHT: state = ST_IDLE_RIGHT; break;
			case IN_JUMP: state = ST_JUMP_FORWARD; /*jump_timer = SDL_GetTicks();*/  break;
			case IN_SLIDE_DOWN: state = ST_SLIDE_FORWARD; break;
			}
		}
		break;

		case ST_WALK_BACKWARD:
		{
			switch (last_input)
			{
			case IN_LEFT_UP: state = ST_IDLE_LEFT; break;
			case IN_LEFT_AND_RIGHT: state = ST_IDLE_LEFT; break;
			case IN_JUMP: state = ST_JUMP_BACKWARD; /*jump_timer = SDL_GetTicks();*/  break;
			case IN_SLIDE_DOWN: state = ST_SLIDE_BACKWARD; break;
			}
		}
		break;

		case ST_JUMP_NEUTRAL:
		{
			switch (last_input)
			{
			//case IN_JUMP_FINISH: state = ST_IDLE; break;
			}
		}
		break;

		case ST_JUMP_FORWARD:
		{
			switch (last_input)
			{
			case IN_JUMP_FINISH: state = ST_IDLE_RIGHT; break;
			}
		}
		break;

		case ST_JUMP_BACKWARD:
		{
			switch (last_input)
			{
			case IN_JUMP_FINISH: state = ST_IDLE_LEFT; break;
			}
		}
		break;

		case ST_SLIDE_FORWARD:
		{
			switch (last_input)
			{
			case IN_SLIDE_FINISH: state = ST_IDLE_RIGHT; break;
			}
		}
		break;
		case ST_SLIDE_BACKWARD:
		{
			switch (last_input)
			{
			case IN_SLIDE_FINISH: state = ST_IDLE_LEFT; break;
			}
		}
		break;
		}
	}
	return state;
}

bool j1Player::Load(const char* file_name) {

	bool ret = true;
	p2SString tmp("%s%s", folder.GetString(), file_name);

	pugi::xml_parse_result result = player_doc.load_file(tmp.GetString());

	if (result == NULL)
	{
		LOG("Could not loadplayer xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	if (ret == true)
	{
		bool ret = true;
		pugi::xml_node player_node = player_doc.child("map").child("tileset");
		if (player_node == NULL)
		{
			LOG("Error parsing player xml file: Cannot find 'tileset' tag.");
			ret = false;
		}
		else
		{
			pugi::xml_node tileset;
			for (tileset = player_node; tileset && ret; tileset = tileset.next_sibling("tileset"))
			{
				TileSet* set = new TileSet();

				if (ret == true)
				{
					ret = App->map->LoadTilesetDetails(tileset, set);
				}

				if (ret == true)
				{
					ret = App->map->LoadTilesetImage(tileset, set, this);
				}

				sprite_tilesets.add(set);
			}
			player_node = player_node.child("tile");
			LoadAnimations(player_node);
			Load_player_info();
		}
		return ret;
	}
}

void j1Player::Load_player_info() {
	p2SString group_name; group_name.create("COLLAIDER_PLAYER");
	p2SString start; start.create("Start");
	p2SString player; player.create("player_collider");
	p2SString slide; slide.create("slide_collider");
	p2List_item<objectgroup*>*it = App->map->data.objectgroup.start;
	while (it != NULL) {
		if (it->data->name == group_name) {
			for (int i = 0; i < it->data->num_objects; ++i) {
				if (it->data->objects[i].name == start) {
					position.x = it->data->objects[i].rect.x;
					position.y = it->data->objects[i].rect.y;
				}
				else if (it->data->objects[i].name == player) {
					player_collider=App->collisions->AddCollider(it->data->objects[i].rect, COLLIDER_PLAYER1, App->player);
				}
				else if (it->data->objects[i].name == slide) {
					slide_collider=App->collisions->AddCollider(it->data->objects[i].rect, COLLIDER_PLAYER1, App->player);
				}
			}
			break;
		}
		it = it->next;
	}
	collider_player_offset_x = position.x - player_collider->rect.x;
	collider_player_offset_y = position.y - player_collider->rect.y;
}
//bool j1Player::positioncamera()
//{
//	App->render->camera.x = position.x - App->render->camera.w / 3;
//	if (App->render->camera.x < 0)App->render->camera.x = 0;
//	return true;
//}
//bool j1Player::PostUpdate() {
//	positioncamera();
//	return true;
//}



bool j1Player::CleanUp() {
	key_inputs.Clear();
	p2List_item<PlayerAnimation*>* item;
	item = Animations.start;

	while (item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	Animations.clear();
	p2List_item<TileSet*>* it;
	it = sprite_tilesets.start;

	while (it != NULL)
	{
		RELEASE(it->data);
		it = it->next;
	}
	sprite_tilesets.clear();
	player_doc.reset();
	return true;
}