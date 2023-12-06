
#pragma once
#include <winsock2.h>
#include <string>
#include <vector>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#pragma comment(lib, "iphlpapi.lib")

std::string SniffOnePackeg(SOCKET Sock, char* Buffer, std::string* fullData);
std::string GetShortData(char* buffer, unsigned int size);
void ClearSocket(SOCKET*);
std::string PrepareForSniffing(SOCKET*, int);
hostent GetLocalInterfaces();
DWORD WINAPI Sniffing(LPVOID lpParam);
std::string GetNameByNumber(unsigned int);
void AddMenuToDataWindow(HWND window);
std::vector<sockaddr_in> GetAllInterfaces();
void SaveListTofile(std::vector<std::string>& strings,const std::string& filename);
std::vector<std::string> LoadListFromFile(std::string filename);
void InsertFullListIntoTable(std::vector<std::string> list);
const std::string TCHARToString(const TCHAR* tcharString);
std::string GetFileName(DWORD);