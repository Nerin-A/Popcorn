#pragma once

#include "Final_Letter.h"

//------------------------------------------------------------------------------------------------------------
enum class EGame_Title_State : unsigned char
{
	Idle,

	Game_Over_Descent,
	Game_Over_Show,
	Game_Over_Destroy,

	Game_Won_Descent,
	Game_Won_Animate,

	Finished
};
//------------------------------------------------------------------------------------------------------------
class AsGame_Title : public AGraphics_Object
{
public:
	~AsGame_Title();
	AsGame_Title();

	virtual void Act();
	virtual void Clear(HDC hdc, RECT& paint_area);
	virtual void Draw(HDC hdc, RECT& paint_area);
	virtual bool Is_Finished();

	void Show(bool is_victory);
	bool Is_Visible();

private:
	EGame_Title_State Game_Title_State;
	int Starting_Tick;
	RECT Title_Rect, Previous_Title_Rect;

	std::vector <AFinal_Letter*> Title_Letters;

	static const int Descent_Timeout = AsConfig::FPS * 6; // 6 seconds to Descend
	static const int Height = 32;
	static const int Game_Over_Showing_Timeout = AsConfig::FPS * 3;
	static const int Explosion_Delay = AsConfig::FPS / 2;
	static const double Lowest_Y_Pos;
};
//------------------------------------------------------------------------------------------------------------