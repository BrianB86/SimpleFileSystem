








#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>

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
 */





static int connect_to_server(const char* ipaddr, char* path, int port)
{

}

int main(int argc, char *argv[])
{
	if (argc !=3)
	{
		fprintf(stderr, "usage: IP Address, Port, directory.\n");
		return 1;
	}

	if(isdigit(argv[2]) == 0)
	{
		fprintf(stderr, "ports only have numbers in them.\n");
		return 1;
	}

	
	

}


