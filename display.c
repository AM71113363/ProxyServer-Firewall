#include "main.h"

void SetHost(int Pos,UCHAR *status)
{
	ListView_SetItemText(hlv, Pos, 0,status);
	UpdateWindow(hlv);
}

void SetIP(int Pos,UCHAR *status)
{
	ListView_SetItemText(hlv, Pos, 1,status);
	UpdateWindow(hlv);
}

void SetClient(int Pos,UCHAR *status)
{
	ListView_SetItemText(hlv, Pos, 2,status);
	UpdateWindow(hlv);
}

void SetServer(int Pos,UCHAR *status)
{
	ListView_SetItemText(hlv, Pos, 3,status);
	UpdateWindow(hlv);
}

int AddList(UCHAR *HostName,UCHAR *IP,UCHAR *Client,UCHAR *Server)
{
    int Item;
	LVITEM LvItem;
	EnterCriticalSection(&cs);
	Item = (int)SendMessage(hlv,LVM_GETITEMCOUNT,0,0);

	LvItem.mask = LVIF_TEXT | LVIF_STATE;
	LvItem.state = 0; 
	LvItem.stateMask = 0;
	LvItem.iItem = Item;
    
    LvItem.iSubItem = 0;
    LvItem.pszText = HostName;
    LvItem.cchTextMax = strlen(HostName) + 1;
    SendMessage(hlv,LVM_INSERTITEM,0,(LPARAM)&LvItem);

    LvItem.iSubItem = 1;
    LvItem.pszText = IP;
    LvItem.cchTextMax = strlen(IP) + 1;
    SendMessage(hlv,LVM_SETITEMTEXT,Item,(LPARAM)&LvItem);

    LvItem.iSubItem = 2;
    LvItem.pszText = Client;
    LvItem.cchTextMax = strlen(Client) + 1;
    SendMessage(hlv,LVM_SETITEMTEXT,Item,(LPARAM)&LvItem);
  
    LvItem.iSubItem = 3;
    LvItem.pszText = Server;
    LvItem.cchTextMax = strlen(Server) + 1;
    SendMessage(hlv,LVM_SETITEMTEXT,Item,(LPARAM)&LvItem);
    SendMessage(hlv, LVM_ENSUREVISIBLE, Item, 1);
    UpdateWindow(hlv);
    LeaveCriticalSection(&cs);
    return Item;
}

HWND CreateListView(HINSTANCE ins, HWND hnd, HWND hl)
{
     LV_COLUMN lvColumn;
                  
     hl = CreateWindow("SysListView32","",WS_CHILD | WS_VISIBLE |LVS_REPORT | LVS_SINGLESEL|LVS_SHOWSELALWAYS, 
     0,20,547,336,hnd,(HMENU)ID_TUNNEL,ins,0);         
    
     lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
             
     lvColumn.fmt = LVCFMT_LEFT;
     lvColumn.cx = 220;  lvColumn.pszText = "HostName";
     SendMessage(hl,LVM_INSERTCOLUMN,0,(LPARAM)&lvColumn); 

     lvColumn.fmt = LVCFMT_CENTER;
     lvColumn.cx = 108;  lvColumn.pszText = "IP";
     SendMessage(hl,LVM_INSERTCOLUMN,1,(LPARAM)&lvColumn); 
             
     lvColumn.fmt = LVCFMT_CENTER;
     lvColumn.cx = 100;  lvColumn.pszText = "Client";
     SendMessage(hl,LVM_INSERTCOLUMN,2,(LPARAM)&lvColumn); 
             
     lvColumn.cx = 100;  lvColumn.pszText = "Server";
     SendMessage(hl,LVM_INSERTCOLUMN,3,(LPARAM)&lvColumn); 

     SNDMSG(hl,LVM_SETEXTENDEDLISTVIEWSTYLE,0,(WPARAM)LVS_EX_FULLROWSELECT); 
     SNDMSG(hl, WM_SETREDRAW, TRUE, 0);
     // SNDMSG(hlv,LVM_SETEXTENDEDLISTVIEWSTYLE,0,(WPARAM)LVS_EX_CHECKBOXES); 
return hl;
}
//---------------------------- FILTERS ----------------------------------------

void ListViewChangeNameHost(int Pos,UCHAR *name)
{
	ListView_SetItemText(hFilter, Pos, 0,name);
	UpdateWindow(hFilter);
}

int ListViewAddHost(UCHAR *name)
{
    int Item;
	LVITEM LvItem;
	Item = (int)SendMessage(hFilter,LVM_GETITEMCOUNT,0,0);

	LvItem.mask = LVIF_TEXT | LVIF_STATE;
	LvItem.state = 0; 
	LvItem.stateMask = 0;
	LvItem.iItem = Item;
	
    LvItem.iSubItem = 0;
    LvItem.pszText = name;
    LvItem.cchTextMax = strlen(name) + 1;
    SendMessage(hFilter,LVM_INSERTITEM,Item,(LPARAM)&LvItem);
    SendMessage(hFilter, LVM_ENSUREVISIBLE, Item, 1);
    UpdateWindow(hFilter);
    return Item;
}


HWND CreateListViewF(HINSTANCE ins, HWND hnd, HWND hl)
{
     LV_COLUMN lvColumn;
                  
     hl = CreateWindow("SysListView32","",WS_CHILD | WS_VISIBLE|LVS_EDITLABELS |LVS_REPORT | LVS_SINGLESEL|LVS_SHOWSELALWAYS, 
     548,20,160,285,hnd,(HMENU)ID_FILTER,ins,0);         
    
     lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;// | LVCF_SUBITEM;
             
     lvColumn.fmt = LVCFMT_CENTER;
     lvColumn.cx = 140;  lvColumn.pszText = "BlackList";
     SendMessage(hl,LVM_INSERTCOLUMN,0,(LPARAM)&lvColumn); 

    SNDMSG(hl,LVM_SETEXTENDEDLISTVIEWSTYLE,0,(WPARAM)LVS_EX_CHECKBOXES); 
    SNDMSG(hl, WM_SETREDRAW, TRUE, 0);
return hl;
}
//------------------------- statusBAR --------------------------------------------
void ShowThreadStatus(UCHAR *m)
{
     SNDMSG(hStatusBar, SB_SETTEXT, 0, (LPARAM)m);
}

void ShowAppStatus(UCHAR *m)
{
     SNDMSG(hStatusBar, SB_SETTEXT, 1, (LPARAM)m);
}
   
void ShowFilterStatus(UCHAR *m)
{
     SNDMSG(hStatusBar, SB_SETTEXT, 2, (LPARAM)m);
}

