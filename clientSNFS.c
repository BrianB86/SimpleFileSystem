
#include <fuse.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define FUSE_USE_VERSION 26

/* Reference to the sockaddr struct
 *
 * struct sockaddr {
 *
 * unsigned short sa_family		address of family, AF_xxx <---ipv4 or ipv6
 * char sa_data[14]				14 bytes of protocol address
 *};
 *
 *
 *
 *
 * Reference to the sockaddr_in struct
 *
 * struct sockaddr_in {
 *
 * short int sin_family			Address family, AF_NET
 * unsigned short int sin_port  port Number
 * struct in_addr sin_addr		Internet address
 * unsigned char sin_zero[8]	pads so that it is the same size as struct sockaddr
 * };
 *
 * struct in_addr {
 *
 * uint32_t s_addr; 32-bit int (4 bytes)
 * };
 *
 * Reference to the addrinfo struct
 *
 * struct addrinfo{
 *
 * int	ai_flags	AI_PASSIVE 
 * int 	ai_family	AF_INET
 * int 	ai_socktype	SOCK_STREAM
 * int	ai_protocol	0 for any
 * size_t	ai_addrlen	size of ai_addr in bytes
 * struct sockaddr	*ai_addr	
 * char	*ai_canonname	full cononical hostname
 *
 * struct addrinfo	*ai_next	linked list next node
 * };
 *
 */



static int connect_to_server(const char* ipaddr, char* port, char* path)
{
	int setup, sock;
	struct addrinfo hints;
	struct addrinfo *server_info;
	char recvBuf[1024] = {0}; //buffer of received data. This is temp to get the idea of recv!!!!
	int recvSize = 1024; // size of recvBuf
	
	int good_port = atoi(port);
	char port_buff[50];
	sprintf(port_buff, "%d",good_port);
	

	memset(&hints, 0, sizeof(hints)); //Makes sure no info is in struct.

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;
	
	setup = getaddrinfo(ipaddr,port_buff,&hints,&server_info); //server_info points to a linked list of struct addrinfos each contains sockaddr.
	
	printf("Port: %s\n",port_buff);
	printf("Ip: %s\n",ipaddr);
	if(setup != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(setup));
		return 1;
	}

	sock = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(sock == -1)
	{
		perror("Socket Error");
		return -1;
	}


	if(connect(sock, server_info->ai_addr, server_info->ai_addrlen) == -1)
	{
		close(sock);
		perror("Connection Error:");
		return -1;
	}
	
	if(server_info == NULL)
	{
		fprintf(stderr, "Failed Connection.");
		return 2;
	}
	
	printf("Client Connecting\n");
	
	/* Testing Code */
	char* message = "Hello Server";
	int len, bytes_sent;
	len = strlen(message);
	
	
	
	bytes_sent = send(sock,message,len,0);
	
	if(bytes_sent == -1)
	{
		perror("Error in sending bytes to Server");
		return -1;
		
	}
	
	freeaddrinfo(server_info);
	
	int bytes_received = recv(sock,recvBuf,recvSize,0);
	
	if(bytes_received == -1)
	{
		perror("Error in Receiving Bytes");
		return -1;
		
	}else if(bytes_received == 0)
		{
			perror("Connection to server is closed");
			return -1;
		}

	recvBuf[bytes_received] = '\0';
	
	printf("client received '%s' \n",recvBuf);
	
	close(sock);
	
	return 0;

}

int fuse_getattr(const char *path, struct stat *statbuf) //gets the arttribues of the file
{
	char* ipaddr = argv[1];
	char* port = argv[2];
	char* path = argv[3];
	
	int res = 0;
	
	
}

int fuse_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
	int res = 0;
	
}

int fuse_mkdir(const char *path, mode_t mode)
{
	int res =0;
	
}

int fuse_truncate(const char *path, off_t size)
{
	int res =0;

}

int fuse_open(const char *path, struct fuse_file_info *fi)
{
	int res = 0;
	
}


static struct fuse_operations fuse_oper = 
{
	.getattr = fuse_getattr,
	.mkdir = fuse_mkdir,
	.truncate = fuse_truncate,
	
};




int main(int argc, char *argv[])
{
	if (argc !=4)
	{
		fprintf(stderr, "usage: IP Address, Port, directory.\n");
		return 1;
	}
	
	return fuse_main(argc, argv, &hello_oper, NULL);

//connect_to_server(argv[1],argv[2],argv[3]);
//return 0;




	

}


