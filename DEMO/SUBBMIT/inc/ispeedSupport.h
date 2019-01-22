//* FILE			: ispeedSupport.h
//* PROJECT			: Industrial Application Development - Assignment 1
//* PROGRAMMER		: Kha Phan
//* FIRST VERSON	: Jan 21, 2018
//* DESCRIPTION		: The file declare all library, prototypes, constant using in the application



#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <stdbool.h>
#include <time.h>


#define MAXLINE 1024



#ifdef _WIN32

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501  /* Windows XP. */
#endif
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include <iphlpapi.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment (lib, "Ws2_32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

#define EXPECTED_INDEX 7
void getIPWIN(char *ip);
int get_local_server_ip_address(char *ip);
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#define bcopy(b1,b2,len) (memmove((b2), (b1), (len)), (void) 0)
#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))
#pragma comment(lib, "iphlpapi.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/time.h>
#include<errno.h>
#include <netinet/ip.h>
#include<ifaddrs.h>
#define NANO_PER_SEC 1000000000.0

void getIPLinux(char IP[BUFSIZ]);
#endif



//	PROTOTYPE
int sockInit(void);
int myMax(int x, int y);
void parseClientMSG(char *buffer, char *CONN_TYPE, char *BLOCK_SIZE, char *NUM_BLOCKS);
void generateResult(char *resultMSG, char *BLOCK_SIZE, char *NUM_BLOCK, int MISSING, int DISORDER, double TIME_SEND, double SPEED);
static double TimeSpecToSeconds(struct timespec* ts);
char *buildData(int NUM_BLOCKS, int BLOCK_SIZE);
void goSleep(int second);
void closeSocket(int socket);
int server(int PORT);
int TCPIPClient(int PORT, char* ADDRESS, int NUM_BLOCKS, int BLOCK_SIZE);
int UDPClient(int PORT, char* ADDRESS, int NUM_BLOCKS, int BLOCK_SIZE);
