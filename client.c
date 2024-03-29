#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

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
		//FILE *fp = fopen("~/Desktop/saved_file.txt", "w+");

		struct sockaddr_in sockaddr;
		int s, rc;
		s = socket(AF_INET, SOCK_STREAM, 0); //create socket: Internet domain, stream socket type (TCP)
		check_it(s, __LINE__);

		//create internet socket address
		sockaddr.sin_family = AF_INET;
		struct in_addr addr;
		addr.s_addr = inet_addr("127.0.0.1");
		sockaddr.sin_addr = addr;
		sockaddr.sin_port = PORT;

		//connect to socket
		rc = connect(s, (const struct sockaddr *) &sockaddr, sizeof(sockaddr));
		check_it(rc, __LINE__);

		printf("Client connected to server\n");
		char buff[MAX];
		strcpy(buff, (const char *) argv[1]);
		rc = send(s, buff, sizeof(buff), 0);
		check_it(rc, __LINE__);
		strcpy(buff, "");
		rc = recv(s, buff, sizeof(buff), 0);
		check_it(rc, __LINE__);
		if(strcmp(buff,"File found") == 0)
		{
			//file found
			printf("File found\n");
			char ch;
			while(1)
			{
				rc = recv(s, &ch, 1, 0);
				check_it(rc, __LINE__);
				if(ch == EOF)
					{
						printf("\nEnd of file!\n");
						break;
					}
				printf("%c", ch);
				//fputc(ch, fp);
			}
		}

		close(s);
		//fclose(fp);
		return 0;
	}
	printf("No desired file given!\n");
	return -1;
}
