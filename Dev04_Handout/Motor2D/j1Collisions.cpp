#include "j1App.h"
#include "j1input.h"
#include "j1Render.h"
#include "j1Collisions.h"
#include "j1Module.h"
#include "p2Log.h"
#include "Entity.h"
#include "player.h"
#include "j1Scene.h"
#include "j1Map.h"

j1Collisions::j1Collisions()
{
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
		colliders[i] = nullptr;

	matrix[COLLIDER_WALL][COLLIDER_WALL] = false;
	matrix[COLLIDER_WALL][COLLIDER_PLAYER1] = true;
	matrix[COLLIDER_WALL][COLLIDER_DEATH] = false;
	matrix[COLLIDER_WALL][COLLIDER_BACKGROUND] = false;
	matrix[COLLIDER_WALL][COLLIDER_ENEMY] = true;

	matrix[COLLIDER_PLAYER1][COLLIDER_WALL] = true;
	matrix[COLLIDER_PLAYER1][COLLIDER_PLAYER1] = false;
	matrix[COLLIDER_PLAYER1][COLLIDER_DEATH] = true;
	matrix[COLLIDER_PLAYER1][COLLIDER_BACKGROUND] = true;
	matrix[COLLIDER_PLAYER1][END_COLLIDER] = true;
	matrix[COLLIDER_PLAYER1][COLLIDER_ENEMY] = true;

	matrix[COLLIDER_DEATH][COLLIDER_WALL] = false;
	matrix[COLLIDER_DEATH][COLLIDER_PLAYER1] = true;
	matrix[COLLIDER_DEATH][COLLIDER_DEATH] = false;
	matrix[COLLIDER_DEATH][COLLIDER_BACKGROUND] = false;
	matrix[COLLIDER_DEATH][COLLIDER_ENEMY] = true;

	matrix[COLLIDER_BACKGROUND][COLLIDER_WALL] = false;
	matrix[COLLIDER_BACKGROUND][COLLIDER_PLAYER1] = true;
	matrix[COLLIDER_BACKGROUND][COLLIDER_DEATH] = true;

	matrix[END_COLLIDER][COLLIDER_PLAYER1] = true;
	matrix[END_COLLIDER][COLLIDER_ENEMY] = false;

	matrix[COLLIDER_ENEMY][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_WALL] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_PLAYER1] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_DEATH] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_BACKGROUND] = true;
	matrix[COLLIDER_ENEMY][END_COLLIDER] = false;

}

// Destructor
j1Collisions::~j1Collisions()
{}

void j1Collisions::Init() {
	enabled = false;
	active = true;
}

bool j1Collisions::PreUpdate()
{
	// Remove all colliders scheduled for deletion
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr && colliders[i]->to_delete == true)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}
	if (App->entities->GetPlayer()->player_collider != nullptr) {
		update_active_colliders();
	}
	// Calculate collisions
	Collider* c1;
	Collider* c2;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		// skip empty colliders
		if (colliders[i] == nullptr)
			continue;

		c1 = colliders[i];

		// avoid checking collisions already checked
		for (uint k = i + 1; k < MAX_COLLIDERS; ++k)
		{
			// skip empty colliders
			if (colliders[k] == nullptr)
				continue;

			c2 = colliders[k];

			if (c1->CheckCollision(c2->rect) == true)
			{
				if (matrix[c1->type][c2->type] && c1->callback&&c1->active)
					c1->callback->OnCollision(c1, c2);

				if (matrix[c2->type][c1->type] && c2->callback&&c2->active)
					c2->callback->OnCollision(c2, c1);
			}
		}
	}

	return UPDATE_CONTINUE;
}

int j1Collisions::closest_xaxis_collider(santa_states state,bool orientation) {
	if (state == ST_SLIDE_BACKWARD || state == ST_SLIDE_FORWARD) {
		if (App->entities->GetPlayer()->speed.x > 0) {
			origin_distance_player.x = App->entities->GetPlayer()->slide_collider->rect.x + App->entities->GetPlayer()->slide_collider->rect.w;
		}
		else {
			origin_distance_player.x = App->entities->GetPlayer()->slide_collider->rect.x;
		}
	}
	else {
		if (App->entities->GetPlayer()->speed.x > 0) {
			origin_distance_player.x = App->entities->GetPlayer()->player_collider->rect.x + App->entities->GetPlayer()->player_collider->rect.w;
		}
		else {
			origin_distance_player.x = App->entities->GetPlayer()->player_collider->rect.x;
		}
	}

	int current;
	int closest;
	if (App->entities->GetPlayer()->looking_right) {
		closest = App->map->data.width*App->map->data.tile_width;
		for (int i = 0; colliders[i]!=nullptr; ++i) {
			if (colliders[i]->type == COLLIDER_WALL || colliders[i]->type == COLLIDER_BACKGROUND) {
				if (colliders[i]->active) {
					if (colliders[i]->rect.x > origin_distance_player.x&&on_the_way_x(i)) {
						current = (colliders[i]->rect.x - origin_distance_player.x)-1;
						if (current < closest) {
							closest = current;
						}
					}
				}
			}
		}
	}
	else {
		closest = App->map->data.width*App->map->data.tile_width;
		for (int i = 0; colliders[i] != nullptr; ++i) {
			if (colliders[i]->type == COLLIDER_WALL || colliders[i]->type == COLLIDER_BACKGROUND) {
				if (colliders[i]->active) {
					if ((colliders[i]->rect.x+ colliders[i]->rect.w) < origin_distance_player.x&&on_the_way_x(i)) {
						current = origin_distance_player.x - (colliders[i]->rect.x+ colliders[i]->rect.w)-1;
						if (current < closest) {
							closest = current;
						}
					}
				}
			}
		}
	}
	return closest;
}

int j1Collisions::closest_yaxis_collider(santa_states state) {
	if (state == ST_SLIDE_BACKWARD || state == ST_SLIDE_FORWARD) {
		if (App->entities->GetPlayer()->speed.y < 0)
			origin_distance_player.y = App->entities->GetPlayer()->slide_collider->rect.y;

		else
			origin_distance_player.y = App->entities->GetPlayer()->slide_collider->rect.y + App->entities->GetPlayer()->slide_collider->rect.h;
	}
	else {
		if(App->entities->GetPlayer()->speed.y < 0)
			origin_distance_player.y = App->entities->GetPlayer()->player_collider->rect.y;
		else
			origin_distance_player.y = App->entities->GetPlayer()->player_collider->rect.y + App->entities->GetPlayer()->player_collider->rect.h;
	}
	
	int closest;
	int current;
	if (App->entities->GetPlayer()->speed.y<0) {
		closest = App->map->data.height*App->map->data.tile_height;
		for (int i = 0; colliders[i] != nullptr; ++i) {
			if (colliders[i]->type == COLLIDER_WALL || colliders[i]->type == COLLIDER_BACKGROUND) {
				if (colliders[i]->active) {
					if (((colliders[i]->rect.y + colliders[i]->rect.h) < origin_distance_player.y) && on_the_way_y(i)) {
						current = (origin_distance_player.y) - (colliders[i]->rect.y + colliders[i]->rect.h);
						if (current < closest) {
							closest = current;
						}
					}
				}
			}
		}
	}
	else {
		closest = App->map->data.height*App->map->data.tile_height;
		for (int i = 0; colliders[i] != nullptr; ++i) {
			if (colliders[i]->type == COLLIDER_WALL || colliders[i]->type == COLLIDER_BACKGROUND) {
				if (colliders[i]->active) {
					if (((colliders[i]->rect.y) > origin_distance_player.y) && on_the_way_y(i)) {
						current =((colliders[i]->rect.y) - origin_distance_player.y)-0.0001f;
						if (current < closest) {
							closest = current;
						}
					}
				}
			}
		}
	}
	return closest;
}

bool j1Collisions::on_the_way_x(int index) {
	bool not_on_the_way;
	if (App->entities->GetPlayer()->player_collider->active) {
		not_on_the_way = ((App->entities->GetPlayer()->player_collider->rect.y + App->entities->GetPlayer()->player_collider->rect.h) < colliders[index]->rect.y)
		|| ((App->entities->GetPlayer()->player_collider->rect.y) > (colliders[index]->rect.y+ colliders[index]->rect.h));
	}
	else if (App->entities->GetPlayer()->slide_collider->active) {
		not_on_the_way = ((App->entities->GetPlayer()->slide_collider->rect.y + App->entities->GetPlayer()->slide_collider->rect.h) < colliders[index]->rect.y)
			|| ((App->entities->GetPlayer()->slide_collider->rect.y) > (colliders[index]->rect.y + colliders[index]->rect.h));

	}
	return !not_on_the_way;
}

bool j1Collisions::on_the_way_y(int index) {
	if (App->entities->GetPlayer()->player_collider->active)
		return !((App->entities->GetPlayer()->player_collider->rect.x + App->entities->GetPlayer()->player_collider->rect.w) < colliders[index]->rect.x || (colliders[index]->rect.x + colliders[index]->rect.w) < App->entities->GetPlayer()->player_collider->rect.x);
	else if (App->entities->GetPlayer()->slide_collider->active)
		return !((App->entities->GetPlayer()->slide_collider->rect.x + App->entities->GetPlayer()->slide_collider->rect.w) < colliders[index]->rect.x || (colliders[index]->rect.x + colliders[index]->rect.w) < App->entities->GetPlayer()->slide_collider->rect.x);
}

void j1Collisions::update_active_colliders() {
	for (int i = 0; i < MAX_COLLIDERS; ++i) {
		while (colliders[i] != nullptr) {
			if (colliders[i]->type == COLLIDER_BACKGROUND) {
				if ((App->entities->GetPlayer()->player_collider->rect.y + App->entities->GetPlayer()->player_collider->rect.h) < colliders[i]->rect.y) {
					colliders[i]->active = true;
				}
				else {
					colliders[i]->active = false;
				}
			}
			++i;
		}
	}
}

// Called before render is available
bool j1Collisions::Update(float dt)
{

	DebugDraw();

	return UPDATE_CONTINUE;
}

void j1Collisions::DebugDraw()
{
	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN||App->scene->colliders) {
		debug =! debug;
		if (App->scene->colliders) {
			App->scene->colliders = false;
		}
	}

	if (debug == false) {
		return;
	}
		

	Uint8 alpha = 140;
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
			continue;
		if (colliders[i]->active) {
			switch (colliders[i]->type)
			{
			case COLLIDER_NONE: // white
				App->render->DrawQuad(colliders[i]->rect, 255, 255, 255, alpha);
				break;
			case COLLIDER_WALL: // blue
				App->render->DrawQuad(colliders[i]->rect, 0, 0, 255, alpha);
				break;
			case COLLIDER_PLAYER1: // green
				App->render->DrawQuad(colliders[i]->rect, 0, 255, 0, alpha);
				break;
			case COLLIDER_DEATH: //red
				App->render->DrawQuad(colliders[i]->rect, 255, 0, 0, alpha);
				break;
			case COLLIDER_BACKGROUND://yellow
				App->render->DrawQuad(colliders[i]->rect, 255, 255, 0, alpha);
				break;
			case END_COLLIDER://light blue
				App->render->DrawQuad(colliders[i]->rect, 0, 255, 255, alpha);
				break;
			case START_COLLIDER://purple
				App->render->DrawQuad(colliders[i]->rect, 255, 0, 255, alpha);
				break;
			case COLLIDER_ENEMY://black
				App->render->DrawQuad(colliders[i]->rect, 0, 0, 0, alpha);
				break;
			}
		}
	}
}

// Called before quitting
bool j1Collisions::CleanUp()
{
	LOG("Freeing all colliders");

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	return true;
}

Collider* j1Collisions::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback)
{
	Collider* ret = nullptr;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
		{
			ret = colliders[i] = new Collider(rect, type, callback);
			break;
		}
	}

	return ret;
}

// -----------------------------------------------------

bool Collider::CheckCollision(const SDL_Rect& r) const
{
	return (rect.x < r.x + r.w &&
		rect.x + rect.w > r.x &&
		rect.y < r.y + r.h &&
		rect.y + rect.h > r.y);
}

