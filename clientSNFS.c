
#include <fuse.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <netdb.h>
#include <string.h>
#include <dirent.h>
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

char** argv;
mode_t mode = S_IRWXU|S_IRWXG|S_IRWXO;

struct command_struct{
		char command;	//type of command
		char* path;	
		int len_path;				
	};
		


static int connect_to_server(const char* ipaddr, char* port, char* path, char* recvBuf, int recvSize)
{
	printf("argv[1]: %s\n", argv[1]);
	printf("argv[2]: %s\n", argv[2]);
	printf("argv[3]: %s\n", argv[3]);
	
	int setup, sock;
	struct addrinfo hints;
	struct addrinfo *server_info;
	//char recvBuf[1024] = {0}; //buffer of received data. This is temp to get the idea of recv!!!!
	//int recvSize = 1024; // size of recvBuf
	
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
	
	
	
	//bytes_sent = send(sock,message,len,0);
	
	bytes_sent = send(sock,recvBuf,len,0);
	
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
	
	//printf("client received '%s' \n",recvBuf);
	
	close(sock);
	return 0;

}

int fuse_getattr(const char *path, struct stat *statbuf)
{
	
	// check if stat struct is accessable from fuse_file_info if it is look at open dir, close dir etc...
	
	//char *ipaddr = argv[1];
	//char *port = argv[2];

	//memset(stbuf, 0, sizeof(struct stat));
	
	//char recvBuf[1024] = {0};
 
    //connect_to_server(ipaddr,port,path,recvBuf,1024);
    
   
	/*
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else if (strcmp(path, file01_path) == 0) {
		stbuf->st_mode = S_IFREG | 0777;
		stbuf->st_nlink = 1;
		stbuf->st_size = file01_len;
	} else if (strcmp(path, file02_path) == 0) {
		stbuf->st_mode = S_IFREG | 0777;
		stbuf->st_nlink = 1;
		stbuf->st_size = file02_len;
	}
	else
		res = -ENOENT;

	return 0;
	*/

	int retstat = lstat(path, statbuf);
    if (retstat != 0)
		perror("error in getting attributes.\n");
    
    return retstat;	
	
}

int fuse_mkdir(const char *path, mode_t mode)
{
	int retstat = 0;
    
    retstat = mkdir(path, mode);
    if (retstat < 0)
	perror("Error in making directory.\n");
    
    return retstat;

}

int fuse_truncate(const char *path, off_t size)
{
	/*
	char *ipaddr = argv[1];
	char *port = argv[2];
	* 
	if(size > MAX_FILE_SIZE || size < 0)
	{
		return -EINVAL;
	}
	*/
	/*
	if(strcmp(path, file01_path) != 0)
	{
		char recvBuf[1024] = {0};
 
		connect_to_server(ipaddr,port,path,recvBuf,1024);
		
		if(recvBuf == -1)
		{
			perror("Error in truncating the file.\n");
			return -1;
		}
	}
	
	if(strcmp(path, file02_path) != 0 )
	{
		char recvBuf[1024] = {0};
 
		connect_to_server(ipaddr,port,path,recvBuf,1024);
		
		if(recvBuf == -1)
		{
			perror("Error in truncating the file.\n");
			return -1;
		}
	
	}
	
	return 0; 
	
	//check errors on server.
	*/
	
	int retstat = 0;
	
    retstat = truncate(path, size);
    if (retstat < 0)
	perror("Error in truncating file.\n");
    
    return retstat;
}

int fuse_open(const char *path, struct fuse_file_info *fi, mode_t mode)
{
	/*
	char *ipaddr = argv[1];
	char *port = argv[2];
	
	if(fi->flags & 3 == O_CREAT && strcmp(path, file01_path == 0))
	{
		return -EEXIST;
	}
	
	if (fi->flags & 3 != O_CREAT && strcmp(path, file01_path) != 0 && strcmp(path, file02_path) != 0)
	{
		return -ENOENT;
	}	
	
	char recvBuf[1024] = {0};
	
	command_struct->command = "O";
	command_struct->path = path;
	command_struct->len_path = strlen(path);
	
	recvBuf[0] = command;
	
    connect_to_server(ipaddr,port,path,recvBuf,1024);
    
    if(recvBuf->st_ino != NULL)
    {
		return recvBuf;
	}
	else{
		perror("Error in opening file.\n");
		return -1;
	}
	*/
	
    int fd;
    
    fd = open(path, fi->flags);
    if (fd < 0)
	perror("Error in opeing the file.\n");
    
    fi->fh = fd;
    
    return 0;
}

int fuse_flush(const char *path, struct fuse_file_info *fi)
{
	/*
	char *ipaddr = argv[1];
	char *port = argv[2];
	
	if (strcmp(path, file01_path) != 0 || strcmp(path, file02_path) != 0 )
	{
		char recvBuf[1024] = {0};
 
		connect_to_server(ipaddr,port,path,recvBuf,1024);
		
		if(recvBuf == -1)
		{
			return -EBADF;
		}
		
	}
	return 0;
	*/
	
	return 0;
	
}

int fuse_release(const char *path, struct fuse_file_info *fi)
{
	/*
	char *ipaddr = argv[1];
	char *port = argv[2];
	
	if (strcmp(path, file01_path) != 0 || strcmp(path, file02_path) != 0 )
	{
			char recvBuf[1024] = {0};
 
			connect_to_server(ipaddr,port,path,recvBuf,1024);
			
			if(recvBuf == -1)
			{
			return -EBADF;
			}
	
	}
	
	return 0;
	*/
	
	int retstat = 0;
	
    retstat = close(fi->fh);
    
    return retstat;
}

static int fuse_create(const char* path, mode_t mode, struct fuse_file_info *fi)
{
	/*
	int fd;
	fi->flags = O_CREAT|O_WRONLY|O_TRUNC; 
	
	int fd = fuse_open(path,fi,mode);
	
	if(fd == -1)
	{
		perror("Error in file creation\n");
		return -1;
	}
	
	return 0;
	*/
	
    int fd;
    fd = creat(path, mode);
    if (fd < 0)
	perror("Error in file creation.\n");
    fi->fh = fd;
    return 0;
}

int fuse_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	/*
	char *ipaddr = argv[1];
	char *port = argv[2];
	
	size_t len;
 
    // if( ( strcmp(path, file01_path) != 0 ) && ( strcmp(path, file02_path) != 0 ) )
    //     return -ENOENT;
 
     char recvBuf[1024] = {0};
 
     connect_to_server(ipaddr,port,path,recvBuf,1024);
 
     len = strlen(recvBuf);
     if (offset < len) {
         if (offset + size > len)
             size = len - offset;
         memcpy(buf, recvBuf + offset, size);
     } else
         size = 0;
 
      return size;
	*/
	
	int retstat = 0;
    
    retstat = pread(fi->fh, buf, size, offset);
    if (retstat < 0)
	perror("Reading error.\n");
    
    return retstat;
	
}

int fuse_write(const char *path,const char *buf,size_t size,off_t offset,struct fuse_file_info *fi)
{
	/*
	char *ipaddr = argv[1];
	char *port = argv[2];
	
	size_t len;
 
    // if( ( strcmp(path, file01_path) != 0 ) && ( strcmp(path, file02_path) != 0 ) )
    //     return -ENOENT;
	
     char recvBuf[1024] = {0};
     
	// basically the same as read except when you write the buffer is filled with what has to be written by the server.
	
     connect_to_server(ipaddr,port,path,recvBuf,1024);
 
     len = strlen(recvBuf);
     if (offset < len) {
         if (offset + size > len)
             size = len - offset;
         memcpy(buf, recvBuf + offset, size);
     } else
         size = 0;
 
      return size;
       */
      
     int retstat = 0;

    retstat = pwrite(fi->fh, buf, size, offset);
    if (retstat < 0)
	perror("Error in writing to file");
    
    return retstat;
}

int fuse_opendir(const char *path, struct fuse_file_info *fi)
{
	/*
	DIR *dp;
	
	if(fi->fh->st_mode != S_IFDIR)  //not sure if this is allowed but we must tst if the file is a directory
	{
		return -ENOTDIR
	}else{
		
		char recvBuf[1024] = {0};
		
		connect_to_server(ipaddr,port,path,recvBuf,1024);
		
		// this returns a pointer to the directory
		
		if (dp == NULL)
		{
			perror("Error in opening the directory\n");
		}
		
	}
	
	return dp;
	*/
	
	DIR *dp;
    int retstat = 0;
    
    dp = opendir(path);
    if (dp == NULL)
	perror("Error in opening directory");
    
    fi->fh = (intptr_t) dp;
    
    return retstat;
	
}

int fuse_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    int retstat = 0;
    DIR *dp;
    struct dirent *de;

    // once again, no need for fullpath -- but note that I need to cast fi->fh
    dp = (DIR *) (uintptr_t) fi->fh;

    de = readdir(dp);

    // This will copy the entire directory into the buffer.  The loop exits
    // when either the system readdir() returns NULL, or filler()
    // returns something non-zero.  The first case just means I've
    // read the whole directory; the second means the buffer is full.
    do {
	if (filler(buf, de->d_name, NULL, 0) != 0) 
	    return -ENOMEM;
	
    } while ((de = readdir(dp)) != NULL);
    
    return retstat;
}

int fuse_releasedir(const char *path, struct fuse_file_info *fi)
{
    int retstat = 0;
    
    closedir((DIR *) (uintptr_t) fi->fh);
    
    return retstat;
}

struct fuse_operations fuse_oper = 
{
	.getattr = fuse_getattr,
	.mkdir = fuse_mkdir,
	.truncate = fuse_truncate,
	.read = fuse_read,
	.open = fuse_open,
	.flush = fuse_flush,
	.release = fuse_release,
	.opendir = fuse_opendir,
	.write = fuse_write,
	.readdir = fuse_readdir,
	.releasedir = fuse_releasedir,
	.create = fuse_create
	
};




int main(int argc, char **av)
{
//	if (argc !=4)
//	{
//		fprintf(stderr, "usage: IP Address, Port, directory.\n");
//		return 1;
//	}

	argv = av;
	
	return fuse_main(argc,argv,&fuse_oper);

return 0;




	

}


