#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include "functionsHeaders.h"

#include <Windows.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <string>
#include <exception>
#include "definesHeader.h"
#include <vector>

class Table
{
private:
	HWND list_window;
	HWND parent_window;
	int list_width;
	int row_numbers;
	int colum_numbers;
	char** colum_names;
	double* colum_width;
	int min_list_width;
	

	bool InsertOneValue(char*,int,int);
public:
	bool scroll;
	LONG_PTR oldListViewProc;

	Table();
	~Table();
	HWND CreateListView(HWND, HMENU, RECT,int,SUBCLASSPROC);
	bool AddNewColum(char*,int,int);
	bool InsertNewRow(char*,int);
	bool InsertNewRow(char*);
	bool ResizeTable(RECT);
	void ClearTable();
	void SetScrollSettings(bool);
	void SetNewNumberOfRows(int);
	void DisplayData(std::vector<std::string>, int);
	void InsertData(LPARAM lParam, std::vector<std::string>);
};

