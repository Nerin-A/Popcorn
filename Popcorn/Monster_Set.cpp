#include "Monster_Set.h"

// AsMonster_Set
//------------------------------------------------------------------------------------------------------------
AsMonster_Set::AsMonster_Set()
	: Monster_Set_State(EMonster_Set_State::Idle), Border(nullptr), Current_Gate_Index(-1), Max_Monsters_Alive(0)
{
}
//------------------------------------------------------------------------------------------------------------
bool AsMonster_Set::Check_Hit(double next_x_pos, double next_y_pos, ABall_Object* ball)
{
	int i;

	for (i = 0; i < Max_Monsters_Count; i++)
		if (Monsters[i]->Check_Hit(next_x_pos, next_y_pos, ball) )
			return true;

	return false;
}
//------------------------------------------------------------------------------------------------------------
bool AsMonster_Set::Check_Hit(double next_x_pos, double next_y_pos)
{
	int i;

	for (i = 0; i < Max_Monsters_Count; i++)
		if (Monsters[i]->Check_Hit(next_x_pos, next_y_pos) )
			return true;

	return false;
}
//------------------------------------------------------------------------------------------------------------
bool AsMonster_Set::Check_Hit(RECT& rect)
{
	int i;

	for (i = 0; i < Max_Monsters_Count; i++)
		if (Monsters[i]->Check_Hit(rect))
			return true;

	return false;
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Act()
{
	int current_monsters_alive_count;
	int i;
	switch (Monster_Set_State)
	{
	case EMonster_Set_State::Idle:
		break;


	case EMonster_Set_State::Selecting_Next_Gate:
		// We count living monsters
		current_monsters_alive_count = 0;

		for (i = 0; i < Max_Monsters_Count; i++)
			if (Monsters[i]->Is_Active())
				++current_monsters_alive_count;

		// Add a monster if possible
		if (current_monsters_alive_count < Max_Monsters_Alive)
		{
			Current_Gate_Index = Border->Long_Open_Gate();
			Monster_Set_State = EMonster_Set_State::Waiting_For_Gate_To_Open;
		}
		break;


	case EMonster_Set_State::Waiting_For_Gate_To_Open:
		if (Border->Is_Gate_Opened(Current_Gate_Index))
		{
			Emit_At_Gate(Current_Gate_Index);
			Monster_Set_State = EMonster_Set_State::Waiting_For_Gate_To_Close;
		}
		break;


	case EMonster_Set_State::Waiting_For_Gate_To_Close:
		if (Border->Is_Gate_Closed(Current_Gate_Index))
			Monster_Set_State = EMonster_Set_State::Selecting_Next_Gate;
		break;


	default:
		AsConfig::Throw();
		break;
	}

	AGame_Objects_Set::Act();
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Init(AsBorder* border)
{
	Border = border;
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Emit_At_Gate(int gate_index)
{
	bool gate_is_left;
	int i;
	int gate_x, gate_y;

	AMonster* monster = 0;

	if (gate_index < 0 || gate_index >= AsConfig::Gates_Count)
		AsConfig::Throw();

	for (i = 0; i < Max_Monsters_Count; i++)
	{
		if (! Monsters[i]->Is_Active())
		{
			monster = Monsters[i];
			break;
		}
	}

	if (monster == 0)
		return; // There are no unused monsters.

	Border->Get_Gate_Pos(gate_index, gate_x, gate_y);

	if (gate_index % 2 == 0)
		gate_is_left = true;
	else
		gate_is_left = false;

	if (! gate_is_left)
		gate_x -= monster->Width - AGate::Width;

	monster->Activate(gate_x, gate_y + 1, gate_is_left);

	//monster->Destroy();
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Activate(int max_monsters_alive)
{
	Monster_Set_State = EMonster_Set_State::Selecting_Next_Gate;
	Max_Monsters_Alive = max_monsters_alive;
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Destroy_All()
{
	int i;

	for (i = 0; i < Max_Monsters_Count; i++)
		if (Monsters[i]->Is_Active())
			Monsters[i]->Destroy();

	Monster_Set_State = EMonster_Set_State::Idle;
}
//------------------------------------------------------------------------------------------------------------
bool AsMonster_Set::Get_Next_GameObject(int& index, AGame_Object** game_object)
{
	if (index < 0 || index >= AsConfig::Max_Balls_Count)
		return false;

	*game_object = Monsters[index++];

	return true;
}
//------------------------------------------------------------------------------------------------------------