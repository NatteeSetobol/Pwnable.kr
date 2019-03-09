#ifndef NIX_SOCKET_H
#define NIX_SOCKET_H
#include <sys/mman.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

int CreateSocket(char urlString[], int portNumber)
{
	int rc=0;
	int sock=0;
	char *tmp_ptr;
	int port = 0;
	struct hostent *host = NULL;
	struct sockaddr_in dest_addr={};

	port = portNumber;

	host = gethostbyname(urlString);

	if (host == NULL)
	{
		printf("unable to get host\n");
	} else {
		sock = socket(AF_INET,SOCK_STREAM ,IPPROTO_TCP);

		if (sock == -1)
		{
			printf("Socket failed\n");
		} else {


			dest_addr.sin_family=AF_INET;
			dest_addr.sin_port = htons(port);

			memcpy(&dest_addr.sin_addr, host->h_addr_list[0], host->h_length);
			//dest_addr.sin_addr.s_addr = *(unsigned long*)(host->h_addr);
		
			memset(&(dest_addr.sin_zero),'\0',8);

		//	tmp_ptr = inet_ntoa(dest_addr.sin_addr);

			if (connect(sock, (struct sockaddr *) &dest_addr, sizeof(struct sockaddr)) == -1)
			{
				printf("could not connect to %s\n", urlString);
			}
			#if 1
			//fcntl(sock,F_SETFL,O_NONBLOCK);
			rc = fcntl(sock, F_SETFL, O_NONBLOCK);
			if (rc == -1) {
				perror("fcntl");
				return (EXIT_FAILURE);
			}

			/* Set stdin non-blocking */
			rc = fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
			if (rc == -1) {
				perror("fcntl");
				return (EXIT_FAILURE);
			}
			#endif


		}
	}


	return sock;
}

#endif
