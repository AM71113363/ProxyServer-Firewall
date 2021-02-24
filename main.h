#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <commctrl.h>
#include <winsock.h>

static char szClassName[ ] = "WindowsAppProxyServer";

WSADATA wsaData;
int listen_fd;        //main sock,incoming CALLS
CRITICAL_SECTION cs;  //used only for update NR_OF_THREADS running

HINSTANCE ins;
HWND hStatusBar;  //General Status Info
HWND hlv;         //listview of clients
HWND hFilter;     //listview of filters
HWND hStart;      //START/STOP

//used in WM_DROPFILES
HWND hFilterName; UCHAR dropped[MAX_PATH];


//event READ/WRITE protection
HANDLE EventReadWrite; 
//call WANT_READ before LOCK_READ
#define LOCK_READ SetEvent(EventReadWrite)
//call UNLOCK_READ to release other threads from SLEEP in WANT_READ
#define UNLOCK_READ ResetEvent(EventReadWrite)
//a thread must call WANT_READ ,to prevent READ & WRITE at the same time
#define WANT_READ { while(WaitForSingleObject(EventReadWrite,0)==0) Sleep(100); }
  
//
UCHAR StopProxy;     //YES/NO change hStart->onClick( Start / Stop )
UCHAR IgnoreClients; //YES/NO ignore connection requests,used to get HostNames from clients
UCHAR IgnoreBlocked; //YES/NO  DEFAULT:[NO] Add Clients to list if they are in BLACK_LIST
UCHAR IwantToExit;   //used to exit App.
UCHAR ImLoading;     //prevent MSG LVN_ITEMCHANGED while adding filters to list


int AddList(UCHAR *,UCHAR *,UCHAR *,UCHAR *);
int ListViewAddHost(UCHAR *);
void ListViewChangeNameHost(int ,UCHAR *);
void SetHost(int ,UCHAR *);
void SetIP(int ,UCHAR *);
void SetClient(int ,UCHAR *);
void SetServer(int ,UCHAR *);
void ReadDataFromFile();
void FileToHost(UCHAR *);
void WriteDataToFile();
void BlackListSetCheckHost(int ,UCHAR);
UCHAR *BlackListGetHost(int );
UCHAR IsHostBlackList(UCHAR *);
void ShowThreadStatus(UCHAR *m);
void ShowAppStatus(UCHAR *m);
void ShowFilterStatus(UCHAR *m);
UINT IsHostInList(UCHAR *name);
int BlackListAddHost(UCHAR *name);

typedef struct FILTER_LIST_
{
    unsigned char block[2];
    unsigned char name[32];
}HOST_LIST;
void start_listen();


#define MAX_SOCKET             2048   //socket pool
#define MAX_FILTER_HOST        128    //max filter to USE

#define NO        0
#define YES       1

#define MAX_BUFSIZE            1024
#define RECV_TIMEOUT	       20     //timeout for IDLE clients

#define LVM_SETEXTENDEDLISTVIEWSTYLE (LVM_FIRST+54)
#define LVS_EX_FULLROWSELECT 32
#define LVS_EX_CHECKBOXES 4

#define ID_START_SERVER                   1080
#define ID_IGNORE_CLIENTS                 1081
#define ID_IGNORE_BLOCKED                 1082
#define ID_CLEAR_LIST                     1083
#define ID_TUNNEL                         1084
#define ID_FILTER                         1085
#define ID_FILTERSAVE                     1086
#define ID_EXIT_APP                       1087
         
#define SFONT(_h_) SendMessage(_h_, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0))
#define SLIMIT(_h_,_n_) SendMessage(_h_,EM_SETLIMITTEXT,(WPARAM)_n_,(LPARAM)0)

#define _lpnm   ((LPNMHDR)lParam)
#define _ItemList   ((LPNMLISTVIEW)lParam)

