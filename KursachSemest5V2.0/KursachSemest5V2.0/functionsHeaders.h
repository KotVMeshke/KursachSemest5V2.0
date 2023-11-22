#include <winsock2.h>
#include <string>
#pragma once

void StartSniffing(SOCKET Sock);
std::string SniffOnePackeg(SOCKET Sock, char* Buffer);
std::string GetShortData(char* buffer, unsigned int size);
void ClearSocket(SOCKET*);

void PrepareForSniffing(SOCKET*);
//void SaveFile();
//void ProcessPacket(char*, int);
//void PrintIpHeader(char*);
//void PrintIcmpPacket(char*, int);
//void PrintUdpPacket(char*, int);
//void PrintTcpPacket(char*, int);
//void ConvertToHex(char*, unsigned int);
char* GetNameByNumber(unsigned int);
//void PrintShortData(char*, unsigned int);
//void PrintData(char*, int);

void AddMenuToWindow(HWND window);