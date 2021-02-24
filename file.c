#include "main.h"

extern HOST_LIST B_HOST[MAX_FILTER_HOST];


void ReadDataFromFile()
{
    HANDLE fd;
    DWORD len;
    DWORD result;
    UCHAR AllData[0x1101]; //the real size is 4352=0x1100, just add an extra,who knows
    UCHAR *p;
    fd = CreateFile(dropped, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
    if(fd == INVALID_HANDLE_VALUE)
    {
       ShowAppStatus("Status: #Error : OpenFIle");
       return;
    }
    len = GetFileSize(fd,NULL);
    if(len != sizeof(B_HOST))
    {
       CloseHandle(fd);
       ShowAppStatus("Status: #Error : Is Not a Filter FILE!!!!");
       return;
    }    
    memset(AllData,0,0x1101);
    // copy the file into the buffer:
    if(ReadFile(fd, AllData, len, &result, NULL) == FALSE)
    {
        CloseHandle(fd);
        ShowAppStatus("Status: #Error : ReadFile");
        return;
    } 
    if(result != len)
    {
        CloseHandle(fd);
 		ShowAppStatus("Status: #ReadFile FAILED!!!");
        return;
    } 
    CloseHandle(fd); 
    FileToHost(AllData);
    p=strrchr(dropped,'\\');
    if(p){ p++; SetWindowText(hFilterName,p); }
    ShowAppStatus("Status: FILTER >> LOAD >> OK");
}

void WriteDataToFile()
{
    HANDLE fd;
    DWORD len;
    DWORD result;
//Get the Name
    memset(dropped,0,MAX_PATH);
    if(GetWindowText(hFilterName,dropped,MAX_PATH)<1)
    {
        SetWindowText(hFilterName,"Filter.dat");
        strcpy(dropped,"Filter.dat\0");
    }
    fd = CreateFile(dropped, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, 0);
    if(fd == INVALID_HANDLE_VALUE)
    {
       ShowAppStatus("Status: #Error : CreateFIle");
       return;
    }
    if(WriteFile(fd, B_HOST, sizeof(B_HOST), &result, NULL) == FALSE)
    {
        CloseHandle(fd);
        ShowAppStatus("Status: #Error : WriteFile");
        return;
    } 
    CloseHandle(fd); 
    ShowAppStatus("Status: FILTER >> SAVE >> OK");
}

