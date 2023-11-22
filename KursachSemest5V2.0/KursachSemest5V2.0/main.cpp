#define _CRT_SECURE_NO_WARNINGS
#include "functionsHeaders.h"
#include <Windows.h>
#include <CommCtrl.h>
#include "commctrl.h"
#include <tchar.h>

#include "definesHeader.h"

#include "Table.h"

HWND hwndList;
HINSTANCE hInst;
Table table;
SOCKET sniffer;
int nCmd;
bool test = true;
DWORD threadId;
HANDLE hThread;
HMENU menuList[3];
char* Buffer;
LPCTSTR fileManager = L"explorer.exe";
LPCTSTR folder = L".\\";
DWORD WINAPI Sniffing(LPVOID lpParam)
{
	Table* table = static_cast<Table*>(lpParam);
	char* Buffer = (char*)malloc(65536*sizeof(char));
	
	while (test) 
	{
		std::string data = SniffOnePackeg(sniffer, Buffer);
		/*if (data != "")*/
			table->InsertNewRow(const_cast<char*>(data.c_str()));
	}
	return 0;
}

void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
	if (Buffer == nullptr)
		Buffer = (char*)malloc(65536);

	std::string data = SniffOnePackeg(sniffer, Buffer);
	if (data != "")
		table.InsertNewRow(const_cast<char*>(data.c_str()));
	//free(str1);


}
LRESULT CALLBACK WndProc(
	HWND   hWnd,
	UINT   message,
	WPARAM wParam,
	LPARAM lParam
);

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	WNDCLASSEX wcex; HWND hWnd; MSG msg;
	hInst = hInstance;
	nCmd = nCmdShow;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = HBRUSH(CreateSolidBrush(RGB(255, 255, 255)));
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _T("KursachSemestr5");
	wcex.hIconSm = wcex.hIcon;
	RegisterClassEx(&wcex);

	hWnd = CreateWindow(_T("KursachSemestr5"), _T("Wireshark 2.0"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	RECT rect = { 10,0,DEFAULT_WINDOW_WIDTH - 30,DEFAULT_WINDOW_HEIGHT - 40 };
	table.CreateListView(hWnd, (HMENU)ID_TABLE, rect, DEFAULT_WINDOW_WIDTH - 40);
	char* colum_name = (char*)malloc(7 * sizeof(char));
	strcpy(colum_name, "Number");
	table.AddNewColum(colum_name, 0, (DEFAULT_WINDOW_WIDTH - 50) / 10);
	colum_name = (char*)realloc(colum_name, 5 * sizeof(char));
	strcpy(colum_name, "Time");
	table.AddNewColum(colum_name, 1, (DEFAULT_WINDOW_WIDTH - 50) / 10);
	colum_name = (char*)realloc(colum_name, 7 * sizeof(char));
	strcpy(colum_name, "Source");
	table.AddNewColum(colum_name, 2, 3 * (DEFAULT_WINDOW_WIDTH - 50) / 10);
	colum_name = (char*)realloc(colum_name, 5 * sizeof(char));
	strcpy(colum_name, "Dest");
	table.AddNewColum(colum_name, 3, 3 * (DEFAULT_WINDOW_WIDTH - 50) / 10);
	colum_name = (char*)realloc(colum_name, 9 * sizeof(char));
	strcpy(colum_name, "Protocol");
	table.AddNewColum(colum_name, 4, (DEFAULT_WINDOW_WIDTH - 50) / 10);
	colum_name = (char*)realloc(colum_name, 7 * sizeof(char));
	strcpy(colum_name, "Length");
	table.AddNewColum(colum_name, 5, (DEFAULT_WINDOW_WIDTH - 50) / 10);
	free(colum_name);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	ShowWindow(hwndList, nCmdShow);

	while (GetMessage(&msg, NULL, 0, 0))
	{

		TranslateMessage(&msg);
		DispatchMessage(&msg);

	}
	return (int)msg.wParam;
}


void AddMenuToWindow(HWND window)
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

	SetMenu(window, menu);
}

void DrawTextPanel(HWND window)
{
	HWND hwndText = CreateWindow(
		L"STATIC",               // класс окна
		L"Это текстовая панель", // текст
		WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER, // стиль окна
		0, 0, 0, 0,             // положение и размеры окна (будут скорректированы после создания окна)
		window,                   // родительское окно
		NULL,                   // идентификатор окна
		NULL,                   // дескриптор экземпляра
		NULL                    // дополнительные параметры создания
	);

	if (hwndText == NULL)
	{
		MessageBox(window, L"Не удалось создать текстовую панель!", L"Ошибка", MB_OK | MB_ICONERROR);
	}
	else
	{
		// Получение размеров клиентской области родительского окна
		RECT rcClient;
		GetClientRect(window, &rcClient);

		// Установка высоты текстовой панели (например, 50 пикселей)
		int panelHeight = 50;

		// Перемещение текстовой панели внизу клиентской области родительского окна
		MoveWindow(hwndText, 0, rcClient.bottom - panelHeight, rcClient.right, panelHeight, TRUE);
	}
}


LRESULT CALLBACK WndProc(
	HWND   hWnd,
	UINT   message,
	WPARAM wParam,
	LPARAM lParam
)
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
			//MessageBox(hWnd, L"File was open", L"Menu works", MB_OK);
			break;
		case OnSaveAsFileMenu:
		{
			OPENFILENAME ofn;
			TCHAR szFile[MAX_PATH] = { 0 };
			ZeroMemory(&ofn, sizeof(ofn));

			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = NULL; // Установите hwndOwner, чтобы указать родительское окно, если нужно
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = _T("All Files (*.*)\0*.*\0"); // Фильтр для типов файлов
			ofn.nFilterIndex = 1;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			if (GetOpenFileName(&ofn) == TRUE) {
				
				MessageBox(NULL, szFile, _T("Выбранный файл"), MB_OK);
			}
			//MessageBox(hWnd, L"File was save as", L"Menu works", MB_OK);
			break;
		}
		case OnSaveFileMenu:
			//MessageBox(hWnd, L"File was save", L"Menu works", MB_OK);
			break;
		case OnStartMenu:
			hThread = CreateThread(NULL, 0, Sniffing, (LPVOID)(&table), NULL, &threadId);
			test = true;
			//MessageBox(hWnd, L"Sniffing was started", L"Menu works", MB_OK);
			break;
		case OnStopMenu:
			test = false;
			WaitForSingleObject(hThread, 100);
			CloseHandle(hThread);
			//MessageBox(hWnd, L"Sniffing was stoped", L"Menu works", MB_OK);
			break;

		}
		break;
	case WM_CREATE:
		AddMenuToWindow(hWnd);
		//DrawTextPanel(hWnd);
		PrepareForSniffing(&sniffer);

		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &paint);

		EndPaint(hWnd, &paint);
		break;
	case WM_SIZE:
		rect.left += TABLE_OFFSET / 4;
		rect.right += LOWORD(lParam) - TABLE_OFFSET / 2;
		rect.bottom += HIWORD(lParam) - TABLE_OFFSET;
		table.ResizeTable(rect);
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


