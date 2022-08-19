//lib -mwindows -lcomctl32 -lws2_32
#include "main.h"

HWND CreateListView(HINSTANCE , HWND , HWND );
HWND CreateListViewF(HINSTANCE , HWND , HWND );
void CenterOnScreen(HWND hnd);

//rename filter name
UCHAR RenameBufFile[32];
int   Selected=-1;
HWND hProxyIPforClients;


void UpdateFilterRenamed()
{
     BlackListChangeHost(Selected,RenameBufFile);
     Selected=-1;
}

void AddIfNotExist()
{
    UINT ret; //int id;
    if(!strchr(RenameBufFile,'.'))
        return; 
    
    ret = IsHostInList(RenameBufFile);
    if(ret<0xFFFF) //it exist
    {
            SendMessage(hFilter, LVM_ENSUREVISIBLE, ret, 0);
            ListView_SetItemState(hFilter,ret,LVIS_SELECTED,LVIS_SELECTED);    
     }
     else
     {
        //id=
        BlackListAddHost(RenameBufFile); 
       /* if(id>=0)
        {
             SetFocus(hFilter);
             ListView_EnsureVisible(hFilter,id,FALSE);  
             ListView_SetItemState(hFilter,id,LVIS_SELECTED,LVIS_SELECTED);
             ListView_EditLabel(hFilter,id); 
        }*/
     }
}

void LoadInitStuff()
{
    Sleep(500); //let the GUI load
    LPHOSTENT hostA; 
    struct in_addr myaddrA;	 
    UCHAR MyIP[250];
    ZeroMemory(MyIP,250);
    if(gethostname(MyIP,250)!=SOCKET_ERROR)
    {
	     if((hostA=gethostbyname(MyIP)) != NULL)
	     {
	         myaddrA= *(struct in_addr far *)(hostA->h_addr);
	         sprintf(MyIP,"PROXY :     IP = %s     PORT = 8080\0",inet_ntoa(myaddrA));
                 SetWindowText(hProxyIPforClients,MyIP);
             }
    }
    EventReadWrite = CreateEvent(NULL, TRUE, FALSE, "CHCONF");
    if(EventReadWrite == NULL){ SetWindowText(hProxyIPforClients,"#Error: CreateEvent"); Sleep(2000); exit(0); }
    StopProxy = YES;
    IgnoreClients = NO;
    ImLoading = NO;
    IgnoreBlocked = NO;
    IwantToExit = NO;
    InitializeCriticalSection(&cs);
}

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CREATE:
        {   
            HFONT hFont; HWND f; int parts[3]={200,547,710};
            InitCommonControls();
            hFont = CreateFont(15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Comic Sans MS");
            hlv = CreateListView(ins, hwnd, hlv);          SFONT(hlv); 
//filter
            hFilter = CreateListViewF(ins, hwnd, hFilter); SFONT(hFilter); 
	    f=CreateWindow("BUTTON","Save Filters:",WS_VISIBLE|WS_CHILD | BS_GROUPBOX|BS_CENTER,548,307,160,71,hwnd,NULL,ins,NULL);
            SFONT(f);
            hFilterName = CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","Filter.dat",WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL,553,328,150,21,hwnd,NULL,ins,NULL);
    	    SLIMIT(hFilterName,MAX_PATH-1);
            CreateWindow("BUTTON","SAVE",WS_VISIBLE|WS_CHILD,554,353,150,20,hwnd,(HMENU)ID_FILTERSAVE,ins,NULL);
//run            
            hStart = CreateWindow("BUTTON","START",WS_VISIBLE|WS_CHILD,52,358,289,22,hwnd,(HMENU)ID_START_SERVER,ins,NULL);	
            f=CreateWindow("BUTTON","IgnoreBlocked",WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|BS_TEXT,344,358,100,20,hwnd,(HMENU)ID_IGNORE_BLOCKED,ins,NULL);
            SFONT(f);
            f=CreateWindow("BUTTON","IgnoreClients",WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|BS_TEXT,446,358,100,20,hwnd,(HMENU)ID_IGNORE_CLIENTS,ins,NULL);
            SFONT(f);
            f = CreateWindow("BUTTON","Clear",WS_CHILD|WS_VISIBLE,2,358,47,22,hwnd,(HMENU)ID_CLEAR_LIST,ins,NULL);
            SFONT(f); 
//statusbar            
            hStatusBar = CreateWindow(STATUSCLASSNAME, 0, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, NULL, ins, 0);
            SNDMSG(hStatusBar, SB_SETPARTS, 3, (LPARAM)&parts);
    
            ShowThreadStatus("Threads: [ 000 ]");
            ShowAppStatus("Status: OFFLINE");
            ShowFilterStatus("Filters: 0 / 0");
            hProxyIPforClients = CreateWindow("STATIC","Loading...",WS_CHILD|WS_VISIBLE,0,0,659,20,hwnd,NULL,ins,NULL);
            CreateWindow("BUTTON","Exit",WS_CHILD|WS_VISIBLE,660,0,50,20,hwnd,(HMENU)ID_EXIT_APP,ins,NULL);
//sock
            if(WSAStartup( MAKEWORD(2,0), &wsaData ) == SOCKET_ERROR )
            {
                SetWindowText(hwnd,"#Error : WSAStartup");
                break;
            }
            CenterOnScreen(hwnd);
            DragAcceptFiles(hwnd,1);
            CreateThread(0,0,(LPTHREAD_START_ROUTINE)LoadInitStuff,0,0,0); 
        }break; 
        case WM_DROPFILES: 
        {
            memset(dropped,0,MAX_PATH);
            DragQueryFile((HDROP)wParam, 0, dropped, MAX_PATH);
            DragFinish((HDROP) wParam);
            if((GetFileAttributes(dropped) & FILE_ATTRIBUTE_DIRECTORY)==FILE_ATTRIBUTE_DIRECTORY)
                   break;
            CreateThread(0,0,(LPTHREAD_START_ROUTINE)ReadDataFromFile,0,0,0);
        }break; 
        case WM_COMMAND:
        {
        switch(LOWORD(wParam))
        {
            case ID_EXIT_APP:
            {
                if(StopProxy == YES)
	            {
                      DeleteCriticalSection(&cs);
                      WSACleanup();  
                      exit(0);
                } 
                ShowAppStatus("Status: Exit...");
                StopProxy = YES;
                IwantToExit = YES;
                EnableWindow(GetDlgItem(hwnd,1080),0);
                NetClose(listen_fd);
            } break;
            case ID_START_SERVER:
            {    
                if(StopProxy == YES)
                {
                     EnableWindow(GetDlgItem(hwnd,1080),0);
                     CreateThread(0,0,(LPTHREAD_START_ROUTINE)start_listen,0,0,0);
                }
                else
                {
                     StopProxy = YES;
                     EnableWindow(GetDlgItem(hwnd,1080),0);
                     NetClose(listen_fd); //Calling close, because server is blocked in accept
                }
            } break; 
            case ID_CLEAR_LIST:
            {
                 SendMessage(hlv,LVM_DELETEALLITEMS,(WPARAM)0,(LPARAM)0);  
            } break;
            case ID_IGNORE_BLOCKED:
            { 
                if(SendDlgItemMessage(hwnd,ID_IGNORE_BLOCKED,BM_GETCHECK,0,0)==BST_CHECKED)
                {
                      IgnoreBlocked=YES;
                }
                else{ IgnoreBlocked=NO;  }
            } break; 
            case ID_IGNORE_CLIENTS:
            { 
                if(SendDlgItemMessage(hwnd,ID_IGNORE_CLIENTS,BM_GETCHECK,0,0)==BST_CHECKED)
                {
                      IgnoreClients=YES;
                }
                else{ IgnoreClients=NO;  }
            } break; 
            case ID_FILTERSAVE:
            {
                 CreateThread(0,0,(LPTHREAD_START_ROUTINE)WriteDataToFile,0,0,0);
            } break;
        }//switch
        }//WM_COMMAND
        break;  
//-------------------------------- notify -------------------------------
        case WM_NOTIFY:
        {
        switch(LOWORD(wParam))
        {
//filters            
            case ID_FILTER: { switch(_lpnm->code)
            {
                case LVN_BEGINLABELEDITA:
                {
                     Selected =-1;
                     Selected= ListView_GetNextItem(hFilter,-1,LVNI_SELECTED);
                     memset(RenameBufFile,0,32);
                } break;
                case LVN_ENDLABELEDITA:
                {
                     LV_DISPINFO *ptvdi = (LV_DISPINFO FAR *) lParam;
                     UCHAR *p = ptvdi[0].item.pszText;
                     if(Selected>=0 && p!=NULL)
                     {
                         strncpy(RenameBufFile,p,31);
                         CreateThread(0,0,(LPTHREAD_START_ROUTINE)UpdateFilterRenamed,0,0,0);
                         SetFocus(hFilter);
                     }else{ Selected =-1; }
                } break;
                case LVN_ITEMCHANGED:
                {
                     if(ImLoading==YES)//prevent UPDATE when loading filters
                        break;
                     NM_LISTVIEW *pnmv = (NM_LISTVIEW FAR *) lParam;
                     
                     if( (pnmv->uNewState == LVIF_DI_SETITEM) || (pnmv->uOldState == LVIF_DI_SETITEM) )
                     {
                         UINT ret = ListView_GetCheckState(hFilter,_ItemList->iItem);
                         if(ret == 1) //checked
                         {
                            BlackListSetCheckHost(_ItemList->iItem,'B');
                         }
                         else  //unchecked
                         {
                           BlackListSetCheckHost(_ItemList->iItem,'N');
                         }  
                     }                            
                } break;
                case NM_DBLCLK:
                {		
                      Selected = _ItemList->iItem;
                      if(Selected >=0) BlackListRemoveHost(Selected);
                      Selected =-1;
                }
            }} break; // ID_FILTER switch(_lpnm->code)
//clients            
            case ID_TUNNEL: 
            {
                  if(_lpnm->code == NM_DBLCLK)
                  {		
                      int ret;
                      memset(RenameBufFile,0,32);
                      ListView_GetItemText(hlv,_ItemList->iItem,0,RenameBufFile,31);
                      CreateThread(0,0,(LPTHREAD_START_ROUTINE)AddIfNotExist,0,0,0);
                  }
            } break;			
        } //switch
        }//WM_NOTIFY
        break;
        case WM_DESTROY: //KILL BY taskManager
        {
            NetClose(listen_fd);
            DeleteCriticalSection(&cs);
            WSACleanup();
            PostQuitMessage (0);
        } break;
        default:
            return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE _a_, HINSTANCE _b_, LPSTR _c_, int _d_)
{
    HWND hwnd;
    MSG messages;
    WNDCLASSEX wincl;
    ins = _a_;
    wincl.hInstance = _a_;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.hIcon =  LoadIcon (ins,MAKEINTRESOURCE(200));
    wincl.hIconSm =  LoadIcon (ins,MAKEINTRESOURCE(200));
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
    
    if (!RegisterClassEx (&wincl))
        return 0;
    
    hwnd = CreateWindowEx(WS_EX_TOPMOST,szClassName,"Proxy Server -> Proxy-Agent: AM71113363",WS_VISIBLE |WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT,
    718, 432, HWND_DESKTOP, NULL, _a_, NULL);
     
    ShowWindow (hwnd, _d_);
    
    while (GetMessage (&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }
    return messages.wParam;
}

void CenterOnScreen(HWND hnd)
{
  RECT rcClient, rcDesktop;
  int nX,nY;
  SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, 0);
  GetWindowRect(hnd, &rcClient);
  nX=((rcDesktop.right - rcDesktop.left) / 2) -((rcClient.right - rcClient.left) / 2);
  nY=((rcDesktop.bottom - rcDesktop.top) / 2) -((rcClient.bottom - rcClient.top) / 2);
  SetWindowPos(hnd, NULL, nX, nY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
return;
}
