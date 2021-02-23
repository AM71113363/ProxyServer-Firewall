//#define _WIN32_IE  0x0501
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>

//void SetTimeStruct(unsigned int sec, struct timeval *tv)
//set timeout sec in tv
void SetTimeStruct(unsigned int , struct timeval *);

//unsigned char TCP_send(int fd,unsigned char *buffer,int size)
//protected : WSAEWOULDBLOCK
//send all bytes in loop
// 0   error send
// 1   success
unsigned char TCP_send(int ,unsigned char *,int );

//int CheckNetForRecv(int fd,int maxfd, fd_set *read_fds,struct timeval *tv)
//protected : WSAEINTR
//return 0   timeout or fail
//return 1 or 2 on success
int CheckNetForRecv(int ,int , fd_set *,struct timeval *);

//int UDP_recvfrom( int fd, unsigned char *buf, unsigned int len, SOCKADDR_IN *client, int *n)
//protected : WSAEWOULDBLOCK , MSG_PEEK
//success  return bytes received
int UDP_recvfrom( int , unsigned char *, unsigned int , SOCKADDR_IN *, int *);

//int TCP_recv( int fd, unsigned char *buf, unsigned int len )
//protected : WSAEWOULDBLOCK
//success  return bytes received
int TCP_recv( int , unsigned char *, unsigned int );

//int TCP_recv_TMO( int fd, int maxfd, unsigned char *buf, unsigned int len, fd_set *read_fds, struct timeval *tv)
//protected : select
//success  return bytes received
int TCP_recv_TMO( int, int, unsigned char *, unsigned int, fd_set *, struct timeval *);

//int TCP_recv_timeout( int fd,int maxfd, unsigned char *buf, unsigned int len, unsigned int sec)
//makeUp time struct and FD_SET
//call TCP_recv_TMO
int TCP_recv_timeout( int , int,  unsigned char *, unsigned int , unsigned int );

//int TCP_accept( int fd)
//protected : WSAEWOULDBLOCK
//success  return socket
int TCP_accept( int );

//int TCP_bind(int port ,int maxlisten)
//-2  error socket
//-1  error bind
// 0  error listen
//success  return socket
int TCP_bind( int , int );

//int UDP_bind(int port)
//-1  error socket
// 0  error bind
//success  return socket
int UDP_bind(int );

//int TCP_connect(unsigned char *host, int port, unsigned char *IP)
//-2  error gethostbyname
//-1  error socket
// 0  error connect
//success  return socket , if IP != NULL sprintf(IP xxx.xxx.xxx.xxx
int TCP_connect(unsigned char *, int ,unsigned char *);

//unsigned char IsSockValid(int fd)
//0 non valid socket
//1 is a valid socket
unsigned char IsSockValid( int );

//void NetClose( int fd );
//Gracefully close the connection
void NetClose( int );




