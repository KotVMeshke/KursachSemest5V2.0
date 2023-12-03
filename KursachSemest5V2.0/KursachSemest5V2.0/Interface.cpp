#include "Interface.h"

//std::vector<HWND> CreateButtons(HWND hwnd, hostent local)
//{
//    std::vector<HWND> buttons;
//    RECT clientRect;
//    GetClientRect(hwnd, &clientRect);
//    int s = (local.h_length + 2) * START_BUTTON_HEIGHT;
//    SetWindowPos(hwnd, NULL, 0, 0, DEFAULT_WINDOW_START_WIDTH, (local.h_length+6) * START_BUTTON_HEIGHT, SWP_NOMOVE | SWP_NOZORDER);
//    GetClientRect(hwnd, &clientRect);
//    int windowWidth = clientRect.right - clientRect.left;
//    int windowHeight = clientRect.bottom - clientRect.top;
//
//    int buttonWidth = START_BUTTON_WIDTH;
//    int buttonHeight = START_BUTTON_HEIGHT;
//
//    int centerX = (windowWidth - buttonWidth) / 2;
//    int centerY = (windowHeight - buttonHeight * local.h_length) / 2;
//    HWND staticText = CreateWindow(L"STATIC", L"Select Interface", WS_VISIBLE | WS_CHILD | SS_CENTER, centerX, centerY - 30, buttonWidth, buttonHeight, hwnd, NULL, NULL, NULL);
//
//    HFONT hFont = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
//    SendMessage(staticText, WM_SETFONT, (WPARAM)hFont, TRUE);
//    in_addr addr;
//    for (int i = 0; local.h_addr_list[i] != 0; ++i)
//    {
//        memcpy(&addr, local.h_addr_list[i], sizeof(struct in_addr));
//        std::string buttonText = inet_ntoa(addr);
//        std::wstring wideButtonText(buttonText.begin(), buttonText.end());
//        LPCWSTR buttonWindowText = wideButtonText.c_str();
//        int x = centerX;
//        int y = centerY + i * buttonHeight + BUTTONS_OFFSET;
//
//         buttons.push_back(CreateWindow(L"BUTTON", buttonWindowText, WS_VISIBLE | WS_CHILD, x, y, buttonWidth, buttonHeight, hwnd, (HMENU)(INTERFACE_SELECTORS + i), NULL, NULL));
//    }
//    return buttons;
//}

std::vector<HWND> CreateButtons(HWND hwnd, std::vector<sockaddr_in> local)
{
    std::vector<HWND> buttons;
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    int s = (local.size() + 2) * START_BUTTON_HEIGHT;
    SetWindowPos(hwnd, NULL, 0, 0, DEFAULT_WINDOW_START_WIDTH, (local.size() + 6) * START_BUTTON_HEIGHT, SWP_NOMOVE | SWP_NOZORDER);
    GetClientRect(hwnd, &clientRect);
    int windowWidth = clientRect.right - clientRect.left;
    int windowHeight = clientRect.bottom - clientRect.top;

    int buttonWidth = START_BUTTON_WIDTH;
    int buttonHeight = START_BUTTON_HEIGHT;

    int centerX = (windowWidth - buttonWidth) / 2;
    int centerY = (windowHeight - buttonHeight * local.size()) / 2;
    HWND staticText = CreateWindow(L"STATIC", L"Select Interface", WS_VISIBLE | WS_CHILD | SS_CENTER, centerX, centerY - 30, buttonWidth, buttonHeight, hwnd, NULL, NULL, NULL);

    HFONT hFont = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
    SendMessage(staticText, WM_SETFONT, (WPARAM)hFont, TRUE);
    in_addr addr;
    for (int i = 0;i < local.size(); i++)
    {
        //memcpy(&addr, local[i], sizeof(struct in_addr));
        char ipAddress[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(local[i].sin_addr), ipAddress, INET_ADDRSTRLEN);
        std::string buttonText = ipAddress;
        std::wstring wideButtonText(buttonText.begin(), buttonText.end());
        LPCWSTR buttonWindowText = wideButtonText.c_str();
        int x = centerX;
        int y = centerY + i * buttonHeight + BUTTONS_OFFSET;

        buttons.push_back(CreateWindow(L"BUTTON", buttonWindowText, WS_VISIBLE | WS_CHILD, x, y, buttonWidth, buttonHeight, hwnd, (HMENU)(INTERFACE_SELECTORS + i), NULL, NULL));
    }
    return buttons;
}