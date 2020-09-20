//lib -mwindows -lcomctl32 -lws2_32
#include "main.h"
int HostMode = -1;
int IPMode = -1;

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CREATE:
        {   
            HFONT hFont;
            DWORD n = 4; int f;
            hWnd = hwnd;
            InitCommonControls();
            hFont = CreateFont(15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Comic Sans MS");
            hlv = CreateListView(ins, hwnd, hlv);
                  SendMessage(hlv, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0)); 
            hFilter = CreateListViewF(ins, hwnd, hFilter);
                  SendMessage(hFilter, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0)); 
            hIP = CreateListViewI(ins, hwnd, hFilter); //|WS_DISABLED
                  SendMessage(hIP, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0)); 
            
	        hFilterAdd = CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","",WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL,548,243,147,21,hwnd,NULL,ins,NULL);
    	    SendMessage(hFilterAdd, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0)); 
	        hIPAdd = CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","",WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL,696,243,147,21,hwnd,NULL,ins,NULL);
    	    SendMessage(hIPAdd, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0)); 
            SendMessage(hIPAdd,EM_SETLIMITTEXT,(WPARAM)15,(LPARAM)0); 

            CreateWindow("BUTTON","ADD",WS_VISIBLE|WS_CHILD,548,264,146,22,hwnd,(HMENU)ID_FILTERMOD,ins,NULL);	
            CreateWindow("BUTTON","ADD",WS_VISIBLE|WS_CHILD,697,264,146,22,hwnd,(HMENU)ID_IPMOD,ins,NULL);	
            CreateWindow("BUTTON","SAVE Filters",WS_VISIBLE|WS_CHILD,548,286,295,22,hwnd,(HMENU)ID_FILTERSAVE,ins,NULL);
            CreateWindow("BUTTON","START",WS_VISIBLE|WS_CHILD,51,286,333,22,hwnd,(HMENU)1080,ins,NULL);	
            CreateWindow("BUTTON","PAUSE",WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|BS_TEXT,386,286,54,20,hwnd,(HMENU)8080,ins,NULL);
            
            hStayClean = CreateWindow("BUTTON","Clear",WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|BS_TEXT,2,286,47,20,hwnd,(HMENU)0,ins,NULL);
            SendMessage(hStayClean, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0)); 
            SendMessage(GetDlgItem(hwnd,8080), WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0)); 
            
            if(WSAStartup( MAKEWORD(2,0), &wsaData ) == SOCKET_ERROR )
            {
                SetWindowText(hWnd,"#Error : WSAStartup");
                break;
            }
            CenterOnScreen(hwnd);
            DragAcceptFiles(hwnd,1);
            InitializeCriticalSection(&cs);
           	LPHOSTENT hostA; 
            struct in_addr myaddrA;	 
            UCHAR MyIP[250];
            ZeroMemory(MyIP,250);
            if(gethostname(MyIP,250)!=SOCKET_ERROR)
            {
		        if((hostA=gethostbyname(MyIP)) != NULL)
		        {
			        myaddrA= *(struct in_addr far *)(hostA->h_addr);
			        sprintf(MyIP,"PROXY >> %s:8080\0",inet_ntoa(myaddrA));
                    SetWindowText(hwnd,MyIP);
                }
	        }
        
        }
        break; 
      case WM_DROPFILES: 
      {
            if(IsRunning() == NO)
            {
                 memset(FilterBuffer,0,MAX_PATH);
                 DragQueryFile((HDROP)wParam, 0, FilterBuffer, MAX_PATH);
		         DragFinish((HDROP) wParam);
		         CreateThread(0,0,(LPTHREAD_START_ROUTINE)ReadDataFromFile,0,0,0);
            }else{ MessageBox(hWnd,"You must add filters before starting server","Info",MB_OK |MB_ICONINFORMATION); }
      }
      break; 
      case WM_COMMAND:
      {
      switch(LOWORD(wParam))
      {
             case 1080:
             {    
                  if(IsRunning() == NO)
                  {
                     SetWindowText(GetDlgItem(hwnd,1080),"STOP");
                     CreateThread(0,0,(LPTHREAD_START_ROUTINE)start_listen,0,0,0);
                  }
                  else
                  {
                     NetClose(listen_fd);
                  }
             }
             break; 
             case 8080:
             { 
                if(SendDlgItemMessage(hwnd,8080,BM_GETCHECK,0,0)==BST_CHECKED)
                {
                      SleepNow(YES);
                 }
                 else
                 {
                      SleepNow(NO);
                 }
             }
             break; 
             case ID_IPMOD:
             {
                  if(IsRunning() == NO)
                  {
                       memset(FilterBuffer,0,MAX_PATH);
                       if(GetWindowText(hIPAdd,FilterBuffer,MAX_PATH-1) == 0)
                       {
                             if(IPMode >= 0)
                                   BlackListRemoveIP(IPMode);
                             break;                                       
                       }
                       if(IPMode < 0)
                       {
                             BlackListAddIP(FilterBuffer); 
                       }
                       else
                       {
                             BlackListChangeIP(IPMode,FilterBuffer);                                                          
                             SetWindowText(hIPAdd,"");
                             SetWindowText(GetDlgItem(hwnd,ID_IPMOD),"ADD");
                             IPMode = -1;                                                         
                       }
                  }else{ MessageBox(hWnd,"Stop The Server First","Info",MB_OK |MB_ICONINFORMATION); }
             }
             break;
             case ID_FILTERMOD:
             {
                  if(IsRunning() == NO)
                  {
                        memset(FilterBuffer,0,MAX_PATH);
                        if(GetWindowText(hFilterAdd,FilterBuffer,MAX_PATH-1) == 0)
                        {
                              if(HostMode >= 0)
                                    BlackListRemoveHost(HostMode);
                              break;                                       
                        }
                        if(HostMode < 0)
                        {
                              BlackListAddHost(FilterBuffer); 
                        }
                        else
                        {
                              BlackListChangeHost(HostMode,FilterBuffer);                                                         
                              SetWindowText(hFilterAdd,"");
                              SetWindowText(GetDlgItem(hwnd,ID_FILTERMOD),"ADD");
                              HostMode = -1;                                                         
                        } 
                  }else{ MessageBox(hWnd,"Stop The Server First","Info",MB_OK |MB_ICONINFORMATION); }  
             }
             break;
             case ID_FILTERSAVE:
             {
                 if(IsRunning() == NO)
                 {
                      CreateThread(0,0,(LPTHREAD_START_ROUTINE)WriteDataToFile,0,0,0);
                 }else{ MessageBox(hWnd,"Stop The Server First","Info",MB_OK |MB_ICONINFORMATION); }
             }
             break;
      }//switch
      }//WM_COMMAND
      break;  
//-------------------------------- notify -------------------------------
      case WM_NOTIFY:
      {
      switch(LOWORD(wParam))
      {
            case ID_FILTER: 
            { 
                  if(_lpnm->code == LVN_ITEMCHANGED)
                  {		
                        unsigned int ret;  
                        ret = ListView_GetCheckState(hFilter,_ItemList->iItem);
                        if(ret == 1) //checked
                        {
                              BlackListSetCheckHost(_ItemList->iItem,'B');
                        }
                        else  //unchecked
                        {
                              BlackListSetCheckHost(_ItemList->iItem,'N');
                        }       
                  } //LVN_ITEMCHANGED
                  else if(_lpnm->code == NM_DBLCLK)
                  {		
                      if(IsRunning() == NO)
                      {
                            HostMode = _ItemList->iItem;
                            SetWindowText(hFilterAdd,BlackListGetHost(HostMode));
                            SetWindowText(GetDlgItem(hwnd,ID_FILTERMOD),"CHANGE");
                      }
                  }
            }//ID_FILTER
            break;
            case ID_IP: 
            { 
                  if(_lpnm->code == LVN_ITEMCHANGED)
                  {		
                        unsigned int ret;  
                        ret = ListView_GetCheckState(hIP,_ItemList->iItem);
                        if(ret == 1) //checked
                        {
                              BlackListSetCheckIP(_ItemList->iItem,'B');
                        }
                        else  //unchecked
                        {
                              BlackListSetCheckIP(_ItemList->iItem,'N');
                        }       
                  } //LVN_ITEMCHANGED
                  else if(_lpnm->code == NM_DBLCLK)
                  {		
                      if(IsRunning() == NO)
                      {
                           IPMode = _ItemList->iItem;
                           SetWindowText(hIPAdd,BlackListGetIP(IPMode));
                           SetWindowText(GetDlgItem(hwnd,ID_IPMOD),"CHANGE");
                      }
                  }
            }//ID_IP
            break;
            case ID_TUNNEL: 
            {
                  if(IsRunning() == NO)
                  {
                  if(_lpnm->code == NM_DBLCLK)
                  {		
                      memset(FilterBuffer,0,MAX_PATH);
                      ListView_GetItemText(hlv,_ItemList->iItem,0,FilterBuffer,MAX_PATH-1);
                      SetWindowText(hFilterAdd,FilterBuffer);
                      SetWindowText(GetDlgItem(hwnd,ID_FILTERMOD),"ADD");
                      HostMode = -1;
                      memset(FilterBuffer,0,MAX_PATH);
                      ListView_GetItemText(hlv,_ItemList->iItem,1,FilterBuffer,MAX_PATH-1);
                      SetWindowText(hIPAdd,FilterBuffer);
                      SetWindowText(GetDlgItem(hwnd,ID_IPMOD),"ADD");
                      IPMode = -1;
                  }
                  }
            }//ID_TUNNEL
            break;			
      } //switch
      }//WM_NOTIFY
      break;
//-------------------------------- end notify --------------------------
      case WM_DESTROY:
      {
            DeleteCriticalSection(&cs);
            WSACleanup();
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
      }
      break;
        default:                      /* for messages that we don't deal with */
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
    
    hwnd = CreateWindowEx(WS_EX_TOPMOST,szClassName,"Proxy Server",WS_VISIBLE |WS_SYSMENU|WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT,
    851, 340, HWND_DESKTOP, NULL, _a_, NULL);
     
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
