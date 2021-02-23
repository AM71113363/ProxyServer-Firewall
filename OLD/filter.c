#include "main.h"

HOST_LIST   B_HOST[MAX_FILTER_HOST];
UINT        B_HOST_len = 0;
IP_LIST     B_IP[MAX_FILTER_IP];
UINT        B_IP_len  = 0;

//-------------------------------------- host -------------------------------------------
UCHAR IsHostBlackList(UCHAR *name)
{
    UINT i;
    if(!strchr(name,'.'))
    {
	    return YES; 
    }
    for(i=0;i<B_HOST_len;i++)
    {
       if(B_HOST[i].block[0] == 'B')
       {
          if(strstr(name, B_HOST[i].name))
              return YES;
       }
    }   
    return NO;   
}

int BlackListAddHost(UCHAR *name)
{
   int id;
   if(B_HOST_len > (MAX_FILTER_HOST-2))  //30
     return; 
   id =  ListViewAddHost(" "); 
   if(id >= 0)
   {
      memset(B_HOST[id].name,0,32);
      strncpy(B_HOST[id].name,name,31);
      ListViewChangeNameHost(id,B_HOST[id].name);
      B_HOST[id].block[0] = 'N';
      B_HOST_len = id+1;
   }
   return id;   
}


void BlackListRemoveHost(int index)
{
   int last = B_HOST_len -1;
   if(last == 0) //have just one in list,no sense just delete file NOOO!!!
   {
      B_HOST_len = 0;
      SendMessage(hFilter,LVM_DELETEALLITEMS,(WPARAM)0,(LPARAM)0);                
      return;
   }
   else if(last != index) //the item to be removet is the last one
   {
        memset(B_HOST[index].name,0,32);     //zeror the item to be removed
        strcpy(B_HOST[index].name,B_HOST[last].name); //copy the last one to this
        ListViewChangeNameHost(index,B_HOST[index].name);     //change the name in listview
        //find if the jump item is checked
        if(ListView_GetCheckState(hFilter, last) == 1) //checked
        {
              ListView_SetCheckState(hFilter, index,1);
        }
        else
        {                                           
           ListView_SetCheckState(hFilter, index,0);
        }  
   }
   B_HOST_len = last;  //reset counter list   
   memset(B_HOST[last].name,0,32);
   ListView_DeleteItem(hFilter,last);   
}

void FileToHost(unsigned char *data)
{
    HOST_LIST *T = (HOST_LIST*)data;
    int ret;
    int id;
    //reset list,new bat list or new dir list
    SendMessage(hFilter,LVM_DELETEALLITEMS,(WPARAM)0,(LPARAM)0); 
    
    B_HOST_len = 0;
    memset(B_HOST,0,sizeof(B_HOST));
    for(ret=0;ret<MAX_FILTER_HOST;ret++)
    {
        if(T[ret].name[0] != 0)
        {
            id =  BlackListAddHost(T[ret].name); 
            if(T[id].block[0] == 'B')
                   ListView_SetCheckState(hFilter, id,1);
            continue;
        }
        break;
    }
     
}

void BlackListChangeHost(int index,UCHAR *name)
{
     memset(B_HOST[index].name,0,32);
     strncpy(B_HOST[index].name,name,31); 
     ListViewChangeNameHost(index,B_HOST[index].name);                                                          
}

void BlackListSetCheckHost(int index,UCHAR l)
{
     B_HOST[index].block[0] = l;
}

UCHAR *BlackListGetHost(int index)
{
     return B_HOST[index].name;
}


                   
//----------------------------------- ip -------------------------------------------

UCHAR IsIPBlackList(UCHAR *IP)
{
    UINT i;
       
    for(i=0;i<B_IP_len;i++)
    {
       if(B_IP[i].block[0] == 'B')
       {
          if(strstr(IP, B_IP[i].ip))
              return YES;
       }
    }    
    return NO;   
}

int BlackListAddIP(UCHAR *name)
{
   int id;
   if(B_IP_len > (MAX_FILTER_IP-2)) //30
     return; 
   id =  ListViewAddIP(" "); 
   if(id >= 0)
   {
      memset(B_IP[id].ip,0,16);
      strncpy(B_IP[id].ip,name,15);
      ListViewChangeNameIP(id,B_IP[id].ip);
      B_IP[id].block[0] = 'N';
      B_IP_len = id+1;
   }
   return id;   
}


void BlackListRemoveIP(int index)
{
   int last = B_IP_len -1;
   if(last == 0) //have just one in list,no sense just delete file NOOO!!!
   {
      B_IP_len = 0;
      SendMessage(hIP,LVM_DELETEALLITEMS,(WPARAM)0,(LPARAM)0);                
      return;
   }
   else if(last != index) //the item to be removet is the last one
   {
        memset(B_IP[index].ip,0,16);     //zeror the item to be removed
        strcpy(B_IP[index].ip,B_IP[last].ip); //copy the last one to this
        ListViewChangeNameIP(index,B_IP[index].ip);     //change the name in listview
        //find if the jump item is checked
        if(ListView_GetCheckState(hIP, last) == 1) //checked
        {
              ListView_SetCheckState(hIP, index,1);
        }
        else
        {                                           
           ListView_SetCheckState(hIP, index,0);
        }  
   }
   B_IP_len = last;  //reset counter list   
   memset(B_IP[last].ip,0,16);
   ListView_DeleteItem(hIP,last);   
}

void FileToIP(unsigned char *data)
{
    IP_LIST *T = (IP_LIST*)data;
    int ret;
    int id;
    //reset list,new bat list or new dir list
    SendMessage(hIP,LVM_DELETEALLITEMS,(WPARAM)0,(LPARAM)0); 
    
    B_IP_len = 0;
    memset(B_IP,0,sizeof(B_IP));
    for(ret=0;ret<MAX_FILTER_IP;ret++)
    {
        if(T[ret].ip[0] != 0)
        {
            id =  BlackListAddIP(T[ret].ip); 
            if(T[id].block[0] == 'B')
                   ListView_SetCheckState(hIP, id,1);
            continue;
        }
        break;
    }
}

void BlackListChangeIP(int index,UCHAR *ip)
{
     memset(B_IP[index].ip,0,16);
     strncpy(B_IP[index].ip,ip,15); 
     ListViewChangeNameIP(index,B_IP[index].ip);                                                          
}


void BlackListSetCheckIP(int index,UCHAR l)
{
     B_IP[index].block[0] = l;
}

UCHAR *BlackListGetIP(int index)
{
     return B_IP[index].ip;
}

//------------------------------------ read save ---------------------------------
void ReadDataFromFile()
{
    HANDLE fd;
    DWORD len;
    DWORD result;
    unsigned char *AllData;
    unsigned char *p;
   
    fd = CreateFile(FilterBuffer, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
    if(fd == INVALID_HANDLE_VALUE)
    {
       SetWindowText(hWnd,"#Error : OpenFIle");
       return;
    }
    len = GetFileSize(fd,NULL);
    if(len == 0xFFFFFFFF)
    {
        CloseHandle(fd);
       SetWindowText(hWnd,"#Error : GetFileSize");
       return;
    }    
    AllData = (unsigned char*) malloc(sizeof(unsigned char)*len);
    if(!AllData)
    {
        CloseHandle(fd);
       SetWindowText(hWnd,"#Error : malloc");
       return;
    }  
    // copy the file into the buffer:
    if(ReadFile(fd, AllData, len, &result, NULL) == FALSE)
    {
        free(AllData);
        CloseHandle(fd);
       SetWindowText(hWnd,"#Error : ReadFile");
       return;
    } 
    if(result != len)
    {
        free(AllData);
        CloseHandle(fd);
 		SetWindowText(hWnd,"#ReadFile( readLen != requestLen )");
       return;
    } 
    CloseHandle(fd); 
    FileToHost(AllData);
    p = AllData + (sizeof(HOST_LIST)*MAX_FILTER_HOST);
    FileToIP(p);
    free(AllData);
    SetWindowText(hWnd,"DONE");
    return;
}

void WriteDataToFile()
{
    HANDLE fd;
    DWORD len;
    DWORD result;
    unsigned char *p;
    if(FilterBuffer[0]==0) strcpy(FilterBuffer,"Filter.dat\0");
    
    fd = CreateFile(FilterBuffer, GENERIC_WRITE	, 0, NULL, CREATE_ALWAYS	, 0, 0);
    if(fd == INVALID_HANDLE_VALUE)
    {
       SetWindowText(hWnd,"#Error : CreateFIle");
       return;
    }
  
    p = (unsigned char*)B_HOST;
    if(WriteFile(fd, p, sizeof(B_HOST), &result, NULL) == FALSE)
    {
        CloseHandle(fd);
        SetWindowText(hWnd,"#Error : WriteFile");
        return;
    } 
    p = (unsigned char*)B_IP;
    if(WriteFile(fd, p, sizeof(B_IP), &result, NULL) == FALSE)
    {
        CloseHandle(fd);
        SetWindowText(hWnd,"#Error : WriteFile");
        return;
    } 
    CloseHandle(fd); 
    SetWindowText(hWnd,"SAVED");
    return;
}
