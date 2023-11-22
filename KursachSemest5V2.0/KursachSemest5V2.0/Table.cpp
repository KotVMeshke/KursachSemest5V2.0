#include "Table.h"


Table::Table()
{
	colum_names = NULL;
	colum_width = NULL;
	row_numbers = 0;
}

Table::~Table()
{
	for (int i = 0; i < this->colum_numbers; i++)
	{
		free(colum_names[i]);
	}
	free(colum_names);

	free(colum_width);
}
HWND Table::CreateListView(HWND parent_window, HMENU table_id,RECT list_size, int min_list_width)
{
	HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(parent_window, GWLP_HINSTANCE);
	this->parent_window = parent_window;
	list_window = CreateWindow(WC_LISTVIEW, _T(""),
		WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_AUTOARRANGE ,
		list_size.left, list_size.top, list_size.right, list_size.bottom,
		parent_window, table_id, hInst, 0);
	list_width = list_size.right - list_size.left;
	this->min_list_width = min_list_width;

	return list_window;
}

bool Table::InsertNewRow(char* values)
{
	char* token;
	char* word;
	char* temp = (char*)malloc(sizeof(char) * strlen(values));
	strcpy(temp, values);
	int i = 0;
	try
	{
		token = strtok(temp, ",");
		while (token != NULL) {
			word = (char*)malloc(strlen(token) + 1);
			strcpy(word, token);
			InsertOneValue(word, i, row_numbers);
			token = strtok(NULL, ",");
			i++;
			free(word);
		}
	}
	catch (...)
	{
		MessageBox(parent_window, _T("Error"), _T("Row create error"), MB_OK);
		return false;
	}
	row_numbers++;

	return true;
}

bool Table::AddNewColum(char* text, int colum_number, int colum_width)
{
	try
	{
		int size = MultiByteToWideChar(CP_UTF8, 0, text, -1, NULL, 0);
		wchar_t* output = (wchar_t*)malloc(size * sizeof(wchar_t));
		MultiByteToWideChar(CP_UTF8, 0, text, -1, output, size);
		LVCOLUMN lvc;
		lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM|LVCF_FMT;
		lvc.fmt =  LVCFMT_RIGHT;
		lvc.cx = colum_width; 
		lvc.pszText = (LPWSTR)(output);
		lvc.iSubItem = colum_number; 

		if (ListView_InsertColumn(list_window, colum_number, &lvc) == -1)
		{
			throw new std::exception("Error");
		}
		colum_names = (char**)realloc(colum_names, (colum_numbers + 1) * sizeof(char*));
		colum_names[colum_numbers] = (char*)malloc((strlen(text)+1)*sizeof(char));
		strcpy(colum_names[colum_numbers], text);
		this->colum_width = (double*)realloc(this->colum_width, (colum_numbers + 1) * sizeof(double));
		this->colum_width[colum_numbers] = (double)(list_width)/(double)(colum_width);

	}
	catch (...)
	{
		MessageBox(parent_window, _T("Error"), _T("Colum create error"), MB_OK);
		return false;
	}
	
	colum_numbers++;
	return true;
}


bool Table::InsertNewRow(char* values, int row_number)
{
	char* token;
	char* word;
	char* temp = (char*)malloc(sizeof(char)*strlen(values));
	strcpy(temp, values);
	int i = 0;
	try
	{
		token = strtok(temp, ",");
		while (token != NULL) {
			word = (char*)malloc(strlen(token) + 1); 
			strcpy(word, token);
			InsertOneValue(word, i, row_number);
			token = strtok(NULL, ",");
			i++;
			free(word);
		}
	}
	catch (...)
	{
		MessageBox(parent_window, _T("Error"), _T("Row create error"), MB_OK);
		return false;
	}
	row_numbers++;
	
	return true;
}

bool Table::InsertOneValue(char* element, int element_position, int row_number)
{
	try
	{
		int size = MultiByteToWideChar(CP_UTF8, 0, element, -1, NULL, 0);
		wchar_t* output = (wchar_t*)malloc(size * sizeof(wchar_t));
		MultiByteToWideChar(CP_UTF8, 0, element, -1, output, size);
		LVITEM lvi;
		lvi.mask = LVIF_TEXT; 
		lvi.cchTextMax = 15;
		lvi.iItem = row_number; 
		lvi.iSubItem = element_position; 

		lvi.pszText = (LPWSTR)output;
		
		if (element_position % this->colum_numbers == 0)
		{
			if (ListView_InsertItem(list_window, &lvi) == -1)
			{
				DWORD error = GetLastError();
				throw std::exception("Error");
			}
		}
		else
		{
			if (ListView_SetItem(list_window, &lvi) == -1)
			{
				DWORD error = GetLastError();
				throw std::exception("Error");
			}
		}

		ListView_EnsureVisible(list_window, lvi.iItem, FALSE);


	}
	catch (...)
	{
		MessageBox(parent_window, _T("Error"), _T("Value create error"), MB_OK);
		return false;
	}
	return true;
}

bool Table::ResizeTable(RECT window_size)
{
	if (window_size.right - window_size.left > min_list_width)
	{
		int last_pos = 0;
		MoveWindow(this->list_window, window_size.left, window_size.top, window_size.right, window_size.bottom, TRUE);
		RECT newSize = {};
		GetWindowRect(list_window, &newSize);
		list_width = newSize.right - newSize.left;
		for (int i = 0; i < this->colum_numbers; i++)
		{
			if (i == colum_numbers - 1)
			{
				ListView_SetColumnWidth(this->list_window, i, list_width-last_pos);
			}
			else
			{
				ListView_SetColumnWidth(this->list_window, i, list_width / colum_width[i]);
			}
			last_pos += list_width / colum_width[i];
		}
	}
	else
	{
		return false;
	}
	return true;
}
