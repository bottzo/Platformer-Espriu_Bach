#include "j1Player.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Map.h"
#include "j1Collisions.h"
#include "j1Window.h"
j1Player::j1Player():j1Module(),player_loaded(false) {
	name.create("player");
}

j1Player::~j1Player() {

}

bool j1Player::Awake(pugi::xml_node&config) {

		LOG("Loading Player Parser");
		bool ret = true;

		folder.create(config.child("folder").child_value());
		player_texture_offset.x =config.child("texture_offset").attribute("x").as_int();
		player_texture_offset.y =config.child("texture_offset").attribute("y").as_int();
		slide_texture_offset.x = config.child("slide_offset").attribute("x").as_int();
		slide_texture_offset.y = config.child("slide_offset").attribute("x").as_int();
		return ret;
}

SDL_Rect&PlayerAnimation::GetCurrentFrame() {
	current_frame += frames->duration;
	if (current_frame >= total_frames)
		current_frame = 0;
	return frames[(int)current_frame].rect;
}

SDL_Rect&PlayerAnimation::DoOneLoop() {
	current_frame += frames->duration;
	if (current_frame >= total_frames)
		current_frame = total_frames-1;
	return frames[(int)current_frame].rect;
}

void j1Player::LoadAnimations(pugi::xml_node&node) {
	LOG("Loading player animations");
	for (node; node; node = node.next_sibling("tile")) {
		PlayerAnimation*animation = new PlayerAnimation();
		animation->name.create(node.child("properties").child("property").attribute("value").as_string());
		animation->total_frames = node.child("properties").child("property").next_sibling("property").attribute("value").as_uint();
		LOG("Loading %s animation with %d frames", animation->name.GetString(),animation->total_frames);
		animation->texture = sprite_tilesets.start->data->texture;
		pugi::xml_node frame_node = node.child("animation").child("frame");
		animation->frames = new Frame[animation->total_frames];
		for (int i = 0;i<animation->total_frames; frame_node = frame_node.next_sibling("frame"),++i) {
			uint tileset_id = frame_node.attribute("tileid").as_uint();
			animation->frames[i].duration = frame_node.attribute("duration").as_float()/100;//Dividir entre 1000 pk sigui canvi d'e frame de l0'animacio en cada segon (esta en milisegons en el tmx)
			animation->frames[i].rect = sprite_tilesets.start->data->TilesetRect(tileset_id+1);//pk el +1??? pk la funcio tilesetrect conta el primer tile com si fos un 1 i no el zero
		}
		Animations.add(animation);
		LOG("Succesfully loaded %s animation", animation->name.GetString());
	}
}

void j1Player::Updateposition(santa_states state) {
	/*if(distance.y<speed.y)*/
	speed.y += App->map->data.gravity;
	if (speed.y > 30) {
		speed.y = 30;
	}
	switch (state) {
	case ST_IDLE_RIGHT:

		if (speed.x > 0) {
			speed.x-=2;
		}
		else {
			speed.x = 0;
		}
		looking_right = true;
		break;
	case ST_IDLE_LEFT:
		if (speed.x < 0) {
			speed.x+=2;
		}
		else {
			speed.x = 0;
		}
		looking_right = false;
		break;
	case ST_WALK_FORWARD:
		if (speed.x < 20) {
			speed.x+=4;
		}
		else {
			speed.x = 20;
		}
		looking_right = true;
		break;
	case ST_WALK_BACKWARD:
		if (speed.x > -20) {
			speed.x-=4;
		}
		else {
			speed.x = -20;
		}
		looking_right = false;
		break;
	case ST_SLIDE_FORWARD:
		speed.x = 30;
		looking_right = true;
		if (start_slide) {
			position.x -= slide_collider->rect.w - (player_collider->rect.w + (player_texture_offset.x - slide_texture_offset.x));
			slide_collider->SetPos(position.x + slide_texture_offset.x, position.y + slide_texture_offset.y);
			start_slide = false;
		}
		break;
	case ST_SLIDE_BACKWARD:
		speed.x = -30;
		looking_right = false;
		if (start_slide) {
			position.x += player_texture_offset.x - slide_texture_offset.x;
			slide_collider->SetPos(position.x + slide_texture_offset.x, position.y + slide_texture_offset.y);
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

	distance.x=App->collisions->closest_xaxis_collider(state,looking_right);
	if (looking_right) {
		if (speed.x >= distance.x) {
			position.x += distance.x;
			speed.x = 0;
		}
		else {
			position.x += speed.x;
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

	distance.y = App->collisions->closest_yaxis_collider(state);
	if (speed.y<0) {
		if (speed.y <= -distance.y) {
			position.y -= distance.y;
			speed.y = 0;
			key_inputs.Push(IN_JUMP_FINISH);
			move_in_air = false;
		}
		else {
			position.y += speed.y;
		}
	}
	else if (speed.y > 0) {
		if (speed.y >= distance.y) {
			position.y += distance.y;
			key_inputs.Push(IN_JUMP_FINISH);
			move_in_air = false;
			Animations.start->next->next->next->data->current_frame = 0;
			speed.y = 0;
			start_jump = true;
		}
		else {
			position.y += speed.y;
		}
	}
	player_collider->SetPos(position.x+ player_texture_offset.x, position.y +player_texture_offset.y);
	slide_collider->SetPos(position.x + slide_texture_offset.x, position.y+ slide_texture_offset.y);

}

void j1Player::Draw_player(santa_states state) {
	switch (state) {
	case ST_IDLE_RIGHT:
		App->render->Blit(Animations.start->data->texture, position.x, position.y, &Animations.start->data->GetCurrentFrame());
		break;
	case ST_IDLE_LEFT:
		App->render->Blit(Animations.start->data->texture, position.x, position.y, &Animations.start->data->GetCurrentFrame(),SDL_FLIP_HORIZONTAL);
		break;
	case ST_WALK_FORWARD:
		App->render->Blit(Animations.start->data->texture, position.x, position.y, &Animations.start->next->data->GetCurrentFrame());
		break;
	case ST_WALK_BACKWARD:
		App->render->Blit(Animations.start->data->texture, position.x, position.y, &Animations.start->next->data->GetCurrentFrame(),SDL_FLIP_HORIZONTAL);
		break;
	case ST_SLIDE_FORWARD:
		App->render->Blit(Animations.start->data->texture, position.x, position.y, &Animations.start->next->next->next->next->data->GetCurrentFrame());
		break;
	case ST_SLIDE_BACKWARD:
		App->render->Blit(Animations.start->data->texture, position.x, position.y, &Animations.start->next->next->next->next->data->GetCurrentFrame(), SDL_FLIP_HORIZONTAL);
		break;
	case ST_JUMP:
		if(looking_right)
			App->render->Blit(Animations.start->data->texture, position.x, position.y, &Animations.start->next->next->next->data->DoOneLoop());
		else {
			App->render->Blit(Animations.start->data->texture, position.x, position.y, &Animations.start->next->next->next->data->DoOneLoop(), SDL_FLIP_HORIZONTAL);
		}
		break;
	}

	
}void j1Player::OnCollision(Collider*c1, Collider*c2) {
	if (c2->type == COLLIDER_DEATH) {
		position.x = start_collider->rect.x;
		position.y = start_collider->rect.y;
	}
	if (c2->type == END_COLLIDER) {
		App->map->ChangeMaps("Santa's mountains.tmx");
	}
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
	p2SString end; start.create("End");
	p2SString player; player.create("player_collider");
	p2SString slide_collider_name; slide_collider_name.create("slide_collider");
	p2List_item<objectgroup*>*it = App->map->data.objectgroup.start;
	while (it != NULL) {
		if (it->data->name == group_name) {
			for (int i = 0; i < it->data->num_objects; ++i) {
				if (it->data->objects[i].name == player) {
					player_collider=App->collisions->AddCollider(it->data->objects[i].rect, COLLIDER_PLAYER1, App->player);
				}
				else if (it->data->objects[i].name == slide_collider_name) {
					slide_collider=App->collisions->AddCollider(it->data->objects[i].rect, COLLIDER_PLAYER1, App->player);
					slide_collider->active = false;
				}
			}
			break;
		}
		it = it->next;
	}
	position.x = start_collider->rect.x;
	position.y = start_collider->rect.y;
}

bool j1Player::positioncamera()
{
	App->render->camera.x = -position.x +((App->win->width / 2)-(sprite_tilesets.start->data->tile_width/2));
	if (App->render->camera.x > 0) {
		App->render->camera.x = 0;
	}
	App->render->camera.y = -(position.y - App->win->height / 2);
	if (App->render->camera.y <= App->render->initial_camera_y)
		App->render->camera.y=App->render->initial_camera_y;

	//if (App->win->width / 2 < 0)App->render->camera.x = 0;
	//if (App->render->camera.y > App->render->initial_camera_y)App->render->camera.y = App->render->initial_camera_y;
	return true;
}
bool j1Player::PostUpdate() {
	positioncamera();
return true;
}



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