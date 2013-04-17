
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <signals.h>
#include <semaphore.h>




/*
struct sockaddr_storage {
    sa_family_t  ss_family;     // address family
    // all this is padding, implementation specific, ignore it:
    char      __ss_pad1[_SS_PAD1SIZE];
    int64_t   __ss_align;
    char      __ss_pad2[_SS_PAD2SIZE];

};
*/

/* Thread semaphores to make sure we don't go over 10 connections*/

sem_t thread_sem[10];  
int   next_thread;  
int   runnable;
int   thread_stopped[10];
 


void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

//may have to make a dedicated function for threads.

/*void my_thread(arguments)
{
	pthread_exit(NULL);
}
*/



static int start_server(char* port, char* path)
{
	int sock;
	int connected;
	int yes = 1;
	struct addrinfo hints;
	struct addrinfo *server_info;
	struct sigaction sigact;
	socklen_t sin_size;
	struct sockaddr_storage client_addr; // client's address info
	char recvBuf[1024];
	int recvSize = 1024;
	
	unsigned int args;                    
    pthread_attr_t attr;     
    pthread_t ids;  

	
	
	
	
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
	
	pthread_attr_init(&attr);
	
	while(1) 
	{
		sin_size = sizeof(client_addr);
		connected = accept(sock, (struct sockaddr *) &client_addr, &sin_size); 
		
		if(connected == -1)
		{
			perror("Connection: accept");
			exit(1);
		}
		
		{
			args = connected;
			//pthread_create (&ids, &attr, my_thread, &args); Create the thread on connection.
 			
			bytes_recieved = recv(connected,recv_data,1024,0);
			recvBuf[bytes_received] = '\0';
	
			printf("Server received '%s' \n",recvBuf);
			
			close(connected);
		}
		
	}
	close(sock);
	return 0;
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
