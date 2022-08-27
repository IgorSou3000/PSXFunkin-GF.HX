/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "gfhx.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//GFHX character structure
enum
{
	GFHX_ArcMain_Idle0,
	GFHX_ArcMain_Idle1,
	GFHX_ArcMain_Idle2,
	GFHX_ArcMain_Idle3,

	GFHX_ArcMain_Left0,
	GFHX_ArcMain_Left1,
	GFHX_ArcMain_Left2,

	GFHX_ArcMain_Down0,
	GFHX_ArcMain_Down1,
	GFHX_ArcMain_Down2,
	GFHX_ArcMain_Down3,
	GFHX_ArcMain_Down4,
	GFHX_ArcMain_Down5,

	GFHX_ArcMain_Up0,
	GFHX_ArcMain_Up1,
	GFHX_ArcMain_Up2,

	GFHX_ArcMain_Right0,
	GFHX_ArcMain_Right1,
	GFHX_ArcMain_Right2,

	GFHX_ArcMain_Scare0,
	GFHX_ArcMain_Scare1,

	GFHX_ArcMain_Left0B,
	GFHX_ArcMain_Left1B,
	GFHX_ArcMain_Left2B,
	GFHX_ArcMain_Left3B,

	GFHX_ArcMain_Down0B,
	GFHX_ArcMain_Down1B,
	GFHX_ArcMain_Down2B,
	GFHX_ArcMain_Down3B,

	GFHX_ArcMain_Up0B,
	GFHX_ArcMain_Up1B,
	GFHX_ArcMain_Up2B,
	GFHX_ArcMain_Up3B,

	GFHX_ArcMain_Right0B,
	GFHX_ArcMain_Right1B,
	GFHX_ArcMain_Right2B,
	GFHX_ArcMain_Right3B,
	
	GFHX_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[GFHX_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_GFHX;

//GFHX character definitions
static const CharFrame char_gfhx_frame[] = {
	{GFHX_ArcMain_Idle0, {  0,   0, 118, 139}, {105, 139}}, //0 idle 1
	{GFHX_ArcMain_Idle0, {127,   0, 124, 138}, {107, 137}}, //1 idle 2
	{GFHX_ArcMain_Idle1, {  0,   0, 104, 140}, {104, 140}}, //2 idle 3
	{GFHX_ArcMain_Idle1, {138,   0, 115, 141}, { 96, 140}}, //3 idle 4
	{GFHX_ArcMain_Idle2, {  0,   0, 114, 141}, { 94, 140}}, //4 idle 5
	{GFHX_ArcMain_Idle2, {139,   0, 114, 140}, { 85, 140}}, //5 idle 6
	{GFHX_ArcMain_Idle3, {  0,   0, 116, 141}, { 88, 140}}, //6 idle 7
	{GFHX_ArcMain_Idle3, {129,   0, 114, 142}, { 89, 141}}, //7 idle 8
	
	{GFHX_ArcMain_Left0, {  0,   0, 111, 142}, {103, 141}}, //8 left 1
	{GFHX_ArcMain_Left0, {123,   0, 115, 142}, {104, 141}}, //9 left 2
	{GFHX_ArcMain_Left1, {  0,   0, 114, 142}, {104, 141}}, //10 left 3
	{GFHX_ArcMain_Left1, {126,   0, 115, 141}, {102, 140}}, //11 left 4
	{GFHX_ArcMain_Left2, {  0,   0, 114, 141}, { 99, 141}}, //12 left 5
	{GFHX_ArcMain_Left2, {125,   0, 114, 141}, { 98, 140}}, //13 left 6
	
	{GFHX_ArcMain_Down0, {  0,   0, 200, 144}, {145, 138}}, //14 down 1
	{GFHX_ArcMain_Down1, {  0,   0, 200, 144}, {145, 138}}, //15 down 2
	{GFHX_ArcMain_Down2, {  0,   0, 200, 144}, {145, 138}}, //16 down 3
	{GFHX_ArcMain_Down3, {  0,   0, 200, 144}, {145, 138}}, //17 down 4
	{GFHX_ArcMain_Down4, {  0,   0, 200, 144}, {145, 139}}, //18 down 5
	{GFHX_ArcMain_Down5, {  0,   0, 200, 144}, {145, 139}}, //19 down 6
	
	{GFHX_ArcMain_Up0, {  0,   0, 116, 146}, { 92 + 5, 146}}, //20 up 1
	{GFHX_ArcMain_Up0, {129,   0, 112, 145}, { 90 + 5, 144}}, //21 up 2
	{GFHX_ArcMain_Up1, {  0,   2, 115, 145}, { 93 + 5, 144}}, //22 up 3
	{GFHX_ArcMain_Up1, {128,   2, 114, 142}, { 93 + 5, 142}}, //23 up 4
	{GFHX_ArcMain_Up2, {  1,   9, 113, 139}, { 95 + 3, 138}}, //24 up 5
	{GFHX_ArcMain_Up2, {127,   9, 113, 139}, { 95 + 3, 138}}, //25 up 6
	
	{GFHX_ArcMain_Right0, {  2,   2, 115, 140}, { 83, 139}}, //26 right 1
	{GFHX_ArcMain_Right0, {130,   2, 113, 140}, { 86, 140}}, //27 right 2
	{GFHX_ArcMain_Right1, {  1,   3, 114, 140}, { 87, 140}}, //28 right 3
	{GFHX_ArcMain_Right1, {127,   3, 114, 139}, { 86, 139}}, //29 right 4
	{GFHX_ArcMain_Right2, {  0,   5, 114, 139}, { 90, 138}}, //30 right 5
	{GFHX_ArcMain_Right2, {126,   3, 114, 139}, { 91, 139}}, //31 right 6

	{GFHX_ArcMain_Scare0, {  0,   0, 180, 156}, {121, 152}}, //32 scare 1
	{GFHX_ArcMain_Scare1, {  0,   0, 180, 156}, {121, 152}}, //33 scare 2

	{GFHX_ArcMain_Left0B, {  0,   0, 220, 150}, {160, 143}}, //34 left alt 1
	{GFHX_ArcMain_Left1B, {  0,   0, 220, 150}, {160, 143}}, //35 left alt 2
	{GFHX_ArcMain_Left2B, {  0,   0, 220, 150}, {160, 143}}, //36 left alt 3
	{GFHX_ArcMain_Left3B, {  0,   0, 220, 150}, {160, 143}}, //37 left alt 4

	{GFHX_ArcMain_Down0B, {  0,   0, 220, 148}, {152, 143}}, //38 down alt 1
	{GFHX_ArcMain_Down1B, {  0,   0, 220, 148}, {148, 145}}, //39 down alt 2
	{GFHX_ArcMain_Down2B, {  0,   0, 220, 148}, {148, 145}}, //40 down alt 3
	{GFHX_ArcMain_Down3B, {  0,   0, 220, 148}, {148, 145}}, //41 down alt 4

	{GFHX_ArcMain_Up0B, {  0,   0, 255, 170}, {159, 157}}, //42 up alt 1
	{GFHX_ArcMain_Up1B, {  0,   0, 255, 170}, {160, 157}}, //43 up alt 2
	{GFHX_ArcMain_Up2B, {  0,   0, 255, 170}, {160, 157}}, //44 up alt 3
	{GFHX_ArcMain_Up3B, {  0,   0, 255, 170}, {160, 157}}, //45 up alt 4

	{GFHX_ArcMain_Right0B, {  0,   0, 255, 170}, {162, 158}}, //46 right alt 1
	{GFHX_ArcMain_Right1B, {  0,   0, 255, 170}, {160, 155}}, //47 right alt 2
	{GFHX_ArcMain_Right2B, {  0,   0, 255, 170}, {160, 155}}, //48 right alt 3
	{GFHX_ArcMain_Right3B, {  0,   0, 255, 170}, {160, 155}}, //49 right alt 4
};

static const Animation char_gfhx_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4,  4,  4,  5,  6,  7,  4,  ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 8,  9, 10, 11, 12, 13, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){14, 15, 16, 17, 18, 19, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){20, 21, 22, 23, 24, 25, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){26, 27, 28, 29, 30, 31, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
	{1, (const u8[]){32, 33, 32, 33, 32, 33, 32, 33, 32, 33, 32, 33, 32, 33, 32, 33, 32, 33, 32, 33, 32, 33, 32, 33, ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_Special
};

static const Animation char_gfhxalt_anim[CharAnim_Max] = {
	{2, (const u8[]){ 4,  ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){34, 35, 36, 37, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){38, 39, 40, 41, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){42, 43, 44, 45, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){46, 47, 48, 49, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
	{1, (const u8[]){32, 33, 32, 33, 32, 33, 32, 33, 32, 33, 32, 33, 32, 33, 32, 33, 32, 33, 32, 33, 32, 33, 32, 33, ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_Special
};

//GFHX character functions
void Char_GFHX_SetFrame(void *user, u8 frame)
{
	Char_GFHX *this = (Char_GFHX*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_gfhx_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_GFHX_Tick(Character *character)
{
	Char_GFHX *this = (Char_GFHX*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);

	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		//Stage specific animations
		if (stage.note_scroll >= 0)
		{
			switch (stage.stage_id)
			{
				case StageId_1_1: //Scream and switch gf
					if ((stage.song_step == 767) || (stage.song_step == 1743) || (stage.song_step == 1840))
						Animatable_Init(&this->character.animatable, char_gfhxalt_anim); //gf alt

					if ((stage.song_step == 896) || (stage.song_step == 1808) || (stage.song_step == 1871))
						Animatable_Init(&this->character.animatable, char_gfhx_anim); //gf

					if ((stage.song_step == 896) || (stage.song_step == 1481) || (stage.song_step == 1873))
						character->set_anim(character, CharAnim_Special);
					break;
				default:
					break;
			}
		}
	}
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_GFHX_SetFrame);
	Character_Draw(character, &this->tex, &char_gfhx_frame[this->frame]);
}

void Char_GFHX_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_GFHX_Free(Character *character)
{
	Char_GFHX *this = (Char_GFHX*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_GFHX_New(fixed_t x, fixed_t y)
{
	//Allocate gfhx object
	Char_GFHX *this = Mem_Alloc(sizeof(Char_GFHX));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_GFHX_New] Failed to allocate gfhx object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_GFHX_Tick;
	this->character.set_anim = Char_GFHX_SetAnim;
	this->character.free = Char_GFHX_Free;
	
	Animatable_Init(&this->character.animatable, char_gfhx_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	//Character icon
	this->character.health_i = 1;

	//Character health bar color
	this->character.health_bar = 0xFFC80053;

	//Character scale
	this->character.scale = FIXED_DEC(130,100);
	
	this->character.focus_x = FIXED_DEC(-50,1);
	this->character.focus_y = FIXED_DEC(-135,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\GFHX.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //GFHX_ArcMain_Idle0
		"idle1.tim", //GFHX_ArcMain_Idle1
		"idle2.tim", //GFHX_ArcMain_Idle2
		"idle3.tim", //GFHX_ArcMain_Idle3

		"left0.tim",  //GFHX_ArcMain_Left0
		"left1.tim",  //GFHX_ArcMain_Left1
		"left2.tim",  //GFHX_ArcMain_Left2

		"down0.tim",  //GFHX_ArcMain_Down0
		"down1.tim",  //GFHX_ArcMain_Down1
		"down2.tim",  //GFHX_ArcMain_Down2
		"down3.tim",  //GFHX_ArcMain_Down3
		"down4.tim",  //GFHX_ArcMain_Down4
		"down5.tim",  //GFHX_ArcMain_Down5

		"up0.tim",    //GFHX_ArcMain_Up0
		"up1.tim",    //GFHX_ArcMain_Up1
		"up2.tim",    //GFHX_ArcMain_Up2

		"right0.tim", //GFHX_ArcMain_Right0
		"right1.tim", //GFHX_ArcMain_Right1
		"right2.tim", //GFHX_ArcMain_Right2

		"scare0.tim", //GFHX_ArcMain_Scare0
		"scare1.tim", //GFHX_ArcMain_Scare1

		"left0b.tim",  //GFHX_ArcMain_Left0B
		"left1b.tim",  //GFHX_ArcMain_Left1B
		"left2b.tim",  //GFHX_ArcMain_Left2B
		"left3b.tim",  //GFHX_ArcMain_Left3B

		"down0b.tim",  //GFHX_ArcMain_Down0B
		"down1b.tim",  //GFHX_ArcMain_Down1B
		"down2b.tim",  //GFHX_ArcMain_Down2B
		"down3b.tim",  //GFHX_ArcMain_Down3B

		"up0b.tim",    //GFHX_ArcMain_Up0B
		"up1b.tim",    //GFHX_ArcMain_Up1B
		"up2b.tim",    //GFHX_ArcMain_Up2B
		"up3b.tim",    //GFHX_ArcMain_Up3B

		"right0b.tim", //GFHX_ArcMain_Right0B
		"right1b.tim", //GFHX_ArcMain_Right1B
		"right2b.tim", //GFHX_ArcMain_Right2B
		"right3b.tim", //GFHX_ArcMain_Right3B
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
