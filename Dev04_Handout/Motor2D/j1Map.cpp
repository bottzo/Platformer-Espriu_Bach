#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Map.h"
#include "j1Player.h"
#include "j1Collisions.h"
//#include <math.h>

j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name.create("map");
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.create(config.child("folder").child_value());

	return ret;
}

void j1Map::Draw()
{
	if (map_loaded == false)
		return;

	map_layer* layer = data.layer.start->data;
	TileSet* tileset = data.tilesets.start->data;
	p2List_item<map_layer*>* it = data.layer.start;

	while (it != nullptr) {
		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				int tile_id = it->data->Get(x, y);
				if (tile_id > 0)
				{
					TileSet* tileset = GetTilesetFromTileId(tile_id);
					if (tileset != nullptr)
					{
						SDL_Rect rect = tileset->TilesetRect(tile_id);
						iPoint pos = MapToWorld(x, y);
						App->render->Blit(tileset->texture, pos.x, pos.y, &rect, SDL_FLIP_NONE, tileset, it->data->parallaxspeed);
					}
				}
			}
		}
		it = it->next;
	}
}

TileSet* j1Map::GetTilesetFromTileId(int id) const
{
	p2List_item<TileSet*>* item;
	item = data.tilesets.start;

	while (item != NULL) {
		if (item->next == NULL) {
			break;
		}
		if (item->next->data->firstgid > id) {
			break;
		}
		item = item->next;
	}
	return item->data;
}

iPoint j1Map::MapToWorld(int x, int y) const {
	iPoint ret(0, 0);
	switch (data.type) {
	case MAPTYPE_ORTHOGONAL:
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
		break;
	case MAPTYPE_ISOMETRIC:
		ret.x = (x - y) * (data.tile_width *0.5f);
		ret.y = (x + y) * (data.tile_height *0.5f);
		break;
	default:
		LOG("Unknown map");
		ret.x = x;
		ret.y = y;
		break;
	}
	return ret;
}

iPoint j1Map::WorldToMap(int x,int y) const {
	iPoint ret(0, 0);
	float half_width = data.tile_width * 0.5f;
	float half_height = data.tile_height * 0.5f;
	switch (data.type) {
	case MAPTYPE_ORTHOGONAL:
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
		break;
	case MAPTYPE_ISOMETRIC:
		ret.x = int((x / half_width + y / half_height) / 2);
		ret.y = int((y / half_height - (x / half_width)) / 2);
		break;
	default:
		LOG("Unknown map");
		ret.x = x;
		ret.y = y;
		break;
	}
	return ret;
}

inline uint map_layer::Get(int x, int y) const{
		return tiled_gid[y*width +x];
}

SDL_Rect TileSet::TilesetRect(uint tiled_gid) {
	int relative_id = tiled_gid - firstgid;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));
	return rect;
}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	p2List_item<TileSet*>* item;
	item = data.tilesets.start;

	while(item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	data.tilesets.clear();

	// TODO 2: clean up all layer data
	// Remove all layers
	p2List_item<map_layer*>* it;
	it = data.layer.start;

	while (it != NULL)
	{
		RELEASE(it->data);
		it = it->next;
	}
	data.layer.clear();

	p2List_item<objectgroup*>* iteration;
	iteration = data.objectgroup.start;

	while (iteration != NULL)
	{
		RELEASE(iteration->data);
		iteration = iteration->next;
	}
	data.objectgroup.clear();

	// Clean up the pugui tree
	map_file.reset();

	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	bool ret = true;
	p2SString tmp("%s%s", folder.GetString(), file_name);

	pugi::xml_parse_result result = map_file.load_file(tmp.GetString());

	if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if(ret == true)
	{
		ret = LoadMap();
	}
	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for(tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if(ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if(ret == true)
		{
			ret = LoadTilesetImage(tileset, set,this);
		}

		data.tilesets.add(set);
	}

	// TODO 4: Iterate all layers and load each of them
	// Load layer info ----------------------------------------------
	for (pugi::xml_node layer=map_file.child("map").child("layer"); layer; layer=layer.next_sibling("layer")) {
		map_layer*lay = new map_layer();
		LoadLayer(layer, lay);
		data.layer.add(lay);
	}

	//Load objects info
	for (pugi::xml_node objects_node = map_file.child("map").child("objectgroup"); objects_node; objects_node = objects_node.next_sibling("objectgroup")) {
		objectgroup* group = new objectgroup();
		group->name.create(objects_node.attribute("name").as_string());
		int total_objects = 0;
		for (pugi::xml_node count_node = objects_node.child("object"); count_node; count_node = count_node.next_sibling("object")) {
			++total_objects;
		}
		group->num_objects = total_objects;
		object*objectgroup = new object[group->num_objects];
		int i = 0;
		for (pugi::xml_node node = objects_node.child("object"); node; node = node.next_sibling("object"),++i) {
			objectgroup[i].name.create(node.attribute("name").as_string());
			objectgroup[i].id = node.attribute("id").as_int();
			SDL_Rect rectangle;
			rectangle.x = node.attribute("x").as_int();
			rectangle.y = node.attribute("y").as_int();
			rectangle.w = node.attribute("width").as_int();
			rectangle.h = node.attribute("height").as_int();
			objectgroup[i].rect = rectangle;
		}
		group->objects = objectgroup;
		data.objectgroup.add(group);
		LOG("Succesfully loaded objectsgroup: %s", group->name.GetString());
	}
	/*
	for (int i = 0; i < data.objectgroup.start->data->num_objects; ++i) {
		LOG("%d: %s", i, data.objectgroup.start->data->objects[i].name.GetString());
		LOG("%d: %d", i, data.objectgroup.start->data->objects[i].id);
	}*/


	if(ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		p2List_item<TileSet*>* item = data.tilesets.start;
		while(item != NULL)
		{
			TileSet* s = item->data;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.GetString(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item = item->next;
		}

		p2List_item<map_layer*>* item_layer = data.layer.start;
		while(item_layer != NULL)
		{
			map_layer* l = item_layer->data;
			LOG("Layer ----");
			LOG("name: %s", l->name.GetString());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			item_layer = item_layer->next;
		}
	}
	add_map_colliders();
	map_loaded = ret;

	return ret;
}

bool j1Map::add_map_colliders() {
	bool ret = true;
	p2SString wall;
	wall.create("COLLAIDER_WALL");
	p2SString death;
	death.create("COLLIDER_DEATH");
	p2SString background;
	background.create("COLLIDER_BACKGROUND");
	p2List_item<objectgroup*>*it = data.objectgroup.start;
	while (it != NULL) {
		if (it->data->name==wall) {
			for (int i = 0; i < it->data->num_objects; ++i) {
				App->collisions->AddCollider(it->data->objects[i].rect, COLLIDER_WALL, App->map);
			}
		}
		if (it->data->name == death) {
			for (int i = 0; i < it->data->num_objects; ++i) {
				App->collisions->AddCollider(it->data->objects[i].rect, COLLIDER_DEATH, App->map);
			}
		}
		if (it->data->name == background) {
			for (int i = 0; i < it->data->num_objects; ++i) {
				App->collisions->AddCollider(it->data->objects[i].rect, COLLIDER_BACKGROUND, App->map);
			}
		}
		it = it->next;
	}
	LOG("Map colliders added");
	return ret;
}
// Load map general properties
bool j1Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if(map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.gravity = map.child("properties").child("property").attribute("value").as_float();
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		p2SString bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if(bg_color.Length() > 0)
		{
			p2SString red, green, blue;
			bg_color.SubString(1, 2, red);
			bg_color.SubString(3, 4, green);
			bg_color.SubString(5, 6, blue);

			int v = 0;

			sscanf_s(red.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.b = v;
		}

		p2SString orientation(map.attribute("orientation").as_string());

		if(orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if(orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if(orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}
	}

	return ret;
}

bool j1Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name.create(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if(offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set,j1Module*module)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if(image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		if (module==this)
			set->texture = App->tex->Load(PATH(folder.GetString(), image.attribute("source").as_string()));
		else
			set->texture = App->tex->Load(PATH(App->player->folder.GetString(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if(set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if(set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
	}

	return ret;
}

bool j1Map::LoadLayer(pugi::xml_node& layer_node, map_layer* layer) {
	bool ret = true;
	layer->name.create(layer_node.attribute("name").as_string());
	layer->width = layer_node.attribute("width").as_uint();
	layer->height = layer_node.attribute("height").as_uint();
	layer->tiled_gid = new uint[layer->width * layer->height]();
	layer->parallaxspeed = layer_node.child("properties").child("property").attribute("value").as_float();
	int i = 0;
	pugi::xml_node lay = layer_node.child("data").child("tile");
	for (lay; lay; lay = lay.next_sibling("tile"),++i) {
		layer->tiled_gid[i] = lay.attribute("gid").as_int();
	}
	return ret;
}

//bool j1Map::ChangeMaps(p2SString* new_map)
//{
//	CleanUp();
//	App->scene;
//	LoadMap();
//	
//
//	return true;
//}