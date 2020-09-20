#include "allsocklib.h"
//code from mbedtls

void SetTimeStruct(unsigned int sec, struct timeval *tv)
{
       tv->tv_sec  = sec;
       tv->tv_usec = 0;
return;
}

unsigned char TCP_send(int fd,unsigned char *buffer,int size)
{
     int ret;
     do
     {
          ret = send(fd,buffer,size,0);
     }while( (ret  == SOCKET_ERROR )  && ( WSAGetLastError() == WSAEWOULDBLOCK ));
     
     if(ret > 0)
     {
         if(ret < size)
             return TCP_send(fd,buffer+ret,size-ret);
         return 1;
     }
   return 0;
}

int CheckNetForRecv(int fd,int maxfd, fd_set *read_fds,struct timeval *tv)
{
    int ret;
    fd_set this_fds;
    do
    {
         memcpy(&this_fds, read_fds, sizeof(fd_set));
         ret = select(maxfd+1, &this_fds, NULL, NULL, tv );
    }
    while((ret == SOCKET_ERROR) && (WSAGetLastError() == WSAEINTR) );
    if(ret > 0)
    {
         if(FD_ISSET(fd, &this_fds))
            return 1;
         return 2;    
    }
  return 0; //timeout
}

int UDP_recvfrom( int fd, unsigned char *buf, unsigned int len, SOCKADDR_IN *client, int *n)
{
    int ret;

    do
	{
         ret = recvfrom(fd, buf, len, 0, (struct sockaddr *)client, n );
	}while( (ret == SOCKET_ERROR)  && ( WSAGetLastError() == WSAEWOULDBLOCK ));
    return ret;
}

int TCP_recv( int fd, unsigned char *buf, unsigned int len )
{
    int ret;
    
    do
    {
         ret = recv( fd, buf, len ,0);
    }while( (ret == SOCKET_ERROR)  && ( WSAGetLastError() == WSAEWOULDBLOCK ));
    return ret;
}

int TCP_recv_TMO( int fd,int maxfd ,unsigned char *buf, unsigned int len, fd_set *read_fds, struct timeval *tv)
{
    int ret;

    ret = CheckNetForRecv(fd, maxfd, read_fds, tv);
    if(ret > 0)
      return TCP_recv( fd, buf, len );

   return 0;
}

int TCP_recv_timeout( int fd,int maxfd, unsigned char *buf, unsigned int len, unsigned int sec)
{
    int ret;
    struct timeval tv;
    fd_set read_fds;
 
    FD_ZERO( &read_fds );
    FD_SET( fd, &read_fds );

    SetTimeStruct(sec, &tv);
    
return TCP_recv_TMO(fd,maxfd, buf,len,&read_fds, &tv);
}

int TCP_accept( int fd)
{
    int ret;
    struct sockaddr_storage client_addr;

    int n = (int) sizeof( client_addr );
    do
    {
       ret = accept(fd, (struct sockaddr *) &client_addr, &n );
    }while( (ret == INVALID_SOCKET) && (WSAGetLastError() == WSAEWOULDBLOCK ) );
    return ret;
}

static unsigned short net_htons( unsigned int port )
{
    unsigned char buf[4];

    buf[0] = (unsigned char)( port >> 8 );
    buf[1] = (unsigned char)( port      );
    buf[2] = buf[3] = 0;

    return( *(unsigned short *) buf );
}

int TCP_bind(int port ,int maxlisten)
{
    int n = 1;
    int fd;
    struct sockaddr_in server_addr;

    fd = socket( AF_INET, SOCK_STREAM, IPPROTO_IP );

    if(fd < 0 )
        return -2;

    setsockopt( fd, SOL_SOCKET, SO_REUSEADDR,(const char *) &n, sizeof( n ) );

    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = net_htons( port );

    if( bind( fd, (struct sockaddr *) &server_addr, sizeof( server_addr ) ) < 0 )
    {
        closesocket(fd );
        return -1;
    }
    if( listen(fd, maxlisten) != 0 )
    {
        closesocket(fd );
        return 0;
    }
    return fd;
}

int UDP_bind(int port)
{
    int n = 1;
    int fd;
    struct sockaddr_in server_addr;

    fd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);

    if(fd < 0 )
        return -1;

    setsockopt( fd, SOL_SOCKET, SO_REUSEADDR,(const char *) &n, sizeof( n ) );

    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = net_htons( port );

    if( bind( fd, (struct sockaddr *) &server_addr, sizeof( server_addr ) ) < 0 )
    {
        closesocket(fd );
        return -0;
    }
 //   if( listen(fd, maxlisten) != 0 )
 //   {
 //       closesocket(fd );
 //       return 0;
 //   }
    return fd;
}

int TCP_connect(unsigned char *host, int port, unsigned char *IP)
{
    struct sockaddr_in server_addr;
    struct hostent *server_host;
    struct in_addr *myaddrB;
    int fd;
    server_host = gethostbyname( host );

    if(server_host == NULL)
        return -2;
    fd = socket( AF_INET, SOCK_STREAM, IPPROTO_IP );

    if(fd < 0 )
        return -1;

    memcpy( (void *) &server_addr.sin_addr,
            (void *) server_host->h_addr,
                     server_host->h_length );

    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = net_htons( port );

    if(connect( fd, (struct sockaddr *) &server_addr,sizeof( server_addr ) ) < 0 )
    {
        closesocket(fd);
        return 0;
    }
    if(IP != NULL)
    {
       	myaddrB= (struct in_addr*)(server_host->h_addr);   
                       
        sprintf(IP,"%u.%u.%u.%u\0", myaddrB->s_net,myaddrB->s_host,myaddrB->s_lh,myaddrB->s_impno);
    }
    return fd;
}

unsigned char IsSockValid(int fd)
{
    int error_code = 0;
	int opt_len = sizeof(error_code);
	getsockopt( fd, SOL_SOCKET, SO_ERROR, (char *)&error_code, &opt_len);
    if(error_code == WSAENOTSOCK)
    {
        return 0;
    }
    else if(error_code == WSAECONNRESET)
    {
         return 0;
    }
    else
    {
         return 1;
    }
}

void NetClose( int fd )
{
    unsigned char Ret;
    if(fd < 1)
        return; 
    Ret = IsSockValid(fd);
    
   	if(Ret > 0)
   	{
       shutdown( fd, 2 );
       closesocket( fd );
    }
    return;
}

