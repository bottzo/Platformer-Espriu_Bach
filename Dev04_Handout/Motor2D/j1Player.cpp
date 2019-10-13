#include "j1Player.h"
#include "p2Log.h"
j1Player::j1Player():j1Module(),player_loaded(false) {

}

j1Player::~j1Player() {

}

bool j1Player::Awake(pugi::xml_node&config) {

		LOG("Loading Player Parser");
		bool ret = true;

		folder.create(config.child("folder").child_value());

		return ret;
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
		pugi::xml_node player_node = player_doc.child("tileset");

		if (player_node == NULL)
		{
			LOG("Error parsing player xml file: Cannot find 'tileset' tag.");
			ret = false;
		}
		else
		{
			//Cal crear ara una struct animation i guardarli les dades del tilset traientles del xml file el qual ia li e posat el xml_node player_node.
			//Mirar la funcio loadtilesetdetails del modulemap i load tileset image
		}
	}
	return ret;
}

bool j1Player::CleanUp() {
	player_doc.reset();
	return true;
}