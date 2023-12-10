#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "functionsHeaders.h"

#include <Windows.h>
#include <CommCtrl.h>
#include "commctrl.h"
#include <fstream>
#include "ThreadSafeQueue.h"
#include "Table.h"
#include "definesHeader.h"
#include <sstream>

HINSTANCE hInst;

DWORD ThreadId;
HWND ListWindow;
Table DataTable;
std::vector<std::string> ScannList;
HANDLE hThread;

SOCKET sniffer;

TCHAR FileName[MAX_PATH];
std::vector<std::string>test;
bool SniffingRule = true;
bool PrintRule = true;

std::string GetWord(std::string str, int number);

void GetSelectedPackage(LPARAM lParam)
{
	NMHDR* nmhdr = reinterpret_cast<NMHDR*>(lParam);
	if (nmhdr->code == (DWORD)(NM_CLICK) && nmhdr->idFrom == ID_TABLE)
	{
		NMITEMACTIVATE* nmitem = reinterpret_cast<NMITEMACTIVATE*>(lParam);

		int selectedIndex = nmitem->iItem;
		std::string selectedPackage = ScannList[selectedIndex];

		HWND InformWindow = FindWindow(L"InformationWindow", NULL);
		if (InformWindow == NULL)
		{
			HINSTANCE hInstance = GetModuleHandle(NULL);
			InformWindow = CreateWindow(_T("InformationWindow"), _T("Package data"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, DEFAULT_WINDOW_DATA_WIDTH, DEFAULT_WINDOW_DATA_HEIGHT, NULL, NULL, hInstance, NULL);
		}
		ShowWindow(InformWindow, SW_SHOW);
		SendMessage(InformWindow, WM_SHOW_PACKAGE, (WPARAM)selectedPackage.c_str(), selectedPackage.length());
	}
}

LRESULT CALLBACK ListViewProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (uMsg)
	{
	case WM_NOTIFY:
	{

		LPNMHDR pNMHDR = (LPNMHDR)lParam;
		if (pNMHDR->idFrom == ID_TABLE)
		{
			switch (pNMHDR->code)
			{
			case LVN_GETDISPINFO:
			{
			}
			break;
			}
		}
	}

	default:
	{
		return CallWindowProc((WNDPROC)dwRefData, hWnd, uMsg, wParam, lParam);
	}
	}
}


void SetColumsToTable(Table* table, HWND hWnd)
{
	RECT rect = { 10,0,DEFAULT_WINDOW_DATA_WIDTH - 30,DEFAULT_WINDOW_DATA_HEIGHT - 40 };
	table->CreateListView(hWnd, (HMENU)ID_TABLE, rect, DEFAULT_WINDOW_DATA_WIDTH - 40, NULL);
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
		{
			ScannList.clear();
			test.clear();
			SendMessage(hWnd, WM_COMMAND, WM_DESTROY, 0);


		}
		case OnOpenFileMenu:
		{
			DataTable.ClearTable();
			ScannList = LoadListFromFile(GetFileName(OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST));
			InsertFullListIntoTable(ScannList);
			MessageBox(hWnd, L"File was open", L"Menu works", MB_OK);
			break;
		}
		case OnSaveAsFileMenu:
		{
			SaveListTofile(ScannList, GetFileName(OFN_PATHMUSTEXIST));
			MessageBox(hWnd, L"File was save as", L"File info", MB_OK);

			break;
		}
		case OnSaveFileMenu:
		{
			if (FileName[0] == L'\0')
			{
				SaveListTofile(ScannList, GetFileName(OFN_PATHMUSTEXIST));
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
			ScannList.clear();
			test.clear();
			DataTable.ClearTable();
		}
		break;
		case OnStartMenu:
		{
			SetTimer(hWnd, ID_TIMER, 1500, ((TIMERPROC)NULL));
			/*UpdateWindow(ListWindow);*/
			//ScannList2 = new ThreadSafeQueue();
			HMENU hMenu = GetMenu(hWnd);
			EnableMenuItem(hMenu, OnStopMenu, MF_BYCOMMAND | MF_ENABLED);
			EnableMenuItem(hMenu, OnStartMenu, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(hMenu, OnResetTable, MF_BYCOMMAND | MF_GRAYED);

			SniffingRule = true;
			PrintRule = true;
				hThread = CreateThread(NULL, 0, Sniffing, (LPVOID)(&DataTable), NULL, &ThreadId);

		}
		break;
		case OnStopMenu:
		{
			KillTimer(hWnd, ID_TIMER);
			PrintRule = false;
			SniffingRule = false;

			//WaitForMultipleObjects(THREADS_NUMBER, DataThreads, TRUE, 100);
			WaitForSingleObject(hThread, 1000);
			//CloseHandle(hThread[0]);
			HMENU hMenu = GetMenu(hWnd);
			EnableMenuItem(hMenu, OnStopMenu, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(hMenu, OnStartMenu, MF_BYCOMMAND | MF_ENABLED);
			EnableMenuItem(hMenu, OnResetTable, MF_BYCOMMAND | MF_ENABLED);
			break;
		}
		case OnBackToStart:
		{
			KillTimer(hWnd, ID_TIMER);
			ScannList.clear();
			test.clear();
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
			DataTable.SetNewNumberOfRows(0);
			test.clear();
			DataTable.ClearTable();
			std::string result = PrepareForSniffing(&sniffer, lParam - INTERFACE_SELECTORS);
			if (result != "correct")
			{
				int size = MultiByteToWideChar(CP_UTF8, 0, result.c_str(), -1, NULL, 0);
				wchar_t* output = (wchar_t*)malloc(size * sizeof(wchar_t));
				MultiByteToWideChar(CP_UTF8, 0, result.c_str(), -1, output, size);
				MessageBox(hWnd, output, L"Something gone wrong", MB_OK);
				HMENU hMenu = GetMenu(hWnd);
				EnableMenuItem(hMenu, OnStopMenu, MF_BYCOMMAND | MF_ENABLED);
				EnableMenuItem(hMenu, OnStartMenu, MF_BYCOMMAND | MF_GRAYED);
				EnableMenuItem(hMenu, OnResetTable, MF_BYCOMMAND | MF_GRAYED);
			}
			break;
		}
		case OnStartScroll:
		{
			DataTable.SetScrollSettings(true);
			break;
		}
		case OnStopScroll:
		{
			DataTable.SetScrollSettings(false);
			break;
		}
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &paint);

		EndPaint(hWnd, &paint);
		break;
	case WM_TIMER:
	{
		if (wParam == ID_TIMER)
		{
			int size = test.size();
			if (size != 0)
			{
				DataTable.SetNewNumberOfRows(size);
				DataTable.DisplayData(test, size);
			}

		}
		break;
	}
	case WM_CREATE:
	{
		AddMenuToDataWindow(hWnd);
		SetColumsToTable(&DataTable, hWnd);
		break;
	}
	case WM_NOTIFY:
	{
		GetSelectedPackage(lParam);
		//DataTable.InsertData(lParam, test);
		LPNMHDR pNMHDR = (LPNMHDR)lParam;
		if (pNMHDR->idFrom == ID_TABLE)
		{
			switch (pNMHDR->code)
			{
			case LVN_GETDISPINFO:
			{

				NMLVDISPINFO* pDispInfo = (NMLVDISPINFO*)lParam;
				int index = pDispInfo->item.iItem;

				if (test.size() > index)
				{
					switch (pDispInfo->item.iSubItem)
					{
					case 0:
					{

						std::string str = GetWord(test[index], 0);
						std::wstring widestr = std::wstring(str.begin(), str.end());
						wchar_t* output = new wchar_t[widestr.length() + 1];
						wcscpy_s(output, widestr.length() + 1, widestr.c_str());
						pDispInfo->item.pszText = output;
						break;
					}
					case 1:
					{
						std::string str = GetWord(test[index], 1);
						std::wstring widestr = std::wstring(str.begin(), str.end());
						wchar_t* output = new wchar_t[widestr.length() + 1];
						wcscpy_s(output, widestr.length() + 1, widestr.c_str());
						pDispInfo->item.pszText = output;

						break;
					}
					case 2:
					{
						std::string str = GetWord(test[index], 2);
						std::wstring widestr = std::wstring(str.begin(), str.end());
						wchar_t* output = new wchar_t[widestr.length() + 1];
						wcscpy_s(output, widestr.length() + 1, widestr.c_str());
						pDispInfo->item.pszText = output;

						break;
					}
					case 3:
					{
						std::string str = GetWord(test[index], 3);
						std::wstring widestr = std::wstring(str.begin(), str.end());
						wchar_t* output = new wchar_t[widestr.length() + 1];
						wcscpy_s(output, widestr.length() + 1, widestr.c_str());

						pDispInfo->item.pszText = output;

						break;
					}
					case 4:
					{
						std::string str = GetWord(test[index], 4);
						std::wstring widestr = std::wstring(str.begin(), str.end());
						wchar_t* output = new wchar_t[widestr.length() + 1];
						wcscpy_s(output, widestr.length() + 1, widestr.c_str());

						pDispInfo->item.pszText = output;

						break;
					}
					case 5:
					{
						std::string str = GetWord(test[index], 5);
						std::wstring widestr = std::wstring(str.begin(), str.end());
						wchar_t* output = new wchar_t[widestr.length() + 1];
						wcscpy_s(output, widestr.length() + 1, widestr.c_str());

						pDispInfo->item.pszText = output;

						break;
					}
					default:
						break;
					}
				}


				UpdateWindow(ListWindow);
			}
			break;
			}
		}
		

		break;
	}
	case WM_SIZE:
		rect.left += TABLE_OFFSET / 4;
		rect.right += LOWORD(lParam) - TABLE_OFFSET / 2;
		rect.bottom += HIWORD(lParam) - TABLE_OFFSET;
		DataTable.ResizeTable(rect);
		break;
	case WM_DESTROY:
	{
		SniffingRule = false;
		SetNumberOfPackagesToZero();
		KillTimer(hWnd, ID_TIMER);
		WaitForSingleObject(hThread, 1000);
		ScannList.clear();
		test.clear();

		DataTable.SetNewNumberOfRows(0);
		DataTable.ClearTable();
		ShowWindow(hWnd, SW_HIDE);
		HWND StartWindow = FindWindow(L"StartWindow", NULL);
		ShowWindow(StartWindow, SW_SHOW);
		ClearSocket(&sniffer);
		break;
	}
	default:

		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;

}

void AddMenuToDataWindow(HWND window)
{
	HMENU menu = CreateMenu();
	HMENU fileMenu = CreateMenu();
	HMENU settingsMenu = CreateMenu();
	AppendMenu(fileMenu, MF_STRING, OnOpenFileMenu, L"Open File");
	AppendMenu(fileMenu, MF_STRING, OnSaveAsFileMenu, L"Save as");
	AppendMenu(fileMenu, MF_STRING, OnSaveFileMenu, L"Save");
	AppendMenu(fileMenu, MF_MENUBARBREAK, NULL, NULL);
	AppendMenu(fileMenu, MF_STRING, OnExitMenu, L"Exit");

	AppendMenu(menu, MF_POPUP, (UINT_PTR)fileMenu, L"File");
	AppendMenu(menu, MF_STRING, OnStartMenu, L"Start");
	AppendMenu(menu, MF_STRING, OnStopMenu, L"Stop");
	AppendMenu(menu, MF_STRING, OnResetTable, L"Reset");
	AppendMenu(menu, MF_STRING, OnBackToStart, L"Start page");

	AppendMenu(settingsMenu, MF_STRING, OnStartScroll, L"Start scroll");
	AppendMenu(settingsMenu, MF_STRING, OnStopScroll, L"Stop scroll");
	AppendMenu(menu, MF_POPUP, (UINT_PTR)settingsMenu, L"Settings");

	SetMenu(window, menu);
	EnableMenuItem(menu, OnStopMenu, MF_BYCOMMAND | MF_GRAYED);
}

std::string GetWord(std::string str, int number)
{
	std::istringstream iss(str);
	std::vector<std::string> words;

	std::string word;
	while (std::getline(iss, word, ',')) {
		words.push_back(word);
	}
	return words[number];
}
