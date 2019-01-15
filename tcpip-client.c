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
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>

//#define PORT 5000


#pragma warning(disable: 4996)

//void validateStr (char *inStr , char *outStr);  
void validateInput(char **argv, char *CONN_TYPE, char *ADDRESS, char *PORT, char *BLOCK_SIZE, char *NUM_BLOCK);





int main (int argc, char *argv[])
{
  int                my_server_socket, len, done;
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

if (argc < 6 || argc > 6 )
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












  // /*
  //  * initialize struct to get a socket to host
  //  */
  memset (&server_addr, 0, sizeof (server_addr));
  server_addr.sin_family = AF_INET;
    if(inet_pton(AF_INET, ADDRESS, &server_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  server_addr.sin_port = htons (PORT);





  // // SET UP BLOCK SIZE AND BYTES
  // // WE ARE GOING TO SEND THE NUMBER OF SIZE OF BLOCK AND BLOCK SIZE
  // // ASSUME THAT NUMBER OF BLOCK IS argv[4], BLOCK SIZE IS argv[3]
  // int sizeOfBlock = argv[3];
  // int numOfBlock = argv[4];








  // /*
  //  * get a socket for communications
  //  */
  printf ("[CLIENT] : Getting STREAM Socket to talk to SERVER\n");
  fflush(stdout);
  if ((my_server_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf ("[CLIENT] : Getting Client Socket - FAILED\n");
    return 3;
  }










  // /*
  //  * attempt a connection to server
  //  */
  printf ("[CLIENT] : Connecting to SERVER\n");
  fflush(stdout);
  if (connect (my_server_socket, (struct sockaddr *)&server_addr,sizeof (server_addr)) < 0)
  {
    printf ("[CLIENT] : Connect to Server - FAILED\n");
    close (my_server_socket);
    return 4;
  }




  //   printf("----------------    %s     %s   \n", argv[3], argv[4]);

  //   // WE ARE GOING TO SEND 2 MESSESSAGES:
  //   //  - GREETING MESSAGE (TELL HOW MANY BLOCK SIZE, SIZE OF BLOCK)
  //   //      THE SERVER WILL RESPONSE A MESSAGGE SAYING THAT THEY ARE READY
  //   //  - SENDING BLOCK IN A LOOP 
    char greetingMsg[BUFSIZ] = "";
    fflush (stdout);
    strcat(greetingMsg, CONN_TYPE);
    strcat(greetingMsg, " ");
    strcat(greetingMsg, tempBLOCK_SIZE);
    strcat(greetingMsg, " ");
    strcat(greetingMsg, tempNUM_BLOCKS);
    write(my_server_socket, greetingMsg, strlen(greetingMsg));

    fflush (stdout);
    char readyMSGServer[BUFSIZ] = "";
    read (my_server_socket, readyMSGServer, sizeof (readyMSGServer));
    // printf("SERVER RESPONSE A MESSAGE [%s]\n", readyMSGServer);




    // RIGHT AFTER THE SERVER IS READY, THE CLIENT WILL SEND THE DATA
    if (strcmp(readyMSGServer, "ready") == 0)
    {


      // Send the total blocks to server.
      clock_t start = clock();
      for (int i = 0 ; i < NUM_BLOCKS; i++)
      {
        char *buffer = NULL;
        char *tempBuffer;
        char *blockOrder = NULL;


        buffer = (char *)malloc(BLOCK_SIZE + 5);
        tempBuffer = (char *)malloc(BLOCK_SIZE + 5);
        blockOrder = (char *)malloc(BLOCK_SIZE + 7);
        //printf("BLOCK SIZE %d\n", BLOCK_SIZE);
        // Set the size of block to send to server
      
        fflush(stdout);
        // printf("1   buffer length %s is %d \n", buffer, (int)strlen(buffer));
        memset(buffer + 0, '0', BLOCK_SIZE);
        strncpy(tempBuffer, buffer, BLOCK_SIZE + 2);

        // WE ASIGN THE NUMBER FOR EACH BLOCK
        fflush(stdout);
        sprintf(blockOrder, "%d", i); 
        fflush (stdout);

        strcat(blockOrder, " ");
        strcat(blockOrder, tempBuffer);
        
        //printf("BLOCK ORDER IS [%s] and [%s]\n",blockOrder, buffer);

        char serverRes[BUFSIZ] = "";
        fflush (stdout);
        write(my_server_socket, blockOrder, BLOCK_SIZE + 1);
        free(buffer);
        free(blockOrder);
        free(tempBuffer);
      }



      //clock_t end;
      // THE SERVER WILL RESPONSE A "done[blockREcieved]".
      // FROM THERE, WE CAN CALCULATE THE timeSend, speed, 
      char testRes[BUFSIZ] = "";
      double timeSend = 0;
      double speed = 0;
      read(my_server_socket, testRes, sizeof(testRes));
      if (strncmp(testRes,"done", 4) == 0)
      {
        clock_t end = clock();
        timeSend = ((double)(end - start)) / CLOCKS_PER_SEC;
        //printf("         >>     THE TIME IS %lf mili seconds \n", (double)(end - start) * 1000 / CLOCKS_PER_SEC);
        speed = (double)((BLOCK_SIZE * NUM_BLOCKS * 8)/(timeSend*1000000));
        //printf("\n\n%s\n\n", testRes);
      }


      // ANOTHER PART THE RESPONSE ABOVE, WE CAN CALCULATE THE MISSING BLOCK
      //    AND BLOCK RECIEVED FROM SERVER
      fflush(stdout);
      sleep(1);
      char serverRecieved[BUFSIZ] = "";
      for (int i = strlen("done") + 1, j = 0; i < strlen(testRes); i++, j++)
      {
        if (testRes[i] != ' ')
        {
          serverRecieved[j] = testRes[i];
        }
      }
      char serverDisorder[BUFSIZ] = "";
      for (int i = strlen("done") + strlen(serverRecieved) + 2, k = 0; i < strlen(testRes); i++, k++)
      {
        serverDisorder[k] = testRes[i];
      }
      int missingBlock = NUM_BLOCKS - atoi(serverRecieved);

      printf("Size: %d, Sent: %d, Time: %.2lf mili seconds, Speed: %lf Mgb/sec, Missing: %d, Disordered: %d\n",
                  BLOCK_SIZE, NUM_BLOCKS, timeSend * 1000.0, speed, missingBlock, atoi(serverDisorder));



    }
    else
    {
      printf("Server fail\n");
    }





  /*
   * cleanup
   */
  close (my_server_socket);

  printf ("[CLIENT] : I'm outta here !\n");

  return 0;
}




void validateInput(char **argv, char *CONN_TYPE, char *ADDRESS, char *PORT, char *BLOCK_SIZE, char *NUM_BLOCK)
{
  for (int i = 1 ; i < 6; i++)
  {
    char temp[BUFSIZ] = "";
    // GET THE NEEDED CONTENT
    int j,k = 0;
    for (j = 2 ; j < strlen(argv[i]); j++)
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
      strcpy(CONN_TYPE,argv[i]);
    }
  }
}


