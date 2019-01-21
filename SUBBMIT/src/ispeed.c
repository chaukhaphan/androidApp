
#ifdef _WIN32
#include "../inc/ispeedSupport.h"
#else
#include "../inc/ispeedSupport.h"
#endif





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
