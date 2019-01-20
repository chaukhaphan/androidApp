
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


#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501  /* Windows XP. */
#endif
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma warning(disable:4996)

#else
/* Assume that any non-Windows platform uses POSIX-style sockets instead. */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
#include <unistd.h> /* Needed for close() */
#include <sys/time.h>
#define NANO_PER_SEC 1000000000.0
#endif



//#define PORT 5002
#define MAXLINE 1024


#ifdef _WIN32
#else
int max(int x, int y)
{
	if (x > y)
		return x;
	else
		return y;
}
#endif





#ifdef _WIN32
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#define bcopy(b1,b2,len) (memmove((b2), (b1), (len)), (void) 0)
#else
#endif


int sockInit(void);
void parseClientMSG(char *buffer, char *CONN_TYPE, char *BLOCK_SIZE, char *NUM_BLOCKS);
void generateResult(char *resultMSG, char *BLOCK_SIZE, char *NUM_BLOCK, int MISSING, int DISORDER, double TIME_SEND, double SPEED);
int get_local_server_ip_address(char *ip);
static double TimeSpecToSeconds(struct timespec* ts);
char *buildData(int NUM_BLOCKS, int BLOCK_SIZE);
void goSleep(int second);
void closeSocket(int socket);



int server(int PORT);
int TCPIPClient(int PORT, char* ADDRESS, int NUM_BLOCKS, int BLOCK_SIZE);
int UDPClient(int PORT, char* ADDRESS, int NUM_BLOCKS, int BLOCK_SIZE);



int main(int argc, char **argv)
{
	int PORT = 0;
	int BLOCK_SIZE = 0;
	int NUM_BLOCKS = 0;
	char ADDRESS[BUFSIZ] = "";
	char CONN_TYPE[BUFSIZ] = "";

	//=========================================================================
	//	- If only port is supported (argc = 3), it is server
	//	- If argc = 10, we check whether user want to run TCP or UDP client and 
	//		call appropriate client.
	//=========================================================================
	if (argc == 3)
	{
		PORT = atoi(argv[2]);
		server(PORT);
	}
	else if (argc == 10)
	{
		bool hasTCP = false;
		for (int i = 0; i < argc; i++)
		{
			if (strcmp(argv[i], "-TCP") == 0)
			{
				hasTCP = true;
			}
			else
			{
				if (strcmp(argv[i], "-p") == 0)
				{
					PORT = atoi(argv[i + 1]);
				}
				else if (strcmp(argv[i], "-a") == 0)
				{
					strcpy(ADDRESS, argv[i + 1]);
				}
				else if (strcmp(argv[i], "-s") == 0)
				{
					BLOCK_SIZE = atoi(argv[i + 1]);
				}
				else if (strcmp(argv[i], "-n") == 0)
				{
					NUM_BLOCKS = atoi(argv[i + 1]);
				}

			}
		}


		if (hasTCP == true)
		{
			TCPIPClient(PORT, ADDRESS, NUM_BLOCKS, BLOCK_SIZE);
		}
		else
		{
			UDPClient(PORT, ADDRESS, NUM_BLOCKS, BLOCK_SIZE);
		}
	}
	else
	{
		printf("\nSERVER: [-p] [PORT]\n");
		printf("CLIENT: [-TCP]/[-UDP] [-p] [PORT] [-a] [ADDRESS] [-n] [NUM OF BLOCK] [-s] [BLOCK SIZE]\n");
	}

	return 0;
}







int server(int PORT)
{
	printf("Is it server\n");
	printf("%d\n", PORT);

	int listenfd, connfd, udpfd, nready, maxfdp1;
	char buffer[MAXLINE];
	fd_set rset;
	size_t n;
	socklen_t len;
	const int on = 1;
	struct sockaddr_in cliaddr, servaddr;
	char* message = (char *)"Hello Client";
	void sig_chld(int);


	fd_set master; // master file descriptor list
	fd_set read_fds; // temp file descriptor list for select()

	FD_ZERO(&master); // clear the master and temp sets
	FD_ZERO(&read_fds);


#ifdef _WIN32
	WSADATA wsaData;	// WinSock initialization data
	char ip[512];
	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR) {
		return 1;
	}	/* endif */
	int result = 0;
	if ( result= sockInit() != 0)
	{
		printf("WSAStartup failed with error: %d\n", result);
		return 1;
	}
	get_local_server_ip_address(ip);
	printf("SERVER IP: %s\n", ip);
#else
	char server_ip[BUFSIZ] = "";
	get_local_server_ip_address(server_ip);
	printf("SERVER IP: %s\n", server_ip);
#endif


	/* create listening TCP socket */
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// binding server addr structure to listenfd
	bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	listen(listenfd, 10);

	/* create UDP socket */
	udpfd = socket(AF_INET, SOCK_DGRAM, 0);
	// binding server addr structure to udp sockfd
	bind(udpfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

	// clear the descriptor set
	FD_ZERO(&rset);

	// get maxfd
	maxfdp1 = max(listenfd, udpfd) + 1;
	for (;;)
	{
		// set listenfd and udpfd in readset
		FD_SET(listenfd, &rset);
		FD_SET(udpfd, &rset);

		// select the ready descriptor
		nready = select(maxfdp1, &rset, NULL, NULL, NULL);


		//===================================================================================//
		//								TCP/IP
		//===================================================================================//
		if (FD_ISSET(listenfd, &rset))
		{
			len = sizeof(cliaddr);
			connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len);


#ifdef _WIN32
			//---------------------------------------------------------//
			fflush(stdout);
			char BLOCK_SIZE[BUFSIZ] = "";
			char NUM_BLOCKS[BUFSIZ] = "";
			char CONN_TYPE[BUFSIZ] = "";
			char greetingMSG[BUFSIZ] = "";
			strncpy(buffer, "", sizeof(buffer));
			recv(connfd, buffer, BUFSIZ, 0);
			//---------------------------------------------------------//

			printf("%s\n", buffer);

			parseClientMSG(buffer, CONN_TYPE, BLOCK_SIZE, NUM_BLOCKS);
			int convertedBLOCK_SIZE = atoi(BLOCK_SIZE);
			int convertedNUM_BLOCK = atoi(NUM_BLOCKS);



			//---------------------------------------------------------//
			send(connfd, "ready", BUFSIZ, 0);
			//---------------------------------------------------------//




			char *dataRecieve;
			dataRecieve = (char *)malloc(convertedBLOCK_SIZE * convertedNUM_BLOCK + convertedNUM_BLOCK);
			strcpy(dataRecieve, "");
			int expected = convertedBLOCK_SIZE * convertedNUM_BLOCK;


#ifdef _WIN32

			int waitTime = 5000;
#else
			int waitTime = 5;
#endif
			struct timeval timeout = { waitTime,0 }; //set timeout for 2 seconds
			setsockopt(connfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(struct timeval));

			//---------------------------------------------------------//
			double TIME_SEND = 0;
			double SPEED = 0;
			int actual = 0;
			int numByteRec = 0;
			bool distract = false;
			LARGE_INTEGER frequency;        // ticks per second
			LARGE_INTEGER t1, t2;           // ticks
			QueryPerformanceFrequency(&frequency);
			QueryPerformanceCounter(&t1);
			while (actual != expected)
			{
				numByteRec = recv(connfd, dataRecieve + actual, expected - actual, 0);
				actual = numByteRec + actual;
				if (numByteRec < 0)
				{
					distract = true;
					break;
				}
			}
			QueryPerformanceCounter(&t2);
			dataRecieve[actual] = '\0';
			printf("Byte recieved: %d\n", actual);
			//---------------------------------------------------------//




			TIME_SEND = (double)((t2.QuadPart - t1.QuadPart) * 1000 / frequency.QuadPart);
			if (TIME_SEND > waitTime && distract == true)
			{
				TIME_SEND = TIME_SEND - waitTime;
			}
			SPEED = (double)((actual * 8) / (TIME_SEND * 1000));




			//---------------------------------------------------------//
			int MISSING = (expected - actual) / convertedBLOCK_SIZE;		// because of the first block does not have '.'
			int DISORDER = 0;

			char *blockToCheck;
			blockToCheck = (char *)malloc(convertedBLOCK_SIZE + 1);
			strcpy(blockToCheck, "");
			int pos = 0;
			int j = 0;

			bool gotFRIST = false;
			int FIRST = 0;
			int SECOND = 0;
			while (1)
			{

				strcpy(blockToCheck, "");
				for (int i = 1 * pos; i < actual; i++, j++)
				{
					blockToCheck[j] = dataRecieve[i];
					if (dataRecieve[i] == '.')
					{
						blockToCheck[j + 1] = '\0';
						pos = pos + j + 1;
						j = 0;
						break;
					}
				}

				// CHECK DISORDER
				char temp[10] = "";

				for (int count = 0; count < (int)strlen(blockToCheck); count++)
				{
					if (blockToCheck[count] != 'a')
					{
						temp[count] = blockToCheck[count];
					}
					else
					{
						if (gotFRIST == false)
						{
							FIRST = atoi(temp);
							gotFRIST = true;
						}
						else
						{
							SECOND = atoi(temp);
							if (SECOND - 1 != FIRST)
							{
								DISORDER++;
							}
							gotFRIST = false;
						}
						break;
					}
				}

				if (pos == actual - j)
				{
					break;
				}

			}


			strncpy(buffer, "", sizeof(buffer));
			generateResult(buffer, BLOCK_SIZE, NUM_BLOCKS, MISSING, DISORDER, TIME_SEND, SPEED);
			send(connfd, buffer, sizeof(buffer), 0);
			fflush(stdout);
			//---------------------------------------------------------//



			closesocket(connfd);

#else
			if ((fork()) == 0)
			{
				fflush(stdout);
				close(listenfd);

				//---------------------------------------------------------//

				char BLOCK_SIZE[BUFSIZ] = "";
				char NUM_BLOCKS[BUFSIZ] = "";
				char CONN_TYPE[BUFSIZ] = "";
				char greetingMSG[BUFSIZ] = "";
				recv(connfd, greetingMSG, BUFSIZ, 0);
				//---------------------------------------------------------//


				parseClientMSG(greetingMSG, CONN_TYPE, BLOCK_SIZE, NUM_BLOCKS);
				int convertedBLOCK_SIZE = atoi(BLOCK_SIZE);
				int convertedNUM_BLOCK = atoi(NUM_BLOCKS);
				//---------------------------------------------------------//
				fflush(stdout);
				send(connfd, "ready", BUFSIZ, 0);
				//---------------------------------------------------------//



				char *dataRecieve;
				dataRecieve = (char *)malloc(convertedBLOCK_SIZE * convertedNUM_BLOCK + convertedNUM_BLOCK);

				strcpy(dataRecieve, "");
				int expected = convertedBLOCK_SIZE * convertedNUM_BLOCK;




#ifdef _WIN32

				int waitTime = 5000;
#else
				int waitTime = 5;
#endif
				struct timeval timeout = { waitTime,0 }; //set timeout for 2 seconds
				setsockopt(connfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(struct timeval));




				//---------------------------------------------------------//
				double TIME_SEND = 0;
				double SPEED = 0;
				int actual = 0;
				int numByteRec = 0;
				bool distract = false;
				struct timespec start, end;
				double start_sec, end_sec, elapsed_sec;
				clock_gettime(CLOCK_REALTIME, &start);
				while (actual != expected)
				{
					numByteRec = recv(connfd, dataRecieve + actual, expected - actual, 0);
					actual = numByteRec + actual;
					if (numByteRec < 0)
					{
						distract = true;
						break;
					}
				}
				clock_gettime(CLOCK_REALTIME, &end);
				start_sec = start.tv_sec + start.tv_nsec / NANO_PER_SEC;
				end_sec = end.tv_sec + end.tv_nsec / NANO_PER_SEC;
				elapsed_sec = end_sec - start_sec;
				dataRecieve[actual] = '\0';
				//---------------------------------------------------------//

				TIME_SEND = elapsed_sec;
				if (TIME_SEND > waitTime && distract == true)
				{
					TIME_SEND = TIME_SEND - waitTime;
				}
				SPEED = (double)((actual * 8) / (TIME_SEND * 1000000));

				//---------------------------------------------------------//
				int MISSING = (expected - actual) / convertedBLOCK_SIZE; // because of the first block does not have '.'
				int DISORDER = 0;

				char *blockToCheck;
				blockToCheck = (char *)malloc(convertedBLOCK_SIZE + 1);
				strcpy(blockToCheck, "");
				int pos = 0;
				int j = 0;

				bool gotFRIST = false;
				int FIRST = 0;
				int SECOND = 0;
				while (1)
				{
					strcpy(blockToCheck, "");
					for (int i = 1 * pos; i < actual; i++, j++)
					{
						blockToCheck[j] = dataRecieve[i];
						if (dataRecieve[i] == '.')
						{
							blockToCheck[j + 1] = '\0';
							pos = pos + j + 1;
							j = 0;
							break;
						}
					}

					// CHECK DISORDER
					char temp[10] = "";

					for (int count = 0; count < (int)strlen(blockToCheck); count++)
					{
						if (blockToCheck[count] != 'a')
						{
							temp[count] = blockToCheck[count];
						}
						else
						{
							if (gotFRIST == false)
							{
								FIRST = atoi(temp);
								gotFRIST = true;
							}
							else
							{
								SECOND = atoi(temp);
								if (SECOND - 1 != FIRST)
								{
									DISORDER++;
								}
								gotFRIST = false;
							}
							break;
						}
					}

					if (pos == actual - j)
					{
						break;
					}

				}


				char myResult[BUFSIZ] = "";
				generateResult(myResult, BLOCK_SIZE, NUM_BLOCKS, MISSING, DISORDER, TIME_SEND, SPEED);
				send(connfd, myResult, strlen(myResult), 0);
				//---------------------------------------------------------//



				close(connfd);
				exit(0);
			}
			else
			{
				fflush(stdout);
				closeSocket(connfd);
			}
			close(connfd);
#endif
		}
		//===================================================================================//
		//								UDP
		//===================================================================================//
		if (FD_ISSET(udpfd, &rset))
		{

			{
				len = sizeof(cliaddr);

				//---------------------------------------------------------//
				fflush(stdout);
				char BLOCK_SIZE[BUFSIZ] = "";
				char NUM_BLOCKS[BUFSIZ] = "";
				char CONN_TYPE[BUFSIZ] = "";
				char greetingMSG[BUFSIZ] = "";
				strncpy(buffer, "", sizeof(buffer));
				recvfrom(udpfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr, &len);
				//---------------------------------------------------------//
				printf("UDP HAHA %s\n", buffer);



				parseClientMSG(buffer, CONN_TYPE, BLOCK_SIZE, NUM_BLOCKS);
				int convertedBLOCK_SIZE = atoi(BLOCK_SIZE);
				int convertedNUM_BLOCK = atoi(NUM_BLOCKS);




				//---------------------------------------------------------//
				sendto(udpfd, "ready", sizeof("ready"), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
				//---------------------------------------------------------//



				char *dataRecieve;
				dataRecieve = (char *)malloc(convertedBLOCK_SIZE * convertedNUM_BLOCK + convertedNUM_BLOCK);
				strcpy(dataRecieve, "");
				int expected = convertedBLOCK_SIZE * convertedNUM_BLOCK;






#ifdef _WIN32

				int waitTime = 5000;
#else
				int waitTime = 5;
#endif
				struct timeval timeout = { waitTime,0 }; //set timeout for 2 seconds
				setsockopt(udpfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(struct timeval));



				//---------------------------------------------------------//
				double TIME_SEND = 0;
				double SPEED = 0;
				int actual = 0;
				int numByteRec = 0;
				bool distract = false;
#ifdef _WIN32

				LARGE_INTEGER frequency;        // ticks per second
				LARGE_INTEGER t1, t2;           // ticks
				QueryPerformanceFrequency(&frequency);
				QueryPerformanceCounter(&t1);
#else
				struct timespec start, end;
				double start_sec, end_sec, elapsed_sec;
				clock_gettime(CLOCK_REALTIME, &start);
#endif


				while (actual != expected)
				{
					numByteRec = recvfrom(udpfd, dataRecieve + actual, expected - actual, 0, (struct sockaddr*)&servaddr, &len);
					actual = numByteRec + actual;
					if (numByteRec < 0)
					{
						printf("what the heck is UDP\n");
						distract = true;
						break;
					}
				}
				printf("Actual is %d expected is %d\n", actual, expected);

#ifdef _WIN32

				QueryPerformanceCounter(&t2);
				TIME_SEND = (double)((t2.QuadPart - t1.QuadPart) * 1000 / frequency.QuadPart);
				if (TIME_SEND > waitTime && distract == true)
				{
					TIME_SEND = TIME_SEND - waitTime;
				}
				SPEED = (double)((actual * 8) / (TIME_SEND * 1000));
#else
				clock_gettime(CLOCK_REALTIME, &end);
				start_sec = start.tv_sec + start.tv_nsec / NANO_PER_SEC;
				end_sec = end.tv_sec + end.tv_nsec / NANO_PER_SEC;
				elapsed_sec = end_sec - start_sec;
				TIME_SEND = (double)(elapsed_sec);	// SEC
				if (TIME_SEND > waitTime && distract == true)
				{
					TIME_SEND = TIME_SEND - waitTime;
				}
				SPEED = (double)((actual * 8) / (TIME_SEND * 1000000));

#endif
				dataRecieve[actual] = '\0';
				//---------------------------------------------------------//








				//---------------------------------------------------------//
				int MISSING = (expected - actual) / convertedBLOCK_SIZE;		// because of the first block does not have '.'
				int DISORDER = 0;

				//printf("%s\n", dataRecieve);
				char *blockToCheck;
				blockToCheck = (char *)malloc(convertedBLOCK_SIZE + 1);
				strcpy(blockToCheck, "");
				int pos = 0;
				int j = 0;

				bool gotFRIST = false;
				int FIRST = 0;
				int SECOND = 0;
				while (1)
				{
					strcpy(blockToCheck, "");
					for (int i = 1 * pos; i < actual; i++, j++)
					{
						blockToCheck[j] = dataRecieve[i];
						if (dataRecieve[i] == '.')
						{
							blockToCheck[j + 1] = '\0';
							pos = pos + j + 1;
							j = 0;
							break;
						}
					}

					// CHECK DISORDER
					char temp[10] = "";

					for (int count = 0; count < (int)strlen(blockToCheck); count++)
					{
						if (blockToCheck[count] != 'a')
						{
							temp[count] = blockToCheck[count];
						}
						else
						{
							if (gotFRIST == false)
							{
								FIRST = atoi(temp);
								gotFRIST = true;
							}
							else
							{
								SECOND = atoi(temp);
								if (SECOND - 1 != FIRST)
								{
									DISORDER++;
								}
								gotFRIST = false;
							}
							break;
						}
					}

					if (pos == actual - j)
					{
						break;
					}

				}



				strncpy(buffer, "", sizeof(buffer));
				generateResult(buffer, BLOCK_SIZE, NUM_BLOCKS, MISSING, DISORDER, TIME_SEND, SPEED);
				sendto(udpfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
				fflush(stdout);
				//---------------------------------------------------------//

			}

		}
	}
	return 0;
}


int UDPClient(int PORT, char* ADDRESS, int NUM_BLOCKS, int BLOCK_SIZE)
{
	printf("Is it UDP client\n");
	printf("%s %d %d %d\n", ADDRESS, PORT, NUM_BLOCKS, BLOCK_SIZE);

	int sockfd;
	char buffer[MAXLINE];
	struct sockaddr_in servaddr;
	int n, len;


#ifdef _WIN32
	int result = sockInit();
	if ( result  != 0)
	{
		printf("WSAStartup failed with error: %d\n", result);
		return 1;
	}
#else
#endif


	// Creating socket file descriptor 
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("socket creation failed");
		exit(0);
	}

	memset(&servaddr, 0, sizeof(servaddr));

	// Filling server information 
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = inet_addr(ADDRESS);

	len = sizeof(servaddr);


	//---------------------------------------------------------//
	fflush(stdout);
	char greetingMsg[BUFSIZ] = "";
	char CONN_TYPE[BUFSIZ] = "UCP";
	char tempBLOCK_SIZE[BUFSIZ] = "";
	char tempNUM_BLOCKS[BUFSIZ] = "";
	sprintf(tempBLOCK_SIZE, "%d", BLOCK_SIZE);
	sprintf(tempNUM_BLOCKS, "%d", NUM_BLOCKS);
	strcat(greetingMsg, CONN_TYPE);
	strcat(greetingMsg, " ");
	strcat(greetingMsg, tempBLOCK_SIZE);
	strcat(greetingMsg, " ");
	strcat(greetingMsg, tempNUM_BLOCKS);
	sendto(sockfd, greetingMsg, strlen(greetingMsg), 0, (const struct sockaddr*)&servaddr, len);
	//---------------------------------------------------------//



	//---------------------------------------------------------//
	fflush(stdout);
	char readyMSGServer[BUFSIZ] = "";
	recvfrom(sockfd, readyMSGServer, sizeof(readyMSGServer), 0, (struct sockaddr*)&servaddr, &len);
	//---------------------------------------------------------//


	//---------------------------------------------------------//
	if (strncmp(readyMSGServer, "ready", (int)strlen("ready")) == 0)
	{
		char *dataSend = NULL;
		dataSend = buildData(NUM_BLOCKS, BLOCK_SIZE);

		int numByteSend = 0;
		int actual = 0;
		int expected = BLOCK_SIZE * NUM_BLOCKS;

		fflush(stdout);
		int byteToSend = BLOCK_SIZE;
		while (actual != expected)
		{
			numByteSend = sendto(sockfd, dataSend + actual, byteToSend, 0, (const struct sockaddr*)&servaddr, len);
			actual = numByteSend + actual;
			byteToSend = expected - actual;
			if (byteToSend >= BLOCK_SIZE)
			{
				byteToSend = BLOCK_SIZE;
			}
		}
	}
	//---------------------------------------------------------//

	goSleep(1);

	//---------------------------------------------------------//
	fflush(stdout);
	char resultMSG[BUFSIZ] = "";
	int a = recvfrom(sockfd, resultMSG, sizeof(resultMSG), 0, (struct sockaddr*)&servaddr, &len);
	printf("%s\n", resultMSG);
	//---------------------------------------------------------//


	closeSocket(sockfd);
	return 0;
}



int TCPIPClient(int PORT, char* ADDRESS, int NUM_BLOCKS, int BLOCK_SIZE)
{
	printf("It is TCP IP client\n");
	printf("%s %d num block %d block %d\n", ADDRESS, PORT, NUM_BLOCKS, BLOCK_SIZE);

	int sockfd;
	char buffer[MAXLINE];
	struct sockaddr_in servaddr;

#ifdef _WIN32
	int result = 0;
	if (result = sockInit() != 0)
	{
		printf("WSAStartup failed with error: %d\n", result);
		return 1;
	}
#else
#endif

	int n, len;
	// Creating socket file descriptor 
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket creation failed");
		exit(0);
	}

	memset(&servaddr, 0, sizeof(servaddr));


	// Filling server information 
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = inet_addr(ADDRESS);

	if (connect(sockfd, (struct sockaddr*)&servaddr,
		sizeof(servaddr)) < 0) {
		printf("\n Error : Connect Failed \n");
		closeSocket(sockfd);
		return -1;
	}



	//---------------------------------------------------------//
	fflush(stdout);
	char greetingMsg[BUFSIZ] = "";
	char CONN_TYPE[BUFSIZ] = "TCP";
	char tempBLOCK_SIZE[BUFSIZ] = "";
	char tempNUM_BLOCKS[BUFSIZ] = "";
	sprintf(tempBLOCK_SIZE, "%d", BLOCK_SIZE);
	sprintf(tempNUM_BLOCKS, "%d", NUM_BLOCKS);
	strcat(greetingMsg, CONN_TYPE);
	strcat(greetingMsg, " ");
	strcat(greetingMsg, tempBLOCK_SIZE);
	strcat(greetingMsg, " ");
	strcat(greetingMsg, tempNUM_BLOCKS);
	send(sockfd, greetingMsg, strlen(greetingMsg), 0);
	//---------------------------------------------------------//



	//---------------------------------------------------------//
	fflush(stdout);
	char readyMSGServer[BUFSIZ] = "";
	recv(sockfd, readyMSGServer, sizeof(readyMSGServer), 0);
	//---------------------------------------------------------//


	//---------------------------------------------------------//
	if (strncmp(readyMSGServer, "ready", (int)strlen("ready")) == 0)
	{
		char *dataSend = NULL;
		dataSend = buildData(NUM_BLOCKS, BLOCK_SIZE);

		int numByteSend = 0;
		int actual = 0;
		int expected = BLOCK_SIZE * NUM_BLOCKS;


		fflush(stdout);
		int byteToSend = BLOCK_SIZE;
		while (actual != expected)
		{
			numByteSend = send(sockfd, dataSend + actual, byteToSend, 0);
			actual = numByteSend + actual;
			byteToSend = expected - actual;
			if (byteToSend >= BLOCK_SIZE)
			{
				byteToSend = BLOCK_SIZE;
			}
		}
	}


	goSleep(1);


	//---------------------------------------------------------//
	fflush(stdout);
	char resultMSG[BUFSIZ] = "";
	int a = recv(sockfd, resultMSG, sizeof(resultMSG), 0);
	printf("%s\n", resultMSG);
	//---------------------------------------------------------//




	closeSocket(sockfd);
	return 0;
}










void generateResult(char *resultMSG, char *BLOCK_SIZE, char *NUM_BLOCK, int MISSING, int DISORDER, double TIME_SEND, double SPEED)
{
#ifdef _WIN32
#else
	TIME_SEND = TIME_SEND * 1000;
#endif
	char missing[BUFSIZ] = "";
	sprintf(missing, "%d", MISSING);
	char disorder[BUFSIZ] = "";
	sprintf(disorder, "%d", DISORDER);
	char time_send[BUFSIZ] = "";
	sprintf(time_send, "%.2lf", TIME_SEND);
	char speed[BUFSIZ] = "";
	sprintf(speed, "%.2lf", SPEED);

	strcpy(resultMSG, "Size: ");
	strcat(resultMSG, BLOCK_SIZE);
	strcat(resultMSG, ", Sent: ");
	strcat(resultMSG, NUM_BLOCK);
	strcat(resultMSG, ", Time: ");
	strcat(resultMSG, time_send);
	strcat(resultMSG, " mili seconds, Speed: ");
	strcat(resultMSG, speed);
	strcat(resultMSG, " Mgb/sec, Missing: ");
	strcat(resultMSG, missing);
	strcat(resultMSG, ", Disordered: ");
	strcat(resultMSG, disorder);
	// printf("Size: %d, Sent: %d, Time: %.2lf mili seconds, Speed: %lf Mgb/sec, Missing: %d, Disordered: %d\n",
	// 			   convertedBLOCK_SIZE, convertedNUM_BLOCK, TIME_SEND * 1000.0, SPEED, MISSING, DISORDER);
}




int sockInit(void)
{
#ifdef _WIN32
	WSADATA wsa_data;
	return WSAStartup(MAKEWORD(1, 1), &wsa_data);
#else
	return 0;
#endif
}




void closeSocket(int socket)
{
#ifdef _WIN32
	closesocket(socket);
#else
	close(socket);
#endif
}




void goSleep(int second)
{
#ifdef _WIN32
	Sleep(second * 1000);
#else
	sleep(second);
#endif
}




char *buildData(int NUM_BLOCKS, int BLOCK_SIZE)
{
	char *dataSend;
	dataSend = (char*)malloc(NUM_BLOCKS*BLOCK_SIZE + NUM_BLOCKS * 3);
	strcpy(dataSend, "");
	char *contentData;
	contentData = (char *)malloc(BLOCK_SIZE + 1);
	memset(contentData + 0, 'a', BLOCK_SIZE);			// A BLOCK WILL BE ASSIGNED
	char *test;
	test = (char *)malloc(BLOCK_SIZE + 10);
	strcpy(test, "");


	for (int i = 0; i < NUM_BLOCKS; i++)
	{
		fflush(stdout);
		// Get number for each block
		char whichData[5] = "";
		sprintf(whichData, "%d", i + 1);

		// The data will be: "[whichData] [contentData]"
		if (i != 0)
		{
			strcat(test, ".");
		}
		strcat(test, whichData);
		strcat(test, contentData);
		test[BLOCK_SIZE] = '\0';
		strcat(dataSend, test);
		strcpy(test, "");
	}

	return dataSend;
}




void parseClientMSG(char *buffer, char *CONN_TYPE, char *BLOCK_SIZE, char *NUM_BLOCKS)
{
	// GET CONN_TYPE
	for (int i = 0; i < (int)strlen(buffer); i++)
	{
		if (buffer[i] != ' ')
		{
			CONN_TYPE[i] = buffer[i];
		}
		else
		{
			break;
		}
	}


	// GET BLOCK_SIZE
	int countBLOCKSIZE = 0;
	for (int i = (int)strlen(CONN_TYPE) + 1; i < (int)strlen(buffer); i++)
	{
		if (buffer[i] != ' ')
		{
			BLOCK_SIZE[countBLOCKSIZE] = buffer[i];
			countBLOCKSIZE++;
		}
		else
		{
			break;
		}
	}


	// GET NUM_BLOCKS
	int countNUMBLOCK = 0;
	for (int i = (int)strlen(CONN_TYPE) + 1 + (int)strlen(BLOCK_SIZE) + 1; i < (int)strlen(buffer); i++)
	{
		NUM_BLOCKS[countNUMBLOCK] = buffer[i];
		countNUMBLOCK++;
	}
}






// make sure you link with WSOCK32.LIB
// LITTLEENDIAN already defined in winsock2.h

int get_local_server_ip_address(char *ip)
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
	sprintf(ip, "%d.%d.%d.%d",
		(addr & 0xFF),
		((addr >> 8) & 0xFF),
		((addr >> 16) & 0xFF),
		((addr >> 24) & 0xFF)
	);
	// #ifdef LITTLEENDIAN
	// 	sprintf(ip, "%d.%d.%d.%d",
	// 		(addr & 0xFF),
	// 		((addr >> 8) & 0xFF),
	// 		((addr >> 16) & 0xFF),
	// 		((addr >> 24) & 0xFF)
	// 	);
	// #else
	// 	sprintf(ip, "%d.%d.%d.%d",
	// 		((addr >> 24) & 0xFF),
	// 		((addr >> 16) & 0xFF),
	// 		((addr >> 8) & 0xFF),
	// 		(addr & 0xFF)
	// 	);
	// #endif

	return 1;
}	/* end get_local_server_ip_addres */




static double TimeSpecToSeconds(struct timespec* ts)
{
	return (double)ts->tv_sec + (double)ts->tv_nsec / 1000000000.0;
}
