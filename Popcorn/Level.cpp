﻿#include "Level.h"

// APoint
//------------------------------------------------------------------------------------------------------------
APoint::APoint()
	: X(0), Y(0)
{
}
//------------------------------------------------------------------------------------------------------------
APoint::APoint(int x, int y)
	: X(x), Y(y)
{
}
//------------------------------------------------------------------------------------------------------------




// AMop_Cylinders
const int AMop_Cylinders::Max_Cylinder_Height[4] = {86, 43, 32, 11};
//------------------------------------------------------------------------------------------------------------
AMop_Cylinders::AMop_Cylinders(int x_pos, int y_pos, int width, int height, int max_height)
	:X_Pos(x_pos), Y_Pos(y_pos), Width(width), Height(height), Max_Height(max_height)
{
}
//------------------------------------------------------------------------------------------------------------
void AMop_Cylinders::Act()
{
	//!!! TODO
}
//------------------------------------------------------------------------------------------------------------
void AMop_Cylinders::Clear(HDC hdc, RECT& paint_area)
{
	//!!! TODO
}
//------------------------------------------------------------------------------------------------------------
void AMop_Cylinders::Draw(HDC hdc, RECT& paint_area)
{
	const int scale = AsConfig::Global_Scale;
	RECT rect;

	// 1. cylinder mount
	rect.left = X_Pos * scale;
	rect.top = Y_Pos * scale;
	rect.right = rect.left + Width * scale;
	rect.bottom = rect.top + 4 * scale;

	AsConfig::Red_Color.Select(hdc);
	AsTools::Round_Rect(hdc, rect);
	AsTools::Rect(hdc, X_Pos + 2, Y_Pos + 1, 1, 2, AsConfig::BG_Color);
	AsTools::Rect(hdc, X_Pos + 4, Y_Pos + 1, 1, 2, AsConfig::BG_Color);
	AsTools::Rect(hdc, X_Pos + Width - 3, Y_Pos + 1, 1, 2, AsConfig::BG_Color);
	AsTools::Rect(hdc, X_Pos + Width - 5, Y_Pos + 1, 1, 2, AsConfig::BG_Color);

	// 2. cylinder body
	AsTools::Rect(hdc, X_Pos + 2, Y_Pos + 4, 2, Height, AsConfig::White_Color);
	AsTools::Rect(hdc, X_Pos + 4, Y_Pos + 4, 1, Height, AsConfig::Blue_Color);
	AsTools::Rect(hdc, X_Pos + 5, Y_Pos + 4, 1, Height, AsConfig::White_Color);
	AsTools::Rect(hdc, X_Pos + 6, Y_Pos + 4, Width - 8, Height, AsConfig::Blue_Color);
}
//------------------------------------------------------------------------------------------------------------
bool AMop_Cylinders::Is_Finished()
{
	return false;
	//!!! TODO
}
//------------------------------------------------------------------------------------------------------------
void AMop_Cylinders::Set_Height_For(double ratio)
{
	Height = (int)((double)Max_Height * ratio);

	if (Height < 1)
		Height = 1;
	else
		if (Height > Max_Height)
			Height = Max_Height;
}
//------------------------------------------------------------------------------------------------------------
int AMop_Cylinders::Get_Height()
{
	return Height;
}
//------------------------------------------------------------------------------------------------------------
void AMop_Cylinders::Set_Y_Pos(int y_pos)
{
	const int scale = AsConfig::Global_Scale;

	Y_Pos = y_pos;

	Cylinder_Rect.left = X_Pos * scale;
	Cylinder_Rect.top = Y_Pos * scale;
	Cylinder_Rect.right = Cylinder_Rect.left + Width * scale;
	Cylinder_Rect.bottom = Cylinder_Rect.top + Height * scale;

	AsTools::Invalidate_Rect(Cylinder_Rect);
}
//------------------------------------------------------------------------------------------------------------




// AMop_Indicators
AColor_Fade AMop_Indicators::Fading_Blue_Colors(AsConfig::Blue_Color, AsConfig::Red_Color, Max_Fade_Step);
//------------------------------------------------------------------------------------------------------------
AMop_Indicators::AMop_Indicators(int x_pos, int y_pos, int time_offset)
	: X_Pos(x_pos), Y_Pos(y_pos), Time_Offset(time_offset), Current_Color(&AsConfig::Red_Color)
{
	Set_Y_Pos(Y_Pos);
}
//------------------------------------------------------------------------------------------------------------
void AMop_Indicators::Act()
{
	int total_timeout = Normal_Timeout + Max_Fade_Step;
	int current_tick = (AsConfig::Current_Timer_Tick + Time_Offset) % total_timeout;
	int current_offset;

	if (current_tick < Normal_Timeout)
		Current_Color = &AsConfig::Red_Color;
	else
	{
		current_offset = current_tick - Normal_Timeout;

		if (current_offset < 0 || current_offset >= Max_Fade_Step)
			AsConfig::Throw();

		Current_Color = Fading_Blue_Colors.Get_Color(current_offset);
	}

	AsTools::Invalidate_Rect(Indicator_Rect);
}
//------------------------------------------------------------------------------------------------------------
void AMop_Indicators::Clear(HDC hdc, RECT& paint_area)
{
	//!!! TODO
}
//------------------------------------------------------------------------------------------------------------
void AMop_Indicators::Draw(HDC hdc, RECT& paint_area)
{
	const int scale = AsConfig::Global_Scale;
	RECT intersection_rect;

	if (!IntersectRect(&intersection_rect, &paint_area, &Indicator_Rect))
		return;


	AsTools::Rect(hdc, Indicator_Rect, *Current_Color);

	// Indicator frame
	AsConfig::Highlight_Color.Select_Pen(hdc);
	MoveToEx(hdc, (X_Pos) * scale, (Y_Pos + Height) * scale, 0);
	LineTo(hdc, X_Pos * scale, Y_Pos * scale);
	LineTo(hdc, (X_Pos + Width) * scale, (Y_Pos) * scale);

	AsConfig::Shadow_Color.Select_Pen(hdc);
	MoveToEx(hdc, (X_Pos + Width) * scale, Y_Pos * scale, 0);
	LineTo(hdc, (X_Pos + Width) * scale, (Y_Pos + Height) * scale);
	LineTo(hdc, X_Pos * scale, (Y_Pos + Height) * scale);
}
//------------------------------------------------------------------------------------------------------------
bool AMop_Indicators::Is_Finished()
{
	return false;
	//!!! TODO
}
//------------------------------------------------------------------------------------------------------------
void AMop_Indicators::Set_Y_Pos(int y_pos)
{
	const int scale = AsConfig::Global_Scale;

	Y_Pos = y_pos;

	Indicator_Rect.left = X_Pos * scale;
	Indicator_Rect.top = Y_Pos * scale;
	Indicator_Rect.right = Indicator_Rect.left + Width * scale;
	Indicator_Rect.bottom = Indicator_Rect.top + Height * scale;
}
//------------------------------------------------------------------------------------------------------------




// AsMop
//------------------------------------------------------------------------------------------------------------
AsMop::~AsMop()
{
	for (auto* indicator : Mop_Indicators)
		delete indicator;

	Mop_Indicators.erase(Mop_Indicators.begin(), Mop_Indicators.end());

	for (auto* cylinder : Mop_Cylinders)
		delete cylinder;

	Mop_Cylinders.erase(Mop_Cylinders.begin(), Mop_Cylinders.end());
}
//------------------------------------------------------------------------------------------------------------
AsMop::AsMop()
	:Y_Pos(0), Starting_Tick(0), Acting(false)
{
	int i;
	int x_pos, y_pos;
	AMop_Indicators* indicator;
	AMop_Cylinders* cylinder;

	for (i = 0; i < 10; i++)
	{
		indicator = new AMop_Indicators(AsConfig::Level_X_Offset + 1 + i * 19, AsConfig::Level_Y_Offset + 1, i * 80);
		Mop_Indicators.push_back(indicator);
	}

	for (i = 0; i < 5; i++)
	{
		x_pos = AsConfig::Level_X_Offset + Width / 2 - 6 - i;
		y_pos = AsConfig::Level_Y_Offset + 7 + i * 5;

		cylinder = new AMop_Cylinders(x_pos, y_pos, 13 + i * 2, 1, AMop_Cylinders::Max_Cylinder_Height[i]);
		Mop_Cylinders.push_back(cylinder);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsMop::Begin_Movement()
{
	//!!! TODO
}
//------------------------------------------------------------------------------------------------------------
void AsMop::Finish_Movement()
{
	//!!! TODO
}
//------------------------------------------------------------------------------------------------------------
void AsMop::Advance(double max_speed)
{
	//!!! TODO
}
//------------------------------------------------------------------------------------------------------------
double AsMop::Get_Speed()
{
	return 0.0;
	//!!! TODO
}
//------------------------------------------------------------------------------------------------------------
void AsMop::Act()
{
	int time_offset;
	double ratio;

	if (!Acting)
		return;

	Previous_Mop_Rect = Mop_Rect;

	time_offset = AsConfig::Current_Timer_Tick - Starting_Tick;

	if (time_offset < Expansion_Timeout)
	{
		ratio = (double)time_offset / (double)Expansion_Timeout;

		for (auto* cylinder : Mop_Cylinders)
			cylinder->Set_Height_For(ratio);

		Set_Mop();
	}

	for (auto* indicator : Mop_Indicators)
		indicator->Act();

	AsTools::Invalidate_Rect(Mop_Rect);
	AsTools::Invalidate_Rect(Previous_Mop_Rect);
}
//------------------------------------------------------------------------------------------------------------
void AsMop::Clear(HDC hdc, RECT& paint_area)
{
	if (!Acting)
		return;

	RECT intersection_rect;

	if (!IntersectRect(&intersection_rect, &paint_area, &Previous_Mop_Rect))
		return;

	AsTools::Rect(hdc, Previous_Mop_Rect, AsConfig::BG_Color);
}
//------------------------------------------------------------------------------------------------------------
void AsMop::Draw(HDC hdc, RECT& paint_area)
{
	if (!Acting)
		return;

	AsTools::Rect(hdc, AsConfig::Level_X_Offset, Y_Pos, Width, Height, AsConfig::Red_Color);

	for (auto* indicator : Mop_Indicators)
		indicator->Draw(hdc, paint_area);

	for (auto* cylinder : Mop_Cylinders)
		cylinder->Draw(hdc, paint_area);
}
//------------------------------------------------------------------------------------------------------------
bool AsMop::Is_Finished()
{
	return false;
	//!!! TODO
}
//------------------------------------------------------------------------------------------------------------
void AsMop::Erase_Level()
{
	Starting_Tick = AsConfig::Current_Timer_Tick;
	Y_Pos = 172;

	Acting = true;

	Set_Mop();
}
//------------------------------------------------------------------------------------------------------------
void AsMop::Set_Mop()
{
	int i;
	int total_cylinder_height = 0;
	const int scale = AsConfig::Global_Scale;

	for (auto* cylinder : Mop_Cylinders)
		total_cylinder_height += cylinder->Get_Height();

	Y_Pos = AsConfig::Max_Y_Pos - total_cylinder_height - Height;

	for (auto* indicator : Mop_Indicators)
		indicator->Set_Y_Pos(Y_Pos + 1);

	for (i = 0; i < (int)Mop_Cylinders.size(); i++)
		Mop_Cylinders[i]->Set_Y_Pos(Y_Pos + Height + i * 5);

	Mop_Rect.left = AsConfig::Level_X_Offset * scale;
	Mop_Rect.top = Y_Pos * scale;
	Mop_Rect.right = Mop_Rect.left + Width * scale;
	Mop_Rect.bottom = Mop_Rect.top + Height * scale;
}
//------------------------------------------------------------------------------------------------------------




// AsLevel
//------------------------------------------------------------------------------------------------------------
AsLevel* AsLevel::Level = nullptr;
//------------------------------------------------------------------------------------------------------------
AsLevel::~AsLevel()
{
	Cancel_All_Activity();
}
//------------------------------------------------------------------------------------------------------------
AsLevel::AsLevel()
: Level_Rect{}, Need_To_Cancel_All(false), Parachute_Color(AsConfig::Red_Color, AsConfig::Blue_Color, AsConfig::Global_Scale), Advertisement(0)
{
	Level = this;
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Check_Hit(double next_x_pos, double next_y_pos, ABall_Object* ball)
{// Correcting the position when reflected from bricks

	int i, j;
	double direction;
	double min_ball_x, max_ball_x;
	double min_ball_y, max_ball_y;
	int min_level_x, max_level_x;
	int min_level_y, max_level_y;
	bool got_horizontal_hit, got_vertical_hit;
	double horizontal_reflection_pos, vertical_reflection_pos;

	if (ball->Get_State() == EBall_State::On_Parachute)
		return false;

	if (next_y_pos - AsConfig::Ball_Radius > AsConfig::Level_Y_Offset + (AsConfig::Level_Height - 1) * AsConfig::Cell_Height + AsConfig::Brick_Height)
		return false;

	direction = ball->Get_Direction();

	min_ball_x = next_x_pos - AsConfig::Ball_Radius;
	max_ball_x = next_x_pos + AsConfig::Ball_Radius;
	min_ball_y = next_y_pos - AsConfig::Ball_Radius;
	max_ball_y = next_y_pos + AsConfig::Ball_Radius;

	min_level_x = (int)( (min_ball_x - AsConfig::Level_X_Offset) / (double)AsConfig::Cell_Width);
	if (min_level_x < 0)
		min_level_x = 0;

	max_level_x = (int)( (max_ball_x - AsConfig::Level_X_Offset) / (double)AsConfig::Cell_Width);
	if (max_level_x >= AsConfig::Level_Width - 1)
		max_level_x = AsConfig::Level_Width - 1;

	min_level_y = (int)( (min_ball_y - AsConfig::Level_Y_Offset) / (double)AsConfig::Cell_Height);
	if (min_level_y < 0)
		min_level_y = 0;

	max_level_y = (int)( (max_ball_y - AsConfig::Level_Y_Offset) / (double)AsConfig::Cell_Height);
	if (max_level_y >= AsConfig::Level_Height - 1)
		max_level_y = AsConfig::Level_Height - 1;

	for (i = max_level_y; i >= min_level_y; i--)
	{
		Current_Brick_Top_Y = AsConfig::Level_Y_Offset + i * AsConfig::Cell_Height;
		Current_Brick_Low_Y = Current_Brick_Top_Y + AsConfig::Brick_Height;

		for (j = min_level_x; j <= max_level_x; j++)
		{
			if (Current_Level[i][j] == 0)
				continue;

			Current_Brick_Left_X = AsConfig::Level_X_Offset + j * AsConfig::Cell_Width;
			Current_Brick_Right_X = Current_Brick_Left_X + AsConfig::Brick_Width;

			got_horizontal_hit = Check_Horizontal_Hit(next_x_pos, next_y_pos, j, i, ball, horizontal_reflection_pos);

			got_vertical_hit = Check_Vertical_Hit(next_x_pos, next_y_pos, j, i, ball, vertical_reflection_pos);

			if (got_horizontal_hit && got_vertical_hit)
			{
				if (On_Hit(j, i, ball, true) )
				{
					if (vertical_reflection_pos < horizontal_reflection_pos)
						ball->Reflect(true);
					else
						ball->Reflect(false);
				}
				return true;
			}
			else
				if (got_horizontal_hit)
				{
					if (On_Hit(j, i, ball, false) )
						ball->Reflect(false);

					return true;
				}
				else
					if (got_vertical_hit)
					{
						if (On_Hit(j, i, ball, true) )
							ball->Reflect(true);

						return true;
					}
		}
	}
	return false;
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Check_Hit(double next_x_pos, double next_y_pos)
{// Return "true" if at given positions our laser beam hits a brick

	int level_x_index, level_y_index;

	level_x_index = (int)( (next_x_pos - AsConfig::Level_X_Offset) / (double)AsConfig::Cell_Width);
	level_y_index = (int)( (next_y_pos - AsConfig::Level_Y_Offset) / (double)AsConfig::Cell_Height);

	if (level_x_index < 0 || level_x_index >= AsConfig::Level_Width)
		return false;

	if (level_y_index < 0 || level_y_index >= AsConfig::Level_Height)
		return false;

	if (Current_Level[level_y_index][level_x_index] == 0)
		return false;

	On_Hit(level_x_index, level_y_index, nullptr, false);

	return true;
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Begin_Movement()
{
	// not yet used.
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Finish_Movement()
{
	// not yet used.
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Advance(double max_speed)
{
	// not yet used.
}
//------------------------------------------------------------------------------------------------------------
double AsLevel::Get_Speed()
{
	// not yet used.
	return 0.0;
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Act()
{
	Act_Objects(Active_Bricks);
	Act_Objects(Falling_Letters);

	if (Advertisement != 0)
		Advertisement->Act();

	Mop.Act();
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Clear(HDC hdc, RECT& paint_area)
{// 1. Erase all moving objects


	for (auto* letter : Falling_Letters)
		letter->Clear(hdc, paint_area);

	if (Advertisement != 0)
		Advertisement->Clear(hdc, paint_area);

	if (Need_To_Cancel_All)
	{
		Cancel_All_Activity();
		Need_To_Cancel_All = false;
	}

	Mop.Clear(hdc, paint_area);
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Draw(HDC hdc, RECT& paint_area)
{// Draw all the objects of the level

	int i, j;
	RECT intersection_rect, brick_rect;

	if (Advertisement != 0)
		Advertisement->Draw(hdc, paint_area);

	if (IntersectRect(&intersection_rect, &paint_area, &Level_Rect))
	{
		for (i = 0; i < AsConfig::Level_Height; i++)
			for (j = 0; j < AsConfig::Level_Width; j++)
			{
				brick_rect.left = (AsConfig::Level_X_Offset + j * AsConfig::Cell_Width) * AsConfig::Global_Scale;
				brick_rect.top = (AsConfig::Level_Y_Offset + i * AsConfig::Cell_Height) * AsConfig::Global_Scale;
				brick_rect.right = brick_rect.left + AsConfig::Brick_Width * AsConfig::Global_Scale;
				brick_rect.bottom = brick_rect.top + AsConfig::Brick_Height * AsConfig::Global_Scale;

				if (IntersectRect(&intersection_rect, &paint_area, &brick_rect) )
					Draw_Brick(hdc, brick_rect, j, i);
			}

		for (auto bricks : Active_Bricks)
			bricks->Draw(hdc, paint_area);
	}

	for (auto letter : Falling_Letters)
		letter->Draw(hdc, paint_area);

	Mop.Draw(hdc, paint_area);
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Is_Finished()
{
	return false; 	// Not used
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Init()
{
	int i;
	ALevel_Data* level_data;

	Level_Rect.left = AsConfig::Level_X_Offset * AsConfig::Global_Scale;
	Level_Rect.top = AsConfig::Level_Y_Offset * AsConfig::Global_Scale;
	Level_Rect.right = Level_Rect.left + AsConfig::Cell_Width * AsConfig::Level_Width * AsConfig::Global_Scale;
	Level_Rect.bottom = Level_Rect.top + AsConfig::Cell_Height * AsConfig::Level_Height * AsConfig::Global_Scale;

	memset(Current_Level, 0, sizeof(Current_Level) );

	for (i = 0; i < ALevel_Data::Max_Level_Number; i++)
	{
		level_data = new ALevel_Data(i + 1);
		Levels_Data.push_back(level_data);

		if (i == 7)

			level_data->Advertisement = new AAdvertisement(4, 7, 2, 3);
		if (i == 9)
			level_data->Advertisement = new AAdvertisement(1, 9, 2, 3);

	}

	Mop.Erase_Level();
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Set_Current_Level(int level_number)
{
	int i, j;
	EBrick_Type brick_type;
	ALevel_Data* levels_data;

	if (level_number < 1 || level_number > (int)Levels_Data.size())
		AsConfig::Throw();

	levels_data = Levels_Data[level_number - 1];

	memcpy(Current_Level, levels_data->Level, sizeof(Current_Level) );

	Advertisement = levels_data->Advertisement;

	// 1. Count the number of teleports
	Teleport_Bricks_Pos.erase(Teleport_Bricks_Pos.begin(), Teleport_Bricks_Pos.end());

	for (i = 0; i < AsConfig::Level_Height; i++)
	{
		for (j = 0; j < AsConfig::Level_Width; j++)
		{
			brick_type = (EBrick_Type)Current_Level[i][j];

			if (brick_type == EBrick_Type::Teleport)
				Teleport_Bricks_Pos.emplace_back(j, i);
		}
	}

	if (Teleport_Bricks_Pos.size() == 1)
		AsConfig::Throw(); // Teleports count must be 0 or more than 1!
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Get_Next_Falling_Letter(int& index, AFalling_Letter** falling_letter)
{
	if (index < (int)Falling_Letters.size())
	{
		*falling_letter = (AFalling_Letter*)Falling_Letters[index++];
		return true;
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Stop()
{
	Need_To_Cancel_All = true;
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Has_Brick_At(int level_x, int level_y)
{
	EBrick_Type brick_type;

	if (level_x < 0 || level_x >= AsConfig::Level_Width)
		return false;	
	
	if (level_y < 0 || level_y >= AsConfig::Level_Height)
		return false;

	brick_type = (EBrick_Type)Level->Current_Level[level_y][level_x];

	if (brick_type == EBrick_Type::None)
		return false;
	else
		return true;
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Has_Brick_At(RECT& monster_rect)
{
	const int scale = AsConfig::Global_Scale;
	const double d_scale = AsConfig::D_Global_Scale;

	int i, j;
	int min_x, max_x;
	int min_y, max_y;
	int min_cell_y, min_cell_x;

	if (monster_rect.top > (AsConfig::Level_X_Offset * scale) + ((AsConfig::Level_Height - 1) * AsConfig::Cell_Height + AsConfig::Brick_Height) * scale)
		return false;

	min_x = (monster_rect.left - AsConfig::Level_X_Offset * scale) / (AsConfig::Cell_Width * scale);
	max_x = (monster_rect.right - AsConfig::Level_X_Offset * scale) / (AsConfig::Cell_Width * scale);

	min_y = (monster_rect.top - AsConfig::Level_Y_Offset * scale) / (AsConfig::Cell_Height * scale);
	max_y = (monster_rect.bottom - AsConfig::Level_Y_Offset * scale) / (AsConfig::Cell_Height * scale);

	/*The cell for a brick is larger than the brick itself. 
	We make a correction so that the collision with the monster is counted when it comes into contact with a brick, 
	and not with a cell for a brick*/

	min_cell_y = min_y * AsConfig::Cell_Height * scale + (AsConfig::Level_Y_Offset * scale);
	min_cell_x = min_x * AsConfig::Cell_Width * scale + (AsConfig::Level_X_Offset * scale);

	if (monster_rect.top - min_cell_y > AsConfig::Brick_Height * scale)
		++min_y;

	if (monster_rect.left - min_cell_x > AsConfig::Brick_Width * scale)
		++min_x;

	//if (min_x >= AsConfig::Level_Width)
	//	min_x = AsConfig::Level_Width - 1; 

	//if (max_x >= AsConfig::Level_Width)
	//	max_x = AsConfig::Level_Width - 1;


	//if (min_y >= AsConfig::Level_Height)
	//	min_y = AsConfig::Level_Height - 1;

	//if (max_y >= AsConfig::Level_Height)
	//	max_y = AsConfig::Level_Height - 1;

	for (i = min_y; i <= max_y; i++)
		for (j = min_x; j <= max_x; j++)
			if (Has_Brick_At(j, i))
				return true;

	return false;
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::On_Hit(int brick_x, int brick_y, ABall_Object* ball, bool vertical_hit)
{
	EBrick_Type brick_type;
	bool can_reflect = true;

	brick_type = (EBrick_Type)Current_Level[brick_y][brick_x];

	if (ball == nullptr && brick_type == EBrick_Type::Parachute)
	{
		brick_type = EBrick_Type::Red;
		Current_Level[brick_y][brick_x] = (char)brick_type;
	}

	if (brick_type == EBrick_Type::Parachute)
	{
		ball->Set_On_Parachute(brick_x, brick_y);
		Current_Level[brick_y][brick_x] = (char)EBrick_Type::None;
		can_reflect = false;
	}
	else if (Add_Falling_Letter(brick_x, brick_y, brick_type) )
		Current_Level[brick_y][brick_x] = (char)EBrick_Type::None;
	else
		can_reflect = Create_Active_Brick(brick_x, brick_y, brick_type, ball, vertical_hit);

	Redraw_Brick(brick_x, brick_y);

	AsInformation_Panel::Update_Score(EScore_Event_Type::Hit_Brick);

	return can_reflect;
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Redraw_Brick(int brick_x, int brick_y)
{
	RECT brick_rect;

	brick_rect.left = (AsConfig::Level_X_Offset + brick_x * AsConfig::Cell_Width) * AsConfig::Global_Scale;
	brick_rect.top = (AsConfig::Level_Y_Offset + brick_y * AsConfig::Cell_Height) * AsConfig::Global_Scale;
	brick_rect.right = brick_rect.left + AsConfig::Brick_Width * AsConfig::Global_Scale;
	brick_rect.bottom = brick_rect.top + AsConfig::Brick_Height * AsConfig::Global_Scale;

	AsTools::Invalidate_Rect(brick_rect);
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Add_Falling_Letter(int brick_x, int brick_y, EBrick_Type brick_type)
{ // Create a falling letter if possible

	int letter_x, letter_y;
	ELetter_Type letter_type;
	AFalling_Letter* falling_letter;

	if (!(brick_type == EBrick_Type::Red || brick_type == EBrick_Type::Blue) )
		return false;

	if (AsTools::Rand(AsConfig::Hits_Per_Letter) != 0)
		return false;
	
	if (Falling_Letters.size() >= AsConfig::Max_Falling_Letters_Count)
		return false;
	
	letter_x = (brick_x * AsConfig::Cell_Width + AsConfig::Level_X_Offset) * AsConfig::Global_Scale;
	letter_y = (brick_y * AsConfig::Cell_Height + AsConfig::Level_Y_Offset) * AsConfig::Global_Scale;

	letter_type = AFalling_Letter::Get_Random_Letter_Type();

	switch (AsTools::Rand(4) )
	{
	case 0:
		letter_type = ELetter_Type::O;
		break;

	case 1:
		letter_type = ELetter_Type::L;
		break;

	case 2:
		letter_type = ELetter_Type::K;
		break;

	case 3:
		letter_type = ELetter_Type::W;
		break;
	}

	falling_letter = new AFalling_Letter(brick_type, letter_type, letter_x, letter_y);
	Falling_Letters.push_back(falling_letter);

	return true;
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Create_Active_Brick(int brick_x, int brick_y, EBrick_Type brick_type, ABall_Object* ball, bool vertical_hit)
{// Create an active brick if possible

	AActive_Brick* active_brick = 0;

	if (Active_Bricks.size() >= AsConfig::Max_Active_Bricks_Count)
		return true; // There are too many active bricks!

	switch (brick_type)
	{
	case EBrick_Type::None:
		return true;

	case EBrick_Type::Red:
	case EBrick_Type::Blue:
		active_brick = new AActive_Brick_Red_Blue(brick_type, brick_x, brick_y);
		Current_Level[brick_y][brick_x] = (char)EBrick_Type::None;
		break;

	case EBrick_Type::Unbreakable:
		active_brick = new AActive_Brick_Unbreakable(brick_x, brick_y);
		break;	
	
	case EBrick_Type::Multihit_1:
		active_brick = new AActive_Brick_Multihit(brick_x, brick_y);
		Current_Level[brick_y][brick_x] = (char)EBrick_Type::None;
		break;

	case EBrick_Type::Multihit_2:
	case EBrick_Type::Multihit_3:
	case EBrick_Type::Multihit_4:
		Current_Level[brick_y][brick_x] = (char)brick_type - 1;
		break;

	case EBrick_Type::Parachute:
		AsConfig::Throw(); // An active brick is not created for the parachute
		break;

	case EBrick_Type::Teleport:
		if (ball != nullptr)
			Add_Active_Brick_Teleport(brick_x, brick_y, ball, vertical_hit);
		return false;

	case EBrick_Type::Ad:
		active_brick = new AActive_Brick_Ad(brick_x, brick_y, Advertisement);
		Current_Level[brick_y][brick_x] = (char)EBrick_Type::Invisible;
		break;

	case EBrick_Type::Invisible:
		return true;

	default:
		AsConfig::Throw();
	}

	if (active_brick != 0)
		Active_Bricks.push_back(active_brick);

	return true;
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Add_Active_Brick_Teleport(int brick_x, int brick_y, ABall_Object* ball, bool vertical_hit)
{
	int i;
	bool got_direction;
	EDirection_Type direction;
	int dest_brick_x, dest_brick_y;
	double pre_teleport_x_pos, pre_teleport_y_pos;
	double curr_ball_x_pos, curr_ball_y_pos;
	AActive_Brick_Teleport* source_teleport, *destination_teleport;

	ball->Get_Center(pre_teleport_x_pos, pre_teleport_y_pos); // The position of the ball before it hits the teleporter

	destination_teleport = Select_Destination_Teleport(brick_x, brick_y);
	source_teleport = new AActive_Brick_Teleport(brick_x, brick_y, ball, destination_teleport);

	destination_teleport->Get_Level_Pos(dest_brick_x, dest_brick_y);

	// After creating a teleport, the ball is placed in its center
	ball->Get_Center(curr_ball_x_pos, curr_ball_y_pos);

	// Finding the direction the ball came from
	if (vertical_hit)
	{
		if (pre_teleport_y_pos < curr_ball_y_pos)
			direction = EDirection_Type::Down;
		else
			direction = EDirection_Type::Up;
	}
	else
	{
		if (pre_teleport_x_pos < curr_ball_x_pos)
			direction = EDirection_Type::Right;
		else
			direction = EDirection_Type::Left;
	}

	// Selecting the direction exit from the destination teleport - looking for a free one
	got_direction = false;

	for (i = 0; i < 4; i++)
	{
		switch (direction)
		{
		case EDirection_Type::Left:
			if (dest_brick_x > 0 && Current_Level[dest_brick_y][dest_brick_x - 1] == (char)EBrick_Type::None)
				got_direction = true;
			break;

		case EDirection_Type::Up:
			if (dest_brick_y > 0 && Current_Level[dest_brick_y -1][dest_brick_x] == (char)EBrick_Type::None)
				got_direction = true;
			break;

		case EDirection_Type::Right:
			if (dest_brick_x < AsConfig::Level_Width - 1 && (EBrick_Type)Current_Level[dest_brick_y][dest_brick_x + 1] == EBrick_Type::None)
				got_direction = true;
			break;

		case EDirection_Type::Down:
			if (dest_brick_y < AsConfig::Level_Height - 1 && (EBrick_Type)Current_Level[dest_brick_y + 1][dest_brick_x] == EBrick_Type::None)
				got_direction = true;
			break;

		default:
			AsConfig::Throw();
		}
		if (got_direction)
			break;

		direction = (EDirection_Type)( (int)direction - 1);
		ball->Set_Direction(ball->Get_Direction() + M_PI_2);

		if ( (int)direction < 0)
			direction = EDirection_Type::Down;
	}

	destination_teleport->Release_Direction = direction;

	Active_Bricks.push_back(source_teleport);
	Active_Bricks.push_back(destination_teleport);
}
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Teleport* AsLevel::Select_Destination_Teleport(int source_x, int source_y)
{
	int dest_index;

	AActive_Brick_Teleport* destination_teleport;

	if (Teleport_Bricks_Pos.size() < 2)
	{
		AsConfig::Throw();
		return 0;
	}

	dest_index = AsTools::Rand(Teleport_Bricks_Pos.size());

	if (Teleport_Bricks_Pos[dest_index].X == source_x && Teleport_Bricks_Pos[dest_index].Y == source_y)
	{// If the source teleport is randomly selected, then go to the next one in the list

		++dest_index;

		if (dest_index >= (int)Teleport_Bricks_Pos.size())
			dest_index = 0; // Go to the beginning of the array, if you have gone beyond it
	}

	destination_teleport = new AActive_Brick_Teleport(Teleport_Bricks_Pos[dest_index].X, Teleport_Bricks_Pos[dest_index].Y, 0, 0);

	return destination_teleport;
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Check_Vertical_Hit(double next_x_pos, double next_y_pos, int level_x, int level_y, ABall_Object* ball, double &reflection_pos)
{
	double direction = ball->Get_Direction();

	if (ball->Is_Moving_Up() )
	{// Checking if it hits the bottom

		if (Hit_Circle_On_Line(next_y_pos - Current_Brick_Low_Y, next_x_pos, Current_Brick_Left_X, Current_Brick_Right_X, AsConfig::Ball_Radius, reflection_pos) )
		{// Checking the possibility of reflection down

			if (level_y == AsConfig::Level_Height - 1 ||
				(level_y < AsConfig::Level_Height - 1 && Current_Level[level_y + 1][level_x] == 0))
				return true;
			else
				return false;
		}
	}
	else
	{// Checking if it hits the top

			if (Hit_Circle_On_Line(next_y_pos - Current_Brick_Top_Y, next_x_pos, Current_Brick_Left_X, Current_Brick_Right_X, AsConfig::Ball_Radius, reflection_pos) )
			{// Checking the possibility of reflecting UP

				if (level_y > 0 && Current_Level[level_y - 1][level_x] == 0)
					return true;
				else
					return false;
			}
	}
	return false;
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Check_Horizontal_Hit(double next_x_pos, double next_y_pos, int level_x, int level_y, ABall_Object* ball, double& reflection_pos)
{
	double direction = ball->Get_Direction();

	if (! ball->Is_Moving_Left() )
	{// Checking for a hit on the "left" edge

		if (Hit_Circle_On_Line(Current_Brick_Left_X - next_x_pos, next_y_pos, Current_Brick_Top_Y, Current_Brick_Low_Y, AsConfig::Ball_Radius, reflection_pos) )
		{// Checking the possibility of reflecting "left"

			if (level_x > 0 && Current_Level[level_y][level_x - 1] == 0)
				return true;
			else
				return false;
		}
	}
	else
	{// Checking if the "right" edge hits

			if (Hit_Circle_On_Line(Current_Brick_Right_X - next_x_pos, next_y_pos, Current_Brick_Top_Y, Current_Brick_Low_Y, AsConfig::Ball_Radius, reflection_pos) )
			{
				// Checking the possibility of reflecting "right"
				if (level_x < AsConfig::Level_Width - 1 && Current_Level[level_y][level_x + 1] == 0)
					return true;

				else
					return false;
			}
	}
	return false;

}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Draw_Brick(HDC hdc, RECT& brick_rect, int level_x, int level_y)
{// Drawing a "brick"


	const AColor* color = 0;
	EBrick_Type brick_type = (EBrick_Type)Current_Level[level_y][level_x];


	switch (brick_type)
	{
	case EBrick_Type::None:
		if (Advertisement != 0 && Advertisement->Has_Brick_At(level_x, level_y))
			break;

	case EBrick_Type::Red:
	case EBrick_Type::Blue:
		AActive_Brick_Red_Blue::Draw_In_Level(hdc, brick_rect, brick_type);
		break;

	case EBrick_Type::Unbreakable:
		AActive_Brick_Unbreakable::Draw_In_Level(hdc, brick_rect);
		break;

	case EBrick_Type::Multihit_1:
	case EBrick_Type::Multihit_2:
	case EBrick_Type::Multihit_3:
	case EBrick_Type::Multihit_4:
		AActive_Brick_Multihit::Draw_In_Level(hdc, brick_rect, brick_type);
		break;

	case EBrick_Type::Parachute:
		Draw_Parachute_In_Level(hdc, brick_rect);
		break;

	case EBrick_Type::Teleport:
		AActive_Brick_Teleport::Draw_In_Level(hdc, brick_rect);
		break;

	case EBrick_Type::Ad:
		AActive_Brick_Ad::Draw_In_Level(hdc, brick_rect);
		break;

	case EBrick_Type::Invisible:
		break;

	default:
		AsConfig::Throw();
	}
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Draw_Parachute_In_Level(HDC hdc, RECT& brick_rect)
{
	Draw_Parachute_Part(hdc, brick_rect, 0, 4);
	Draw_Parachute_Part(hdc, brick_rect, 4, 6);
	Draw_Parachute_Part(hdc, brick_rect, 10, 4);
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Draw_Parachute_Part(HDC hdc, RECT& brick_rect, int offset, int width)
{
	const int scale = AsConfig::Global_Scale;
	RECT rect;

	// 1. Upper segment
	rect.left = brick_rect.left + offset * scale + 1;
	rect.top = brick_rect.top + 1;
	rect.right = rect.left + width * scale + 1;
	rect.bottom = rect.top + 3 * scale + 1;

	Parachute_Color.Select(hdc);
	AsTools::Round_Rect(hdc, rect);

	// 1. Lower segment
	rect.top += 3 * scale;
	rect.bottom += 3 * scale;

	AsTools::Round_Rect(hdc, rect);
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Delete_Objects(std::vector<AGraphics_Object*>& falling_letters)
{
	for (auto it = falling_letters.begin(); it != falling_letters.end(); ++it)
		delete* it;

	falling_letters.erase(falling_letters.begin(), falling_letters.end());
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Act_Objects(std::vector <AGraphics_Object*> &falling_letters)
{
	auto it = falling_letters.begin();

	while (it != falling_letters.end())
	{
		(*it)->Act();

		if ((*it)->Is_Finished())
		{
			delete* it; // deleting object in heap
			it = falling_letters.erase(it); // delete pointer to deleted object
		}
		else
			it++;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Cancel_All_Activity()
{
	Delete_Objects(Active_Bricks);
	Delete_Objects(Falling_Letters);
}
//------------------------------------------------------------------------------------------------------------