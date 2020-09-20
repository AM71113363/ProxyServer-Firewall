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

	LvItem.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE;
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

HWND CreateListView(HINSTANCE ins, HWND hWnd, HWND hl)
{
     LV_COLUMN lvColumn;
                  
     hl = CreateWindow("SysListView32","",WS_CHILD | WS_VISIBLE |LVS_REPORT | LVS_SINGLESEL|LVS_SHOWSELALWAYS, 
     0,0,547,285,hWnd,(HMENU)ID_TUNNEL,ins,0);         
    
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


void ListViewChangeNameHost(int Pos,UCHAR *name)
{
	ListView_SetItemText(hFilter, Pos, 0,name);
	UpdateWindow(hFilter);
}

void ListViewChangeNameIP(int Pos,UCHAR *name)
{
	ListView_SetItemText(hIP, Pos, 0,name);
	UpdateWindow(hIP);
}


int AddListHI(HWND hl, UCHAR *name)
{
    int Item;
	LVITEM LvItem;
	Item = (int)SendMessage(hl,LVM_GETITEMCOUNT,0,0);

	LvItem.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE;
	LvItem.state = 0; 
	LvItem.stateMask = 0;
	LvItem.iItem = Item;
	
    LvItem.iSubItem = 0;
    LvItem.pszText = name;
    LvItem.cchTextMax = strlen(name) + 1;
    SendMessage(hl,LVM_INSERTITEM,0,(LPARAM)&LvItem);
    SendMessage(hl, LVM_ENSUREVISIBLE, Item, 1);
    UpdateWindow(hl);
    return Item;
}

int ListViewAddHost(UCHAR *name)
{
    return AddListHI(hFilter, name);
}


int ListViewAddIP(UCHAR *name)
{
    return AddListHI(hIP, name);
}


HWND CreateListViewF(HINSTANCE ins, HWND hWnd, HWND hl)
{
     LV_COLUMN lvColumn;
                  
     hl = CreateWindow("SysListView32","",WS_CHILD | WS_VISIBLE |LVS_REPORT | LVS_SINGLESEL|LVS_SHOWSELALWAYS, 
     548,0,147,242,hWnd,(HMENU)ID_FILTER,ins,0);         
    
     lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
             
     lvColumn.fmt = LVCFMT_LEFT;
     lvColumn.cx = 128;  lvColumn.pszText = "BlackList-HOST";
     SendMessage(hl,LVM_INSERTCOLUMN,0,(LPARAM)&lvColumn); 

    SNDMSG(hl,LVM_SETEXTENDEDLISTVIEWSTYLE,0,(WPARAM)LVS_EX_CHECKBOXES|LVS_EX_FULLROWSELECT); 
    SNDMSG(hl, WM_SETREDRAW, TRUE, 0);
return hl;
}


HWND CreateListViewI(HINSTANCE ins, HWND hWnd, HWND hl)
{
     LV_COLUMN lvColumn;
                  
     hl = CreateWindow("SysListView32","",WS_CHILD | WS_VISIBLE |LVS_REPORT | LVS_SINGLESEL|LVS_SHOWSELALWAYS, 
     696,0,147,242,hWnd,(HMENU)ID_IP,ins,0);         
    
     lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
             
     lvColumn.fmt = LVCFMT_LEFT;
     lvColumn.cx = 128;  lvColumn.pszText = "BlackList-IP";
     SendMessage(hl,LVM_INSERTCOLUMN,0,(LPARAM)&lvColumn); 

    SNDMSG(hl,LVM_SETEXTENDEDLISTVIEWSTYLE,0,(WPARAM)LVS_EX_CHECKBOXES|LVS_EX_FULLROWSELECT); 
    SNDMSG(hl, WM_SETREDRAW, TRUE, 0);
return hl;
}

