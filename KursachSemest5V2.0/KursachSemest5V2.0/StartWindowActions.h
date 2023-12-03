#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "functionsHeaders.h"

#include <Windows.h>
#include <CommCtrl.h>
#include <vector>
#include "commctrl.h"
#include "Table.h"
#include "definesHeader.h"

std::vector<HWND> buttons;

void AddMenuToStartWindow(HWND window)
{
	HMENU menu = CreateMenu();
	AppendMenu(menu, MF_STRING, OnReloadInterface, L"Reload");

	SetMenu(window, menu);
}

LRESULT CALLBACK WndStartProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT paint;
	RECT rect = {};
	switch (message)
	{
	case WM_CREATE:
	{
		AddMenuToStartWindow(hWnd);
		//hostent local = GetLocalInterfaces();

		std::vector<sockaddr_in> addresses = GetAllInterfaces();
		//buttons = CreateButtons(hWnd, local);
		buttons = CreateButtons(hWnd, addresses);

		break;
	}
	case WM_COMMAND:
		switch (wParam)
		{
		case OnReloadInterface:
		{
			for (auto& elem : buttons)
			{
				DestroyWindow(elem);
			}
			buttons.clear();
			InvalidateRect(hWnd, NULL, TRUE);
			//hostent local = GetLocalInterfaces();
			std::vector<sockaddr_in> local = GetAllInterfaces();
			buttons = CreateButtons(hWnd, local);
			break;
		}
		default:
			int buttonId = LOWORD(wParam);
			if (buttonId >= INTERFACE_SELECTORS && buttonId < INTERFACE_SELECTORS + buttons.size())
			{
				HWND DataW = FindWindow(L"DataWindow", NULL);
				ShowWindow(DataW, SW_SHOW);
				ShowWindow(hWnd, SW_HIDE);
				SendMessage(DataW, WM_COMMAND, SelectInterface, buttonId);
			}
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &paint);
		EndPaint(hWnd, &paint);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:

		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

