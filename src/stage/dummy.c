/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "dummy.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../timer.h"

//Dummy background structure
typedef struct
{
	//Stage background base structure
	StageBack back;

	//Textures
	Gfx_Tex tex_error; //Error

	//error stuff
	fixed_t scale;

	fixed_t fade;
	fixed_t fadespd;
} Back_Dummy;

//Dummy background functions
void Back_Dummy_Free(StageBack *back)
{
	Back_Dummy *this = (Back_Dummy*)back;
	
	//Free structure
	Mem_Free(this);
}

void Back_Dummy_FG(StageBack *back)
{
	Back_Dummy *this = (Back_Dummy*)back;
	
	RECT error_src = {2, 3, 252, 65};
	RECT_FIXED error_dst = {
		FIXED_DEC(-130,1),
		FIXED_DEC(-30,1),
		error_src.w << FIXED_SHIFT,
		error_src.h << FIXED_SHIFT
	};

	if (stage.song_step >= 113)
	{
		//scale error image
		if (this->scale >= FIXED_DEC(0,1))
			this->scale -= FIXED_DEC(13,1000);
		
		//removing black fade
		if (this->fade > 0)
			this->fade -= FIXED_MUL(this->fadespd, timer_dt);
	}
	
	//draw error
	if (this->scale > FIXED_DEC(0,1) && stage.song_step > 0)
		Stage_DrawTex(&this->tex_error, &error_src, &error_dst, this->scale);

	//Draw black fade
	if (this->fade > 0)
	{
		static const RECT flash = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
		u8 flash_col = this->fade >> FIXED_SHIFT;
		Gfx_BlendRect(&flash, flash_col, flash_col, flash_col, 2);
	}
}

StageBack *Back_Dummy_New(void)
{
	//Allocate background structure
	Back_Dummy *this = (Back_Dummy*)Mem_Alloc(sizeof(Back_Dummy));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_Dummy_FG;
	this->back.draw_md = NULL;
	this->back.draw_bg = NULL;
	this->back.free = Back_Dummy_Free;

	//Load background textures
	IO_Data arc_back = IO_Read("\\WEEK1\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_error, Archive_Find(arc_back, "error.tim"), 0);
	Mem_Free(arc_back);
	
	//Use black background
	Gfx_SetClear(0, 0, 0);

	this->scale = FIXED_DEC(100,100);

	this->fade = FIXED_DEC(255,1);
	this->fadespd = FIXED_DEC(120,1);
	
	return (StageBack*)this;
}
