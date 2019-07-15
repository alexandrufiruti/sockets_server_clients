#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/fcntl.h>

#define PORT 8000
#define MAX 80

void check_it(int result, int line)
{
	if(result < 0)
	{
		fprintf(stderr, "(Error in %s at line %d: %s)\n", __FILE__, line-1, strerror(errno));
		exit(errno);
	}
}

int main(int argc, char **argv)
{
	if(argc==2)
	{
		int port;
		sscanf(argv[1], "%d", &port);
		struct sockaddr_in sockaddr, client_sockaddress;
		int s, c, rc;
		s = socket(AF_INET, SOCK_STREAM, 0); //create socket: Internet domain, stream socket type (TCP)
		check_it(s, __LINE__);

		//create internet socket address
		sockaddr.sin_family = AF_INET;
		struct in_addr addr;
		addr.s_addr = INADDR_ANY;
		sockaddr.sin_addr = addr;
		sockaddr.sin_port = port;

		//bind address to socket
		rc = bind(s, (const struct sockaddr *) &sockaddr, sizeof(sockaddr));
		check_it(rc, __LINE__);

		//listen on socket (passive socket), set backlog argument to 5 connection.
		rc = listen(s, 5);
		check_it(rc, __LINE__);
		// when a connection appears accept it
		socklen_t client_sockaddress_size; //size of the accepted client address structure (in bytes)
		c = accept(s, (struct sockaddr *) &client_sockaddress, &client_sockaddress_size);
		check_it(c, __LINE__);


		char buff[MAX];
		rc = recv(c, buff, sizeof(buff),0);
		check_it(rc, __LINE__);
		printf("%s\n", buff);
		//open file from received path
		FILE *fr = fopen(buff, "r");
		if(fr == NULL)
		{
			rc = send(c, "File not found", sizeof("File not found"), 0);
			check_it(rc, __LINE__);
		}
		else
		{
			fseek(fr, 0, SEEK_SET);
			rc = send(c, "File found", sizeof("File found"), 0);
			check_it(rc, __LINE__);
			char ch;
			//read and send file char by char
			while((ch=fgetc(fr))!=EOF)
			{
				printf("%c", ch);
				rc = send(c, &ch, 1, 0);
				check_it(rc, __LINE__);
			}
			fclose(fr);
			rc = send(c, &ch, 1, 0);
			check_it(rc, __LINE__);
		}

		close(s);

		return 0;
	}
	printf("No socket port given!\n");
	return -1;
}
