﻿#pragma once

#include "Falling_Letter.h"
#include "Indicator.h"
#include "Label.h"

//------------------------------------------------------------------------------------------------------------
enum class EScore_Event_Type : unsigned char
{
	Hit_Brick,
	Hit_Monster,
	Catch_Letter
};
//------------------------------------------------------------------------------------------------------------
class AsInformation_Panel : public AGame_Object
{
public:
	AsInformation_Panel();

	virtual void Begin_Movement();
	virtual void Finish_Movement();
	virtual void Advance(double max_speed);
	virtual double Get_Speed();

	virtual void Act();
	virtual void Clear(HDC hdc, RECT& paint_area);
	virtual void Draw(HDC hdc, RECT& paint_area);
	virtual bool Is_Finished();

	void Init();
	void Add_Life();
	bool Remove_A_Life();
	bool Edit_Player_Name(wchar_t symbol);

	static void Update_Score(EScore_Event_Type event_type, EBrick_Type brick_type = EBrick_Type::None);

	AIndicator Floor_Panel, Monsters_Panel;

private:
	void Choose_Font();
	void Show_Extra_Lives(HDC hdc);
	void Draw_Extra_Life(HDC hdc, int x, int y);

	bool Entering_User_Name;
	int Lives_Left;
	int Start_Timer_Tick;
	AColor Shaded_Blue, Dark_Red_Color;

	AFalling_Letter Letter_P, Letter_G, Letter_M;

	ALabel Player_Name_Label, Score_Label;

	static RECT Logo_Rect;
	static RECT Data_Rect; // score and lives 

	static int Score;

	static const int Logo_X_Pos = 212;
	static const int Logo_Y_Pos = 0;
	static const int Shade_X_Offset = 5;
	static const int Shade_Y_Offset = 6;
	
	static const int Score_X_Pos = 208;
	static const int Score_Y_Pos = 108;
	static const int Score_Width = 110;
	static const int Score_Height = 90;
	static const int Score_Val_Offset = 20;
	static const int Indicator_Y_Offset = 55;
	static const int Blink_Timeout = AsConfig::FPS / 2;
};
//------------------------------------------------------------------------------------------------------------