








#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>


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





static int connect_to_server(const char* ipaddr, char* path, char* port)
{
	int setup, sock;
	struct addrinfo hints;
	struct addrinfo *server_info;


	memset(&hints, 0, sizeof(hints)); //Makes sure no info is in struct.

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;
	
	setup = getaddrinfo(ipaddr,port,&hints,&server_info); //server_info points to a linked list of struct addrinfos each contains sockaddr.

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


	connect(sock, server_info->ai_addr, server_info->ai_addrlen);

	// dont forget to check for errors!!!





}

int main(int argc, char *argv[])
{
	if (argc !=3)
	{
		fprintf(stderr, "usage: IP Address, Port, directory.\n");
		return 1;
	}

connect_to_server(argv[0],argv[1],argv[2]);

//check fuse wether we can make our own connect or connect through fuse.



	

}


