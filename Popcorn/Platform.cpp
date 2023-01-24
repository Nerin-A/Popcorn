﻿#include "Platform.h"

// AsPlatform
const double AsPlatform::Max_Glue_Spot_Height_Ratio = 1.0;
const double AsPlatform::Min_Glue_Spot_Height_Ratio = 0.4;
const double AsPlatform::Glue_Spot_Ratio_Step = 0.05;
//------------------------------------------------------------------------------------------------------------
AsPlatform::~AsPlatform()
{
	delete[] Normal_Platform_Image;
}
//------------------------------------------------------------------------------------------------------------
AsPlatform::AsPlatform()
: X_Pos(AsConfig::Border_X_Offset), Platform_State(EPS_Missing), Platform_Substate_Meltdown (EPSM_Unknown), Platform_Substate_RollIng (EPSR_Unknown), Platform_Substate_Glue (EPSG_Unknown), Platform_Moving_State(EPMS_Stop),
  Right_Key_Down (false),Left_Key_Down (false), Inner_Width(Normal_Platform_Inner_Width),Rolling_Step (0), Speed (0.0), Glue_Spot_Height_Ratio (0.0), Ball_Set(0), 
  Normal_Platform_Image_Width(0), Normal_Platform_Image_Height(0),Normal_Platform_Image(0), Width(Normal_Width), Platform_Rect{}, Prev_Platform_Rect{},
  Highlight_Color(255, 255, 255), Platform_Circle_Color(151, 0, 0), Platform_Inner_Color(0, 128, 192)
{
	X_Pos = (AsConfig::Max_X_Pos - Width) / 2;
}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Check_Hit(double next_x_pos, double next_y_pos, ABall* ball)
{
	double iiner_left_x, inner_right_x;
	double inner_top_y, inner_low_y;
	double inner_y;
	double reflection_pos;
	double ball_x, ball_y;

	if (next_y_pos + ball->Radius < AsConfig::Platform_Y_Pos)
		return false;

	inner_top_y = (double)(AsConfig::Platform_Y_Pos + 1);
	inner_low_y = (double)(AsConfig::Platform_Y_Pos + Height - 1);
	iiner_left_x = (double)(X_Pos + Circle_Size - 1);
	inner_right_x = (double)(X_Pos + Width - (Circle_Size - 1) );

	// 1. Checking the reflection from the side balls
	if (Reflect_On_Circle(next_x_pos, next_y_pos, 0.0, ball) )
		goto _on_hit; // From the left ball

	if (Reflect_On_Circle(next_x_pos, next_y_pos, Width - Circle_Size, ball) )
		goto _on_hit; // From  the right ball


	// Checking the reflection from the central part of the platform
	if (ball->Is_Moving_Up() )
		inner_y = inner_low_y; // From the bottom edge
	else
		inner_y = inner_top_y; // From the top edge

	if (Hit_Circle_On_Line(next_y_pos - inner_y, next_x_pos, iiner_left_x, inner_right_x, ball->Radius, reflection_pos) )
	{
		ball->Reflect(true);
		goto _on_hit;
	}
	return false;

_on_hit:
	if (ball->Get_State() == EBS_On_Parachute)
		ball->Set_State(EBS_Off_Parachute);

	if (Platform_State == EPS_Glue && Platform_Substate_Glue == EPSG_Active)
	{
		ball->Get_Center(ball_x, ball_y);
		ball->Set_State(EBS_On_Platform, ball_x, ball_y);
	}
	return true;
}
 //------------------------------------------------------------------------------------------------------------
void AsPlatform::Begin_Movement()
{

}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Finish_Movement()
{
	if (Platform_Moving_State == EPMS_Stop)
		return;

	Redraw_Platform();

	if (Platform_Moving_State == EPMS_Stopping)
		Platform_Moving_State = EPMS_Stop;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Advance(double max_speed)
{
	double max_platform_x, next_step;

	if (Platform_Moving_State == EPMS_Stopping || Platform_Moving_State == EPMS_Stop)
		return;

	max_platform_x = AsConfig::Max_X_Pos - Width + 1;
	next_step = Speed / max_speed * AsConfig::Moving_step_size;

	X_Pos += next_step;

	if (X_Pos <= AsConfig::Border_X_Offset)
		{
			X_Pos = AsConfig::Border_X_Offset;
			Speed = 0.0;
			Platform_Moving_State = EPMS_Stopping;
		}

	if (X_Pos >= max_platform_x)
		{
			X_Pos = max_platform_x;
			Speed = 0.0;
			Platform_Moving_State = EPMS_Stopping;
		}

	// move glued balls
	if (Platform_State == EPS_Glue || (Platform_State == EPS_Ready && Platform_Substate_Glue == EPSG_Active) )
	{
		if (Platform_Moving_State == EPMS_Moving_Left)
			Ball_Set->On_Platform_Advance(M_PI, fabs(Speed), max_speed);
		else
			if (Platform_Moving_State == EPMS_Moving_Right)
				Ball_Set->On_Platform_Advance(0.0, fabs(Speed), max_speed);
	}
}
//------------------------------------------------------------------------------------------------------------
double AsPlatform::Get_Speed()
{
	return Speed;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Act()
{
	switch (Platform_State)
	{
	case EPS_Meltdown:
		Act_For_Meltdown_State();
		break;

	case EPS_Rolling:
		Act_For_Rolling_State();
		break;

	case EPS_Glue:
		Act_For_Glue_State();
		break;

	default:
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Clear(HDC hdc, RECT & paint_area)
{
	RECT intersection_rect;

	if (! IntersectRect(&intersection_rect, &paint_area, &Prev_Platform_Rect))
		return;
	switch (Platform_State)
	{
	case EPS_Ready:
	case EPS_Normal:
	case EPS_Rolling:
	case EPS_Glue:

		// Clearing the old place with the background color
		AsConfig::BG_Color.Select(hdc);
		Rectangle(hdc, Prev_Platform_Rect.left, Prev_Platform_Rect.top, Prev_Platform_Rect.right, Prev_Platform_Rect.bottom);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw(HDC hdc, RECT& paint_area)
{// Drawing the platform

	RECT intersection_rect;

	if (!IntersectRect(&intersection_rect, &paint_area, &Platform_Rect))
		return;

	switch (Platform_State)
	{
	case EPS_Ready:
	case EPS_Normal:
		Draw_Normal_State(hdc, paint_area);
		break;

	case EPS_Meltdown:
		if (Platform_Substate_Meltdown == EPSM_Active)
			Draw_Meltdown_State(hdc, paint_area);
		 break;

	case EPS_Rolling:
		Draw_Rolling_State(hdc, paint_area);
		break;

	case EPS_Glue:
		Draw_Glue_State(hdc, paint_area);
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Is_Finished()
{
	return false; // Not used
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Init(AsBall_Set* ball_set)
{
	Ball_Set = ball_set;
}
//------------------------------------------------------------------------------------------------------------
EPlatform_State AsPlatform::Get_State()
{
	return Platform_State;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Set_State(EPlatform_State new_state)
{
	int i, len;

	if (Platform_State == new_state)
		return;

	switch (new_state)
	{
	case EPS_Normal:
		if (Platform_State == EPS_Glue)
		{
			Platform_Substate_Glue = EPSG_Finalize;

			while (Ball_Set->Release_Next_Ball())
			{
			}
			return;
		}
		break;

	case EPS_Meltdown:
		Speed = 0.0;
		Platform_Substate_Meltdown = EPSM_Init;

		len = sizeof(Meltdown_Platform_Y_Pos) / sizeof(Meltdown_Platform_Y_Pos[0]);

		for (i = 0; i < len; i++)
			Meltdown_Platform_Y_Pos[i] = Platform_Rect.top;
		break;

	case EPS_Rolling:
		Platform_Substate_RollIng = EPSR_Roll_In;

		X_Pos = AsConfig::Max_X_Pos - 1;
		Rolling_Step = Max_Rolling_Step - 1;
		break;

	case EPS_Glue:
		if (Platform_Substate_Glue == EPSG_Finalize)
			return;
		else
		{
			Platform_Substate_Glue = EPSG_Init;
				
			Glue_Spot_Height_Ratio = Min_Glue_Spot_Height_Ratio;
		}
		break;
	}
		Platform_State = new_state;
}

//------------------------------------------------------------------------------------------------------------
void AsPlatform::Redraw_Platform(bool update_rect)
{
	int platform_width;
	if (update_rect)
	{
		Prev_Platform_Rect = Platform_Rect;

		if (Platform_State == EPS_Rolling && Platform_Substate_RollIng == EPSR_Roll_In)
			platform_width = Circle_Size;
		else
			platform_width = Width;



		Platform_Rect.left = (int)(X_Pos * AsConfig::D_Global_Scale);
		Platform_Rect.top = AsConfig::Platform_Y_Pos * AsConfig::Global_Scale;
		Platform_Rect.right = Platform_Rect.left + platform_width * AsConfig::Global_Scale;
		Platform_Rect.bottom = Platform_Rect.top + Height * AsConfig::Global_Scale;

		if (Platform_State == EPS_Meltdown)
			Prev_Platform_Rect.bottom = (AsConfig::Max_Y_Pos + 1) * AsConfig::Global_Scale;
	}
	AsConfig::Invalidate_Rect(Prev_Platform_Rect);
	AsConfig::Invalidate_Rect(Platform_Rect);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Move(bool to_left, bool key_down)
{
	if (! (Platform_State == EPS_Normal || Platform_State == EPS_Glue) )
		return;

	if (to_left)
		Left_Key_Down = key_down;
	else
		Right_Key_Down = key_down;

	if (Left_Key_Down && Right_Key_Down)
		return; // ignore simultaneous pressing of two keys

	if (! Left_Key_Down && ! Right_Key_Down)
	{
		Speed = 0.0;
		Platform_Moving_State = EPMS_Stopping;
		return;
	}

	if (Left_Key_Down)
	{
		Platform_Moving_State = EPMS_Moving_Left;
		Speed = -X_Step;
	}

	if (Right_Key_Down)
	{
		Platform_Moving_State = EPMS_Moving_Right;
		Speed = X_Step;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::On_Space_Key(bool key_down)
{
	if (!key_down)
		return;
	switch (Get_State())
	{
	case EPS_Ready:
		Ball_Set->Release_From_The_Platform(Get_Middle_Pos() );
		Set_State(EPS_Normal);
		break;

	case EPS_Glue:
		Ball_Set->Release_Next_Ball();
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Hit_By(AFalling_Letter* falling_letter)
{
	RECT intersection_rect, falling_letter_rect;

	falling_letter->Get_Letter_Cell(falling_letter_rect);

	if (IntersectRect(&intersection_rect, &falling_letter_rect, &Platform_Rect) )
		return true;
	else
		return false;

}
//------------------------------------------------------------------------------------------------------------
double AsPlatform::Get_Middle_Pos()
{
	return X_Pos + (double)Width / 2.0;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Act_For_Meltdown_State()
{
	switch (Platform_Substate_Meltdown)
	{
	case EPSM_Init:
		Platform_Substate_Meltdown = EPSM_Active;
		break;

	case EPSM_Active:
		Redraw_Platform();
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Act_For_Rolling_State()
{
	Redraw_Platform();
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Act_For_Glue_State()
{
	switch (Platform_Substate_Glue)
	{
	case EPSG_Init:
		if (Glue_Spot_Height_Ratio < Max_Glue_Spot_Height_Ratio)
			Glue_Spot_Height_Ratio += 0.02;
		else
			Platform_Substate_Glue = EPSG_Active;

		Redraw_Platform(false);
		break;

	case EPSG_Finalize:
		if (Glue_Spot_Height_Ratio > Min_Glue_Spot_Height_Ratio)
			Glue_Spot_Height_Ratio -= Glue_Spot_Ratio_Step;
		else
		{
			Platform_State = EPS_Normal;
			Platform_Substate_Glue = EPSG_Unknown;
		}

		Redraw_Platform(false);
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Circle_Highlight(HDC hdc, int x, int y)
{// Drawing hightlight on the ball
	Highlight_Color.Select_Pen(hdc);

	Arc(hdc, x + AsConfig::Global_Scale, y + AsConfig::Global_Scale, x + (Circle_Size - 1) * AsConfig::Global_Scale - 1, y + (Circle_Size - 1) * AsConfig::Global_Scale - 1,
		x + 2 * AsConfig::Global_Scale, y + AsConfig::Global_Scale, x + AsConfig::Global_Scale, y + 3 * AsConfig::Global_Scale);

}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Normal_State(HDC hdc, RECT &paint_area)
{// Draw the platform in the normal state

	double x = X_Pos;
	int y = AsConfig::Platform_Y_Pos;
	const int scale = AsConfig::Global_Scale;
	const double d_scale = AsConfig::D_Global_Scale;
	RECT inner_rect, rect;

	// 1. Draw side balls
	Platform_Circle_Color.Select(hdc);

	rect.left = (int)(x * d_scale);
	rect.top = y * scale;
	rect.right = (int)((x + (double)Circle_Size) * d_scale);
	rect.bottom = (y + Circle_Size) * scale;

	Ellipse(hdc, rect.left, rect.top, rect.right - 1.0, rect.bottom - 1);

	rect.left = (int)((x + Inner_Width) * d_scale);
	rect.top = y * scale;
	rect.right = (int)((x + (double)Circle_Size + Inner_Width) * d_scale);
	rect.bottom = (y + Circle_Size) * scale;
	
	Ellipse(hdc, rect.left, rect.top, rect.right - 1.0, rect.bottom - 1);

	// 2. Draw the highlight
	Draw_Circle_Highlight(hdc, (int)(x * d_scale), y * scale);
	
	// 3. Draw the middle part
	Platform_Inner_Color.Select(hdc);
	
	inner_rect.left = (int)((x + 4) * d_scale);
	inner_rect.top = (y + 1) * scale;
	inner_rect.right = (int)((x + 4 + Inner_Width - 1) * d_scale);
	inner_rect.bottom = (y + 1 + 5) * scale;

	AsConfig::Round_Rect(hdc, inner_rect, 3);

	if (Normal_Platform_Image == 0 && Platform_State == EPS_Ready)
		Get_Normal_Platform_Image(hdc);

}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Get_Normal_Platform_Image(HDC hdc)
{
	int i, j;
	int offset = 0;
	int x = (int)(X_Pos * AsConfig::D_Global_Scale);
	int y = AsConfig::Platform_Y_Pos * AsConfig::Global_Scale;

	Normal_Platform_Image_Width = Width * AsConfig::Global_Scale;
	Normal_Platform_Image_Height = Height * AsConfig::Global_Scale;

	Normal_Platform_Image = new int[Normal_Platform_Image_Width * Normal_Platform_Image_Height];

	for (i = 0; i < Normal_Platform_Image_Height; i++)
		for (j = 0; j < Normal_Platform_Image_Width; j++)
			Normal_Platform_Image[offset++] = GetPixel(hdc, x + j, y + i);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Meltdown_State(HDC hdc, RECT &paint_area)
{// Drawing a platform in a molten state

	int i, j;
	int x, y;
	int y_offset;
	int stroke_len;
	int moved_columns_count = 0;
	int max_platform_y;
	const AColor* color;

	max_platform_y = (AsConfig::Max_Y_Pos + 1) * AsConfig::Global_Scale;

	for (i = 0; i < Normal_Platform_Image_Width; i++)
	{
		if (Meltdown_Platform_Y_Pos[i] > max_platform_y)
			continue;
		
		++moved_columns_count;

		y_offset = AsConfig::Rand(Meltdown_Speed) + 1;
		x = Platform_Rect.left + i;

		j = 0;
		y = Meltdown_Platform_Y_Pos[i];

		MoveToEx(hdc, x, y, 0);

		// Draw a sequence of vertical strokes of different colors (according to the prototype saved in Normal_Platform_Image)
		while (Get_Platform_Image_Stroke_Color(i, j, &color, stroke_len) )
		{
			color->Select_Pen(hdc);
			LineTo(hdc, x, y + stroke_len);

			y += stroke_len;
			j += stroke_len;
		}
		
		// Erase the background pixels above the stroke
		y = Meltdown_Platform_Y_Pos[i];
		MoveToEx(hdc, x, y, 0);
		AsConfig::BG_Color.Select_Pen(hdc);
		LineTo(hdc, x, y + y_offset);


		Meltdown_Platform_Y_Pos[i] += y_offset;
	}
	if (moved_columns_count == 0)
		Platform_State = EPS_Missing; // the whole platform is moved outside the window
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Rolling_State(HDC hdc, RECT& paint_area)
{ //draw a rolling out and expanding platform

	switch (Platform_Substate_RollIng)
	{
	case EPSR_Roll_In:
		Draw_Roll_In_State(hdc, paint_area);
		break;

	case EPSR_Expand_Roll_In:
		Draw_Expanding_Roll_In_State(hdc, paint_area);
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Roll_In_State(HDC hdc, RECT & paint_area)
{// Draw a rolling out platform

	int x = (int)(X_Pos * AsConfig::D_Global_Scale);
	int y = AsConfig::Platform_Y_Pos * AsConfig::Global_Scale;
	int roller_size = Circle_Size * AsConfig::Global_Scale;
	double alpha;
	XFORM xform, old_xform;

	// 1. The ball
	Platform_Circle_Color.Select(hdc);

	Ellipse(hdc, x, y , x + roller_size - 1, y + roller_size - 1);

	// 2. The dividing line
	alpha = -2.0 * M_PI / (double)Max_Rolling_Step * (double)Rolling_Step;

	xform.eM11 = (float)cos(alpha);
	xform.eM12 = (float)sin(alpha);
	xform.eM21 = (float)-sin(alpha);
	xform.eM22 = (float)cos(alpha);
	xform.eDx =  (float)(x + roller_size /2);
	xform.eDy =  (float)(y + roller_size / 2);
	GetWorldTransform(hdc, &old_xform);
	SetWorldTransform(hdc, &xform);

	AsConfig::BG_Color.Select(hdc);

	Rectangle(hdc, - AsConfig::Global_Scale / 2, -roller_size / 2, AsConfig::Global_Scale / 2, roller_size / 2);

	SetWorldTransform(hdc, &old_xform);

	// 3. Highlight
	Draw_Circle_Highlight(hdc, x, y);

	++Rolling_Step;

	if (Rolling_Step >= Max_Rolling_Step)
		Rolling_Step -= Max_Rolling_Step;

	X_Pos -= Rolling_Platform_Speed;

	if (X_Pos <= Roll_In_Platform_End_X_Pos)
	{
		X_Pos += Rolling_Platform_Speed;
		Platform_Substate_RollIng = EPSR_Expand_Roll_In;
		Inner_Width = 1;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Expanding_Roll_In_State(HDC hdc, RECT& paint_area)
{// Drawing an expanding platform

	Draw_Normal_State(hdc, paint_area);

	--X_Pos;
	Inner_Width += 2;

	if (Inner_Width >= Normal_Platform_Inner_Width)
	{
		Inner_Width = Normal_Platform_Inner_Width;
		Platform_State = EPS_Ready;
		Redraw_Platform();
	}
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Glue_State(HDC hdc, RECT& paint_area)
{// draw a platform with spreading glue

	HRGN region;
	RECT glue_rect;

	Draw_Normal_State(hdc, paint_area);

	glue_rect.left = (int)((X_Pos + 5.0) * AsConfig::D_Global_Scale);
	glue_rect.top = (AsConfig::Platform_Y_Pos + 1) * AsConfig::Global_Scale;
	glue_rect.right = glue_rect.left + Normal_Platform_Inner_Width * AsConfig::Global_Scale;
	glue_rect.bottom = glue_rect.top + (Height - 2) * AsConfig::D_Global_Scale;

	region = CreateRectRgnIndirect(&glue_rect);
	SelectClipRgn(hdc, region);

	AsConfig::BG_Color.Select(hdc);
	Draw_Glue_Spot(hdc, 0, 13, 5);
	Draw_Glue_Spot(hdc, 6, 6, 5);
	Draw_Glue_Spot(hdc, 9, 9, 6);

	AsConfig::White_Color.Select(hdc);
	Draw_Glue_Spot(hdc, 0, 9, 4);
	Draw_Glue_Spot(hdc, 6, 6, 4);
	Draw_Glue_Spot(hdc, 9, 9, 5);

	SelectClipRgn(hdc, 0);
	DeleteObject(region);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Glue_Spot(HDC hdc, int x_offset, int width, int height)
{// draw a spot of glue on the platform

	RECT spot_rect;
	int platform_top = (AsConfig::Platform_Y_Pos + 1) * AsConfig::Global_Scale;
	int spot_height = (int) ((double) height * AsConfig::D_Global_Scale * Glue_Spot_Height_Ratio);

	// draw a spot of glue
	spot_rect.left = (int)((X_Pos + 5.0 + (double) x_offset) * AsConfig::D_Global_Scale);
	spot_rect.top = platform_top - spot_height;
	spot_rect.right = spot_rect.left + width * AsConfig::Global_Scale;
	spot_rect.bottom = platform_top + spot_height - AsConfig::Global_Scale;

	Chord(hdc, spot_rect.left, spot_rect.top, spot_rect.right - 1, spot_rect.bottom - 1, spot_rect.left, platform_top - 1, spot_rect.right - 1, platform_top - 1);

}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Reflect_On_Circle(double next_x_pos, double next_y_pos, double platform_ball_x_offset, ABall* ball)
{
	double dx, dy;
	double platform_ball_x, platform_ball_y, platform_ball_radius;
	double distance, two_radiuses;
	double alpha, beta, gamma;
	double related_ball_direction;

	const double pi_2 = 2.0 * M_PI;

	platform_ball_radius = (double)Circle_Size / 2.0;
	platform_ball_x = (double)X_Pos + platform_ball_radius + platform_ball_x_offset;
	platform_ball_y = (double)AsConfig::Platform_Y_Pos + platform_ball_radius;

	dx = next_x_pos - platform_ball_x;
	dy = next_y_pos - platform_ball_y;

	distance = sqrt(dx * dx + dy * dy);
	two_radiuses = platform_ball_radius + ball->Radius;

	if (distance + AsConfig::Moving_step_size < two_radiuses)
	{// The ball touched the side ball

		beta = atan2(-dy, dx);

		related_ball_direction = ball->Get_Direction();
		related_ball_direction -= beta;

		if (related_ball_direction > pi_2)
			related_ball_direction -= pi_2;

		if (related_ball_direction < 0.0)
			related_ball_direction += pi_2;

		if (related_ball_direction > M_PI_2 && related_ball_direction < M_PI + M_PI_2)
		{
			alpha = beta + M_PI - ball->Get_Direction();
			gamma = beta + alpha;

			ball->Set_Direction(gamma);
			return true;
		}
	}
	return false;
}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Get_Platform_Image_Stroke_Color(int x, int y, const AColor** color, int& stroke_len)
{// Calculate the length of the next vertical stroke
	int i;
	int offset = y * Normal_Platform_Image_Width + x; // Position in the Normal_Platform_Image array corresponding to the offset (x, y)
	int color_value;

	stroke_len = 0;

	if (y >= Normal_Platform_Image_Height)
		return false;

	for (i = y; i < Normal_Platform_Image_Height; i++)
	{
		if (i == y)
		{
			color_value = Normal_Platform_Image[offset];
			stroke_len = 1;
		}
		else
		{
			if (color_value == Normal_Platform_Image[offset])
				++stroke_len;
			else
				break;
		}
		offset += Normal_Platform_Image_Width; // Go to line below
	}


	if (color_value == Highlight_Color.Get_RGB() )
		*color = & Highlight_Color;
	else if (color_value == Platform_Circle_Color.Get_RGB() )
		*color = &Platform_Circle_Color;
	else if (color_value == Platform_Inner_Color.Get_RGB() )
		*color = &Platform_Inner_Color;
	else if (color_value == AsConfig::BG_Color.Get_RGB() )
		*color = &AsConfig::BG_Color;
	else
		AsConfig::Throw();

	return true;
}
//------------------------------------------------------------------------------------------------------------
