#include "Border.h"

//AsBorder
//------------------------------------------------------------------------------------------------------------
void AsBorder::init()
{
	AsEngine::Create_Pen_Brush(85, 255, 255, Border_Blue_Pen, Border_Blue_Brush);
	AsEngine::Create_Pen_Brush(255, 255, 255, Border_White_Pen, Border_White_Brush);
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Draw_Element(HDC hdc, int x, int y, bool top_boder, AsEngine* engine)
{// ������ ������� ����� ������

 // �������� �����
	SelectObject(hdc, Border_Blue_Pen);
	SelectObject(hdc, Border_Blue_Brush);

	if (top_boder)
		Rectangle(hdc, x * AsEngine::Global_Scale, (y + 1) * AsEngine::Global_Scale, (x + 4) * AsEngine::Global_Scale, (y + 4) * AsEngine::Global_Scale);
	else
		Rectangle(hdc, (x + 1) * AsEngine::Global_Scale, y * AsEngine::Global_Scale, (x + 4) * AsEngine::Global_Scale, (y + 4) * AsEngine::Global_Scale);

	// ����� �����
	SelectObject(hdc, Border_White_Pen);
	SelectObject(hdc, Border_White_Brush);

	if (top_boder)
		Rectangle(hdc, x * AsEngine::Global_Scale, y * AsEngine::Global_Scale, (x + 4) * AsEngine::Global_Scale, (y + 1) * AsEngine::Global_Scale);
	else
		Rectangle(hdc, x * AsEngine::Global_Scale, y * AsEngine::Global_Scale, (x + 1) * AsEngine::Global_Scale, (y + 4) * AsEngine::Global_Scale);

	// ����������
	SelectObject(hdc, engine->BG_Pen);
	SelectObject(hdc, engine->BG_Brush);

	if (top_boder)
		Rectangle(hdc, (x + 2) * AsEngine::Global_Scale, (y + 2) * AsEngine::Global_Scale, (x + 3) * AsEngine::Global_Scale, (y + 3) * AsEngine::Global_Scale);
	else
		Rectangle(hdc, (x + 2) * AsEngine::Global_Scale, (y + 1) * AsEngine::Global_Scale, (x + 3) * AsEngine::Global_Scale, (y + 2) * AsEngine::Global_Scale);
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Draw(HDC hdc, RECT& paint_area, AsEngine* engine)
{// ������ ����� ������

	int i;

	// 1. ����� �����
	for (i = 0; i < 50; i++)
		Draw_Element(hdc, 2, 1 + i * 4, false, engine);

	// 2. ����� ������
	for (i = 0; i < 50; i++)
		Draw_Element(hdc, 201, 1 + i * 4, false, engine);

	// 3. ����� ������
	for (i = 0; i < 50; i++)
		Draw_Element(hdc, 3 + i * 4, 0, true, engine);
}
//------------------------------------------------------------------------------------------------------------
