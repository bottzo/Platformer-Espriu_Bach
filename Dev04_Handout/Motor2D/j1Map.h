#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"
struct Properties
{
	struct Property
	{
		p2SString name;
		int value;
	};

	~Properties()
	{
		p2List_item<Property*>* item;
		item = list.start;

		while (item != NULL)
		{
			RELEASE(item->data);
			item = item->next;
		}

		list.clear();
	}

	int Get(const char* name, int default_value = 0) const;

	p2List<Property*>	list;
};

// TODO 1: Create a struct for the map layer
// ----------------------------------------------------
struct map_layer {
	p2SString name;
	uint width;
	uint height;
	uint* tiled_gid;
	Properties	Properties;
	inline uint Get(int x, int y)const;
	float parSpeed;
};
	// TODO 6: Short function to get the value of x,y

// ----------------------------------------------------
struct TileSet
{
	// TODO 7: Create a method that receives a tile id and returns it's Rect

	p2SString			name;
	int					firstgid;
	int					margin;
	int					spacing;
	int					tile_width;
	int					tile_height;
	SDL_Texture*		texture;
	int					tex_width;
	int					tex_height;
	int					num_tiles_width;
	int					num_tiles_height;
	int					offset_x;
	int					offset_y;
	SDL_Rect TilesetRect(uint tiled_gid);
};

struct object {
	p2SString name;
	uint id;
	SDL_Rect rect;
};

struct objectgroup {
	p2SString name;
	uint num_objects;
	object* objects;
};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};
// ----------------------------------------------------
struct MapData
{
	int					width;
	int					height;
	int					tile_width;
	int					tile_height;
	float               gravity;
	SDL_Color			background_color;
	MapTypes			type;
	p2List<TileSet*>	tilesets;
	p2List<map_layer*>  layer;
	p2List<objectgroup*>objectgroup;
};

// ----------------------------------------------------
class j1Map : public j1Module
{
public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	// TODO 8: Create a method that translates x,y coordinates from map positions to world positions
	int translate_x(int x);
	int translate_y(int y);

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set,j1Module*module);
	bool LoadLayer(pugi::xml_node& tileset_node, map_layer* layer);
	bool add_map_colliders();
	TileSet* GetTilesetFromTileId(int id) const;
	bool parallax_movement();

public:

	MapData data;

private:

	pugi::xml_document	map_file;
	p2SString			folder;
	bool				map_loaded;
	float gravity;
	friend class j1Player;
	p2Point<float> position_parallax;
	p2Point<float> speed_parallax;
};

#endif // __j1MAP_H__