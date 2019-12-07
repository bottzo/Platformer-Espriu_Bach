#include "Enemy.h"
#include "player.h"
#include "j1Map.h"
#include "j1App.h"
#include "j1Collisions.h"
#include "p2Log.h"
#include "Entity.h"
#include "p2DynArray.h"
#include "j1Pathfinding.h"


enemy::enemy() {}

ground_enemy::ground_enemy() {
	Load_Entity(App->entities->ground_enemy_sprite.GetString());
	this->kind = Enemies::ground;
	this->type = Entity::Types::ground_enemy;
	on_the_ground = false;
}

flying_enemy::flying_enemy() {
	Load_Entity(App->entities->flying_enemy_sprite.GetString());
	this->kind = Enemies::air;
	this->type = Entity::Types::flying_enemy;
}

void enemy::Draw_Enemy(float dt) {
	switch (kind) {
	case Enemies::ground:
		if (App->entities->GetPlayer()->position.x > this->position.x)
			App->render->Blit(Animations.start->data->texture, position.x, position.y, &Animations.start->data->GetCurrentFrame(dt), SDL_FLIP_NONE, this);
		else {
			App->render->Blit(Animations.start->data->texture, position.x, position.y, &Animations.start->data->GetCurrentFrame(dt), SDL_FLIP_HORIZONTAL, this);
		}
		break;
	case Enemies::air:
		if (App->entities->GetPlayer()->position.x > this->position.x)
			App->render->Blit(Animations.start->data->texture, position.x, position.y, &Animations.start->data->GetCurrentFrame(dt), SDL_FLIP_HORIZONTAL, this);
		else {
			App->render->Blit(Animations.start->data->texture, position.x, position.y, &Animations.start->data->GetCurrentFrame(dt), SDL_FLIP_NONE, this);
		}
		break;
	}
}

void ground_enemy::move()  {
	if (count == 4) {
		App->pathfinding->CreatePath(App->map->WorldToMap((int)position.x, (int)position.y), App->map->WorldToMap((int)App->entities->GetPlayer()->position.x + (int)App->entities->GetPlayer()->player_collider->rect.w, (int)App->entities->GetPlayer()->position.y));
		const iPoint* destination_ptr = App->pathfinding->GetLastPath()->At(1);
		iPoint destination(destination_ptr->x, destination_ptr->y);
		destination = App->map->MapToWorld(destination.x, destination.y);
		position.x = destination.x;
		enemy_collider->SetPos(position.x+App->entities->ground_texture_offset, position.y);
		count = 0;
	}
	count++;
}

void flying_enemy::move() {
	if (count == 4) {
		App->pathfinding->CreatePath(App->map->WorldToMap((int)position.x, (int)position.y), App->map->WorldToMap((int)App->entities->GetPlayer()->position.x + (int)App->entities->GetPlayer()->player_collider->rect.w, (int)App->entities->GetPlayer()->position.y));
		const iPoint* destination_ptr = App->pathfinding->GetLastPath()->At(1);
		iPoint destination(destination_ptr->x, destination_ptr->y);
		destination = App->map->MapToWorld(destination.x, destination.y);
		position.x = destination.x; position.y = destination.y;
		enemy_collider->SetPos(position.x, position.y);
		count = 0;
	}
	count++;
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
