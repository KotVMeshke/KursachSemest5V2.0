#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "stdio.h"
#include "winsock2.h"
#include <Windows.h>
#include <CommCtrl.h>
#include "commctrl.h"
#include <vector>
#include <tchar.h>
#include <string>
#include "StructHeader.h"
#include "DefinesHeader.h"

#pragma comment(lib,"ws2_32.lib") 


std::vector<HWND> CreateButtons(HWND hwnd, hostent local);
