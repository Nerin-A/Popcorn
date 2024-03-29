﻿#include "Monster.h"

// AMonster
//------------------------------------------------------------------------------------------------------------
AMonster::~AMonster()
{
}
//------------------------------------------------------------------------------------------------------------
AMonster::AMonster()
	:Need_To_Freeze(false), Monster_State(EMonster_State::Missing), X_Pos(0.0), Y_Pos(0.0), Speed(0.0), Previous_Speed(0.0), Direction(0.0), Next_Direction_Switch_Tick(0), Monster_Is_Alive_Timer(0), Monster_Rect{},
	Previous_Monster_Rect{}
{
}
//------------------------------------------------------------------------------------------------------------
bool AMonster::Check_Hit(double next_x_pos, double next_y_pos, ABall_Object* ball)
{ // Checking if the ball hit the monster

	double radius = (double)Width / 2.0;

	if (!(Monster_State == EMonster_State::Emitting || Monster_State == EMonster_State::Alive))
		return false;

	if (! AsTools::Reflect_On_Circle(next_x_pos, next_y_pos, X_Pos + radius, Y_Pos + radius, radius, ball))
		return false;

	Destroy();

	return true;
}
//------------------------------------------------------------------------------------------------------------
bool AMonster::Check_Hit(double next_x_pos, double next_y_pos)
{// Return "true" if at given positions our laser beam hits a monster

	if (!(Monster_State == EMonster_State::Emitting || Monster_State == EMonster_State::Alive))
		return false;

	if (next_x_pos >= X_Pos && next_x_pos <= X_Pos + (double)Width && next_y_pos >= Y_Pos && next_y_pos <= Y_Pos + (double)Height)
	{
		Destroy();
		return true;
	}
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
bool AMonster::Check_Hit(RECT& rect)
{// Return "true" if at given positions our platform hits a monster

	RECT intersection_rect;

	if (! (Monster_State == EMonster_State::Emitting || Monster_State == EMonster_State::Alive) )
		return false;

	if (IntersectRect(&intersection_rect, &rect, &Monster_Rect) )
	{
		Destroy();
		return true;
	}
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Begin_Movement()
{
	// Not used
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Finish_Movement()
{
	if (Is_Finished())
		return;

	Redraw_Monster();
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Advance(double max_speed)
{
	int i;
	double original_direction;
	double next_step;
	double next_x_pos, next_y_pos;
	bool changed_direction = false;
	RECT monster_rect;

	if (! (Monster_State == EMonster_State::Emitting || Monster_State == EMonster_State::Alive) || Speed == 0.0)
		return;

	next_step = Speed / max_speed * AsConfig::Moving_Step_Size;

	original_direction = Direction;

	for (i = 0; i < 60; ++i)
	{
		next_x_pos = X_Pos + next_step * cos(Direction);
		next_y_pos = Y_Pos - next_step * sin(Direction);

		Get_Monster_Rect(next_x_pos, next_y_pos, monster_rect);

		if (AsLevel::Has_Brick_At(monster_rect))
			Direction += M_PI / 30.0;
		else
		{
			changed_direction = true;
			break;
		}
	}

	if (! changed_direction)
	{
		Direction = original_direction - M_PI;
		return;
	}

	// This code will restrict monster movement to level border.
	if (Monster_State == EMonster_State::Alive)
	{
		if (next_x_pos < (double)AsConfig::Level_X_Offset)
			next_x_pos = (double)AsConfig::Level_X_Offset;

		if (next_x_pos + (double)Width > (double)AsConfig::Max_X_Pos)
			next_x_pos = (double)AsConfig::Max_X_Pos - (double)Width;

		if (next_y_pos < (double)AsConfig::Level_Y_Offset)
			next_y_pos = (double)AsConfig::Level_Y_Offset;

		if (next_y_pos + (double)Height > (double)AsConfig::Floor_Y_Pos)
			next_y_pos = (double)AsConfig::Floor_Y_Pos - (double)Height;
	}

	Y_Pos = next_y_pos;
	X_Pos = next_x_pos;
}
//------------------------------------------------------------------------------------------------------------
double AMonster::Get_Speed()
{
	return Speed;
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Act()
{
	switch (Monster_State)
	{
	case EMonster_State::Missing:
		return;


	case EMonster_State::Emitting:
		Act_Alive();

		if (Monster_Is_Alive_Timer < AsConfig::Current_Timer_Tick)
			Monster_State = EMonster_State::Alive;
		return;


	case EMonster_State::Alive:
		if (Need_To_Freeze)
		{
			Previous_Speed = Speed;
			Speed = 0.0;
			Need_To_Freeze = false;
		}

		Act_Alive();
		Change_Direction();
		break;


	case EMonster_State::Destroying:
		Act_Destroying();
		break;


	default:
		AsConfig::Throw();
		break;
	}

	AsTools::Invalidate_Rect(Monster_Rect);
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Clear(HDC hdc, RECT& paint_area)
{
	RECT intersection_rect;

	if (!IntersectRect(&intersection_rect, &paint_area, &Previous_Monster_Rect))
		return;

	AsTools::Ellipse(hdc, Previous_Monster_Rect, AsConfig::BG_Color);
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Draw(HDC hdc, RECT& paint_area)
{
	RECT intersection_rect;

	if (!IntersectRect(&intersection_rect, &paint_area, &Monster_Rect))
		return;

	switch (Monster_State)
	{
	case EMonster_State::Missing:
		break;

	case EMonster_State::Emitting:
	case EMonster_State::Alive:
		Draw_Alive(hdc);
		break;

	case EMonster_State::Destroying:
		Draw_Explosion(hdc, paint_area);
		break;

	default:
		AsConfig::Throw();
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
bool AMonster::Is_Finished()
{
	if (Monster_State == EMonster_State::Missing)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Activate(double x_pos, double y_pos, bool moving_right)
{
	int random_speed;
	int emitting_offset;

	Monster_State = EMonster_State::Emitting;

	X_Pos = x_pos;
	Y_Pos = y_pos;

	random_speed = AsTools::Rand(5) + 1;

	Speed = (double)random_speed / 10.0;

	emitting_offset = (int)((double)AGate::Width / Speed);

	Monster_Is_Alive_Timer = AsConfig::Current_Timer_Tick + emitting_offset;

	if (moving_right)
		Direction = 0.0;
	else
		Direction = M_PI;

	On_Activation();

	Redraw_Monster();
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Destroy()
{
	if (!(Monster_State == EMonster_State::Emitting || Monster_State == EMonster_State::Alive))
		return;

	Start_Explosion(Monster_Rect);

	Monster_State = EMonster_State::Destroying;

	AsInformation_Panel::Update_Score(EScore_Event_Type::Hit_Monster);
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Set_Freeze_State(bool freeze)
{
	if (! freeze)
		Speed = Previous_Speed;

	Need_To_Freeze = freeze;
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Draw_Alive(HDC hdc)
{
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Act_Alive()
{
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Act_Destroying()
{
	if (Act_On_Explosion())
		Monster_State = EMonster_State::Missing;
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Get_Monster_Rect(double x_pos, double y_pos, RECT& rect)
{
	const int scale = AsConfig::Global_Scale;
	const double d_scale = AsConfig::D_Global_Scale;

	rect.left = (int)(x_pos * d_scale);
	rect.top = (int)(y_pos * d_scale);
	rect.right = rect.left + Width * scale;
	rect.bottom = rect.top + Height * scale;
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Redraw_Monster()
{
	Previous_Monster_Rect = Monster_Rect;

	Get_Monster_Rect(X_Pos, Y_Pos, Monster_Rect);

	AsTools::Invalidate_Rect(Monster_Rect);
	AsTools::Invalidate_Rect(Previous_Monster_Rect);
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Change_Direction()
{
	double direction_delta;

	if (AsConfig::Current_Timer_Tick > Next_Direction_Switch_Tick)
	{
		Next_Direction_Switch_Tick += AsConfig::FPS;

		// Random direction of movement in the range of +/- 45 degrees.
		direction_delta = (double)(AsTools::Rand(90) - 45) * M_PI / 180.0;

		Direction += direction_delta;
	}
}
//------------------------------------------------------------------------------------------------------------




//  AMonster_Eye 
const double AMonster_Eye::Max_Cornea_Height = 11.0;
const double AMonster_Eye::Blinking_Timeouts[AMonster_Eye::Blink_Stages_Count] = { 0.4, 0.3, 1, 0.4, 0.4, 0.4, 0.9 };
const EEye_State AMonster_Eye::Blinking_States[AMonster_Eye::Blink_Stages_Count] =
{
	EEye_State::Closed,
	EEye_State::Opening,
	EEye_State::Staring,
	EEye_State::Closing,
	EEye_State::Opening,
	EEye_State::Staring,
	EEye_State::Closing
};
//------------------------------------------------------------------------------------------------------------
AMonster_Eye::AMonster_Eye()
	: Eye_State(EEye_State::Closed), Blink_Ticks(Blink_Stages_Count), Cornea_Height(Max_Cornea_Height), Start_Blinking_Time(0), Total_Animation_Time(0)
{
}
//------------------------------------------------------------------------------------------------------------
void AMonster_Eye::Draw_Alive(HDC hdc)
{
	const int scale = AsConfig::Global_Scale;
	const double d_scale = AsConfig::D_Global_Scale;
	const int half_scale = scale / 2;

	HRGN region;
	RECT rect, cornea_rect;

	if (Monster_State == EMonster_State::Missing)
		return;

	// 1. Draw background
	// 1.1 Creating a rect to draw background
	rect = Monster_Rect;
	++rect.right;
	++rect.bottom;

	region = CreateEllipticRgnIndirect(&rect);
	SelectClipRgn(hdc, region);

	// 1.2 Dark red part of the BG of the "eye" monster
	AsTools::Ellipse(hdc, Monster_Rect, AsConfig::Dark_Red_Color);

	// 1.3 Red part of the BG of the "eye" monster
	rect = Monster_Rect;
	rect.left -= 2 * scale;
	rect.top -= 3 * scale;
	rect.right -= 2 * scale;
	rect.bottom -= 3 * scale;

	AsTools::Ellipse(hdc, rect, AsConfig::Red_Color);

	SelectClipRgn(hdc, 0);
	DeleteObject(region);

	// 2 Draw the eye itself
	if (Eye_State == EEye_State::Closed)
		return;

	// 2.1 Draw the cornea of the "eye" monster

	cornea_rect = Monster_Rect;
	cornea_rect.left += scale + half_scale;
	cornea_rect.top += 2 * scale + (int)((Max_Cornea_Height / 2.0 - Cornea_Height / 2.0) * d_scale);
	cornea_rect.right -= scale + half_scale;
	cornea_rect.bottom = (int)(Cornea_Height * d_scale) + cornea_rect.top;

	// 2.2 Limiting the output of the monster's eye to the size of the cornea
	region = CreateEllipticRgnIndirect(&cornea_rect);
	SelectClipRgn(hdc, region);


	AsTools::Ellipse(hdc, cornea_rect, AsConfig::Eye_Monster_Cornea_Color);

	// 2.3 Draw the iris of the "eye" monster
	rect = Monster_Rect;
	rect.left += 4 * scale + half_scale;
	rect.top += 4 * scale;
	rect.right -= 4 * scale + half_scale;
	rect.bottom -= 5 * scale;

	AsTools::Ellipse(hdc, rect, AsConfig::Eye_Monster_Iris_Color);

	// 2.4 Draw the pupil of the "eye" monster
	rect = Monster_Rect;
	rect.left += 7 * scale;
	rect.top += 6 * scale;
	rect.right = rect.left + 2 * scale;
	rect.bottom = rect.top + 3 * scale;

	AsTools::Ellipse(hdc, rect, AsConfig::BG_Color);

	SelectClipRgn(hdc, 0);
	DeleteObject(region);

	// 2.5 Outlining the cornea
	AsConfig::BG_Outline_Color.Select_Pen(hdc);

	Arc(hdc, cornea_rect.left, cornea_rect.top, cornea_rect.right - 1, cornea_rect.bottom - 1, 0, 0, 0, 0);
}
//------------------------------------------------------------------------------------------------------------
void AMonster_Eye::Act_Alive()
{
	int i;
	int current_tick_offset, previous_tick;
	double ratio;

	if (Monster_State == EMonster_State::Missing)
		return;

	current_tick_offset = (AsConfig::Current_Timer_Tick - Start_Blinking_Time) % Total_Animation_Time;

	for (i = 0; i < Blink_Stages_Count; i++)
	{
		if (current_tick_offset < Blink_Ticks[i])
		{
			Eye_State = Blinking_States[i];
			break;
		}
	}

	if (i == 0)
		previous_tick = 0;
	else
		previous_tick = Blink_Ticks[i - 1];

	ratio = (double)(current_tick_offset - previous_tick) / (double)(Blink_Ticks[i] - previous_tick);

	switch (Eye_State)
	{
	case EEye_State::Closed:
		Cornea_Height = 0.0;
		break;

	case EEye_State::Opening:
		Cornea_Height = Max_Cornea_Height * ratio;
		break;

	case EEye_State::Staring:
		Cornea_Height = Max_Cornea_Height;
		break;

	case EEye_State::Closing:
		Cornea_Height = Max_Cornea_Height * (1.0 - ratio);
		break;

	default:
		AsConfig::Throw();
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
void AMonster_Eye::On_Activation()
{	// Blink animation ticks calculation
	int i;
	int tick_offset;
	double current_timeout = 0.0;

	current_timeout;
	Start_Blinking_Time = AsConfig::Current_Timer_Tick;

	for (i = 0; i < Blink_Stages_Count; i++)
	{
		current_timeout += Blinking_Timeouts[i];

		tick_offset = (int)((double)AsConfig::FPS * current_timeout);
		Blink_Ticks[i] = tick_offset;
	}

	Total_Animation_Time = tick_offset;
}
//------------------------------------------------------------------------------------------------------------




// AMonster_Comet
//------------------------------------------------------------------------------------------------------------
AMonster_Comet::AMonster_Comet()
	: Current_Angle(0.0), Ticks_Per_Rotation(0)
{
	int rotation_ticks_range = Max_Ticks_Per_Rotation - Min_Ticks_Per_Rotation;

	Ticks_Per_Rotation = AsTools::Rand(rotation_ticks_range) + Min_Ticks_Per_Rotation;
}
//------------------------------------------------------------------------------------------------------------
void AMonster_Comet::Clear(HDC hdc, RECT& paint_area)
{
	RECT intersection_rect;

	if (!IntersectRect(&intersection_rect, &paint_area, &Previous_Monster_Rect))
		return;

	AsTools::Rect(hdc, Previous_Monster_Rect, AsConfig::BG_Color);
}
//------------------------------------------------------------------------------------------------------------
void AMonster_Comet::Draw_Alive(HDC hdc)
{
	int i;
	XFORM xform, old_xform;
	RECT rect;
	double alpha;
	double monster_radius;

	const int scale = AsConfig::Global_Scale;
	const double d_scale = AsConfig::D_Global_Scale;
	int ball_size = 4 * scale - scale / 2;

	if (Monster_State == EMonster_State::Missing)
		return;

	monster_radius = (double)(Width * d_scale / 2.0 - 1.0);

	// 2. The dividing line
	alpha = Current_Angle;

	GetWorldTransform(hdc, &old_xform);

	for (i = 0; i < 2; i++)
	{
		xform.eM11 = (float)cos(alpha);
		xform.eM12 = (float)sin(alpha);
		xform.eM21 = (float)-sin(alpha);
		xform.eM22 = (float)cos(alpha);
		xform.eDx = (float)(X_Pos * d_scale + monster_radius);
		xform.eDy = (float)(Y_Pos * d_scale + monster_radius);
		SetWorldTransform(hdc, &xform);

		alpha += M_PI;

		rect.left = (int)-monster_radius;
		rect.top = -ball_size / 2;
		rect.right = rect.left + ball_size;
		rect.bottom = rect.top + ball_size;

		AsTools::Ellipse(hdc, rect, AsConfig::White_Color);

		AsConfig::Comet_Monster_Tail.Select_Pen(hdc);

		rect.left = (int)(-monster_radius + 2.0 * d_scale);
		rect.top = (int)(-monster_radius + 2.0 * d_scale);
		rect.right = (int)(monster_radius - 2.0 * d_scale);
		rect.bottom = (int)(monster_radius - 2.0 * d_scale);

		Arc(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1, 0, (int)-monster_radius, (int)-monster_radius, -4 * scale);

		rect.left += scale;
		rect.right -= scale;
		rect.bottom -= scale;

		Arc(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1, 0, (int)-monster_radius, (int)-monster_radius, -4 * scale);
	}

	SetWorldTransform(hdc, &old_xform);
}
//------------------------------------------------------------------------------------------------------------
void AMonster_Comet::Act_Alive()
{
	int time_offset;
	double ratio;

	if (Monster_State == EMonster_State::Missing)
		return;
	
	time_offset = (AsConfig::Current_Timer_Tick - Monster_Is_Alive_Timer) % Ticks_Per_Rotation;
	ratio = (double)time_offset / (double)Ticks_Per_Rotation;

	Current_Angle = ratio * 2.0 * -M_PI;
}
//------------------------------------------------------------------------------------------------------------
void AMonster_Comet::On_Activation()
{
}
//------------------------------------------------------------------------------------------------------------