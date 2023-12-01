//#include "DataWindowActions.h"
//
//LRESULT CALLBACK WndDataProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	HDC hdc;
//	PAINTSTRUCT paint;
//	RECT rect = {};
//	switch (message)
//	{
//	case WM_COMMAND:
//		switch (wParam)
//		{
//		case OnExitMenu:
//			PostQuitMessage(0);
//		case OnOpenFileMenu:
//			DataTable.ClearTable();
//			ScannList = LoadListFromFile(GetFileName());
//			InsertFullListIntoTable(ScannList);
//			MessageBox(hWnd, L"File was open", L"Menu works", MB_OK);
//			break;
//		case OnSaveAsFileMenu:
//		{
//			SaveListTofile(ScannList, GetFileName());
//			MessageBox(hWnd, L"File was save as", L"File info", MB_OK);
//
//			break;
//		}
//		case OnSaveFileMenu:
//		{
//			if (FileName[0] == L'\0')
//			{
//				SaveListTofile(ScannList, GetFileName());
//				MessageBox(hWnd, L"File was save as", L"File info", MB_OK);
//			}
//			else
//			{
//				SaveListTofile(ScannList, TCHARToString(FileName));
//				MessageBox(hWnd, L"File was save", L"File info", MB_OK);
//			}
//
//		}
//		case OnResetTable:
//		{
//			DataTable.ClearTable();
//		}
//		break;
//		case OnStartMenu:
//		{
//			sniffer = NULL;
//			PrepareForSniffing(&sniffer);
//			SniffingRule = true;
//
//			hThread = CreateThread(NULL, 0, Sniffing, (LPVOID)(&DataTable), NULL, &ThreadId);
//			//MessageBox(hWnd, L"Sniffing was started", L"Menu works", MB_OK);
//		}
//		break;
//		case OnStopMenu:
//			SniffingRule = false;
//			WaitForSingleObject(hThread, 100);
//			CloseHandle(hThread);
//
//			ClearSocket(&sniffer);
//			sniffer == NULL;
//
//			//MessageBox(hWnd, L"Sniffing was stoped", L"Menu works", MB_OK);
//			break;
//
//		}
//		break;
//	case WM_PAINT:
//		hdc = BeginPaint(hWnd, &paint);
//
//		EndPaint(hWnd, &paint);
//		break;
//	case WM_SIZE:
//		rect.left += TABLE_OFFSET / 4;
//		rect.right += LOWORD(lParam) - TABLE_OFFSET / 2;
//		rect.bottom += HIWORD(lParam) - TABLE_OFFSET;
//		DataTable.ResizeTable(rect);
//		break;
//	case WM_DESTROY:
//		ClearSocket(&sniffer);
//		PostQuitMessage(0);
//		break;
//	default:
//
//		return DefWindowProc(hWnd, message, wParam, lParam);
//	}
//	return 0;
//
//}
//
//LRESULT CALLBACK ListViewProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//	switch (msg)
//	{
//	case WM_NOTIFY:
//	{
//		NMHDR* pNMHDR = (NMHDR*)lParam;
//
//
//		if (pNMHDR->code == LVN_ITEMCHANGED)
//		{
//			NMLISTVIEW* pNMLV = (NMLISTVIEW*)lParam;
//
//			// Проверяем, что было изменение выделения
//			if ((pNMLV->uChanged & LVIF_STATE) && (pNMLV->uNewState & LVIS_SELECTED))
//			{
//				// Вся строка должна быть выделена
//				ListView_SetItemState(hwnd, pNMLV->iItem, LVIS_SELECTED, LVIS_SELECTED);
//			}
//		}
//
//	}
//	break;
//
//	default:
//		return CallWindowProc((WNDPROC)DataTable.oldListViewProc, hwnd, msg, wParam, lParam);
//	}
//
//	// Если сообщение обработано, верните ноль
//	return 0;
//}
//
//void AddMenuToDataWindow(HWND window)
//{
//	HMENU menu = CreateMenu();
//	HMENU subMenu = CreateMenu();
//	AppendMenu(subMenu, MF_STRING, OnOpenFileMenu, L"Open File");
//	AppendMenu(subMenu, MF_STRING, OnSaveAsFileMenu, L"Save as");
//	AppendMenu(subMenu, MF_STRING, OnSaveFileMenu, L"Save");
//	AppendMenu(subMenu, MF_MENUBARBREAK, NULL, NULL);
//	AppendMenu(subMenu, MF_STRING, OnExitMenu, L"Exit");
//
//	AppendMenu(menu, MF_POPUP, (UINT_PTR)subMenu, L"File");
//	AppendMenu(menu, MF_STRING, OnStartMenu, L"Start");
//	AppendMenu(menu, MF_STRING, OnStopMenu, L"Stop");
//	AppendMenu(menu, MF_STRING, OnResetTable, L"Reset");
//
//	SetMenu(window, menu);
//}