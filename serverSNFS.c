


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>



void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

static int start_server(char* port, char* path)
{
	int sock;
	int yes = 1;
	struct addrinfo hints;
	struct addrinfo *server_info;
	struct sigaction sigact;
	socklen_t sin_size;
	struct sockaddr_storage client_addr; // client's address info

	
	
	
	
	memset(&hints, 0, sizeof(hints)); //Makes sure no info is in struct.
	
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;
	hints.ai_flags = AI_PASSIVE; // using computers IP address
	
	setup = getaddrinfo(NULL,port,&hints,&server_info);
	
	if(setup != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(setup));
		return 1;
	}
	
	sock = socket(server_info->ai_family,server_info->ai_socketype, server_info->ai_protocol);
	
	if(sock == -1)
	{
		perror("server: socket");
		return -1;
	}
	
	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		perror("setsockopt");
		exit(1);
	} 
	
	if(bind(sock,server_info->ai_addr, server_info->ai_addrlen) == -1)
	{
		close(sock);
		perror("server:bind");
		exit(1);
	}
	
	freeaddrinfo(server_info);
	
	if(listen(sock,10) == -1)
	{
		perror("listen");
		exit(1);
	}
	
	sigact.sa_handler = sigchld_handler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = SA_RESTART;
	
	if (sigaction(SIGCHLD, &sigact, NULL) == -1) 
	{
		perror("sigaction");
		exit(1);
	}
	
	printf("Waiting for connections...\n");
	
	while(1) 
	{
		sin_size = sizeof(client_addr);
	}
	
	

	
}



int main(int argc, char *argv[])
{
	if (argc !=2)
	{
		fprintf(stderr, "usage: Port, directory.\n");
		return 1;
	}
	
	start_server(argv[0].argv[1]);
	
}
