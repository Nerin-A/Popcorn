#pragma once

#include "Ball.h"

//------------------------------------------------------------------------------------------------------------
class AsBall_Set : public AGame_Objects_Set
{
public:
	virtual void Act();

	void Release_From_The_Platform(double platform_x_pos);
	bool Release_Next_Ball();
	void Set_On_The_Platform(double platform_x_pos);
	bool All_Balls_Are_Lost();
	void Set_For_Test();
	bool Is_Test_Finished();
	void Triple_Balls();
	void Inverse_Balls();
	void Accelerate();
	void Reset_Speed();
	void On_Platform_Advance(double direction, double platform_speed, double max_speed);

private:
	virtual bool Get_Next_GameObject(int& index, AGame_Object** game_object);

	ABall Balls[AsConfig::Max_Balls_Count];
};
//------------------------------------------------------------------------------------------------------------