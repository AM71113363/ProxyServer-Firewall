#include "main.h"

unsigned char SocketList[MAX_SOCKET];
int NrTreads = 0;
UCHAR ShowBytesBuf[]="Thread[ 12345678 ]\0";
UCHAR MainThreadDone = YES;
UCHAR MainThreadSleep = NO;

UCHAR IsRunning()
{
    if(MainThreadDone == YES)
       return NO; 
   return YES;   
}

void SleepNow(UCHAR x)
{
   MainThreadSleep = x; 
}

//call ShowBytes before closing socket
void ShowBytes(unsigned char n, int sock,int remote)
{
    EnterCriticalSection(&cs);
    if(n == YES){ NrTreads++; SocketList[sock] = YES; SocketList[remote] = YES; }
    else {        NrTreads--; SocketList[sock] = NO;  SocketList[remote] = NO;  }
    sprintf(ShowBytesBuf,"Thread[ %03i ]\0",NrTreads);
    SetWindowText(hWnd,ShowBytesBuf);
	if(NrTreads == 0)
	{
         if(MainThreadDone == YES)
         {
	          SetWindowText(hWnd,"ALL THREADS ARE DONE");
         }
         else if(SNDMSG(hStayClean,BM_GETCHECK,0,0)==BST_CHECKED)
         {
             SendMessage(hlv,LVM_DELETEALLITEMS,(WPARAM)0,(LPARAM)0); 
         }
    }
	LeaveCriticalSection(&cs);
	return;
}

void GetOut(HANDLE heap, UCHAR *buffer, int sock ,int remote, int id, UCHAR *Cmsg, UCHAR *Smsg)
{
     ShowBytes(NO,sock,remote);   
     NetClose(sock);
     NetClose(remote);
     HeapFree(heap,0,(LPVOID)buffer); 
     SetClient(id,Cmsg);	
     SetServer(id,Smsg);	
}


void WORKER(LPVOID _f_)
{
    int sock = (int)_f_;
    int remotefd = -1;
    unsigned char *buffer = NULL;
    unsigned char *pbuffer = NULL;
    unsigned int bufferLen = MAX_BUFSIZE;
    unsigned int DefLen = 0;  //the size of received buffer
    fd_set read_fds;

    unsigned char url[MAX_PATH];
    unsigned char hst[MAX_PATH];
    unsigned char PP[]="80\0";
    unsigned char *p, *t;
    unsigned char Ret;
    int MyID;
    int ret;
    int len;
    int maxfd;
    int infd;
	int outfd;
	int err;
    DWORD nClient = 0;
    DWORD nServer = 0;
    HANDLE heap = GetProcessHeap(); //HeapFree(hHeap,0,(LPVOID)buffer);

    buffer = (unsigned char*)HeapAlloc(heap,0,MAX_BUFSIZE);
    if(buffer == NULL)
    {
        closesocket(sock);
        return;
    }
    
	FD_ZERO(&read_fds);
	FD_SET(sock, &read_fds);

    pbuffer = buffer;
    sprintf(url,"CLIENT[ %i ]\0",sock);
    ShowBytes(YES,sock,0);
    MyID = AddList(url,"","WAIT","");
	while(1)
	{
	     ret = TCP_recv_TMO(sock,sock, pbuffer, bufferLen, &read_fds, &tv);
         if(ret>0)
	     {
              DefLen += ret;
	          if(strstr(buffer,"\r\n\r\n"))
	                break;
	          bufferLen -= ret;
	          pbuffer += ret; 
              continue;
         }  
         break;
	}
    if(ret < 1)
    {
        GetOut(heap,buffer,sock, 0, MyID, "","");
        return;
    }
// --------------------------------------Analyse Packet---------------------------------------------
    ret = 0;
    p = strstr(buffer,"CONNECT ");
    if(p != NULL)
    {
        p+=8;
		t = strstr(p," HTTP/1");
		if(t != NULL)
		{
		    t[0] = 0;  
		    t = strstr(p,":");
		    if(t == NULL){ t = PP; } else { t[0] = 0; t++; }
            if(atoi(t) != 0)
	           ret = 1;
		}
        if(ret < 1)
        {
             GetOut(heap,buffer,sock, 0, MyID, "","");	
	         return;
        }   //p = host        //t = port
	    DefLen = 0;
    }
    else //!SSL,GET,POST ecc...
    {
        p = strstr(buffer,"\nHost:"); // No FullUrl No HostName
	    if(p==NULL)
		{
			 GetOut(heap,buffer,sock, 0, MyID, "","");
			 return;  
		} 
		len = 0;
		while(*p++ != ' '){ };
		memset(url,0,MAX_PATH);
		for(ret = 0; ret < MAX_PATH; ret++)
		{
	       if(p[ret]==' '|| p[ret]==':'|| p[ret]=='?' || p[ret]=='/' || p[ret]=='\\' || p[ret]=='\r' || p[ret]=='\n')
		   {
				break;
		   }
		  url[len++] = p[ret];
		}
		p=url;
		t = PP;
    }
    sprintf(hst,"%s:%s\0",p,t);
    SetHost(MyID, hst);	
    SetClient(MyID,"0");
    SetServer(MyID,"Connecting...");
    memset(hst,0,MAX_PATH);
	//----------------------------- PASS GRANTED -------------------------------------
    remotefd = TCP_connect(p, atoi(t) ,hst);
    SetIP(MyID,hst); 
    if(remotefd < 1)
    {
	    GetOut(heap,buffer,sock, 0, MyID, "!Connect","");
		return;  
	}
    //sanity check-- BlackList hostname
    Ret = IsHostBlackList(p);
    if(Ret == YES)
    {
	    GetOut(heap,buffer,sock, remotefd, MyID, "BLACK LIST","");
		return; 
    }
    //sanity check-- BlackList IP
    Ret = IsIPBlackList(hst);
    if(Ret == YES)
    {
	    GetOut(heap,buffer,sock, remotefd, MyID,"", "BLACK LIST");
        return; 
    }
    SetServer(MyID,"0");
    SocketList[remotefd] = YES;

    if(DefLen == 0) //proxy request
    {
         Ret = TCP_send(sock,OKgoON,strlen(OKgoON));
         if(Ret == 0)
         {
	         GetOut(heap,buffer,sock, remotefd, MyID,"", "");	
		     return;
         }
    }
	else
	{
	     sprintf(url,"%d\0",DefLen);
         SetClient(MyID,url);	
         nClient = DefLen;
         Ret = TCP_send(remotefd, buffer, DefLen);
	     if(Ret == 0)
         {
	         GetOut(heap,buffer,sock, remotefd, MyID,"", "");	
		     return;
         } 
    }
	//---------------------------------- loop -----------------------------------
	FD_SET(remotefd, &read_fds); //add remote sock to FD
	maxfd = remotefd;
	if(sock > remotefd) maxfd = sock;
	
	while(1)
	{
         len = CheckNetForRecv(sock , maxfd, &read_fds, &tv);
         if(len > 0)
         {
	          if(len == 1){    infd = sock;      outfd = remotefd;  }
	          else        {    infd = remotefd;  outfd = sock;      }
		      ret = TCP_recv(infd,buffer, MAX_BUFSIZE);
		      if(ret > 0)
		      {
                    if(len == 1){ nClient+=ret; sprintf(url,"%i\0",nClient); SetClient(MyID,url); }
                    else        { nServer+=ret; sprintf(url,"%i\0",nServer); SetServer(MyID,url); }    
                    Ret = TCP_send(outfd, buffer, ret);
		            if(Ret == 1)
                        continue;
              }
         }
         break;
        
	 }
     GetOut(heap,buffer,sock, remotefd, MyID,"", "");
    return;  
}     



void start_listen() 
{
	int c;
    int ret;
    HANDLE hTH;
    DWORD lpThreadId;
    memset(SocketList,0,MAX_SOCKET);
    
	listen_fd = TCP_bind(8080 , 64);
	if(listen_fd < 1)
	{
		SetWindowText(hWnd,"!TCP_bind");
        return;                 
    }
    SetWindowText(hWnd,"Thread[ 000 ]");
    EnableWindow(GetDlgItem(hWnd,1080),1);
    MainThreadDone = NO;
	while(1)
	{    
        if(MainThreadSleep == YES){ _sleep(4000); continue; }             
      
        c = TCP_accept(listen_fd);
		if(c>0 && c<MAX_SOCKET)
		{
             hTH = CreateThread(0,0,(LPTHREAD_START_ROUTINE)WORKER,(LPVOID)c,0,&lpThreadId);
             if(hTH != NULL)
		     {
                CloseHandle(hTH);
                continue;
             }
        }
        if(c < 1)
		   break; 
		closesocket(c);
	}
	MainThreadDone = YES;
	ret=MAX_SOCKET;
	while(ret>1)
	{
        if(SocketList[ret] == YES)
            NetClose(ret);  
        ret--;
    }
	EnableWindow(GetDlgItem(hWnd,1080),0);
  return;
}
