#pragma once

#include <windows.h>

class Window
{
public:
	bool Create(HINSTANCE instance,
		int width,
		int height,
		const wchar_t* title
	);

	HWND GetHwnd() const { return hwnd;}

	bool ProcessMessages();

private:
	HWND hwnd = nullptr;

	static LRESULT CALLBACK WindowProc(
		HWND hwnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam
	);
	
};

