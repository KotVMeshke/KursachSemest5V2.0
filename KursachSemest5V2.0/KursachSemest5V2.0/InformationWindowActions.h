#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "functionsHeaders.h"

#include <Windows.h>
#include <CommCtrl.h>
#include "commctrl.h"
#include "Table.h"
#include "definesHeader.h"

HWND TextBox;
//struct sockaddr_in source, dest;
//char hex[2];
//unsigned int number_of_packege = 0;
//LARGE_INTEGER frequency;
//LARGE_INTEGER start;
//LARGE_INTEGER end;
//double elapsedSeconds;
//IPV4_HDR* iphdr;
//TCP_HDR* tcpheader;
//UDP_HDR* udpheader;
//ICMP_HDR* icmpheader;

void PrintToTextBox(const wchar_t* text)
{
	if (TextBox != nullptr)
	{
		int length = GetWindowTextLength(TextBox);
		SendMessage(TextBox, EM_SETSEL, length, length); // Перемещаем курсор в конец текстового поля
		SendMessage(TextBox, EM_REPLACESEL, FALSE, reinterpret_cast<LPARAM>(text)); // Добавляем текст в текстовое поле
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
	MoveWindow(TextBox, 0, 0, rect.right-rect.left, rect.bottom - rect.top, TRUE);
}

void SetTextViewFont(HWND textView, const wchar_t* fontName, int fontSize)
{
	HFONT newFont = CreateFont(
		fontSize,                     // Размер шрифта
		0,                            // Ширина символов (0 - по умолчанию)
		0,                            // Угол наклона (0 - по умолчанию)
		0,                            // Угол поворота (0 - по умолчанию)
		FW_NORMAL,                    // Толщина шрифта (FW_NORMAL - нормальная толщина)
		FALSE,                        // Полужирный шрифт (FALSE - нет)
		FALSE,                        // Курсивный шрифт (FALSE - нет)
		FALSE,                        // Подчеркнутый шрифт (FALSE - нет)
		DEFAULT_CHARSET,              // Кодировка символов (DEFAULT_CHARSET - текущая кодировка)
		OUT_DEFAULT_PRECIS,           // Точность вывода (OUT_DEFAULT_PRECIS - по умолчанию)
		CLIP_DEFAULT_PRECIS,          // Точность отсечения (CLIP_DEFAULT_PRECIS - по умолчанию)
		DEFAULT_QUALITY,              // Качество вывода (DEFAULT_QUALITY - по умолчанию)
		DEFAULT_PITCH | FF_DONTCARE,  // Начертание шрифта (DEFAULT_PITCH | FF_DONTCARE - по умолчанию)
		fontName                      // Имя шрифта
	);

	SendMessage(textView, WM_SETFONT, reinterpret_cast<WPARAM>(newFont), TRUE);
	InvalidateRect(textView, nullptr, TRUE);
}

HWND CreateTextView(HWND parentWindow, HINSTANCE hInstance)
{
	RECT rect = {};
	GetWindowRect(parentWindow, &rect);
	HWND textPanel = CreateWindowEx(0,L"EDIT",nullptr, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE| ES_READONLY,0, 0, rect.right-rect.left, rect.bottom - rect.top,parentWindow,nullptr,hInstance,nullptr);
	return textPanel;
}

void AddMenuToInformWindow(HWND window)
{
	HMENU menu = CreateMenu();
	HMENU fileMenu = CreateMenu();
	AppendMenu(fileMenu, MF_STRING, OnSaveAsFileMenu, L"Save Data");
	AppendMenu(fileMenu, MF_MENUBARBREAK, NULL, NULL);
	AppendMenu(fileMenu, MF_STRING, OnExitMenu, L"Close");

	AppendMenu(menu, MF_POPUP, (UINT_PTR)fileMenu, L"File");

	SetMenu(window, menu);
}

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
		PostQuitMessage(0);
		break;
	default:

		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;

}

void ProcessPacket(char* Buffer, int Size)
{
	IPV4_HDR* iphdr = (IPV4_HDR*)Buffer;

	switch (iphdr->ip_protocol)
	{
	case ICMP:
		PrintIcmpPacket(Buffer, Size);
		break;

	case IGMP:
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

char* GetNameByNumber(unsigned int protocol)
{
	char* str;
	switch (protocol)
	{
	case ICMP:
		str = (char*)malloc(4 * sizeof(char));
		strcpy(str, "ICMP");
		break;

	case IGMP:
		str = (char*)malloc(4 * sizeof(char));
		strcpy(str, "IGMP");
		break;

	case TCP:
		str = (char*)malloc(3 * sizeof(char));
		strcpy(str, "TCP");
		break;

	case UDP:
		str = (char*)malloc(3 * sizeof(char));
		strcpy(str, "UDP");
		break;

	default:
		str = (char*)malloc(5 * sizeof(char));
		strcpy(str, "INCOR");
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

	//fprintf(logfile, "\n\n***********************UDP Packet*************************\n");

	PrintIPHeaderInfo(Buffer);


	/*fprintf(logfile, "\nUDP Header\n");
	fprintf(logfile, " |-Source Port : %d\n", ntohs(udpheader->source_port));
	fprintf(logfile, " |-Destination Port : %d\n", ntohs(udpheader->dest_port));
	fprintf(logfile, " |-UDP Length : %d\n", ntohs(udpheader->udp_length));
	fprintf(logfile, " |-UDP Checksum : %d\n", ntohs(udpheader->udp_checksum));

	fprintf(logfile, "\n");
	fprintf(logfile, "IP Header\n");

	PrintData(Buffer, iphdrlen);
	fprintf(logfile, "UDP Header\n");

	PrintData(Buffer + iphdrlen, sizeof(UDP_HDR));

	fprintf(logfile, "Data Payload\n");

	PrintData(Buffer + iphdrlen + sizeof(UDP_HDR), (Size - sizeof(UDP_HDR) - iphdr->ip_header_len * 4));

	fprintf(logfile, "\n###########################################################");*/
}

void PrintIcmpPacket(char* Buffer, int Size)
{
	unsigned short iphdrlen;

	IPV4_HDR* iphdr = (IPV4_HDR*)Buffer;
	iphdrlen = iphdr->ip_header_len * 4;

	ICMP_HDR* icmpheader = (ICMP_HDR*)(Buffer + iphdrlen);

	//fprintf(logfile, "\n\n***********************ICMP Packet*************************\n");
	PrintIPHeaderInfo(Buffer);


	/*fprintf(logfile, "\n");

	fprintf(logfile, "ICMP Header\n");
	fprintf(logfile, " |-Type : %d", (unsigned int)(icmpheader->type));

	if ((unsigned int)(icmpheader->type) == 11)
	{
		fprintf(logfile, " (TTL Expired)\n");
	}
	else if ((unsigned int)(icmpheader->type) == 0)
	{
		fprintf(logfile, " (ICMP Echo Reply)\n");
	}

	fprintf(logfile, " |-Code : %d\n", (unsigned int)(icmpheader->code));
	fprintf(logfile, " |-Checksum : %d\n", ntohs(icmpheader->checksum));
	fprintf(logfile, " |-ID : %d\n", ntohs(icmpheader->id));
	fprintf(logfile, " |-Sequence : %d\n", ntohs(icmpheader->seq));
	fprintf(logfile, "\n");

	fprintf(logfile, "IP Header\n");
	PrintData(Buffer, iphdrlen);

	fprintf(logfile, "UDP Header\n");
	PrintData(Buffer + iphdrlen, sizeof(ICMP_HDR));

	fprintf(logfile, "Data Payload\n");
	PrintData(Buffer + iphdrlen + sizeof(ICMP_HDR), (Size - sizeof(ICMP_HDR) - iphdr->ip_header_len * 4));

	fprintf(logfile, "\n###########################################################");*/
}

void PrintData(char* data, int Size)
{
	//char a, line[17], c;
	//int j;

	////loop over each character and print
	//for (i = 0; i < Size; i++)
	//{
	//	c = data[i];

	//	//Print the hex value for every character , with a space. Important to make unsigned
	//	fprintf(logfile, " %.2x", (unsigned char)c);

	//	//Add the character to data line. Important to make unsigned
	//	a = (c >= 32 && c <= 128) ? (unsigned char)c : '.';

	//	line[i % 16] = a;

	//	//if last character of a line , then print the line - 16 characters in 1 line
	//	if ((i != 0 && (i + 1) % 16 == 0) || i == Size - 1)
	//	{
	//		line[i % 16 + 1] = '\0';

	//		//print a big gap of 10 characters between hex and characters
	//		fprintf(logfile, "          ");

	//		//Print additional spaces for last lines which might be less than 16 characters in length
	//		for (j = strlen(line); j < 16; j++)
	//		{
	//			fprintf(logfile, "   ");
	//		}

	//		fprintf(logfile, "%s \n", line);
	//	}
	//}

	//fprintf(logfile, "\n");
}
