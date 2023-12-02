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
#include "functionsHeaders.h"
#include "StructHeader.h"
#include "DefinesHeader.h"

#pragma comment(lib,"ws2_32.lib") 

//FILE* logfile;
//FILE* logfile2;
std::ofstream logfile;
std::ofstream logfile2;

int tcp = 0, udp = 0, icmp = 0, others = 0, igmp = 0, total = 0, i, j;
struct sockaddr_in source, dest;
char hex[2];
unsigned int number_of_packege = 0;
LARGE_INTEGER frequency;
LARGE_INTEGER start;
LARGE_INTEGER end;
double elapsedSeconds;
char* str;
IPV4_HDR* iphdr;
TCP_HDR* tcpheader;
UDP_HDR* udpheader;
ICMP_HDR* icmpheader;
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



void PrepareForSniffing(SOCKET* sniffer,int InterfaceNumber)
{
	struct in_addr addr;
	int in;

	char hostname[100];
	struct hostent* local;
	WSADATA wsa;

	QueryPerformanceFrequency(&frequency);
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return;
	}

	*sniffer = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
	if (*sniffer == INVALID_SOCKET)
	{
		return;
	}

	if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR)
	{
		return;
	}

	local = gethostbyname(hostname);
	if (local == NULL)
	{
		return ;
	}

	for (i = 0; local->h_addr_list[i] != 0; ++i)
	{
		memcpy(&addr, local->h_addr_list[i], sizeof(struct in_addr));
	}
	memset(&dest, 0, sizeof(dest));
	memcpy(&dest.sin_addr.s_addr, local->h_addr_list[InterfaceNumber], sizeof(dest.sin_addr.s_addr));
	dest.sin_family = AF_INET;
	dest.sin_port = 0;

	if (bind(*sniffer, (struct sockaddr*)&dest, sizeof(dest)) == SOCKET_ERROR)
	{
		return;
	}

	j = 1;
	if (WSAIoctl(*sniffer, SIO_RCVALL, &j, sizeof(j), 0, 0, (LPDWORD)&in, 0, 0) == SOCKET_ERROR)
	{
		return;
	}
}

void StartSniffing(SOCKET sniffer, char* Buffer)
{
	Buffer = (char*)malloc(65536);
	int mangobyte;

	if (Buffer == NULL)
	{
		//printf("malloc() failed.\n");
		return;
	}
	//fprintf(logfile2, "%8s| %8s| %15s| %15s| %6s| %6s\n", "Number", "Time", "Source", "Dest", "Proto", "Length");

	do
	{
		mangobyte = recvfrom(sniffer, Buffer, 65536, 0, 0, 0);

		if (mangobyte > 0)
		{
			number_of_packege++;
			//ProcessPacket(Buffer, mangobyte);
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
	mangobyte = recvfrom(Sock, Buffer, 65536, 0, 0, 0);
	if (mangobyte > 0)
	{
		//ProcessPacket(Buffer, mangobyte);
		*fullData = std::string(Buffer,mangobyte);

		//*fullData = std::to_string(reinterpret_cast<char>(Buffer));
		shortData = GetShortData(Buffer, mangobyte);

	}
	else
	{
		//printf("recvfrom() failed.\n");
	}
	return shortData;
}

void ClearSocket(SOCKET* socket)
{
	closesocket(*socket);
	WSACleanup();
}

//void ProcessPacket(char* Buffer, int Size)
//{
//	iphdr = (IPV4_HDR*)Buffer;
//	++total;
//
//	switch (iphdr->ip_protocol)
//	{
//	case ICMP:
//		++icmp;
//		PrintIcmpPacket(Buffer, Size);
//		break;
//
//	case IGMP:
//		++igmp;
//		break;
//
//	case TCP:
//		++tcp;
//		PrintTcpPacket(Buffer, Size);
//		break;
//
//	case UDP:
//		++udp;
//		PrintUdpPacket(Buffer, Size);
//		break;
//
//	default:
//		++others;
//		break;
//	}
//
//}
//
//void PrintIpHeader(char* Buffer)
//{
//	unsigned short iphdrlen;
//
//	iphdr = (IPV4_HDR*)Buffer;
//	iphdrlen = iphdr->ip_header_len * 4;
//
//	memset(&source, 0, sizeof(source));
//	source.sin_addr.s_addr = iphdr->ip_srcaddr;
//
//	memset(&dest, 0, sizeof(dest));
//	dest.sin_addr.s_addr = iphdr->ip_destaddr;
//
//	/*fprintf(logfile, "\n");
//	fprintf(logfile, "IP Header\n");
//	fprintf(logfile, " |-IP Version : %d\n", (unsigned int)iphdr->ip_version);
//	fprintf(logfile, " |-IP Header Length : %d DWORDS or %d Bytes\n", (unsigned int)iphdr->ip_header_len, ((unsigned int)(iphdr->ip_header_len)) * 4);
//	fprintf(logfile, " |-Type Of Service : %d\n", (unsigned int)iphdr->ip_tos);
//	fprintf(logfile, " |-IP Total Length : %d Bytes(Size of Packet)\n", ntohs(iphdr->ip_total_length));
//	fprintf(logfile, " |-Identification : %d\n", ntohs(iphdr->ip_id));
//	fprintf(logfile, " |-Reserved ZERO Field : %d\n", (unsigned int)iphdr->ip_reserved_zero);
//	fprintf(logfile, " |-Dont Fragment Field : %d\n", (unsigned int)iphdr->ip_dont_fragment);
//	fprintf(logfile, " |-More Fragment Field : %d\n", (unsigned int)iphdr->ip_more_fragment);
//	fprintf(logfile, " |-TTL : %d\n", (unsigned int)iphdr->ip_ttl);
//	fprintf(logfile, " |-Protocol : %d\n", (unsigned int)iphdr->ip_protocol);
//	fprintf(logfile, " |-Checksum : %d\n", ntohs(iphdr->ip_checksum));
//	fprintf(logfile, " |-Source IP : %s\n", inet_ntoa(source.sin_addr));
//	fprintf(logfile, " |-Destination IP : %s\n", inet_ntoa(dest.sin_addr));*/
//}
//
//char* GetNameByNumber(unsigned int protocol)
//{
//	char* str;
//	switch (protocol)
//	{
//	case ICMP:
//		str = (char*)malloc(4 * sizeof(char));
//		strcpy(str, "ICMP");
//		break;
//
//	case IGMP:
//		str = (char*)malloc(4 * sizeof(char));
//		strcpy(str, "IGMP");
//		break;
//
//	case TCP:
//		str = (char*)malloc(3 * sizeof(char));
//		strcpy(str, "TCP");
//		break;
//
//	case UDP:
//		str = (char*)malloc(3 * sizeof(char));
//		strcpy(str, "UDP");
//		break;
//
//	default:
//		str = (char*)malloc(5 * sizeof(char));
//		strcpy(str, "INCOR");
//		break;
//
//	}
//
//	return str;
//}
//
//
//
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
//
////void PrintShortData(char* buffer, unsigned int size)
////{
////	iphdr = (IPV4_HDR*)buffer;
////	if (number_of_packege == 1)
////	{
////		QueryPerformanceCounter(&start);
////		double elapsedSeconds = 0;
////	}
////	QueryPerformanceCounter(&end);
////	double elapsedSeconds = static_cast<double>(end.QuadPart - start.QuadPart) / frequency.QuadPart;
////	memset(&source, 0, sizeof(source));
////	source.sin_addr.s_addr = iphdr->ip_srcaddr;
////
////	memset(&dest, 0, sizeof(dest));
////	dest.sin_addr.s_addr = iphdr->ip_destaddr;
////	//fprintf(logfile2, "%8d| %.6f| %15s| %15s| %6s| %6hu\n", number_of_packege, elapsedSeconds, inet_ntoa(source.sin_addr), inet_ntoa(dest.sin_addr), GetNameByNumber((unsigned int)iphdr->ip_protocol), ntohs(iphdr->ip_total_length));
////	free(str);
////}
//
//
//void PrintTcpPacket(char* Buffer, int Size)
//{
//	unsigned short iphdrlen;
//
//	iphdr = (IPV4_HDR*)Buffer;
//	iphdrlen = iphdr->ip_header_len * 4;
//
//	tcpheader = (TCP_HDR*)(Buffer + iphdrlen);
//
//	fprintf(logfile, "\n\n***********************TCP Packet*************************\n");
//
//	PrintIpHeader(Buffer);
//
//	fprintf(logfile, "\n");
//	fprintf(logfile, "TCP Header\n");
//	fprintf(logfile, " |-Source Port : %u\n", ntohs(tcpheader->source_port));
//	fprintf(logfile, " |-Destination Port : %u\n", ntohs(tcpheader->dest_port));
//	fprintf(logfile, " |-Sequence Number : %u\n", ntohl(tcpheader->sequence));
//	fprintf(logfile, " |-Acknowledge Number : %u\n", ntohl(tcpheader->acknowledge));
//	fprintf(logfile, " |-Header Length : %d DWORDS or %d BYTES\n"
//		, (unsigned int)tcpheader->data_offset, (unsigned int)tcpheader->data_offset * 4);
//	fprintf(logfile, " |-CWR Flag : %d\n", (unsigned int)tcpheader->cwr);
//	fprintf(logfile, " |-ECN Flag : %d\n", (unsigned int)tcpheader->ecn);
//	fprintf(logfile, " |-Urgent Flag : %d\n", (unsigned int)tcpheader->urg);
//	fprintf(logfile, " |-Acknowledgement Flag : %d\n", (unsigned int)tcpheader->ack);
//	fprintf(logfile, " |-Push Flag : %d\n", (unsigned int)tcpheader->psh);
//	fprintf(logfile, " |-Reset Flag : %d\n", (unsigned int)tcpheader->rst);
//	fprintf(logfile, " |-Synchronise Flag : %d\n", (unsigned int)tcpheader->syn);
//	fprintf(logfile, " |-Finish Flag : %d\n", (unsigned int)tcpheader->fin);
//	fprintf(logfile, " |-Window : %d\n", ntohs(tcpheader->window));
//	fprintf(logfile, " |-Checksum : %d\n", ntohs(tcpheader->checksum));
//	fprintf(logfile, " |-Urgent Pointer : %d\n", tcpheader->urgent_pointer);
//	fprintf(logfile, "\n");
//	fprintf(logfile, " DATA Dump ");
//	fprintf(logfile, "\n");
//	fprintf(logfile, "IP Header\n");
//	PrintData(Buffer, iphdrlen);
//
//	fprintf(logfile, "TCP Header\n");
//	PrintData(Buffer + iphdrlen, tcpheader->data_offset * 4);
//
//	fprintf(logfile, "Data Payload\n");
//	PrintData(Buffer + iphdrlen + tcpheader->data_offset * 4
//		, (Size - tcpheader->data_offset * 4 - iphdr->ip_header_len * 4));
//
//	fprintf(logfile, "\n###########################################################");
//}
//
//void PrintUdpPacket(char* Buffer, int Size)
//{
//	unsigned short iphdrlen;
//
//	iphdr = (IPV4_HDR*)Buffer;
//	iphdrlen = iphdr->ip_header_len * 4;
//
//	udpheader = (UDP_HDR*)(Buffer + iphdrlen);
//
//	fprintf(logfile, "\n\n***********************UDP Packet*************************\n");
//
//	PrintIpHeader(Buffer);
//
//	fprintf(logfile, "\nUDP Header\n");
//	fprintf(logfile, " |-Source Port : %d\n", ntohs(udpheader->source_port));
//	fprintf(logfile, " |-Destination Port : %d\n", ntohs(udpheader->dest_port));
//	fprintf(logfile, " |-UDP Length : %d\n", ntohs(udpheader->udp_length));
//	fprintf(logfile, " |-UDP Checksum : %d\n", ntohs(udpheader->udp_checksum));
//
//	fprintf(logfile, "\n");
//	fprintf(logfile, "IP Header\n");
//
//	PrintData(Buffer, iphdrlen);
//	fprintf(logfile, "UDP Header\n");
//
//	PrintData(Buffer + iphdrlen, sizeof(UDP_HDR));
//
//	fprintf(logfile, "Data Payload\n");
//
//	PrintData(Buffer + iphdrlen + sizeof(UDP_HDR), (Size - sizeof(UDP_HDR) - iphdr->ip_header_len * 4));
//
//	fprintf(logfile, "\n###########################################################");
//}
//
//void PrintIcmpPacket(char* Buffer, int Size)
//{
//	unsigned short iphdrlen;
//
//	iphdr = (IPV4_HDR*)Buffer;
//	iphdrlen = iphdr->ip_header_len * 4;
//
//	icmpheader = (ICMP_HDR*)(Buffer + iphdrlen);
//
//	fprintf(logfile, "\n\n***********************ICMP Packet*************************\n");
//	PrintIpHeader(Buffer);
//
//	fprintf(logfile, "\n");
//
//	fprintf(logfile, "ICMP Header\n");
//	fprintf(logfile, " |-Type : %d", (unsigned int)(icmpheader->type));
//
//	if ((unsigned int)(icmpheader->type) == 11)
//	{
//		fprintf(logfile, " (TTL Expired)\n");
//	}
//	else if ((unsigned int)(icmpheader->type) == 0)
//	{
//		fprintf(logfile, " (ICMP Echo Reply)\n");
//	}
//
//	fprintf(logfile, " |-Code : %d\n", (unsigned int)(icmpheader->code));
//	fprintf(logfile, " |-Checksum : %d\n", ntohs(icmpheader->checksum));
//	fprintf(logfile, " |-ID : %d\n", ntohs(icmpheader->id));
//	fprintf(logfile, " |-Sequence : %d\n", ntohs(icmpheader->seq));
//	fprintf(logfile, "\n");
//
//	fprintf(logfile, "IP Header\n");
//	PrintData(Buffer, iphdrlen);
//
//	fprintf(logfile, "UDP Header\n");
//	PrintData(Buffer + iphdrlen, sizeof(ICMP_HDR));
//
//	fprintf(logfile, "Data Payload\n");
//	PrintData(Buffer + iphdrlen + sizeof(ICMP_HDR), (Size - sizeof(ICMP_HDR) - iphdr->ip_header_len * 4));
//
//	fprintf(logfile, "\n###########################################################");
//}
//
//void PrintData(char* data, int Size)
//{
//	char a, line[17], c;
//	int j;
//
//	//loop over each character and print
//	for (i = 0; i < Size; i++)
//	{
//		c = data[i];
//
//		//Print the hex value for every character , with a space. Important to make unsigned
//		fprintf(logfile, " %.2x", (unsigned char)c);
//
//		//Add the character to data line. Important to make unsigned
//		a = (c >= 32 && c <= 128) ? (unsigned char)c : '.';
//
//		line[i % 16] = a;
//
//		//if last character of a line , then print the line - 16 characters in 1 line
//		if ((i != 0 && (i + 1) % 16 == 0) || i == Size - 1)
//		{
//			line[i % 16 + 1] = '\0';
//
//			//print a big gap of 10 characters between hex and characters
//			fprintf(logfile, "          ");
//
//			//Print additional spaces for last lines which might be less than 16 characters in length
//			for (j = strlen(line); j < 16; j++)
//			{
//				fprintf(logfile, "   ");
//			}
//
//			fprintf(logfile, "%s \n", line);
//		}
//	}
//
//	fprintf(logfile, "\n");
//}



