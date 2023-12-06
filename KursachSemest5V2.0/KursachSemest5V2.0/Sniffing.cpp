#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include "stdio.h"
#include "winsock2.h"
#include <string>
#include <string.h>
#include <stdlib.h>
#include <iomanip>
#include <fstream>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include "functionsHeaders.h"
#include "StructHeader.h"
#include "DefinesHeader.h"

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib,"ws2_32.lib") 

struct sockaddr_in source, dest;
unsigned int number_of_packege = 0;
LARGE_INTEGER frequency;
LARGE_INTEGER start;
LARGE_INTEGER end;
double elapsedSeconds;
IPV4_HDR* iphdr;
TCP_HDR* tcpheader;
UDP_HDR* udpheader;
ICMP_HDR* icmpheader;
CRITICAL_SECTION critical;

hostent GetLocalInterfaces()
{
	struct hostent* local;
	WSADATA wsa;
	char hostname[100];
	WSAStartup(MAKEWORD(2, 2), &wsa);
	gethostname(hostname, sizeof(hostname));
	local = gethostbyname(hostname);
	WSACleanup();
	return *local;
}

std::vector<sockaddr_in> GetAllInterfaces()
{
	std::vector<sockaddr_in> addresses;
	ULONG bufferSize = 0;
	PIP_ADAPTER_ADDRESSES pAddresses = nullptr;

	DWORD result = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, nullptr,
		nullptr, &bufferSize);

	if (result == ERROR_BUFFER_OVERFLOW)
	{
		pAddresses = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(new char[bufferSize]);
		result = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, nullptr,pAddresses, &bufferSize);

		if (result == NO_ERROR)
		{
			PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses;
			while (pCurrAddresses)
			{
				IP_ADAPTER_UNICAST_ADDRESS* pUnicast = pCurrAddresses->FirstUnicastAddress;
				while (pUnicast)
				{
					SOCKADDR* pAddr = pUnicast->Address.lpSockaddr;
					if (pAddr->sa_family == AF_INET)
					{
						sockaddr_in* pSockAddrIn = reinterpret_cast<sockaddr_in*>(pAddr);
						addresses.push_back(*pSockAddrIn);
					}

					pUnicast = pUnicast->Next;
				}

				pCurrAddresses = pCurrAddresses->Next;
			}
		}
		else
	
		delete[] reinterpret_cast<char*>(pAddresses);
	}
	

	return addresses;
}

//void PrepareForSniffing(SOCKET* sniffer,int InterfaceNumber)
//{
//	struct in_addr addr;
//	int in;
//
//	char hostname[100];
//	struct hostent* local;
//	WSADATA wsa;
//	
//
//	QueryPerformanceFrequency(&frequency);
//	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
//	{
//		return;
//	}
//
//	*sniffer = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
//	if (*sniffer == INVALID_SOCKET)
//	{
//		return;
//	}
//
//	if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR)
//	{
//		return;
//	}
//
//	local = gethostbyname(hostname);
//	if (local == NULL)
//	{
//		return ;
//	}
//
//	for (i = 0; local->h_addr_list[i] != 0; ++i)
//	{
//		memcpy(&addr, local->h_addr_list[i], sizeof(struct in_addr));
//	}
//	memset(&dest, 0, sizeof(dest));
//	memcpy(&dest.sin_addr.s_addr, local->h_addr_list[InterfaceNumber], sizeof(dest.sin_addr.s_addr));
//	dest.sin_family = AF_INET;
//	dest.sin_port = 0;
//
//	if (bind(*sniffer, (struct sockaddr*)&dest, sizeof(dest)) == SOCKET_ERROR)
//	{
//		return;
//	}
//
//	j = 1;
//	if (WSAIoctl(*sniffer, SIO_RCVALL, &j, sizeof(j), 0, 0, (LPDWORD)&in, 0, 0) == SOCKET_ERROR)
//	{
//		return;
//	}
//}

std::string PrepareForSniffing(SOCKET* sniffer,int InterfaceNumber)
{
	struct in_addr addr;
	int in;
	//InitializeCriticalSection(&critical);
	char hostname[100];
	struct hostent* local;
	WSADATA wsa;
	
	std::vector<sockaddr_in> addresess = GetAllInterfaces();
	QueryPerformanceFrequency(&frequency);
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return "1";
	}

	*sniffer = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
	if (*sniffer == INVALID_SOCKET)
	{
		return "2";
	}

	/*if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR)
	{
		return;
	}

	local = gethostbyname(hostname);
	if (local == NULL)
	{
		return ;
	}*/

	//for (i = 0; local->h_addr_list[i] != 0; ++i)
	//{
	//	memcpy(&addr, local->h_addr_list[i], sizeof(struct in_addr));
	//}
	//memset(&dest, 0, sizeof(dest));
	//memcpy(&dest.sin_addr.s_addr, local->h_addr_list[InterfaceNumber], sizeof(dest.sin_addr.s_addr));
	//dest.sin_family = AF_INET;
	//dest.sin_port = 0;

	if (bind(*sniffer, (struct sockaddr*)&addresess[InterfaceNumber], sizeof(sockaddr)) == SOCKET_ERROR)
	{
		int in = WSAGetLastError();
		return "Incorrect ip or not permission";
	}

	int j = 1;
	if (WSAIoctl(*sniffer, SIO_RCVALL, &j, sizeof(j), 0, 0, (LPDWORD)&InterfaceNumber, 0, 0) == SOCKET_ERROR)
	{
		return "3";
	}

	return "correct";
}

void StartSniffing(SOCKET sniffer, char* Buffer)
{
	Buffer = (char*)malloc(65536);
	int mangobyte;

	if (Buffer == NULL)
	{
		return;
	}
	do
	{
		mangobyte = recvfrom(sniffer, Buffer, 65536, 0, 0, 0);

		if (mangobyte > 0)
		{
			number_of_packege++;
		}
		else
		{
			printf("recvfrom() failed.\n");
		}
	} while (mangobyte > 0);

	free(Buffer);
}

std::string SniffOnePackeg(SOCKET Sock, char* Buffer, std::string* fullData)
{
	int mangobyte = 0;
	std::string shortData;
	//EnterCriticalSection(&critical);
	mangobyte = recvfrom(Sock, Buffer, 65536, 0, 0, 0);
	//LeaveCriticalSection(&critical);
	if (mangobyte > 0)
	{
		*fullData = std::string(Buffer,mangobyte);
		shortData = GetShortData(Buffer, mangobyte);

	}
	
	return shortData;
}

void ClearSocket(SOCKET* socket)
{
	//DeleteCriticalSection(&critical);
	closesocket(*socket);
	WSACleanup();
}


std::string GetShortData(char* buffer, unsigned int size)
{
	number_of_packege++;

	std::string shortData = "";
	iphdr = (IPV4_HDR*)buffer;
	if (number_of_packege == 1)
	{
		QueryPerformanceCounter(&start);
		double elapsedSeconds = 0;
	}

	QueryPerformanceCounter(&end);
	double elapsedSeconds = static_cast<double>(end.QuadPart - start.QuadPart) / frequency.QuadPart;
	memset(&source, 0, sizeof(source));
	source.sin_addr.s_addr = iphdr->ip_srcaddr;

	memset(&dest, 0, sizeof(dest));
	dest.sin_addr.s_addr = iphdr->ip_destaddr;

	std::string source_addr = inet_ntoa(source.sin_addr);
	std::string dest_addr = inet_ntoa(dest.sin_addr);

	shortData.append(std::to_string(number_of_packege));
	shortData.append(",");
	shortData.append(std::to_string(elapsedSeconds));
	shortData.append(",");
	shortData.append(source_addr);
	shortData.append(",");
	shortData.append(dest_addr);
	shortData.append(",");
	shortData.append(GetNameByNumber((unsigned int)iphdr->ip_protocol));
	shortData.append(",");
	shortData.append(std::to_string(ntohs(iphdr->ip_total_length)));
	
	return shortData;
}


