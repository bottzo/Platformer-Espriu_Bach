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
	Load_Entity(App->entities->player_sprite.GetString());
}

player::~player() 
{
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