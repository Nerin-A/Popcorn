﻿#pragma once

#include "resource.h"
#include "Engine.h"

//------------------------------------------------------------------------------------------------------------
class AsFrame_DC
{
public:
	~AsFrame_DC();
	AsFrame_DC();
	HDC Get_DC(HWND hwnd, HDC hdc);

	int Width, Height;

private:
	HBITMAP Bitmap;
	HDC DC;
};
//------------------------------------------------------------------------------------------------------------
class AsMain_Window
{
public:
	AsMain_Window();

	int APIENTRY Main(HINSTANCE instance, int cmd_show);

private:
	static const int MAX_LOADSTRING = 100;

	AsEngine Engine;
	HINSTANCE Instance;                                // current instance
	WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text  //!!! replace with AString
	WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name  //!!! replace with AString
	AsFrame_DC DC;

	ATOM Register_Class();
	BOOL Init_Instance(int cmd_show);
	void On_Paint(HWND hwnd);

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

	static AsMain_Window* Self;
};
//------------------------------------------------------------------------------------------------------------