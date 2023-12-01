#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <string>
#include <exception>

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
	LONG_PTR oldListViewProc;

	Table();
	~Table();
	HWND CreateListView(HWND, HMENU, RECT,int,LONG_PTR);
	bool AddNewColum(char*,int,int);
	bool InsertNewRow(char*,int);
	bool InsertNewRow(char*);
	bool ResizeTable(RECT);
	void ClearTable();
};

