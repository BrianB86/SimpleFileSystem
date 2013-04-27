
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
#include <signal.h>
#include <semaphore.h>
#include <netdb.h>
#include <sys/wait.h>




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

#define RECV_SIZE 1024


sem_t thread_sem[10];  
int   next_thread;  
int   runnable;
int   threads_stopped[10];
unsigned int client_connected;

 

/* Testing code */

void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

void *client(void *params)			// thread implementation!! check what each call sends.
{
	unsigned int client_connected_cp; // cp == copy dunno if it is needed
	
	char recvBuf[RECV_SIZE];
	
	client_connected_cp = *(unsigned int *)params;
	
	int bytes_received = recv(client_connected,recvBuf,RECV_SIZE,0);
	
	if(bytes_received < 0)
	{
		perror("Client could not be read properly recv error\n");
	}
	
	recvBuf[bytes_received] = '\0';
	
	printf("Server received '%s' \n",recvBuf);
	
	/* This code area is for finding out what command and params got sent to server */
	/*BEGIN*/
	
	static const char* file01_content = "You can see file01\n"; //placeholder take from example
	
	if(strcmp(recvBuf, "/file01") == 0)
	{
		send(client_connected,file01_content,strlen(file01_content),0);
	}
	
	/*END*/
	
	close(client_connected);
	pthread_exit(NULL);
}



static int start_server(char* port, char* path)
{
	int server_sock, setup;
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
	
	int good_port = atoi(port);
	char port_buff[50];
	sprintf(port_buff, "%d",good_port);
	
	
	
	memset(&hints, 0, sizeof(hints)); //Makes sure no info is in struct.
	
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // using computers IP address
	
	setup = getaddrinfo(NULL,port_buff,&hints,&server_info);
	printf("Port: %s\n",port_buff);
	
	if(setup != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(setup));
		return 1;
	}
	
	server_sock = socket(server_info->ai_family,server_info->ai_socktype, server_info->ai_protocol);
	
	if(server_sock == -1)
	{
		perror("server: socket\n");
		exit(1);
	}
	
	if(setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		perror("setsockopt\n");
		exit(1);
	} 
	
	if(bind(server_sock,server_info->ai_addr, server_info->ai_addrlen) == -1)
	{
		close(server_sock);
		perror("server:bind\n");
		exit(1);
	}
	
	freeaddrinfo(server_info);
	
	if(listen(server_sock,10) == -1)
	{
		perror("listening error:\n");
		exit(1);
	}
	
	/*Testing Code */
	sigact.sa_handler = sigchld_handler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = SA_RESTART;	
	if (sigaction(SIGCHLD, &sigact, NULL) == -1) 
	{
		perror("sigaction\n");
		exit(1);
	}
	
	printf("Waiting for connections...\n");
	
	pthread_attr_init(&attr);
	fflush(stdout);

	
	while(1) 
	{
		printf("Server up.\n");
		sin_size = sizeof(client_addr);
		client_connected = accept(server_sock, (struct sockaddr *)&client_addr, &sin_size);
		
		printf("Creating a new client connection.\n");
		 
		
		if(client_connected == -1)
		{
			perror("Connection: error in creating socket\n");
			exit(1);
		}
		
		{
			args = client_connected;
			pthread_create(&ids, &attr, client, &args); //Create the thread on connection.
 			
			int bytes_received = recv(client_connected,recvBuf,recvSize,0);
			recvBuf[bytes_received] = '\0';
	
			printf("Server received '%s' \n",recvBuf);
			
			/* Testing code*/
			
			if (!fork()) 
			{ // this is the child process
				close(server_sock); // child doesn't need the listener
				if (send(client_connected, "Hello, world!", 13, 0) == -1)
					perror("send\n");
				close(client_connected);
				exit(0);
			}
			
			close(client_connected);
		}
		
	}
	close(server_sock);
	return 0;
}



int main(int argc, char *argv[])
{
	if (argc !=3)
	{
		fprintf(stderr, "usage: Port, directory.\n");
		return 1;
	}
	
	start_server(argv[1],argv[2]);
	
	return 0;
	
}
