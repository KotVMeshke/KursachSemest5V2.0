#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "functionsHeaders.h"
#include "Interface.h"
#include <Windows.h>
#include <CommCtrl.h>
#include "commctrl.h"
#include <fstream>
#include <tchar.h>

#include "DataWindowActions.h"
#include "StartWindowActions.h"
#include "InformationWindowActions.h"

HWND StartWindow;
HWND TableWindow;
HWND InformationWindow;


DWORD WINAPI Sniffing(LPVOID lpParam)
{
	Table* DataTable = static_cast<Table*>(lpParam);
	char* Buffer = (char*)malloc(65536 * sizeof(char));
	std::string fullData;

	while (SniffingRule)
	{
		std::string data = SniffOnePackeg(sniffer, Buffer, &fullData);
		DataTable->InsertNewRow(const_cast<char*>(data.c_str()));
		ScannList.push_back(fullData);
	}

	return 0;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance,_In_opt_ HINSTANCE hPrevInstance,_In_ LPSTR lpCmdLine,_In_ int nCmdShow)
{
	WNDCLASSEX wcex; 
	MSG msg;
	hInst = hInstance;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = WndDataProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = HBRUSH(CreateSolidBrush(RGB(255, 255, 255)));
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _T("DataWindow");
	wcex.hIconSm = wcex.hIcon;
	RegisterClassEx(&wcex);

	wcex.lpszClassName = _T("StartWindow");
	wcex.lpfnWndProc = WndStartProc;
	RegisterClassEx(&wcex);

	wcex.lpszClassName = _T("InformationWindow");
	wcex.lpfnWndProc = WndInformProc;
	RegisterClassEx(&wcex);
	StartWindow = CreateWindow(_T("StartWindow"), _T("Start"), WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_MAXIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, DEFAULT_WINDOW_DATA_WIDTH, DEFAULT_WINDOW_DATA_HEIGHT, NULL, NULL, hInstance, NULL);
	TableWindow = CreateWindow(_T("DataWindow"), _T("Sniffing"), WS_OVERLAPPEDWINDOW , CW_USEDEFAULT, CW_USEDEFAULT, DEFAULT_WINDOW_DATA_WIDTH, DEFAULT_WINDOW_DATA_HEIGHT, NULL, NULL, hInstance, NULL);
	InformationWindow = CreateWindow(_T("InformationWindow"), _T("Package data"), WS_OVERLAPPEDWINDOW , CW_USEDEFAULT, CW_USEDEFAULT, DEFAULT_WINDOW_DATA_WIDTH, DEFAULT_WINDOW_DATA_HEIGHT, NULL, NULL, hInstance, NULL);

	ShowWindow(StartWindow, nCmdShow);
	UpdateWindow(StartWindow);

	while (GetMessage(&msg, NULL, 0, 0))
	{

		TranslateMessage(&msg);
		DispatchMessage(&msg);

	}
	return (int)msg.wParam;
}
const std::string TCHARToString(const TCHAR* tcharString)
{
	int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, tcharString, -1, nullptr, 0, nullptr, nullptr);
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, tcharString, -1, &result[0], sizeNeeded, nullptr, nullptr);
	return result;
}



void InsertFullListIntoTable(std::vector<std::string> list)
{
	std::string temp;
	for (const auto& str : list)
	{
		temp = GetShortData(const_cast<char*>(str.c_str()), str.length());
		DataTable.InsertNewRow(const_cast<char*>(temp.c_str()));
	}
}

std::string GetFileName(DWORD tags)
{
	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = FileName;
	ofn.nMaxFile = sizeof(FileName);
	ofn.lpstrFilter = _T("Text Files (*.txt)\0*.txt\0Binary Files (*.bin)\0*.bin\0All Files (*.*)\0*.*\0");
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST ;

	if (GetOpenFileName(&ofn) == TRUE) {

		MessageBox(NULL, FileName, _T("SelectedFile"), MB_OK);
	}
	return TCHARToString(FileName);
}




void SaveListTofile(std::vector<std::string>& strings, const std::string& filename)
{
	std::ofstream outFile(filename, std::ios::out | std::ios::binary);

	if (outFile.is_open())
	{
		for (const auto& line : strings) {
			size_t len = line.size();
			outFile.write(reinterpret_cast<char*>(&len), sizeof(size_t));
			outFile.write(line.c_str(), len);
		}
	}

	outFile.close();
}
std::vector<std::string> LoadListFromFile(std::string filename)
{
	std::ifstream inFile(filename, std::ios::in | std::ios::binary);
	std::vector<std::string> list;
	size_t len;
	if (inFile.is_open())
	{
		while (inFile.read(reinterpret_cast<char*>(&len), sizeof(size_t))) {
			char* buffer = new char[len + 1];
			inFile.read(buffer, len);
			buffer[len] = '\0';
			list.emplace_back(buffer, len + 1);
			delete[] buffer;
		}
	}

	inFile.close();

	return list;
}

