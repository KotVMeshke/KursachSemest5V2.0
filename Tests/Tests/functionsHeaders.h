#include <winsock2.h>
#pragma once

void StartSniffing(SOCKET Sock);

void PrepareForSniffing();
void ProcessPacket(char*, int);
void PrintIpHeader(char*);
void PrintIcmpPacket(char*, int);
void PrintUdpPacket(char*, int);
void PrintTcpPacket(char*, int);
void ConvertToHex(char*, unsigned int);
char* GetNameByNumber(unsigned int);
void PrintShortData(char*, unsigned int);
void PrintData(char*, int);