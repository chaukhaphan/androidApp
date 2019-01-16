/*
 * tcpip-client.c
 *
 * This is a sample internet client application that will talk
 * to the server application via port 5000
 */

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
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#else
 /* Assume that any non-Windows platform uses POSIX-style sockets instead. */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
#include <unistd.h> /* Needed for close() */
#endif


//#define PORT 5000


#pragma warning(disable: 4996)

//void validateStr (char *inStr , char *outStr);  
void validateInput(char **argv, char *CONN_TYPE, char *ADDRESS, char *PORT, char *BLOCK_SIZE, char *NUM_BLOCK);





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






int main(int argc, char *argv[])
{
#ifdef _WIN32
	SOCKET my_server_socket = INVALID_SOCKET;
#else
	int my_server_socket = 0;
#endif
	int                len, done;
	struct sockaddr_in server_addr;
	struct hostent*    host;



	// char *test;
	// test = (char *)malloc(10000+1);
	// memset(test,'0', 10000);
	// printf("%s  with the size is %d\n", test, strlen(test));
	// free(test);

	/*
	 * check for sanity
	 */
	 // if (argc != 2)
	 // {
	 //   printf ("USAGE : tcpipClient <server_name>\n");
	 //   return 1;
	 // }

	 /*
	  * determine host info for server name supplied
	 //  */
	 // if ((host = gethostbyname (argv[1])) == NULL)
	 // {
	 //   printf ("[CLIENT] : Host Info Search - FAILED\n");
	 //   return 2;
	 // }





   // argv[1] : type of connection
   // argv[2] : -aADDRESS
   // argv[3] : -pPORT
   // argv[4] : -sBLOCK_SIZE
   // argv[5] : -nNUM_BLOCKS
	char CONN_TYPE[BUFSIZ] = "";
	char ADDRESS[BUFSIZ] = "";
	int PORT = 0;
	int BLOCK_SIZE = 0;
	int NUM_BLOCKS = 0;
	char tempPORT[BUFSIZ] = "";
	char tempBLOCK_SIZE[BUFSIZ] = "";
	char tempNUM_BLOCKS[BUFSIZ] = "";

	if (argc < 6 || argc > 6)
	{
		printf("The number of argc is incorrect\n");
		return -10;
	}
	else
	{
		validateInput(argv, CONN_TYPE, ADDRESS, tempPORT, tempBLOCK_SIZE, tempNUM_BLOCKS);
		PORT = atoi(tempPORT);
		BLOCK_SIZE = atoi(tempBLOCK_SIZE);
		NUM_BLOCKS = atoi(tempNUM_BLOCKS);
	}






	//======================================================================================================================
//  IF WE ARE WORKING WITH WINDOWS, IT IS GOOD TO INITIALIZE THE SOCKET
//======================================================================================================================
#ifdef _WIN32
	if (int result = sockInit() != 0)
	{
		printf("WSAStartup failed with error: %d\n", result);
		return 1;
	}
#else
#endif





	// /*
	//  * initialize struct to get a socket to host
	//  */
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	//if (inet_pton(AF_INET, ADDRESS, &server_addr.sin_addr) <= 0)
	//{
	//	printf("\nInvalid address/ Address not supported \n");
	//	return -1;
	//}
	server_addr.sin_port = htons(PORT);





	// // SET UP BLOCK SIZE AND BYTES
	// // WE ARE GOING TO SEND THE NUMBER OF SIZE OF BLOCK AND BLOCK SIZE
	// // ASSUME THAT NUMBER OF BLOCK IS argv[4], BLOCK SIZE IS argv[3]
	// int sizeOfBlock = argv[3];
	// int numOfBlock = argv[4];








	// /*
	//  * get a socket for communications
	//  */
	printf("[CLIENT] : Getting STREAM Socket to talk to SERVER\n");
	fflush(stdout);
	my_server_socket = socket(AF_INET, SOCK_STREAM, 0);
#ifdef _WIN32
	if (my_server_socket == INVALID_SOCKET)
	{
		printf("[CLIENT] : Getting Client Socket - FAILED\n");
		sockQuit();
		return 3;
	}
#else
	if (my_server_socket < 0)
	{
		printf("[CLIENT] : Getting Client Socket - FAILED\n");
		return 3;
	}
#endif









	// /*
	//  * attempt a connection to server
	//  */
	printf("[CLIENT] : Connecting to SERVER\n");
	fflush(stdout);
	int connectResult = connect(my_server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
#ifdef _WIN32
	if (connectResult == SOCKET_ERROR)
	{
		printf("[CLIENT] : Connect to Server - FAILED\n");
		closesocket(my_server_socket);
		sockQuit();
		return 3;
	}
#else
	if (connectResult < 0)
	{
		printf("[CLIENT] : Connect to Server - FAILED\n");
		close(my_server_socket);
		return 4;
	}
#endif




	//   printf("----------------    %s     %s   \n", argv[3], argv[4]);

	//   // WE ARE GOING TO SEND 2 MESSESSAGES:
	//   //  - GREETING MESSAGE (TELL HOW MANY BLOCK SIZE, SIZE OF BLOCK)
	//   //      THE SERVER WILL RESPONSE A MESSAGGE SAYING THAT THEY ARE READY
	//   //  - SENDING BLOCK IN A LOOP 
	char greetingMsg[BUFSIZ] = "";
	fflush(stdout);
	strcat(greetingMsg, CONN_TYPE);
	strcat(greetingMsg, " ");
	strcat(greetingMsg, tempBLOCK_SIZE);
	strcat(greetingMsg, " ");
	strcat(greetingMsg, tempNUM_BLOCKS);
	send(my_server_socket, greetingMsg, strlen(greetingMsg), 0);

	fflush(stdout);
	char readyMSGServer[BUFSIZ] = "";
	recv(my_server_socket, readyMSGServer, sizeof(readyMSGServer), 0);
	// printf("SERVER RESPONSE A MESSAGE [%s]\n", readyMSGServer);







	// RIGHT AFTER THE SERVER IS READY, THE CLIENT WILL SEND THE DATA
	if (strcmp(readyMSGServer, "ready") == 0)
	{
		// MALLOC THE BUFFER TO HOLD THE WHOLE DATA
		char *data;
		data = (char*)malloc(NUM_BLOCKS*BLOCK_SIZE + NUM_BLOCKS*3);


		// GET BYTE
		char *contentData;
		contentData = (char *)malloc(BLOCK_SIZE + 1);
		memset(contentData + 0, '0', BLOCK_SIZE);			// A BLOCK WILL BE ASSIGNED






		// GET NUMBER AND FILL DATA FOR SENDING
		for (int i = 0; i < NUM_BLOCKS; i++)
		{
			char *test;
			test = (char *)malloc(BLOCK_SIZE + 1);
			fflush(stdout);
			// Get number for each block
			char whichData[5] = "";
			sprintf(whichData, "%d", i);

			// The data will be: "[whichData] [contentData]"
			strcat(test, whichData);
			strcat(test, ".");
			strcat(test, contentData);
			test[BLOCK_SIZE+1] = '\0';
			strcat(data, test);			
		}





		// BUFFER = "NUMBER 00000 NUMBER 00000 NUMBER 00000"
		//DUNG CAI LOOP CHAY 10 LAN, MOI LAN GUI LA "NUMBER 00000"
		int numByteSend = 0;
		int actual = 0;
		int expected = BLOCK_SIZE * NUM_BLOCKS + NUM_BLOCKS;
		while(actual != expected)
		{
			//printf("%s\n", data + actual);
			numByteSend = send(my_server_socket, data + actual, expected - actual, 0);
			//printf("actual %d but send %d\n",expected, numByteSend);
			
			actual = numByteSend + actual;
		}
		//printf("%s\n", data);


		

#ifdef _WIN32
		Sleep(1000);
#else
		sleep(1);
#endif

		fflush(stdout);
		char result[BUFSIZ] = "";
		int numByteRec = recv(my_server_socket, result, sizeof(result), 0);
		if (numByteRec >= 0)
		{
			printf("%s\n", result);
		}


		//free(data);
		//free(contentData);














		// clock_t start = clock();
		// for (int i = 0; i < NUM_BLOCKS; i++)
		// {
		// 	char *buffer = NULL;
		// 	char *tempBuffer;
		// 	char *blockOrder = NULL;


		// 	buffer = (char *)malloc(BLOCK_SIZE + 5);
		// 	tempBuffer = (char *)malloc(BLOCK_SIZE + 5);
		// 	blockOrder = (char *)malloc(BLOCK_SIZE + 7);
		// 	//printf("BLOCK SIZE %d\n", BLOCK_SIZE);
		// 	// Set the size of block to send to server

		// 	fflush(stdout);
		// 	// printf("1   buffer length %s is %d \n", buffer, (int)strlen(buffer));
		// 	memset(buffer + 0, '0', BLOCK_SIZE);
		// 	strncpy(tempBuffer, buffer, BLOCK_SIZE + 2);

		// 	// WE ASIGN THE NUMBER FOR EACH BLOCK
		// 	fflush(stdout);
		// 	sprintf(blockOrder, "%d", i);
		// 	fflush(stdout);

		// 	strcat(blockOrder, " ");
		// 	strcat(blockOrder, tempBuffer);

		// 	//printf("BLOCK ORDER IS [%s] and [%s]\n",blockOrder, buffer);

		// 	char serverRes[BUFSIZ] = "";
		// 	fflush(stdout);
		// 	send(my_server_socket, blockOrder, BLOCK_SIZE + 1, 0);
		// 	free(buffer);
		// 	free(blockOrder);
		// 	free(tempBuffer);
		// }



		//clock_t end;
		// THE SERVER WILL RESPONSE A "done[blockREcieved]".
		// FROM THERE, WE CAN CALCULATE THE timeSend, speed, 
		// char testRes[BUFSIZ] = "";
		// double timeSend = 0;
		// double speed = 0;
		// recv(my_server_socket, testRes, sizeof(testRes), 0);
		// if (strncmp(testRes, "done", 4) == 0)
		// {
		// 	clock_t end = clock();
		// 	timeSend = ((double)(end - start)) / CLOCKS_PER_SEC;
		// 	//printf("         >>     THE TIME IS %lf mili seconds \n", (double)(end - start) * 1000 / CLOCKS_PER_SEC);
		// 	speed = (double)((BLOCK_SIZE * NUM_BLOCKS * 8) / (timeSend * 1000000));
		// 	//printf("\n\n%s\n\n", testRes);
		// }


		// ANOTHER PART THE RESPONSE ABOVE, WE CAN CALCULATE THE MISSING BLOCK
		//    AND BLOCK RECIEVED FROM SERVER

		// char serverRecieved[BUFSIZ] = "";
		// for (int i = strlen("done") + 1, j = 0; i < strlen(testRes); i++, j++)
		// {
		// 	if (testRes[i] != ' ')
		// 	{
		// 		serverRecieved[j] = testRes[i];
		// 	}
		// }
		// char serverDisorder[BUFSIZ] = "";
		// for (int i = strlen("done") + strlen(serverRecieved) + 2, k = 0; i < strlen(testRes); i++, k++)
		// {
		// 	serverDisorder[k] = testRes[i];
		// }
		// int missingBlock = NUM_BLOCKS - atoi(serverRecieved);

		// printf("Size: %d, Sent: %d, Time: %.2lf mili seconds, Speed: %lf Mgb/sec, Missing: %d, Disordered: %d\n",
		// 	BLOCK_SIZE, NUM_BLOCKS, timeSend * 1000.0, speed, missingBlock, atoi(serverDisorder));



	}
	else
	{
		printf("Server fail\n");
	}





	/*
	 * cleanup
	 */
#ifdef _WIN32

	closesocket(my_server_socket);
	sockQuit();

#else
	close(my_server_socket);
#endif

	printf("[CLIENT] : I'm outta here !\n");

	return 0;
}




void validateInput(char **argv, char *CONN_TYPE, char *ADDRESS, char *PORT, char *BLOCK_SIZE, char *NUM_BLOCK)
{
	for (int i = 1; i < 6; i++)
	{
		char temp[BUFSIZ] = "";
		// GET THE NEEDED CONTENT
		int j, k = 0;
		for (j = 2; j < strlen(argv[i]); j++)
		{
			temp[k] = argv[i][j];
			k++;
		}

		// CHECK FOR WHAT TYPE OF CONTENT AND ADD TO THE SPECIFIC FIELD
		if (argv[i][0] == '-')
		{
			if (argv[i][1] == 'a')
			{
				strcpy(ADDRESS, temp);
			}
			else if (argv[i][1] == 'p')
			{
				strcpy(PORT, temp);
			}
			else if (argv[i][1] == 's')
			{
				strcpy(BLOCK_SIZE, temp);
			}
			else if (argv[i][1] == 'n')
			{
				strcpy(NUM_BLOCK, temp);
			}

		}
		else
		{
			strcpy(CONN_TYPE, argv[i]);
		}
	}
}







