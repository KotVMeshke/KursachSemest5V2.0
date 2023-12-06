#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "functionsHeaders.h"

#include <Windows.h>
#include <CommCtrl.h>
#include "commctrl.h"
#include <fstream>
#include "Table.h"
#include "definesHeader.h"

HWND TextBox;

void PrintData(char* data, int Size);

void PrintToTextBox(const wchar_t* text)
{
	if (TextBox != nullptr)
	{
		int length = GetWindowTextLength(TextBox);
		SendMessage(TextBox, EM_SETSEL, length, length); 
		SendMessage(TextBox, EM_REPLACESEL, FALSE, reinterpret_cast<LPARAM>(text));
	}
}

void PrintToTextBoxWithNewLine(const char* text)
{
	int size = MultiByteToWideChar(CP_UTF8, 0, text, -1, NULL, 0);
	wchar_t* output = (wchar_t*)malloc(size * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, text, -1, output, size);
	std::wstring textWithNewLine = std::wstring(output) + L"\r\n";
	PrintToTextBox(textWithNewLine.c_str());
}

void PrintIPHeaderInfo(char* buffer)
{
	PrintToTextBoxWithNewLine("IP Header");
	unsigned short iphdrlen;
	sockaddr_in source, dest;
	IPV4_HDR* iphdr = (IPV4_HDR*)buffer;
	iphdrlen = iphdr->ip_header_len * 4;

	memset(&source, 0, sizeof(source));
	source.sin_addr.s_addr = iphdr->ip_srcaddr;

	memset(&dest, 0, sizeof(dest));
	dest.sin_addr.s_addr = iphdr->ip_destaddr;

	PrintToTextBoxWithNewLine(std::string(" -IP Version : " + std::to_string((unsigned int)iphdr->ip_version)).c_str());
	PrintToTextBoxWithNewLine(std::string(" -IP Header Length : " + std::to_string((unsigned int)iphdr->ip_header_len) + " DWORDS or " + std::to_string(((unsigned int)(iphdr->ip_header_len)) * 4) + " Bytes").c_str());
	PrintToTextBoxWithNewLine(std::string(" -Type Of Service : " + std::to_string((unsigned int)iphdr->ip_tos)).c_str());
	PrintToTextBoxWithNewLine(std::string(" -IP Total Length : " + std::to_string(ntohs(iphdr->ip_total_length)) + " Bytes(Size of Packet)").c_str());
	PrintToTextBoxWithNewLine(std::string(" -Identification : " + std::to_string(ntohs(iphdr->ip_id))).c_str());
	PrintToTextBoxWithNewLine(std::string(" -Reserved ZERO Field : " + std::to_string((unsigned int)iphdr->ip_reserved_zero)).c_str());
	PrintToTextBoxWithNewLine(std::string(" -Dont Fragment Field : " + std::to_string((unsigned int)iphdr->ip_dont_fragment)).c_str());
	PrintToTextBoxWithNewLine(std::string(" -More Fragment Field : " + std::to_string((unsigned int)iphdr->ip_more_fragment)).c_str());
	PrintToTextBoxWithNewLine(std::string(" -TTL : " + std::to_string((unsigned int)iphdr->ip_ttl)).c_str());
	PrintToTextBoxWithNewLine(std::string(" -Protocol : " + std::to_string((unsigned int)iphdr->ip_protocol)).c_str());
	PrintToTextBoxWithNewLine(std::string(" -Checksum : " + std::to_string(ntohs(iphdr->ip_checksum))).c_str());
	PrintToTextBoxWithNewLine(std::string(" -Source IP : " + std::string(inet_ntoa(source.sin_addr))).c_str());
	PrintToTextBoxWithNewLine(std::string(" -Destination IP : " + std::string(inet_ntoa(dest.sin_addr))).c_str());
}

void ResizeTextBox(HWND parentWindow, HWND TextBox)
{
	RECT rect = {};
	GetWindowRect(parentWindow, &rect);
	MoveWindow(TextBox, 0, 0, rect.right - rect.left, rect.bottom - rect.top - 50, TRUE);
}

void SetTextViewFont(HWND textView, const wchar_t* fontName, int fontSize)
{
	HFONT newFont = CreateFont(
		fontSize,                     
		0,                          
		0,                            
		0,                           
		FW_NORMAL,                   
		FALSE,                       
		FALSE,                        
		FALSE,                       
		DEFAULT_CHARSET,             
		OUT_DEFAULT_PRECIS,           
		CLIP_DEFAULT_PRECIS,          
		DEFAULT_QUALITY,              
		DEFAULT_PITCH | FF_DONTCARE,
		fontName    
	);

	SendMessage(textView, WM_SETFONT, reinterpret_cast<WPARAM>(newFont), TRUE);
	InvalidateRect(textView, nullptr, TRUE);
}

HWND CreateTextView(HWND parentWindow, HINSTANCE hInstance)
{
	RECT rect = {};
	GetWindowRect(parentWindow, &rect);
	HWND textPanel = CreateWindowEx(0, L"EDIT", nullptr, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_READONLY, 0, 0, rect.right - rect.left, rect.bottom - rect.top-50, parentWindow, nullptr, hInstance, nullptr);
	return textPanel;
}

void AddMenuToInformWindow(HWND window)
{
	HMENU menu = CreateMenu();
	HMENU fileMenu = CreateMenu();
	AppendMenu(fileMenu, MF_STRING, OnSavePackageData, L"Save Data");
	AppendMenu(fileMenu, MF_MENUBARBREAK, NULL, NULL);
	AppendMenu(fileMenu, MF_STRING, OnExitMenu, L"Close");

	AppendMenu(menu, MF_POPUP, (UINT_PTR)fileMenu, L"File");

	SetMenu(window, menu);
}

void SafePackageToFile(std::string fileName, std::string package)
{
	std::ofstream outFile(fileName);
	package.erase(std::remove(package.begin(), package.end(), '\n'), package.end());
	if (outFile.is_open())
	{
		outFile.write(package.c_str(), package.length());
	}
}

std::string GetTextBoxString()
{
	int textLength = GetWindowTextLength(TextBox);

	if (textLength > 0) {
		TCHAR* buffer = new TCHAR[textLength + 1];
		GetWindowText(TextBox, buffer, textLength + 1);
		std::string text = TCHARToString(buffer);
		delete[] buffer;
		return text;
	}
	else {
		return "";
	}
}

void ProcessPacket(char* Buffer, int Size);

LRESULT CALLBACK WndInformProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
		{
			SendMessage(hWnd, WM_DESTROY, 0, 0);
		}
		case OnSavePackageData:
		{
			SafePackageToFile(GetFileName(OFN_PATHMUSTEXIST), GetTextBoxString());
		}
		}
		break;
	case WM_SHOW_PACKAGE:
	{
		SendMessage(TextBox, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(L""));
		ProcessPacket(reinterpret_cast<char*>(wParam), lParam);
		break;
	}
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &paint);

		EndPaint(hWnd, &paint);
		break;
	case WM_CREATE:
	{
		HINSTANCE hInst = GetModuleHandle(nullptr);
		TextBox = CreateTextView(hWnd, hInst);
		SetTextViewFont(TextBox, L"Lucida Console", 14);
		AddMenuToInformWindow(hWnd);

		break;
	}
	case WM_SIZE:
		ResizeTextBox(hWnd, TextBox);
		break;
	case WM_DESTROY:
		ShowWindow(hWnd, SW_HIDE);
		break;
	default:

		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;

}
void PrintUdpPacket(char* Buffer, int Size);
void PrintIcmpPacket(char* Buffer, int Size);
void PrintTcpPacket(char* Buffer, int Size);
void PrintIgmpPacket(char* Buffer, int Size);
void ProcessPacket(char* Buffer, int Size)
{
	IPV4_HDR* iphdr = (IPV4_HDR*)Buffer;

	switch (iphdr->ip_protocol)
	{
	case ICMP:
		PrintIcmpPacket(Buffer, Size);
		break;

	case IGMP:
		PrintIgmpPacket(Buffer, Size);
		break;

	case TCP:
		PrintTcpPacket(Buffer, Size);
		break;

	case UDP:
		PrintUdpPacket(Buffer, Size);
		break;

	default:
		break;
	}

}


std::string GetNameByNumber(unsigned int protocol)
{
	std::string str;
	switch (protocol)
	{
	case ICMP:
		str = "ICMP";
		break;

	case IGMP:
		str = "IGMP";
		break;

	case TCP:
		str = "TCP";
		break;

	case UDP:
		str = "UDP";
		break;

	default:
		str = "OTHER";
		break;

	}

	return str;
}



void PrintTcpPacket(char* Buffer, int Size)
{
	unsigned short iphdrlen;

	IPV4_HDR* iphdr = (IPV4_HDR*)Buffer;
	iphdrlen = iphdr->ip_header_len * 4;

	TCP_HDR* tcpheader = (TCP_HDR*)(Buffer + iphdrlen);

	PrintIPHeaderInfo(Buffer);

	PrintToTextBoxWithNewLine("");
	PrintToTextBoxWithNewLine("TCP Header");
	PrintToTextBoxWithNewLine(std::string(" -Source Port : " + std::to_string(ntohs(tcpheader->source_port))).c_str());
	PrintToTextBoxWithNewLine(std::string(" -Destination Port : " + std::to_string(ntohs(tcpheader->dest_port))).c_str());
	PrintToTextBoxWithNewLine(std::string(" -Sequence Number : " + std::to_string(ntohl(tcpheader->sequence))).c_str());
	PrintToTextBoxWithNewLine(std::string(" -Acknowledge Number : " + std::to_string(ntohl(tcpheader->acknowledge))).c_str());
	PrintToTextBoxWithNewLine(std::string(" -Header Length : " + std::to_string((unsigned int)tcpheader->data_offset) + " DWORDS or " + std::to_string((unsigned int)tcpheader->data_offset * 4) + " BYTES").c_str());
	PrintToTextBoxWithNewLine(std::string(" -CWR Flag : " + std::to_string((unsigned int)tcpheader->cwr)).c_str());
	PrintToTextBoxWithNewLine(std::string(" -ECN Flag : " + std::to_string((unsigned int)tcpheader->ecn)).c_str());
	PrintToTextBoxWithNewLine(std::string(" -Urgent Flag : " + std::to_string((unsigned int)tcpheader->urg)).c_str());
	PrintToTextBoxWithNewLine(std::string(" -Acknowledgement Flag : " + std::to_string((unsigned int)tcpheader->ack)).c_str());
	PrintToTextBoxWithNewLine(std::string(" -Push Flag : " + std::to_string((unsigned int)tcpheader->psh)).c_str());
	PrintToTextBoxWithNewLine(std::string(" -Reset Flag : " + std::to_string((unsigned int)tcpheader->rst)).c_str());
	PrintToTextBoxWithNewLine(std::string(" -Synchronise Flag : " + std::to_string((unsigned int)tcpheader->syn)).c_str());
	PrintToTextBoxWithNewLine(std::string(" -Finish Flag : " + std::to_string((unsigned int)tcpheader->fin)).c_str());
	PrintToTextBoxWithNewLine(std::string(" -Window : " + std::to_string(ntohs(tcpheader->window))).c_str());
	PrintToTextBoxWithNewLine(std::string(" -Checksum : " + std::to_string(ntohs(tcpheader->checksum))).c_str());
	PrintToTextBoxWithNewLine(std::string(" -Urgent Pointer : " + std::to_string(tcpheader->urgent_pointer)).c_str());

	PrintToTextBoxWithNewLine("");
	PrintToTextBoxWithNewLine(" DATA Dump ");
	PrintToTextBoxWithNewLine("");

	PrintToTextBoxWithNewLine("IP Header");
	PrintData(Buffer, iphdrlen);

	PrintToTextBoxWithNewLine("TCP Header");
	PrintData(Buffer + iphdrlen, tcpheader->data_offset * 4);

	PrintToTextBoxWithNewLine("Data Payload");
	PrintData(Buffer + iphdrlen + tcpheader->data_offset * 4, (Size - tcpheader->data_offset * 4 - iphdr->ip_header_len * 4));
}

void PrintUdpPacket(char* Buffer, int Size)
{
	unsigned short iphdrlen;

	IPV4_HDR* iphdr = (IPV4_HDR*)Buffer;
	iphdrlen = iphdr->ip_header_len * 4;

	UDP_HDR* udpheader = (UDP_HDR*)(Buffer + iphdrlen);

	PrintIPHeaderInfo(Buffer);

	PrintToTextBoxWithNewLine("UDP Header");
	PrintToTextBoxWithNewLine(std::string(" |-Source Port : " + std::to_string(ntohs(udpheader->source_port))).c_str());
	PrintToTextBoxWithNewLine(std::string(" |-Destination Port : " + std::to_string(ntohs(udpheader->dest_port))).c_str());
	PrintToTextBoxWithNewLine(std::string(" |-UDP Length : " + std::to_string(ntohs(udpheader->udp_length))).c_str());
	PrintToTextBoxWithNewLine(std::string(" |-UDP Checksum : " + std::to_string(ntohs(udpheader->udp_checksum))).c_str());

	PrintToTextBoxWithNewLine("");
	PrintToTextBoxWithNewLine("IP Header");

	PrintData(Buffer, iphdrlen);

	PrintToTextBoxWithNewLine("UDP Header");
	PrintData(Buffer + iphdrlen, sizeof(UDP_HDR));

	PrintToTextBoxWithNewLine("Data Payload");
	PrintData(Buffer + iphdrlen + sizeof(UDP_HDR), (Size - sizeof(UDP_HDR) - iphdr->ip_header_len * 4));
}

void PrintIcmpPacket(char* Buffer, int Size)
{
	unsigned short iphdrlen;

	IPV4_HDR* iphdr = (IPV4_HDR*)Buffer;
	iphdrlen = iphdr->ip_header_len * 4;

	ICMP_HDR* icmpheader = (ICMP_HDR*)(Buffer + iphdrlen);

	PrintIPHeaderInfo(Buffer);

	PrintToTextBoxWithNewLine("ICMP Header");
	PrintToTextBoxWithNewLine(std::string(" |-Type : " + std::to_string((unsigned int)(icmpheader->type))).c_str());

	if ((unsigned int)(icmpheader->type) == 11)
	{
		PrintToTextBoxWithNewLine(" (TTL Expired)");
	}
	else if ((unsigned int)(icmpheader->type) == 0)
	{
		PrintToTextBoxWithNewLine(" (ICMP Echo Reply)");
	}

	PrintToTextBoxWithNewLine(std::string(" |-Code : " + std::to_string((unsigned int)(icmpheader->code))).c_str());
	PrintToTextBoxWithNewLine(std::string(" |-Checksum : " + std::to_string(ntohs(icmpheader->checksum))).c_str());
	PrintToTextBoxWithNewLine(std::string(" |-ID : " + std::to_string(ntohs(icmpheader->id))).c_str());
	PrintToTextBoxWithNewLine(std::string(" |-Sequence : " + std::to_string(ntohs(icmpheader->seq))).c_str());

	PrintToTextBoxWithNewLine("");

	PrintToTextBoxWithNewLine("IP Header");
	PrintData(Buffer, iphdrlen);

	PrintToTextBoxWithNewLine("UDP Header");
	PrintData(Buffer + iphdrlen, sizeof(ICMP_HDR));

	PrintToTextBoxWithNewLine("Data Payload");
	PrintData(Buffer + iphdrlen + sizeof(ICMP_HDR), (Size - sizeof(ICMP_HDR) - iphdr->ip_header_len * 4));
}

void PrintIgmpPacket(char* Buffer, int Size)
{
	unsigned short iphdrlen;

	sockaddr_in addr;
	IPV4_HDR* iphdr = (IPV4_HDR*)Buffer;
	iphdrlen = iphdr->ip_header_len * 4;

	IGMP_HDR* igmpheader = (IGMP_HDR*)(Buffer + iphdrlen);


	memset(&addr, 0, sizeof(addr));
	addr.sin_addr.s_addr = igmpheader->group_address;

	PrintIPHeaderInfo(Buffer);

	PrintToTextBoxWithNewLine("IGMP Header");
	PrintToTextBoxWithNewLine(std::string(" |-Message type : " + std::to_string((unsigned int)(igmpheader->message_type))).c_str());
	PrintToTextBoxWithNewLine(std::string(" |-Max resp time : " + std::to_string((unsigned int)(igmpheader->max_resp_time))).c_str());

	switch ((igmpheader->message_type))
	{
	case 0x11:
	{
		PrintToTextBoxWithNewLine(" (Membership Query) ");
		break;
	}
	case 0x12:
	{
		PrintToTextBoxWithNewLine(" (Membership Report V1) ");
		break;
	}
	case 0x16:
	{
		PrintToTextBoxWithNewLine(" (Membership Report V2) ");
		break;
	}case 0x22:
	{
		PrintToTextBoxWithNewLine(" (Membership Report V3) ");
		break;
	}case 0x17:
	{
		PrintToTextBoxWithNewLine(" (Leave Group) ");
		break;
	}
	default:
	{		
		PrintToTextBoxWithNewLine(" (Incorrect message) ");
		break;
	}

	}
	

	PrintToTextBoxWithNewLine(std::string(" |-Checksum : " + std::to_string(ntohs(igmpheader->checksum))).c_str());
	PrintToTextBoxWithNewLine(std::string(" |-Groupe address : " + std::string(inet_ntoa(addr.sin_addr))).c_str());


	PrintToTextBoxWithNewLine("");

	PrintToTextBoxWithNewLine("IP Header");
	PrintData(Buffer, iphdrlen);

	PrintToTextBoxWithNewLine("IGMP Header");
	PrintData(Buffer + iphdrlen, sizeof(IGMP_HDR));

	PrintToTextBoxWithNewLine("Data Payload");
	PrintData(Buffer + iphdrlen + sizeof(IGMP_HDR), (Size - sizeof(IGMP_HDR) - iphdr->ip_header_len * 4));
}


void PrintData(char* data, int size) {
	char line[17];
	int j;
	std::string temp;
	for (int i = 0; i < size; i++) {
		char c = data[i];
		char buffer[5];
		sprintf(buffer, " %.2x", (unsigned char)c);
		temp += buffer;

		char a = (c >= 32 && c <= 128) ? (unsigned char)c : '.';
		line[i % 16] = a;

		if ((i != 0 && (i + 1) % 16 == 0) || i == size - 1) {
			line[i % 16 + 1] = '\0';
			temp += "          ";

			for (j = strlen(line); j < 16; j++) {
				temp += "   ";
			}
			temp += line;
			PrintToTextBoxWithNewLine(temp.c_str());
			temp = "";
		}
	}

	PrintToTextBoxWithNewLine("");
}