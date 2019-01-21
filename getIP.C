#include <stdio.h>
#include <winsock2.h>
#include "pch.h"
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma warning(disable: 4996)
// make sure you link with WSOCK32.LIB
// LITTLEENDIAN already defined in winsock2.h
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "WSOCK32.LIB")
#pragma comment (lib, "AdvApi32.lib")
int
get_local_server_ip_address(char *ip)
{
	unsigned int addr;
	struct hostent *hp;
	char *localip = (char *)"127.0.0.1";
	char buf[512];

	gethostname(buf, 512);
	addr = inet_addr(buf);
	if (addr == 0xFFFFFFFF) {
		hp = gethostbyname(buf);
		if (hp == NULL)
			return 0;
		addr = *((unsigned int *)hp->h_addr_list[0]);
	}
	else {
		hp = gethostbyaddr((char *)&addr, 4, AF_INET);
		if (hp == NULL)
			return 0;
		addr = *((unsigned int *)hp->h_addr_list[0]);
	}

#ifdef LITTLEENDIAN
	sprintf(ip, "%d.%d.%d.%d",
		(addr & 0xFF),
		((addr >> 8) & 0xFF),
		((addr >> 16) & 0xFF),
		((addr >> 24) & 0xFF)
	);
#else
	sprintf(ip, "%d.%d.%d.%d",
		((addr >> 24) & 0xFF),
		((addr >> 16) & 0xFF),
		((addr >> 8) & 0xFF),
		(addr & 0xFF)
	);
#endif

	return 1;
}	/* end get_local_server_ip_addres */


int
main(void)
{
	WSADATA wsaData;	// WinSock initialization data
	char ip[512];

	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR) {
		return 1;
	}	/* endif */

	get_local_server_ip_address(ip);
	printf("ip is: %s\n", ip);

	WSACleanup();
	return 0;
}

