#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "functionsHeaders.h"

#include <Windows.h>
#include <CommCtrl.h>
#include "commctrl.h"
#include "Table.h"
#include "definesHeader.h"

HINSTANCE hInst;

DWORD ThreadId;
HWND ListWindow;
Table DataTable;
std::vector<std::string> ScannList;
HANDLE hThread;

SOCKET sniffer;

TCHAR FileName[MAX_PATH];

bool SniffingRule = true;
LRESULT CALLBACK ListViewProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_NOTIFY:
	{
		NMHDR* pNMHDR = (NMHDR*)lParam;


		if (pNMHDR->code == LVN_ITEMCHANGED)
		{
			NMLISTVIEW* pNMLV = (NMLISTVIEW*)lParam;

			// Проверяем, что было изменение выделения
			if ((pNMLV->uChanged & LVIF_STATE) && (pNMLV->uNewState & LVIS_SELECTED))
			{
				// Вся строка должна быть выделена
				ListView_SetItemState(hwnd, pNMLV->iItem, LVIS_SELECTED, LVIS_SELECTED);
			}
		}

	}
	break;

	default:
		return CallWindowProc((WNDPROC)DataTable.oldListViewProc, hwnd, msg, wParam, lParam);
	}

	// Если сообщение обработано, верните ноль
	return 0;
}

void SetColumsToTable(Table* table, HWND hWnd)
{
	RECT rect = { 10,0,DEFAULT_WINDOW_DATA_WIDTH - 30,DEFAULT_WINDOW_DATA_HEIGHT - 40 };
	table->CreateListView(hWnd, (HMENU)ID_TABLE, rect, DEFAULT_WINDOW_DATA_WIDTH - 40, (LONG_PTR)ListViewProc);
	char* colum_name = (char*)malloc(7 * sizeof(char));
	strcpy(colum_name, "Number");
	table->AddNewColum(colum_name, 0, (DEFAULT_WINDOW_DATA_WIDTH - 50) / 10);
	colum_name = (char*)realloc(colum_name, 5 * sizeof(char));
	strcpy(colum_name, "Time");
	table->AddNewColum(colum_name, 1, (DEFAULT_WINDOW_DATA_WIDTH - 50) / 10);
	colum_name = (char*)realloc(colum_name, 7 * sizeof(char));
	strcpy(colum_name, "Source");
	table->AddNewColum(colum_name, 2, 3 * (DEFAULT_WINDOW_DATA_WIDTH - 50) / 10);
	colum_name = (char*)realloc(colum_name, 5 * sizeof(char));
	strcpy(colum_name, "Dest");
	table->AddNewColum(colum_name, 3, 3 * (DEFAULT_WINDOW_DATA_WIDTH - 50) / 10);
	colum_name = (char*)realloc(colum_name, 9 * sizeof(char));
	strcpy(colum_name, "Protocol");
	table->AddNewColum(colum_name, 4, (DEFAULT_WINDOW_DATA_WIDTH - 50) / 10);
	colum_name = (char*)realloc(colum_name, 7 * sizeof(char));
	strcpy(colum_name, "Length");
	table->AddNewColum(colum_name, 5, (DEFAULT_WINDOW_DATA_WIDTH - 50) / 10);
	free(colum_name);
}

LRESULT CALLBACK WndDataProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT paint;
	RECT rect = {};
	switch (message)
	{
	case WM_COMMAND:
		switch (wParam)
		{
		case OnExitMenu:
			PostQuitMessage(0);
		case OnOpenFileMenu:
			DataTable.ClearTable();
			ScannList = LoadListFromFile(GetFileName());
			InsertFullListIntoTable(ScannList);
			MessageBox(hWnd, L"File was open", L"Menu works", MB_OK);
			break;
		case OnSaveAsFileMenu:
		{
			SaveListTofile(ScannList, GetFileName());
			MessageBox(hWnd, L"File was save as", L"File info", MB_OK);

			break;
		}
		case OnSaveFileMenu:
		{
			if (FileName[0] == L'\0')
			{
				SaveListTofile(ScannList, GetFileName());
				MessageBox(hWnd, L"File was save as", L"File info", MB_OK);
			}
			else
			{
				SaveListTofile(ScannList, TCHARToString(FileName));
				MessageBox(hWnd, L"File was save", L"File info", MB_OK);
			}

		}
		case OnResetTable:
		{
			DataTable.ClearTable();
		}
		break;
		case OnStartMenu:
		{
			HMENU hMenu = GetMenu(hWnd);
			EnableMenuItem(hMenu, OnStopMenu, MF_BYCOMMAND | MF_ENABLED);
			EnableMenuItem(hMenu, OnStartMenu, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(hMenu, OnResetTable, MF_BYCOMMAND | MF_GRAYED);

			SniffingRule = true;

			hThread = CreateThread(NULL, 0, Sniffing, (LPVOID)(&DataTable), NULL, &ThreadId);
		}
		break;
		case OnStopMenu:
		{
			HMENU hMenu = GetMenu(hWnd);
			EnableMenuItem(hMenu, OnStopMenu, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(hMenu, OnStartMenu, MF_BYCOMMAND | MF_ENABLED);
			EnableMenuItem(hMenu, OnResetTable, MF_BYCOMMAND | MF_ENABLED);
			SniffingRule = false;
			break;
		}
		case OnBackToStart:
		{
			HMENU hMenu = GetMenu(hWnd);
			EnableMenuItem(hMenu, OnStopMenu, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(hMenu, OnStartMenu, MF_BYCOMMAND | MF_ENABLED);
			EnableMenuItem(hMenu, OnResetTable, MF_BYCOMMAND | MF_GRAYED);


			SniffingRule = false;
			WaitForSingleObject(hThread, 100);
			CloseHandle(hThread);

			ClearSocket(&sniffer);
			sniffer = NULL;
			DataTable.ClearTable();
			HWND StartWindow = FindWindow(L"StartWindow", NULL);
			ShowWindow(hWnd, SW_HIDE);
			ShowWindow(StartWindow, SW_SHOW);
			break;
		}
		case SelectInterface:
		{
			sniffer = NULL;
			PrepareForSniffing(&sniffer,lParam-INTERFACE_SELECTORS);
			break;
		}
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &paint);

		EndPaint(hWnd, &paint);
		break;
	case WM_CREATE:
	{
		AddMenuToDataWindow(hWnd);
		SetColumsToTable(&DataTable, hWnd);
		break;
	}
	case WM_SIZE:
		rect.left += TABLE_OFFSET / 4;
		rect.right += LOWORD(lParam) - TABLE_OFFSET / 2;
		rect.bottom += HIWORD(lParam) - TABLE_OFFSET;
		DataTable.ResizeTable(rect);
		break;
	case WM_DESTROY:
		ClearSocket(&sniffer);
		PostQuitMessage(0);
		break;
	default:

		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;

}

void AddMenuToDataWindow(HWND window)
{
	HMENU menu = CreateMenu();
	HMENU subMenu = CreateMenu();
	AppendMenu(subMenu, MF_STRING, OnOpenFileMenu, L"Open File");
	AppendMenu(subMenu, MF_STRING, OnSaveAsFileMenu, L"Save as");
	AppendMenu(subMenu, MF_STRING, OnSaveFileMenu, L"Save");
	AppendMenu(subMenu, MF_MENUBARBREAK, NULL, NULL);
	AppendMenu(subMenu, MF_STRING, OnExitMenu, L"Exit");

	AppendMenu(menu, MF_POPUP, (UINT_PTR)subMenu, L"File");
	AppendMenu(menu, MF_STRING, OnStartMenu, L"Start");
	AppendMenu(menu, MF_STRING, OnStopMenu, L"Stop");
	AppendMenu(menu, MF_STRING, OnResetTable, L"Reset");
	AppendMenu(menu, MF_STRING, OnBackToStart, L"Start page");

	SetMenu(window, menu);
	EnableMenuItem(menu, OnStopMenu, MF_BYCOMMAND | MF_GRAYED);
}
