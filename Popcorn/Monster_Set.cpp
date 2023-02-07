#include "Monster_Set.h"

// AMonster
const double AMonster::Max_Cornea_Height = 11.0;
//------------------------------------------------------------------------------------------------------------
AMonster::AMonster()
	: Is_Active(false), X_Pos(0), Y_Pos(0), Cornea_Height(Max_Cornea_Height), Monster_Rect{}
{
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Begin_Movement()
{
	//!!! TODO
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Finish_Movement()
{
	//!!! TODO
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Advance(double max_speed)
{
	//!!! TODO
}
//------------------------------------------------------------------------------------------------------------
double AMonster::Get_Speed()
{
	//!!! TODO
	return 0.0;
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Act()
{
	//!!! TODO
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Clear(HDC hdc, RECT& paint_area)
{
	//!!! TODO
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Draw(HDC hdc, RECT& paint_area)
{
	const int scale = AsConfig::Global_Scale;
	const int d_scale = AsConfig::D_Global_Scale;
	const int half_scale = scale / 2;

	HRGN region;
	RECT intersection_rect, rect;

	if (!Is_Active)
		return;

	if (!IntersectRect(&intersection_rect, &paint_area, &Monster_Rect))
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

	// 2 Draw the eye itself
	// 2.1 Draw the cornea of the "eye" monster
	Cornea_Height = Max_Cornea_Height / 2.0;

	rect = Monster_Rect;
	rect.left += scale + half_scale;
	rect.top += 2 * scale + (int)((Max_Cornea_Height / 2.0 - Cornea_Height / 2.0) * d_scale);
	rect.right -= scale + half_scale;
	rect.bottom = Cornea_Height * scale + rect.top;

	AsTools::Ellipse(hdc, rect, AsConfig::Eye_Monster_Cornea_Color);

	// 2.2 Draw the iris of the "eye" monster
	rect = Monster_Rect;
	rect.left += 4 * scale + half_scale;
	rect.top += 4 * scale;
	rect.right -= 4 * scale + half_scale;
	rect.bottom -= 5 * scale;

	AsTools::Ellipse(hdc, rect, AsConfig::Eye_Monster_Iris_Color);

	// 2.3 Draw the pupil of the "eye" monster
	rect = Monster_Rect;
	rect.left += 7 * scale;
	rect.top += 6 * scale;
	rect.right = rect.left + 2 * scale;
	rect.bottom = rect.top + 3 * scale;

	AsTools::Ellipse(hdc, rect, AsConfig::BG_Color);


	SelectClipRgn(hdc, 0);
	DeleteObject(region);
}
//------------------------------------------------------------------------------------------------------------
bool AMonster::Is_Finished()
{
	//!!! TODO
	return false;
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Activate(int x_pos, int y_pos)
{
	const int scale = AsConfig::Global_Scale;

	Is_Active = true;

	X_Pos = x_pos;
	Y_Pos = y_pos;

	Monster_Rect.left = X_Pos * scale;
	Monster_Rect.top = Y_Pos * scale;
	Monster_Rect.right = Monster_Rect.left + Width * scale;
	Monster_Rect.bottom = Monster_Rect.top + Height * scale;
}
//------------------------------------------------------------------------------------------------------------




// AsMonster_Set
//------------------------------------------------------------------------------------------------------------
AsMonster_Set::AsMonster_Set()
	: Border(nullptr)
{
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Init(AsBorder* border)
{
	Border = border;
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Emit_At_Gate(int gate_index)
{
	int i;
	int gate_x, gate_y;

	AMonster* monster = 0;

	if (gate_index < 0 || gate_index >= AsConfig::Gates_Count)
		AsConfig::Throw();

	for (i = 0; i < Max_Monsters_Count; i++)
	{
		if (! Monsters[i].Is_Active)
		{
			monster = &Monsters[i];
			break;
		}
	}

	if (monster == 0)
		return; // There are no unused monsters.

	Border->Get_Gate_Pos(gate_index, gate_x, gate_y);

	monster->Activate(gate_x, gate_y);
}
//------------------------------------------------------------------------------------------------------------
bool AsMonster_Set::Get_Next_GameObject(int& index, AGame_Object** game_object)
{
	if (index < 0 || index >= AsConfig::Max_Balls_Count)
		return false;

	*game_object = &Monsters[index++];

	return true;
}
//------------------------------------------------------------------------------------------------------------