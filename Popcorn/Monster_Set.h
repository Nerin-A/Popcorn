﻿#pragma once

#include "Border.h"
#include "Monster.h"

//------------------------------------------------------------------------------------------------------------
enum class EMonster_Set_State : unsigned char
{
	Idle,
	Selecting_Next_Gate,
	Waiting_For_Gate_To_Open,
	Waiting_For_Gate_To_Close,

};
//------------------------------------------------------------------------------------------------------------
class AsMonster_Set : public AHit_Checker, public AGame_Objects_Set
{
public:
	~AsMonster_Set();
	AsMonster_Set();

	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall_Object* ball);
	virtual bool Check_Hit(double next_x_pos, double next_y_pos);
	virtual bool Check_Hit(RECT& rect);

	virtual void Act();

	void Init(AsBorder* border);
	void Emit_At_Gate(int gate_index);
	void Activate(int max_monsters_alive);
	void Destroy_All();
	bool Are_All_Destroyed();
	void Set_Freeze_State(bool freeze);

private:
	bool Get_Next_Game_Object(int& index, AGame_Object** game_object);

	int Current_Gate_Index;
	int Max_Monsters_Alive;
	bool Is_Frozen;
	EMonster_Set_State Monster_Set_State;
	AsBorder *Border; // UNO

	std::vector<AMonster*> Monsters;
};
//------------------------------------------------------------------------------------------------------------