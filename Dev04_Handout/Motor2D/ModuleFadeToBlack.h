#ifndef __j1ModuleFADETOBLACK_H__
#define __j1ModuleFADETOBLACK_H__

#include "j1Module.h"
#include "SDL\include\SDL_rect.h"

class ModuleFadeToBlack : public j1Module
{
public:
	ModuleFadeToBlack();
	~ModuleFadeToBlack();

	bool Start();
	bool Update(float dt);
	bool CleanUp();
	bool FadeToBlack(j1Module* j1Module_off, j1Module* j1Module_on, float time = 2.0f);
	bool IsFading() const;

private:
	j1Module * Moduleoff;
	j1Module * Moduleon;
	enum fade_step
	{
		none,
		fade_to_black,
		fade_from_black
	} current_step = fade_step::none;

	Uint32 start_time = 0;
	Uint32 total_time = 0;
	SDL_Rect screen;
};

#endif //__j1ModuleFADETOBLACK_H__