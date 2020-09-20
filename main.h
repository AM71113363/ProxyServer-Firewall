#define _WIN32_IE  0x0501
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <commctrl.h>
#include <winsock.h>
#include "resource.h"


HWND hlv,hFilter,hIP,hFilterAdd,hIPAdd;
HWND hStayClean;
unsigned char FilterBuffer[MAX_PATH]; //used by filter list, do not use multithread
static char szClassName[ ] = "WindowsApp";
HINSTANCE ins;
HWND hWnd;

WSADATA wsaData;
SOCKET listen_fd;
CRITICAL_SECTION cs;

HWND CreateListView(HINSTANCE , HWND , HWND );
HWND CreateListViewF(HINSTANCE , HWND , HWND );
HWND CreateListViewI(HINSTANCE , HWND , HWND );

int AddList(UCHAR *,UCHAR *,UCHAR *,UCHAR *);
int ListViewAddHost(UCHAR *);
int ListViewAddIP(UCHAR *);
void ListViewChangeNameHost(int ,UCHAR *);
void ListViewChangeNameIP(int ,UCHAR *);
void SetHost(int ,UCHAR *);
void SetIP(int ,UCHAR *);
void SetClient(int ,UCHAR *);
void SetServer(int ,UCHAR *);

void CenterOnScreen(HWND hnd);

//static DWORD TIMEOUT_SEND = SEND_TIMEOUT;
static struct timeval tv = {.tv_sec = RECV_TIMEOUT, .tv_usec = 0};
   
static UCHAR OKgoON[]="HTTP/1.1 200 Connection established\r\nProxy-Agent: AM71113363\r\nProxy-Connection: keep-alive\r\nKeep-Alive: timeout=20\r\n\r\n";

typedef struct FILTER_LIST_
{
    unsigned char block[2];
    unsigned char name[32];
}HOST_LIST;

typedef struct IP_LIST_
{
    unsigned char block[2];
    unsigned char ip[16];
}IP_LIST;

UCHAR IsHostBlackList(UCHAR *);
UCHAR IsIPBlackList(UCHAR *);
UCHAR IsRunning();
void SleepNow(UCHAR); //YES = pause  NO = go
void start_listen();
void ReadDataFromFile();
void WriteDataToFile();
void BlackListSetCheckHost(int ,UCHAR);
void BlackListSetCheckIP(int ,UCHAR);
UCHAR *BlackListGetHost(int );
UCHAR *BlackListGetIP(int );

