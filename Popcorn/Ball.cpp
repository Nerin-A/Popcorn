﻿#include "Ball.h"

// ABall
AHit_Checker_List ABall::Hit_Checker_List;
//------------------------------------------------------------------------------------------------------------
ABall::ABall()
	: Ball_State (EBall_State::Disabled), Previous_Ball_State(EBall_State::Disabled), Release_Timer_Tick (0), Center_X_Pos(0), Center_Y_Pos(0.0), 
	  Ball_Speed(0.0), Prev_Ball_Speed (0.0), Ball_Direction(0.0), Prev_Ball_Direction (0.0), Testing_Is_Active(false), Test_Iteration(0), 
	  Ball_Rect{}, Prev_Ball_Rect{}, Recent_Collisions_Position(0), Recent_Hits_Count(0)
{
}
//------------------------------------------------------------------------------------------------------------
void ABall::Begin_Movement()
{
	Prev_Ball_Rect = Ball_Rect;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Finish_Movement()
{
	if (Ball_State == EBall_State::Disabled || Ball_State == EBall_State::Lost)
		return;

	Redraw_Ball();

	if (Ball_State == EBall_State::On_Parachute)
	{
		Previous_Parachute_Rect = Parachute_Rect;

		Parachute_Rect.bottom = Ball_Rect.bottom;
		Parachute_Rect.top = Parachute_Rect.bottom - Parachute_Size * AsConfig::Global_Scale;

		Redraw_Parachute();
	}
}
//------------------------------------------------------------------------------------------------------------
void ABall::Advance(double max_speed)
{
	int prev_hit_count = 0;
	const int max_hits_count = 8;
	bool got_hit = true;
	double next_x_pos, next_y_pos;
	double next_step;

	if (Ball_State == EBall_State::On_Platform || Ball_State == EBall_State::Disabled || Ball_State == EBall_State::Lost || Ball_State == EBall_State::Teleporting)
		return;

	next_step = Ball_Speed / max_speed * AsConfig::Moving_Step_Size;

	while (got_hit)
	{
		got_hit = false;
		next_x_pos = Center_X_Pos + next_step * cos(Ball_Direction);
		next_y_pos = Center_Y_Pos - next_step * sin(Ball_Direction);

		// Correcting the position when reflecting:
		got_hit = Hit_Checker_List.Check_Hit(next_x_pos, next_y_pos, this);

		// Correcting the position when reflected from the platform
		if (got_hit)
		{
			++prev_hit_count;

			if (prev_hit_count >= max_hits_count)
			{
				Ball_Direction += AsConfig::Min_Ball_Angle;
				prev_hit_count = 0;
			}

			if (Detect_Hits_Cycling())
				Set_Direction(Ball_Direction += AsConfig::Min_Ball_Angle);
		}
		else
		{
			// the ball will continue to move if it does not collide with other objects
			Center_X_Pos = next_x_pos;
			Center_Y_Pos = next_y_pos;

			if (Testing_Is_Active)
				Rest_Test_Distance -= next_step;
		}

		if (Ball_State == EBall_State::On_Platform)
			break;
	}
}
//------------------------------------------------------------------------------------------------------------
double ABall::Get_Speed()
{
	return Ball_Speed;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Act()
{
	// Not used, because the ball itself does nothing (no animation)
}
//------------------------------------------------------------------------------------------------------------
void ABall::Clear(HDC hdc, RECT& paint_area)
{
	RECT intersection_rect;

	if (Ball_State == EBall_State::Disabled && Previous_Ball_State != EBall_State::Lost)
		return;	

	if ((Ball_State == EBall_State::Teleporting || Ball_State == EBall_State::Lost) && Ball_State == Previous_Ball_State)
		return;

	// 0. Clearing background
	if (Ball_State == EBall_State::On_Parachute)
		Clear_Parachute(hdc);

	// 1. clearing the background
	if (IntersectRect(&intersection_rect, &paint_area, &Prev_Ball_Rect))
		AsTools::Ellipse(hdc, Prev_Ball_Rect, AsConfig::BG_Color);
}
//------------------------------------------------------------------------------------------------------------
void ABall::Draw(HDC hdc, RECT& paint_area)
{
	RECT intersection_rect;

	if (Ball_State == EBall_State::Disabled)
		return;

	if ((Ball_State == EBall_State::Teleporting || Ball_State == EBall_State::Lost) && Ball_State == Previous_Ball_State)
		return;

	switch (Ball_State)
	{
	case EBall_State::On_Parachute:
		Draw_Parachute(hdc, paint_area);
		break;

	case EBall_State::Off_Parachute:
		Clear_Parachute(hdc);
		Set_State(EBall_State::Normal, Center_X_Pos, Center_Y_Pos);
		break;

	case EBall_State::Lost:
		if (Previous_Ball_State == EBall_State::On_Parachute)
			Clear_Parachute(hdc);
		return;

	case EBall_State::Teleporting:
		return;
	}

	if (Ball_State == EBall_State::Lost)
		return;

	// 2. Drawing the ball
	if (IntersectRect(&intersection_rect, &paint_area, &Ball_Rect))
		AsTools::Ellipse(hdc, Ball_Rect, AsConfig::White_Color);
}
//------------------------------------------------------------------------------------------------------------
bool ABall::Is_Finished()
{	// Not used, because the ball itself does nothing (no animation)

	return false;
}
//------------------------------------------------------------------------------------------------------------
double ABall::Get_Direction()
{
	return Ball_Direction;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Set_Direction(double new_direction)
{
	const double pi_2 = 2.0 * M_PI;
	const double min_angle = AsConfig::Min_Ball_Angle;

	while (new_direction > pi_2)
		new_direction -= pi_2;

	while (new_direction < 0.0)
		new_direction += pi_2;


	// 2. Correcting the angle of the ball when approaching the horizontal closer than min_angle

	// 2.1 Left side
	// 2.1.1 Top
	if (new_direction < min_angle)
		new_direction = min_angle;

	// 2.1.2 Bottom
	if (new_direction > pi_2 - min_angle)
		new_direction = pi_2 - min_angle;

	// 2.2 Right side
	// 2.2.1 Top
	if (new_direction > M_PI - min_angle && new_direction < M_PI)
		new_direction = M_PI - min_angle;

	// 2.2.2 Bottom
	if (new_direction >= M_PI && new_direction < M_PI + min_angle)
		new_direction = M_PI + min_angle;	
	
	
	// 3. Correcting the angle of the ball when approaching the vertical closer than min_angle

	// 3.1 from top
	// 3.1.1 from right side
	if (new_direction > M_PI_2 - min_angle && new_direction < M_PI_2)
		new_direction = M_PI_2 - min_angle;

	// 3.1.2 from left side
	if (new_direction > M_PI_2 && new_direction < M_PI_2 + min_angle)
		new_direction = M_PI_2 + min_angle;

	// 3.2 from bottom
	// 3.2.1 from right side
	if (new_direction > M_PI + M_PI_2 - min_angle && new_direction < M_PI + M_PI_2)
		new_direction = M_PI + M_PI_2 - min_angle;

	// 3.2.2 from left side
	if (new_direction >= M_PI + M_PI_2 && new_direction < M_PI + M_PI_2 + min_angle)
		new_direction = M_PI + M_PI_2 + min_angle;

	Ball_Direction = new_direction;
}
//------------------------------------------------------------------------------------------------------------
EBall_State ABall::Get_State()
{
	return Ball_State;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Set_State(EBall_State new_state, double x_pos, double y_pos)
{
	switch (new_state)
	{
	case EBall_State::Disabled:
		Ball_Speed = 0.0;
		break;


	case	EBall_State::Normal:
		Center_X_Pos = x_pos;
		Center_Y_Pos = y_pos;
		Ball_Speed = AsConfig::Normal_Ball_Speed;
		Ball_Direction = M_PI_4;
		Redraw_Ball();
		break;


	case	EBall_State::Lost:
		if (!(Ball_State == EBall_State::Normal || Ball_State == EBall_State::On_Parachute))
			AsConfig::Throw(); // Only these conditions can lead to the loss of the ball!

		Redraw_Ball();
		Redraw_Parachute();
		Ball_Speed = 0.0;
		break;


	case	EBall_State::On_Platform:
		Center_X_Pos = x_pos;
		Center_Y_Pos = y_pos;
		Ball_State = EBall_State::Normal;
		Prev_Ball_Speed = Ball_Speed;
		Ball_Speed = 0.0;
		Prev_Ball_Direction = Ball_Direction;
		Release_Timer_Tick = AsConfig::Current_Timer_Tick + On_Platform_Timeout;
		Redraw_Ball();
		break;


	case	EBall_State::On_Parachute:
		AsConfig::Throw(); // To put on a parachute, you need to call the special method Set_On_Parachute ()
		break;

	case	EBall_State::Off_Parachute:
		if (Ball_State != EBall_State::On_Parachute)
			AsConfig::Throw(); // This state can only be entered from EBall_State::On_Parachute

		Ball_Speed = 0.0;
		Redraw_Ball();
		Redraw_Parachute();
		break;


	case EBall_State::Teleporting:
		if (!(Ball_State == EBall_State::Normal || Ball_State == EBall_State::On_Parachute || Ball_State == EBall_State::Teleporting))
			AsConfig::Throw(); // Only from these states you can enter the teleport!

		Center_X_Pos = x_pos;
		Center_Y_Pos = y_pos;
		Ball_Speed = 0.0;
		Redraw_Ball();

		if (Ball_State == EBall_State::On_Parachute)
			Redraw_Parachute();
		break;


	default:
		AsConfig::Throw();
	}

	Previous_Ball_State = Ball_State;
	Ball_State = new_state;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Reflect(bool from_horizontal)
{
	if (from_horizontal)
		Set_Direction(-Ball_Direction);
	else
		Set_Direction(M_PI - Ball_Direction);
}
//------------------------------------------------------------------------------------------------------------
void ABall::Get_Center(double& x_pos, double& y_pos)
{
	x_pos = Center_X_Pos;
	y_pos = Center_Y_Pos;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Draw_Teleporting(HDC hdc, int step)
{
	int top_y = Ball_Rect.top + step / 2;
	int low_y = Ball_Rect.bottom - step / 2 - 1;

	if (top_y >= low_y)
		return;

	AsConfig::White_Color.Select(hdc);
	Ellipse(hdc, Ball_Rect.left, top_y, Ball_Rect.right - 1, low_y);
}
//------------------------------------------------------------------------------------------------------------
void ABall::Set_On_Parachute(int brick_x, int brick_y)
{
	int scale = AsConfig::Global_Scale;
	int cell_x = AsConfig::Level_X_Offset + brick_x * AsConfig::Cell_Width;
	int cell_y = AsConfig::Level_Y_Offset + brick_y * AsConfig::Cell_Height;

	Ball_Direction = M_PI + M_PI_2;
	Ball_Speed = 1.0;
	Ball_State = EBall_State::On_Parachute;

	Parachute_Rect.left = cell_x * scale;
	Parachute_Rect.top = cell_y * scale;
	Parachute_Rect.right = Parachute_Rect.left + Parachute_Size * scale;
	Parachute_Rect.bottom = Parachute_Rect.top + Parachute_Size * scale;

	Previous_Parachute_Rect = Parachute_Rect;

	Center_X_Pos = (double)(cell_x + AsConfig::Cell_Width / 2) - 1.0 / AsConfig::D_Global_Scale;
	Center_Y_Pos = (double)(cell_y + Parachute_Size) - AsConfig::Ball_Radius * 2.0;

	Redraw_Parachute();
}
//------------------------------------------------------------------------------------------------------------
bool ABall::Is_Moving_Up()
{
	if (Ball_Direction >= 0.0 && Ball_Direction < M_PI)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
bool ABall::Is_Moving_Left()
{
	if (Ball_Direction >= M_PI_2 && Ball_Direction < M_PI + M_PI_2)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Set_Speed(double new_speed)
{
	Ball_Speed = new_speed;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Set_For_Test()
{
	Testing_Is_Active = true;
	Rest_Test_Distance = 50.0;

	Set_State(EBall_State::Normal, 130 + Test_Iteration, 90);
	Ball_Direction = M_PI_4;
	Ball_Speed = AsConfig::Normal_Ball_Speed;

	++Test_Iteration;
}
//------------------------------------------------------------------------------------------------------------
bool ABall::Is_Test_Finished()
{
	if (Testing_Is_Active)
	{
		if (Rest_Test_Distance <= 0.0)
		{
			Testing_Is_Active = false;
			Set_State(EBall_State::Lost);
			return true;
		}
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Forced_Advance(double direction, double speed, double max_speed)
{
	EBall_State prev_ball_state = Ball_State;
	double prev_direction = Ball_Direction;
	double prev_speed = Ball_Speed;

	Ball_State = EBall_State::Normal;
	Ball_Direction = direction;
	Ball_Speed = speed;

	Advance(max_speed);

	Ball_Direction = prev_direction;
	Ball_Speed = prev_speed;
	Ball_State = prev_ball_state;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Release()
{ // continue the interrupted movement of the ball
	Set_State(EBall_State::Normal, Center_X_Pos, Center_Y_Pos);
	Ball_Speed = Prev_Ball_Speed;

	if (Ball_Speed < AsConfig::Normal_Ball_Speed)
		Ball_Speed = AsConfig::Normal_Ball_Speed;

	Ball_Direction = Prev_Ball_Direction;
	Release_Timer_Tick = 0;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Redraw_Ball()
{
	Ball_Rect.left = (int)( (Center_X_Pos - AsConfig::Ball_Radius) * AsConfig::D_Global_Scale);
	Ball_Rect.top = (int)( (Center_Y_Pos - AsConfig::Ball_Radius) * AsConfig::D_Global_Scale);
	Ball_Rect.right = (int)( (Center_X_Pos + AsConfig::Ball_Radius) * AsConfig::D_Global_Scale);
	Ball_Rect.bottom = (int)( (Center_Y_Pos + AsConfig::Ball_Radius) * AsConfig::D_Global_Scale);

	AsTools::Invalidate_Rect(Prev_Ball_Rect);
	AsTools::Invalidate_Rect(Ball_Rect);
}
//------------------------------------------------------------------------------------------------------------
void ABall::Redraw_Parachute()
{
	AsTools::Invalidate_Rect(Previous_Parachute_Rect);
	AsTools::Invalidate_Rect(Parachute_Rect);
}
//------------------------------------------------------------------------------------------------------------
void ABall::Draw_Parachute(HDC hdc, RECT& paint_area)
{
	const int scale = AsConfig::Global_Scale;
	int dome_height = (Parachute_Rect.bottom - Parachute_Rect.top) / 2;
	int arc_height = 4 * scale;
	int arc_x;
	int line_y;
	int ball_center_x, ball_center_y;
	RECT intersection_rect, sub_arc, other_arc;

	if (!IntersectRect(&intersection_rect, &paint_area, &Parachute_Rect))
		return;

	// 1. The dome
	AsConfig::Blue_Color.Select(hdc);
	Chord(hdc, Parachute_Rect.left, Parachute_Rect.top, Parachute_Rect.right - 1, Parachute_Rect.bottom - 1,
		Parachute_Rect.right, Parachute_Rect.top + dome_height - 1, Parachute_Rect.left, Parachute_Rect.top + dome_height - 1);
	
	// 2. Dome arches
	AsConfig::BG_Color.Select(hdc);

	arc_x = Parachute_Rect.left + 1;

	// 2.1 Left
	sub_arc.left = arc_x;
	sub_arc.top = Parachute_Rect.top + dome_height - arc_height / 2;
	sub_arc.right = sub_arc.left + 3 * scale;
	sub_arc.bottom = sub_arc.top + 4 * scale;

	AsTools::Ellipse(hdc, sub_arc, AsConfig::BG_Color);

	// 2.2. Central
	other_arc = sub_arc;

	other_arc.left = arc_x + 3 * scale + 1;
	other_arc.right = arc_x + 11 * scale;

	AsTools::Ellipse(hdc, other_arc, AsConfig::BG_Color);

	// 2.3 Right
	other_arc = sub_arc;
	other_arc.left = arc_x + 11 * scale + 1;
	other_arc.right = arc_x + 14 * scale + 1;

	AsTools::Ellipse(hdc, sub_arc, AsConfig::BG_Color);

	// 3. Slings
	ball_center_x = (Parachute_Rect.left + Parachute_Rect.right) / 2;
	ball_center_y = Parachute_Rect.bottom - 2 * scale;
	line_y = Parachute_Rect.top + dome_height;

	AsConfig::White_Color.Select(hdc);

	MoveToEx(hdc, Parachute_Rect.left, line_y, 0);
	LineTo(hdc, ball_center_x, ball_center_y);

	MoveToEx(hdc, Parachute_Rect.left + 3 * scale + 1, line_y, 0);
	LineTo(hdc, ball_center_x, ball_center_y);

	MoveToEx(hdc, Parachute_Rect.right - 4 * scale + 1, line_y, 0);
	LineTo(hdc, ball_center_x, ball_center_y);

	MoveToEx(hdc, Parachute_Rect.right - 1, line_y - 1, 0);
	LineTo(hdc, ball_center_x, ball_center_y);
}
//------------------------------------------------------------------------------------------------------------
void ABall::Clear_Parachute(HDC hdc)
{ // Clearing background
	AsConfig::BG_Color.Select(hdc);
	AsTools::Round_Rect(hdc, Previous_Parachute_Rect);
}
//------------------------------------------------------------------------------------------------------------
bool ABall::Detect_Hits_Cycling()
{
	int i;
	int current_position;
	int matched_positions_count;
	int buffer_size = sizeof(Recent_Collisions) / sizeof(Recent_Collisions[0]);

	Recent_Collisions[Recent_Collisions_Position++].Set_As((int)Center_X_Pos, (int)Center_Y_Pos);

	if (Recent_Collisions_Position >= buffer_size)
		Recent_Collisions_Position = 0; // The position goes to the [0] element of the buffer

	++Recent_Hits_Count;

	if (Recent_Hits_Count < buffer_size)
		return false; // We have to fill the buffer_size before we can Detect_Hits_Cycling

	// Scanning the array with "windows" in search of a pattern. The "window" size is 3
	for (current_position = 3; current_position < buffer_size; current_position++)
	{
		for (i = 0; i < 3; i++)
		{
			matched_positions_count = 0;

			if (Recent_Collisions[i] == Recent_Collisions[current_position + i])
				matched_positions_count++;
		}

		if (matched_positions_count >= 3)
		{
			Recent_Hits_Count = 0; // We collect statistics again, not taking into account previous patterns
			return true;
		}
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------