#pragma once

#include <Windows.h>

#define _USE_MATH_DEFINES
#include <math.h>

//------------------------------------------------------------------------------------------------------------
class AColor
{
public:
	AColor();
	AColor(unsigned char r, unsigned char g, unsigned char b);
	AColor(const AColor& color, int pen_size);
	AColor(unsigned char r, unsigned char g, unsigned char b, int pen_size);
	AColor(const AColor& pen_color, const AColor& brush_color, int pen_size);

	int Get_RGB() const;
	void Select(HDC hdc) const;
	void Select_Pen(HDC hdc) const;
	HBRUSH Get_Brush() const;


	unsigned char R, G, B;

private:
	HPEN Pen;
	HBRUSH Brush;
};
//------------------------------------------------------------------------------------------------------------
class AsConfig
{
public:
 	static int Rand(int range);
	static void Round_Rect(HDC hdc, RECT &rect, int corner_radius = 2);
	static void Throw();

	static bool Level_Has_Floor;
	static int Current_Timer_Tick;
	static const AColor BG_Color, Red_Color, Blue_Color, White_Color, Letter_Color;
	static const AColor Unbreakable_Blue_Highlight, Unbreakable_Red_Highlight, Teleport_Portal_Color;
	static const AColor Advertisement_Blue_Table, Advertisement_Red_Table;
	static HWND Hwnd;

	static const double D_Global_Scale;
	static const double Moving_step_size;
	static const double Start_Ball_Y_Pos;

	static const int Global_Scale = 3;
	static const int Border_X_Offset = 6;
	static const int Border_Y_Offset = 4;
	static const int Level_Width = 12;  // Level width in cells
	static const int Level_Height = 14;  // Level height in cells
	static const int Level_X_Offset = 8;
	static const int Level_Y_Offset = 6;
	static const int Cell_Width = 16;
	static const int Cell_Height = 8;
	static const int Brick_Width = 15;
	static const int Brick_Height = 7;
	static const int Max_X_Pos = Level_X_Offset + Cell_Width * Level_Width;
	static const int Max_Y_Pos = 199;
	static const int Platform_Y_Pos = 185;
	static const int FPS = 20;
	static const int Max_Active_Bricks_Count = 10;
	static const int Hits_Per_Letter = 1; // Probability of hitting a letter - 1 / Hits_Per_Letter
	static const int Max_Falling_Letters_Count = 10;
	static const int Max_Balls_Count = 10;
	static const int Max_Movers_Count = 10;
};
//------------------------------------------------------------------------------------------------------------
