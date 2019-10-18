#include "j1Player.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Map.h"
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
			animation->frames[i].duration = frame_node.attribute("duration").as_float();
			animation->frames[i].rect = sprite_tilesets.start->data->TilesetRect(tileset_id);
		}
		Animations.add(animation);
		LOG("Succesfully loaded %s animation", animation->name);
	}
}

void j1Player::Draw_player() {
	//Cal crear la struc del player amb la seva posicio i les animacions ia fetes i renderitzar-lo amb la position i l'animation
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
					ret = App->map->LoadTilesetImage(tileset, set,this);
				}

				sprite_tilesets.add(set);
			}
			player_node = player_node.child("tile");
			LoadAnimations(player_node);
		}
	}
	return ret;
}

bool j1Player::CleanUp() {
	Animations.clear();
	sprite_tilesets.clear();
	player_doc.reset();
	return true;
}