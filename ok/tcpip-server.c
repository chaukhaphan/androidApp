/*
 * tcpip-server.c
 *
 * This is a sample internet server application that will respond
 * to requests on port 5000
 */




 //#include <unistd.h>
 //#include <sys/socket.h>
 //#include <arpa/inet.h>
 //#include <netdb.h>
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
 /* See http://stackoverflow.com/questions/12765743/getaddrinfo-on-win32 */
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
#endif




int numByteRec = 0;
int numByteSend = 0;
char buffer[BUFSIZ];




void generateResult(char *resultMSG, char *BLOCK_SIZE, char *NUM_BLOCK, int MISSING, int DISORDER, double TIME_SEND, double SPEED);
int sockInit(void);
int sockQuit();
int getPORT(char *argv[]);
void parseClientMSG(char *buffer, char *CONN_TYPE, char *BLOCK_SIZE, char *NUM_BLOCKS);
int get_local_server_ip_address (char *ip);



int main(int argc, char *argv[])
{
#ifdef _WIN32
	SOCKET server_socket = INVALID_SOCKET;
	SOCKET client_socket = INVALID_SOCKET;
#else
	int server_socket = 0;
	int client_socket = 0;
#endif

	int client_len;
	struct sockaddr_in client_addr, server_addr;
	int len = 0, i = 0;
	FILE *p = NULL;

	char hostbuffer[256];
	struct hostent *host_entry;





	if (argc < 2)
	{
		fprintf(stderr, "ERROR, no port provided\n");
		exit(0);
	}


	char server_ip[BUFSIZ] = "";
	int PORT = getPORT(argv);
	get_local_server_ip_address (server_ip);
	printf ("SERVER IP : %s, PORT : %d\n", server_ip, PORT);




	fflush(stdout); // try to ensure that the print messages appear on the screen
#ifdef _WIN32
	if (int result = sockInit() != 0)
	{
		printf("WSAStartup failed with error: %d\n", result);
		return 1;
	}
#else
#endif







// GET A SOCKET FOR COMMUNICATION
	//printf("[SERVER] : Obtaining STREAM Socket ...\n");
	fflush(stdout);
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
#ifdef _WIN32
	if (server_socket == INVALID_SOCKET)
	{
		printf("[SERVER] : Server Socket.getting - FAILED\n");
		sockQuit();
		return 1;
	}
#else
	if (server_socket < 0)
	{
		printf("[SERVER] : Server Socket.getting - FAILED\n");
		return 1;
	}
#endif














	// INITIALIZE THE SERVER SOCKET FOR BINDING CLIENTS
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);







	// BINDING THE SOCKET TO SERVER ADDRESS
	fflush(stdout);
	if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		printf("[SERVER] : Binding of Server Socket - FAILED\n");
#ifdef _WIN32
		closesocket(server_socket);
#else
		close(server_socket);
#endif
		sockQuit();
		return 2;
	}

		



	// SERVER IS LISTENING ON SOCKET FOR INCOMMING CLIENTS
	//printf("[SERVER] : Begin listening on socket for incoming message ...\n");
	fflush(stdout);
	if (listen(server_socket, 5) < 0)
	{
		printf("[SERVER] : Listen on Socket - FAILED.\n");
#ifdef _WIN32
		closesocket(server_socket);
#else
		close(server_socket);
#endif
		sockQuit();
		return 3;
	}



	while (1)
	{
		/*
		 * accept a packet from the client
		 */
		client_len = sizeof(client_addr);
		if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len)) < 0)
		{
#ifdef _WIN32
			closesocket(server_socket);
#else
			close(server_socket);
#endif
			sockQuit();
			return 4;
		}


		memset(buffer, 0, BUFSIZ);
		fflush(stdout);
		if (fork() == 0)
		{
		/*
		 * fork a child to do the actual processing for the incoming command
		 */
			fflush(stdout);
#ifdef _WIN32
			closesocket(server_socket);
#else
			close(server_socket);
#endif
			sockQuit();





			// SERVER FIRST TIME WILL RECIEVE THE GREETING MESSAGE FROM CLIENT
			//    THEN GET THE SIZE OF BLOCK AND BLOCK SIZE
			recv(client_socket, buffer, BUFSIZ, 0);
			//printf("SERVER RECIEVED GRERTING MSG FROM CLIENT [%s]\n", buffer);






			char BLOCK_SIZE[BUFSIZ] = "";
			char NUM_BLOCKS[BUFSIZ] = "";
			char CONN_TYPE[BUFSIZ] = "";
			parseClientMSG(buffer, CONN_TYPE, BLOCK_SIZE, NUM_BLOCKS);
			//printf("CONN TYPE [%s], NUM BLOCK [%s], BLOCK SIZE [%s]\n", CONN_TYPE, NUM_BLOCKS, BLOCK_SIZE);






			//printf("SERVER IS RESPONDING THAT IT IS READY TO RECIEVE DATA\n");
			fflush(stdout);
			char readyMsg[BUFSIZ] = "ready";
			send(client_socket, readyMsg, strlen(readyMsg), 0);






			int convertedBLOCK_SIZE = atoi(BLOCK_SIZE);
			int convertedNUM_BLOCK = atoi(NUM_BLOCKS);



			char *dataRecieve;
			dataRecieve = (char *)malloc(convertedBLOCK_SIZE * convertedNUM_BLOCK + convertedNUM_BLOCK);

			int actual = 0;
			int expected = convertedBLOCK_SIZE * convertedNUM_BLOCK + convertedNUM_BLOCK;

		
			struct timespec start, end;
    		double start_sec, end_sec, elapsed_sec;
			double NANO_PER_SEC = 1000000000.0;
	    	clock_gettime(CLOCK_REALTIME, &start);
			while(actual != expected)
			{
				numByteRec = recv(client_socket, dataRecieve + actual, expected - actual, 0);
				actual = numByteRec + actual;
			}
			clock_gettime(CLOCK_REALTIME, &end);
			start_sec = (double)(start.tv_sec + (double)(start.tv_nsec / NANO_PER_SEC));
			end_sec = (double)(end.tv_sec + (double)(end.tv_nsec / NANO_PER_SEC));



			double TIME_SEND = (double)(end_sec - start_sec);
			double SPEED = (double)((actual*8)/(TIME_SEND*1000000));
			//printf(">>  the byte recieved: %d and the length of recv buffer is %d\n", actual, (int)strlen(dataRecieve));
			//printf(">>>>    %s\n", dataRecieve);
			//printf(">>>>>>>> TIME SEND IN SECOND     %lf\n",TIME_SEND);
			//printf(">>>>>>>>>> SPEED IS MEGABIT/SEC   %lf\n", SPEED);


			// WE HAVE dataRecieve right now.
			// FROM THAT WE NEED TO CATCH UP MISSING AND DISORDER
			int MISSING = convertedNUM_BLOCK;
			int DISORDER = 0;

			char *tempBlock;
			char *blockToCheck;
			int firstOrder = 0;
			int secondOrder = 0;
			blockToCheck = (char *)malloc(convertedBLOCK_SIZE + 3);
			tempBlock = dataRecieve;
			// printf("               LENGTH OF RECIEVED BUFFER %d\n", (int)strlen(dataRecieve));
			for (int i = 0, orderBlock = 0; i < (int)strlen(dataRecieve); i++, orderBlock++)
			{
				char stepLength[5] = "";
				sprintf(stepLength, "%d", i);
				int convertedStepLength = (int)strlen(stepLength);


				fflush(stdout);
				// BECAUSE WE DONT TAKE THE SPACE BETWEEN BLOCKS IN THE DATA BUFFER,
				//	SO THE blockToCheck has the size smaller than actual 1 unit
				strncpy(blockToCheck, tempBlock, convertedBLOCK_SIZE);
				//printf("%s has length %d\n", blockToCheck, (int)strlen(blockToCheck));

				// USING ANOTHER LOOP TO CHECK THE MISSING AND DISORDER of blockToCHeck
				char whichBlock[4] = "";
				if ((int)strlen(blockToCheck) == convertedBLOCK_SIZE)
				{
					MISSING = MISSING - 1;
					// USING FOR LOOP TO CHECK THE ASSIGNED NUMBER OF EACH BLOCK
					for (int j = 0; j < 5; j++)
					{
						if (blockToCheck[j] != '.')
						{
							whichBlock[j] = blockToCheck[j];
					
						}
						else
						{
							break;
						}
					}
				}


				// IF THE SECOND NUMBER != FIRST NUMBER + 1. WE HAVE AN DISORDER
				if (i % 2 == 0)
				{
					firstOrder = atoi(whichBlock);
				}
				else
				{
					secondOrder = atoi(whichBlock);
					//printf("we are comparing %d and %d\n", firstOrder, secondOrder);
					if (secondOrder != firstOrder + 1)
					{
						DISORDER++;
					}
				}
				

				tempBlock = tempBlock + (int)strlen(blockToCheck) + 1;		// PLUS 1 BECAUSE OF '.'

				if ((int)strlen(tempBlock) == 0)
				{
					break;
				}
				
			}
			//printf("      MISSISSING:    %d\n", MISSING);
			printf("      DISORDER:    %d\n", DISORDER);
			//printf("DONE\n");


			char result[BUFSIZ] = "";
			generateResult(result, BLOCK_SIZE, NUM_BLOCKS, MISSING, DISORDER, TIME_SEND, SPEED);
			numByteSend = send(client_socket, result, strlen(result), 0);
			

			#ifdef _WIN32
						closesocket(client_socket);
						sockQuit();
			#else
						close(client_socket);

			#endif
						return 0;
		}
		else
		{
			//printf("[SERVER] : Closing CLIENT Socket\n");
			fflush(stdout);
			close(client_socket);
		}
	}
	return 0;
}





void generateResult(char *resultMSG, char *BLOCK_SIZE, char *NUM_BLOCK, int MISSING, int DISORDER, double TIME_SEND, double SPEED)
{
	char missing[BUFSIZ] = "";
	sprintf(missing, "%d", MISSING);
	char disorder[BUFSIZ] = "";
	sprintf(disorder, "%d", DISORDER);
	char time_send[BUFSIZ] = "";
	sprintf(time_send, "%lf", TIME_SEND*1000);
	char speed[BUFSIZ] = "";
	sprintf(speed, "%lf", SPEED);

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







int get_local_server_ip_address (char *ip)
{
	unsigned int addr;
	struct hostent *hp;				
	char *localip = "127.0.0.1";
	char buf[512];

	gethostname (buf, 512);
	addr = inet_addr(buf);
	if (addr == 0xFFFFFFFF) {
		hp = gethostbyname (buf);
		if (hp == NULL)
			return 0;
		addr = *((unsigned int *)hp->h_addr_list[0]);
	} else {
		hp = gethostbyaddr ((char *) &addr, 4, AF_INET);
		if (hp == NULL)
			return 0;
		addr = *((unsigned int *)hp->h_addr_list[0]);
	}
	sprintf (ip, "%d.%d.%d.%d", 
		(addr & 0xFF),
		((addr >> 8) & 0xFF),
		((addr >> 16) & 0xFF),
		((addr >> 24) & 0xFF)
		);
	return 1;
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






int getPORT(char *argv[])
{
	int PORT = 0;
	char myPORT[BUFSIZ] = "";

	for (int i = 2, j = 0; i < (int)strlen(argv[1]); i++, j++)
	{
		myPORT[j] = argv[1][i];
	}
	PORT = atoi(myPORT);
	return PORT;
}






//==============================================================================================
//	FOR WINDOWS
//==============================================================================================
int sockInit(void)
{
#ifdef _WIN32
	WSADATA wsa_data;
	return WSAStartup(MAKEWORD(1, 1), &wsa_data);
#else
	return 0;
#endif
}

int sockQuit()
{
#ifdef _WIN32
	return WSACleanup();
#else
	return 0;
#endif
}