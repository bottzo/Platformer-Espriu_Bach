#include "player.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Map.h"
#include "j1Collisions.h"
#include "j1Window.h"
#include "brofiler/Brofiler/Brofiler.h"

player::player() : Entity(Types::player)
{
	Load_Player(App->entities->player_sprite.GetString());
}

player::~player() 
{
}

bool player::Load_Player(const char* file_name) {

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

			LOG("Loading player animations");
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
			LOG("Loading player position and colliders");
			p2SString group_name; group_name.create("COLLAIDER_PLAYER");
			p2SString player; player.create("player_collider");
			p2SString slide_collider_name; slide_collider_name.create("slide_collider");
			p2List_item<objectgroup*>*it = App->map->data.objectgroup.start;
			while (it != NULL) {
				if (it->data->name == group_name) {
					for (int i = 0; i < it->data->num_objects; ++i) {
						if (it->data->objects[i].name == player) {
							player_collider = App->collisions->AddCollider(it->data->objects[i].rect, COLLIDER_PLAYER1, App->entities);
						}
						else if (it->data->objects[i].name == slide_collider_name) {
							slide_collider = App->collisions->AddCollider(it->data->objects[i].rect, COLLIDER_PLAYER1, App->entities);
							slide_collider->active = false;
						}
					}
					break;
				}
				it = it->next;
			}
			position.x = App->map->data.start->rect.x;
			position.y = App->map->data.start->rect.y;
		}
		return ret;
	}
}

void player::Updateposition(santa_states state) {
	BROFILER_CATEGORY("Updateposition", Profiler::Color::DarkRed);
	speed.y += App->map->data.gravity;
	switch (state) {
	case ST_IDLE_RIGHT:
			speed.x = 0;
		looking_right = true;
		break;
	case ST_IDLE_LEFT:
		speed.x = 0;
		looking_right = false;
		break;
	case ST_WALK_FORWARD:
		speed.x = 20;
		looking_right = true;
		break;
	case ST_WALK_BACKWARD:
		speed.x = -20;
		looking_right = false;
		break;
	case ST_SLIDE_FORWARD:
		speed.x = 30;
		looking_right = true;
		if (start_slide) {
			position.x -= slide_collider->rect.w - (player_collider->rect.w + (App->entities->player_texture_offset.x - App->entities->slide_texture_offset.x));
			slide_collider->SetPos(position.x + App->entities->slide_texture_offset.x, position.y + App->entities->slide_texture_offset.y);
			start_slide = false;
		}
		break;
	case ST_SLIDE_BACKWARD:
		speed.x = -30;
		looking_right = false;
		if (start_slide) {
			position.x += App->entities->player_texture_offset.x - App->entities->slide_texture_offset.x;
			slide_collider->SetPos(position.x + App->entities->slide_texture_offset.x, position.y + App->entities->slide_texture_offset.y);
			start_slide = false;
		}
		break;
	case ST_JUMP:
		if (start_jump) {
			speed.y = -35;
			start_jump = false;
		}
		if (move_in_air) {
			if (looking_right) {
				if (speed.x < 20) {
					speed.x += 4;
				}
				else
					speed.x = 20;
			}
			else {
				if (speed.x > -20) {
					speed.x -= 4;
				}
				else
					speed.x = -20;
			}
		}
		break;
	}

	distance.y = App->collisions->closest_yaxis_collider(state);
	if (speed.y < 0) {
		if (speed.y <= -distance.y) {
			position.y -= distance.y;
			speed.y = 0;
		}
		else {
			position.y += speed.y;
		}
	}
	else if (speed.y > 0) {
		if (speed.y >= distance.y) {
			position.y += distance.y;
			speed.y = 0;
			if (state == ST_JUMP) {
				App->entities->key_inputs.Push(IN_JUMP_FINISH);
				move_in_air = false;
				Animations.start->next->next->next->data->current_frame = 0;
				start_jump = true;
			}
		}
		else {
			position.y += speed.y;
		}
	}

	player_collider->SetPos(position.x + App->entities->player_texture_offset.x, position.y + App->entities->player_texture_offset.y);
	slide_collider->SetPos(position.x + App->entities->slide_texture_offset.x, position.y + App->entities->slide_texture_offset.y);

	distance.x=App->collisions->closest_xaxis_collider(state,looking_right);
	if (looking_right) {
		if (speed.x >= distance.x) {
			position.x += distance.x;
			speed.x = 0;
		}
		else {
			position.x += (speed.x);
		}
	}
	else {
		if (speed.x <= -distance.x) {
			position.x += -distance.x;
			speed.x = 0;
		}
		else {
			position.x += speed.x;
		}
	}
	player_collider->SetPos(position.x+ App->entities->player_texture_offset.x, position.y +App->entities->player_texture_offset.y);
	slide_collider->SetPos(position.x + App->entities->slide_texture_offset.x, position.y+ App->entities->slide_texture_offset.y);
}

void player::Draw_player(santa_states state,float dt) {
	BROFILER_CATEGORY("DrawPlayer", Profiler::Color::DarkKhaki);
	switch (state) {
	case ST_IDLE_RIGHT:
		App->render->Blit(Animations.start->data->texture, position.x, position.y, &Animations.start->data->GetCurrentFrame(dt));
		break;
	case ST_IDLE_LEFT:
		App->render->Blit(Animations.start->data->texture, position.x, position.y, &Animations.start->data->GetCurrentFrame(dt),SDL_FLIP_HORIZONTAL);
		break;
	case ST_WALK_FORWARD:
		App->render->Blit(Animations.start->data->texture, position.x, position.y, &Animations.start->next->data->GetCurrentFrame(dt));
		break;
	case ST_WALK_BACKWARD:
		App->render->Blit(Animations.start->data->texture, position.x, position.y, &Animations.start->next->data->GetCurrentFrame(dt),SDL_FLIP_HORIZONTAL);
		break;
	case ST_SLIDE_FORWARD:
		App->render->Blit(Animations.start->data->texture, position.x, position.y, &Animations.start->next->next->next->next->data->GetCurrentFrame(dt));
		break;
	case ST_SLIDE_BACKWARD:
		App->render->Blit(Animations.start->data->texture, position.x, position.y, &Animations.start->next->next->next->next->data->GetCurrentFrame(dt), SDL_FLIP_HORIZONTAL);
		break;
	case ST_JUMP:
		if(looking_right)
			App->render->Blit(Animations.start->data->texture, position.x, position.y, &Animations.start->next->next->next->data->DoOneLoop(dt));
		else {
			App->render->Blit(Animations.start->data->texture, position.x, position.y, &Animations.start->next->next->next->data->DoOneLoop(dt), SDL_FLIP_HORIZONTAL);
		}
		break;

	}
}

santa_states player::current_santa_state(p2Qeue<inputs>& input)
{
	static santa_states state = ST_IDLE_RIGHT;
	inputs last_input;

	while (input.Pop(last_input))
	{
		switch (state)
		{
		case ST_IDLE_RIGHT:
		{
			switch (last_input)
			{
			case IN_RIGHT_DOWN: state = ST_WALK_FORWARD; break;
			case IN_LEFT_DOWN: state = ST_WALK_BACKWARD; break;
			case IN_JUMP: state = ST_JUMP; /*jump_timer = SDL_GetTicks();*/  break;
			case IN_SLIDE_DOWN: state = ST_SLIDE_FORWARD; slide_timer = SDL_GetTicks(); player_collider->active = false; slide_collider->active = true; start_slide = true; break;
			}
		}
		break;

		case ST_IDLE_LEFT:
		{
			switch (last_input)
			{
			case IN_RIGHT_DOWN: state = ST_WALK_FORWARD; break;
			case IN_LEFT_DOWN: state = ST_WALK_BACKWARD; break;
			case IN_JUMP: state = ST_JUMP; /*jump_timer = SDL_GetTicks();*/  break;
			case IN_SLIDE_DOWN: state = ST_SLIDE_BACKWARD; slide_timer = SDL_GetTicks(); player_collider->active = false; slide_collider->active = true; start_slide = true; break;
			}
		}
		break;

		case ST_WALK_FORWARD:
		{
			switch (last_input)
			{
			case IN_RIGHT_UP: state = ST_IDLE_RIGHT; break;
			case IN_LEFT_AND_RIGHT: state = ST_IDLE_RIGHT; break;
			case IN_JUMP: state = ST_JUMP; /*jump_timer = SDL_GetTicks();*/  break;
			case IN_SLIDE_DOWN: state = ST_SLIDE_FORWARD; slide_timer = SDL_GetTicks(); player_collider->active = false; slide_collider->active = true; start_slide = true; break;
			}
		}
		break;

		case ST_WALK_BACKWARD:
		{
			switch (last_input)
			{
			case IN_LEFT_UP: state = ST_IDLE_LEFT; break;
			case IN_LEFT_AND_RIGHT: state = ST_IDLE_LEFT; break;
			case IN_JUMP: state = ST_JUMP; /*jump_timer = SDL_GetTicks();*/  break;
			case IN_SLIDE_DOWN: state = ST_SLIDE_BACKWARD; slide_timer = SDL_GetTicks(); player_collider->active = false; slide_collider->active = true; start_slide = true; break;
			}
		}
		break;

		case ST_JUMP:
		{
			switch (last_input)
			{
			case IN_RIGHT_DOWN: looking_right = true; move_in_air = true; break;
			case IN_LEFT_DOWN: looking_right = false; move_in_air = true; break;
			case IN_JUMP_FINISH: if (looking_right) { state = ST_IDLE_RIGHT; } else { state = ST_IDLE_LEFT; } move_in_air = false; break;
			case IN_RIGHT_UP: move_in_air = false; break;
			case IN_LEFT_UP: move_in_air = false; break;
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