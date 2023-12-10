#pragma once

#define DEFAULT_WINDOW_DATA_WIDTH			800
#define DEFAULT_WINDOW_DATA_HEIGHT			600

#define DEFAULT_WINDOW_START_WIDTH			400
#define DEFAULT_WINDOW_START_HEIGHT			75

#define START_BUTTON_WIDTH					200
#define START_BUTTON_HEIGHT					25

#define TABLE_OFFSET						30
#define ID_TABLE							101

#define THREADS_NUMBER						10

#define ID_TIMER							42
	
#define WM_SHOW_PACKAGE						(WM_USER + 1)

#define SIO_RCVALL _WSAIOW(IOC_VENDOR,1) 
#define ICMP								1
#define IGMP								2
#define TCP									6
#define UDP									17
#define INET_ADDR_STRLEN					15

#define INTERFACE_SELECTORS					100
#define BUTTONS_OFFSET						15

#define OnExitMenu							1
#define OnOpenFileMenu						2				
#define OnSaveFileMenu						3			
#define OnSaveAsFileMenu					4			
#define OnStartMenu							5			
#define OnStopMenu							6		
#define OnResetTable						7	
#define OnBackToStart						8
#define OnStartScroll						9
#define OnStopScroll						10

#define OnReloadInterface					11

#define OnSavePackageData					12

#define SelectInterface						13

