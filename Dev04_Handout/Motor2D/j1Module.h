// ----------------------------------------------------
// j1Module.h
// Interface for all engine modules
// ----------------------------------------------------

#ifndef __j1MODULE_H__
#define __j1MODULE_H__

#include "p2SString.h"
#include "PugiXml\src\pugixml.hpp"
#include "SDL\include\SDL_rect.h"


enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

struct Collider;
class j1App;
class UiElement;

class j1Module
{
public:

	j1Module() : active(false)
	{}

	virtual void Init()
	{
		active = true;
	}

	// Called before render is available
	virtual bool Awake(pugi::xml_node&)
	{
		return true;
	}

	// Called before the first frame
	virtual bool Start()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PreUpdate()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool Update(float dt)
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PostUpdate()
	{
		return true;
	}

	// Called before quitting
	virtual bool CleanUp()
	{
		return true;
	}

	virtual bool Load(pugi::xml_node&)
	{
		return true;
	}

	virtual bool Save(pugi::xml_node&) const
	{
		return true;
	}

	virtual void ui_callback(UiElement*element) {

	}
	void Enable()
	{
		if (enabled == false)
		{
			enabled = true;
			Start();
		}
	}

	void Disable()
	{
		if (enabled = true) {
			enabled = false;
			CleanUp();
		}
	}

	const bool IsEneabled() {
		return enabled;
	}


public:

	p2SString	name;
	bool		active;
	virtual void OnCollision(Collider*, Collider*) {};
protected:
	bool enabled = true;

};

#endif // __j1MODULE_H__