﻿#pragma once

#include "Platform_State.h"
#include "Ball_Set.h"

//------------------------------------------------------------------------------------------------------------
class AsPlatform_Glue
{
public:
	AsPlatform_Glue(AsPlatform_State& platform_state);

	bool Act(AsBall_Set* ball_set, EPlatform_State& next_state);
	void Draw(HDC hdc, double x_pos);
	void Reset();

private:
	void Draw_Glue_Spot(HDC hdc, int x_offset, int width, int height, double x_pos);

	double Glue_Spot_Height_Ratio;
	AsPlatform_State* Platform_State;

	static const double Max_Glue_Spot_Height_Ratio, Min_Glue_Spot_Height_Ratio, Glue_Spot_Height_Ratio_Step;
};
//------------------------------------------------------------------------------------------------------------