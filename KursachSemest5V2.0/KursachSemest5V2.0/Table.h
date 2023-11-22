#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <exception>

#pragma once
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

	bool InsertOneValue(char* element, int element_position, int row_number);
public:
	Table();
	~Table();
	HWND CreateListView(HWND parent_window, HMENU table_id, RECT list_size, int min_list_width);
	bool AddNewColum(char* text, int colum_number, int culum_width = 100);
	bool InsertNewRow(char* values, int row_number);
	bool InsertNewRow(char* values);
	bool ResizeTable(RECT window_size);
	
	

	
};

