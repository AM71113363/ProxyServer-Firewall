#include "main.h"

unsigned char SocketList[MAX_SOCKET];
int NrTreads = 0;
UCHAR ShowThreadsBuf[]="Thread[ 12345678 ]\0";

//static DWORD TIMEOUT_SEND = SEND_TIMEOUT;
static struct timeval tv = {.tv_sec = RECV_TIMEOUT, .tv_usec = 0};

static UCHAR OKgoON[]="HTTP/1.1 200 Connection established\r\nProxy-Agent: AM71113363\r\nProxy-Connection: keep-alive\r\nKeep-Alive: timeout=20\r\n\r\n";

//all sockets created after MAIN_Sock are greater
UCHAR SetSockList(int sock, UCHAR mode)
{
     SocketList[ (UINT)(sock-listen_fd) ] = mode;
  //   SocketList[ (UINT)(sock) ] = mode;
}

UCHAR GetSockList(int sock)
{
 //    if(SocketList[ (UINT)(sock) ] == YES)
     if(SocketList[ (UINT)(sock-listen_fd) ] == YES)
       return YES;
  return NO;
}

//call ShowThreads before closing socket
void ShowThreads(UCHAR n, int sock,int remote)
{
    EnterCriticalSection(&cs);
    if(n == YES){ NrTreads++; SetSockList(sock,YES); SetSockList(remote,YES); }
    else {        NrTreads--; SetSockList(sock, NO);  SetSockList(remote,NO);  }
    sprintf(ShowThreadsBuf,"Threads: [ %03i ]\0",NrTreads);
    ShowThreadStatus(ShowThreadsBuf);
	LeaveCriticalSection(&cs);
	return;
}

void GetOut(int sock ,int remote, int id, UCHAR *Cmsg, UCHAR *Smsg)
{
     ShowThreads(NO,sock,remote);   
     NetClose(remote);
     NetClose(sock);
     if(Cmsg) SetClient(id,Cmsg);	
     if(Smsg) SetServer(id,Smsg);	
}


void WORKER(LPVOID _f_)
{
    int sock = (int)_f_;
    int remotefd = -1;
    UCHAR buffer[MAX_BUFSIZE];
    unsigned char *pbuffer = NULL;
    unsigned int bufferLen = MAX_BUFSIZE;
    UINT DefLen = 0;  //the size of received buffer
    fd_set read_fds;
    UCHAR *index;
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
    
	FD_ZERO(&read_fds);
	FD_SET(sock, &read_fds);

    pbuffer = buffer;
    ShowThreads(YES,sock,0);

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
        GetOut(sock, 0, MyID, NULL,NULL);
        return;
    }
// ------Analyse Packet--
    ret = 0;
    p = strstr(buffer,"CONNECT ");
    if(p != NULL)
    {
        p+=8;
		t = strstr(p," HTTP/1");
		if(t != NULL)
		{
		    t[0] = 0;  
		    t = strchr(p,':');
		    if(t == NULL){ t = PP; } else { t[0] = 0; t++; }
            if(atoi(t) != 0)
	           ret = 1;
		}
        if(ret != 1)
        {
             GetOut(sock, 0, MyID, NULL,NULL);	
	         return;
        }   //p = host        //t = port
	    DefLen = 0;
    }
    else //!SSL,GET,POST ecc...
    {
        p = strstr(buffer,"\nHost:"); // No FullUrl No HostName
	    if(p==NULL)
		{
			 GetOut(sock, 0, MyID, NULL,NULL);
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
    //some Browsers send Random URLs, ignore them
    if(!strchr(p,'.'))
    {
        GetOut(sock, 0, MyID, NULL,NULL);
        return;                  
    }
    if(IgnoreClients==YES)
    {
        AddList(hst,"","","Refused");
        GetOut(sock, 0, MyID, NULL,NULL);
        return;                  
    }
    
    Ret = IsHostBlackList(hst);
    if(Ret == YES)
    {                
        GetOut(sock, 0, MyID, NULL,NULL);
		if(IgnoreBlocked==YES)
		     return;
		AddList(hst,"","BLACK_LIST",""); 
        return; 
    }     
	//------Connect to HOST    
    MyID = AddList(hst,"","0","Connecting...");
    memset(hst,0,MAX_PATH);

    remotefd = TCP_connect(p, atoi(t) ,hst);
    SetIP(MyID,hst); 
    if(remotefd < 1)
    {
	    GetOut(sock, 0, MyID, "!Connect",p);
		return;  
	}

    SetServer(MyID,"0");
    SetSockList(remotefd,YES);

    if(DefLen == 0) //proxy request
    {
         Ret = TCP_send(sock,OKgoON,strlen(OKgoON));
         if(Ret == NO)
         {
	         GetOut(sock, remotefd, MyID," ", " ");	
		     return;
         }
    }
	else
	{
	     sprintf(url,"%d\0",DefLen);
         SetClient(MyID,url);	
         nClient = DefLen;
         Ret = TCP_send(remotefd, buffer, DefLen);
	     if(Ret == NO)
         {
	         GetOut(sock, remotefd, MyID," ", " ");	
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
		            if(Ret == YES)
                        continue;
              }
         }
         break;
        
	 }
     GetOut(sock, remotefd, MyID," ", " ");
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
		ShowAppStatus("Status: #Error: TCP_bind");
        return;                 
    }
    ShowThreadStatus("Threads: [ 000 ]");
    ShowAppStatus("Status: ONLINE");
    SetWindowText(hStart,"STOP");
    EnableWindow(hStart,1);
    StopProxy = NO;
	while(1)
	{    
        if(StopProxy == YES)
          break;          
        
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
	ret=MAX_SOCKET;
	ShowAppStatus("Status: CloseAllConnections >> RUN...");
	while(ret>1)
	{
        if(GetSockList(ret) == YES)
            NetClose(ret);  
        ret--;
    }
    SetWindowText(hStart,"START");
    EnableWindow(hStart,1);
    StopProxy = YES;
	ShowAppStatus("Status: OFFLINE");
	if(IwantToExit == YES)
	{
        DeleteCriticalSection(&cs);
        WSACleanup();  
        exit(0);
   }         
  return;
}
