/*
 * tcpip-server.c
 *
 * This is a sample internet server application that will respond
 * to requests on port 5000
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
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>

//#define PORT 5000

// used for accepting incoming command and also holding the command's response
char buffer[BUFSIZ];

// global to keep track of the number of connections
static int nClients = 0;
static int nNoConnections = 0;

/* Watch dog timer - to keep informed and watch how long the server goes without a connection */
void alarmHandler(int signal_number)
{
  if (nClients == 0)
  {
    nNoConnections++;
    // It's been 10 seconds - determine how many 10 second intervals its been without a connection
    printf("[SERVER WATCH-DOG] : It's been %d interval(s) without a new client connection ...\n", nNoConnections);
    alarm(10); // reset alarm
  }
  else
  {
    // reset the number of times we've checked with no client connections
    nNoConnections = 0;
  }

  // reactivate signal handler for next time ...

  if ((nNoConnections == 3) && (nClients == 0))
  {
    printf("[SERVER WATCH-DOG] : Its been 30 seconds without a new CLIENT ... LEAVING !\n");
    exit(-1);
  }
  signal(signal_number, alarmHandler);
}

/*
 * this signal handler is used to catch the termination
 * of the child. Needed so that we can avoid wasting
 * system resources when "zombie" processes are created
 * upon exit of the child (as the parent could concievably
 * wait for the child to exit)
 */

void WatchTheKids(int n)
{
  /* when this signal is invoked, it means that one of the children of
       this process (the clients) had exited.  So let's decrement the 
       nClients value here ... */
  nClients--;

  /* given that a CLIENT has exited, let's reset the alarm 
       to make us check in 10 seconds */
  alarm(10); // reset alarm

  /* we received this signal because a CHILD (the CLIENT PROCESSOR) has exited ... 
       we (the SERVER) have no intention of doing anything or need anything from 
       that process - so the wait3() function call tells the O/S that we are 
       not waiting on anything from the terminated process ... so free up its 
       resources and get rid of it ...

       ... without this wait3() call ... the system would be littered with ZOMBIES */
  wait3(NULL, WNOHANG, NULL);
  signal(SIGCHLD, WatchTheKids);
}










// Returns hostname for the local computer 
void checkHostName(int hostname) 
{ 
    if (hostname == -1) 
    { 
        perror("gethostname"); 
        exit(1); 
    } 
} 
  
// Returns host information corresponding to host name 
void checkHostEntry(struct hostent * hostentry) 
{ 
    if (hostentry == NULL) 
    { 
        perror("gethostbyname"); 
        exit(1); 
    } 
} 

// Converts space-delimited IPv4 addresses 
// to dotted-decimal format 
void checkIPbuffer(char *IPbuffer) 
{ 
    if (NULL == IPbuffer) 
    { 
        perror("inet_ntoa"); 
        exit(1); 
    } 
} 





void parseClientMSG(char *buffer, char *CONN_TYPE, char *BLOCK_SIZE, char *NUM_BLOCKS)
{
  // GET CONN_TYPE
  for (int i = 0 ; i < strlen(buffer); i++)
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
  for (int i = strlen(CONN_TYPE) + 1; i < strlen(buffer); i++)
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
  for (int i = strlen(CONN_TYPE) + 1 + strlen(BLOCK_SIZE) + 1; i < strlen(buffer); i++)
  {
    NUM_BLOCKS[countNUMBLOCK] = buffer[i];
    countNUMBLOCK++;
  }
}






int getPORT(char *argv[])
{
  int PORT = 0;
  char myPORT[BUFSIZ] = "";

  for (int i = 2, j = 0; i < strlen(argv[1]); i++, j++)
  {
    myPORT[j] = argv[1][i];
  }
  PORT = atoi(myPORT);
  return PORT;
}



int main(int argc, char *argv[])
{
  int server_socket, client_socket;
  int client_len;
  struct sockaddr_in client_addr, server_addr;
  int len, i;
  FILE *p;

  char hostbuffer[256];
  struct hostent *host_entry;

  if (argc < 2)
  {
    fprintf(stderr, "ERROR, no port provided\n");
    exit(0);
  }

  // To retrieve hostname
  int hostname = gethostname(hostbuffer, sizeof(hostbuffer));
  checkHostName(hostname);

  // To retrieve host information
  host_entry = gethostbyname(hostbuffer);
  checkHostEntry(host_entry);

  // To convert an Internet network
  // address into ASCII string
  char *IPbuffer = inet_ntoa(*((struct in_addr *)
                                   host_entry->h_addr_list[0]));

  printf("Host IP: %s\n", IPbuffer);

  /*
   * install a signal handler for SIGCHILD (sent when the child terminates)
   */
  printf("[SERVER] : Installing signal handler to manage ZOMBIE processes ...\n");
  signal(SIGCHLD, WatchTheKids);
  printf("[SERVER] : Installing signal handler for WATCHDOG ...\n");
  signal(SIGALRM, alarmHandler);
  alarm(10);
  fflush(stdout); // try to ensure that the print messages appear on the screen








// GET A SOCKET FOR COMMUNICATION
  printf("[SERVER] : Obtaining STREAM Socket ...\n");
  fflush(stdout);
  if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("[SERVER] : Server Socket.getting - FAILED\n");
    return 1;
  }





int PORT = getPORT(argv);
printf("[prt iss         %d\n", PORT);


	



// INITIALIZE THE SERVER SOCKET FOR BINDING CLIENTS
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(PORT);







// BINDING THE SOCKET TO SERVER ADDRESS
  printf("[SERVER] : Binding socket to server address ...\n");
  fflush(stdout);
  if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    printf("[SERVER] : Binding of Server Socket - FAILED\n");
    close(server_socket);
    return 2;
  }







// SERVER IS LISTENING ON SOCKET FOR INCOMMING CLIENTS
  printf("[SERVER] : Begin listening on socket for incoming message ...\n");
  fflush(stdout);
  if (listen(server_socket, 5) < 0)
  {
    printf("[SERVER] : Listen on Socket - FAILED.\n");
    close(server_socket);
    return 3;
  }








  /*
   * for this server, run an endless loop that will
   * accept incoming requests from a remote client.
   * the server will fork a child copy of itself to handle the
   * request, and the parent will continue to listen for the
   * next request
   */
  while (1)
  {
    /*
     * accept a packet from the client
     */
    client_len = sizeof(client_addr);
    if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len)) < 0)
    {
      printf("[SERVER] : Accept Packet from Client - FAILED\n");
      close(server_socket);
      return 4;
    }






    /* Clear out the Buffer */
    memset(buffer, 0, BUFSIZ);

    printf("[SERVER] : Incoming CLIENT connection on SOCKET %d\n", client_socket);
    fflush(stdout);
    nClients++;







    /*
     * fork a child to do the actual processing for the incoming command
     */
    if (fork() == 0)
    {
      int whichClientAmI = 0;
      // I  am the child, so let's process the incoming client connection
      whichClientAmI = nClients;

      printf("  [SERVER'S CHILD #%d] : Closing SERVER socket ...\n", whichClientAmI);
      fflush(stdout);
      close(server_socket); // the CHILD doesn't need this ...



      // SERVER FIRST TIME WILL RECIEVE THE GREETING MESSAGE FROM CLIENT
      //    THEN GET THE SIZE OF BLOCK AND BLOCK SIZE 
      read(client_socket, buffer, BUFSIZ);
      printf("SERVER RECIEVED GRERTING MSG FROM CLIENT [%s]\n", buffer);



      char BLOCK_SIZE[BUFSIZ] = "";
      char NUM_BLOCKS[BUFSIZ] = "";
      char CONN_TYPE[BUFSIZ] = "";
      parseClientMSG(buffer, CONN_TYPE, BLOCK_SIZE, NUM_BLOCKS);
      printf("CONN TYPE [%s], NUM BLOCK [%s], BLOCK SIZE [%s]\n", CONN_TYPE, NUM_BLOCKS, BLOCK_SIZE);




      printf("SERVER IS RESPONDING THAT IT IS READY TO RECIEVE DATA\n");
      fflush(stdout);
      char readyMsg[BUFSIZ] = "ready";
      write(client_socket, readyMsg, strlen(readyMsg));





      // FROM THIS, WE GET TYPE OF CONNECTION, EXPECTED SIZE OF BLOCK, EXPECTED BLOCK SIZE
      char temp[BUFSIZ] = "";
      int tempBlockRecieved = 0;
      int disorder = 0;

      for (int i = 0 ; i < atoi(NUM_BLOCKS) ; i++)
      {
        char *bufferRecieved;
        bufferRecieved = (char*)malloc(atoi(BLOCK_SIZE) + 10);
        fflush(stdout);
        int a = read(client_socket, bufferRecieved, atoi(BLOCK_SIZE)+1);
        //printf("NUMBER OF BYTE READ [%d]", a);
        tempBlockRecieved = tempBlockRecieved + a/atoi(BLOCK_SIZE);


        //printf(">>>>>>>>>>>>>   [%s]\n", bufferRecieved);



        // GET THE ORDER FROM CLIENT PACKETS
        char packetORDER[BUFSIZ] = "";
        for (int i = 0; i < strlen(bufferRecieved); i++)
        {
          if (bufferRecieved[i] == ' ')
          {
            break;
          }
          packetORDER[i] = bufferRecieved[i];
        }


        // CHECK THE ORDER
        if (atoi(packetORDER) != i)
        {

          disorder++;
        }
        free(bufferRecieved);
      }





      //printf("\n\nDISORDER IS %d \n\n", disorder);





      // LET'S SEND THE RESULT = "done [block recieved] [disorder]"
      //printf("The number of byte recieved: %d\n", tempBlockRecieved);
      fflush(stdout);
      char endReading[BUFSIZ] = "";
      char blockRecieved[BUFSIZ] = "";
      char disorderBLOCK[BUFSIZ] = "";
      sprintf(blockRecieved,"%d", tempBlockRecieved);
      sprintf(disorderBLOCK, "%d", disorder); 
      strcpy(endReading,"done");
      strcat(endReading," ");
      strcat(endReading, blockRecieved);
      strcat(endReading, " ");
      strcat(endReading, disorderBLOCK);
      printf("[%s] will be sent to client lastly\n", endReading);
      write(client_socket, endReading, strlen(endReading));







      fflush(stdout);
      printf("  [SERVER'S CHILD #%d] : Closing CLIENT Socket and EXITING...\n", whichClientAmI);
      fflush(stdout);
      close(client_socket);

      return 0; // this will signal the parent (server)
    }
    else
    {
      /*
       * this is done by parent ONLY - we'll close the client socket that we
       * opened and go back and listen for another command
       */
      printf("[SERVER] : Closing CLIENT Socket\n");
      fflush(stdout);
      close(client_socket);
    }
  }

  return 0;
}





