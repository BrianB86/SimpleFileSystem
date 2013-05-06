#define FUSE_USE_VERSION 28
#define _XOPEN_SOURCE 500
#define _OPEN_SYS_ITOA_EXT


#include <fuse.h>
#include <fuse_common.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <fcntl.h>
#include <libgen.h>
#include <unistd.h>
#include <limits.h>
#include <netdb.h>
#include <string.h>
#include <dirent.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <attr/xattr.h>
#include <inttypes.h>

char * argv_1; 				// port to connect
char * argv_2;   			// the server's ip address

int sock; 					// socket descriptor
int bytes_received; 
char buffer[1024];

static int snfs_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;
	
	// sending the type of command to server
	send(sock,"1",strlen("1"), 0);
	sleep(1);
	// send the path
	send(sock,path,strlen(path), 0);
		
	//then, receive the stbuf 
	bytes_received=recv(sock,buffer,1024,0);
	memcpy(stbuf,buffer,sizeof(struct stat));
	
	//receiv the res
	bytes_received=recv(sock,buffer,1024,0);
	//buffer[bytes_received] = '\0';
	//res = atoi(buffer);
	memcpy(&res, buffer, sizeof(int));
	
	return res;
}

static int snfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
	int res = 0;
	
	// send the type of command to server
	send(sock,"3",strlen("3"), 0);
	sleep(1);
	
	// pass fi->fh to the server 
	memcpy(buffer, &(fi->fh), sizeof(uint64_t)) ;
	send(sock,buffer,sizeof(uint64_t), 0);
	
	
	/*DIR *dp;
	struct dirent *de;
	
	dp = (DIR *) (uintptr_t) fi->fh;
	de = readdir(dp);
	
	
	
	if(de == 0){
		printf("snfs_readdir: %s\n", strerror(errno));
		res = -errno;
		return res;
	}

	// 
	do {
		if (filler(buf, de->d_name, NULL, 0) != 0) 
			return -ENOMEM;
		
	} while ((de = readdir(dp)) != NULL);
	*/	

	// receive the response
	bytes_received=recv(sock,buffer,1024,0);
	buffer[bytes_received] = '\0';
	if (strcmp(buffer,"res") == 0)
	{
		bytes_received=recv(sock,buffer,1024,0);
		buffer[bytes_received] = '\0';
		res = atoi(buffer);
		return res;
	}
	else
	{		
		do
		{
			if (filler(buf, buffer, NULL, 0) != 0)
				return -ENOMEM;
			//buffer[bytes_received] = '\0';
			//if(filler(buf,buffer, NULL, 0) != 0)
				
		}while( (bytes_received=recv(sock,buffer,1024,0)) != 0) ;
		
	}
	
	return res;
}

static int snfs_open(const char *path, struct fuse_file_info *fi)
{
	int res = 0;
	int fd;
	
	// send the type of command
	send(sock,"5",strlen("5"), 0);
	sleep(1);
	// sned the path
	send(sock,path,strlen(path), 0);
	sleep(1);
	// send the flags
	memcpy(buffer,&(fi->flags), sizeof(int));
	send(sock,buffer,strlen(buffer), 0);

	
	// receive the fd
	bytes_received=recv(sock,buffer,1024,0);
	memcpy(&fd,buffer,sizeof(int));
	fi->fh = fd;
	
	// receive the response
	bytes_received=recv(sock,buffer,1024,0);
	//buffer[bytes_received] = '\0';
	//res = atoi(buffer);	
	memcpy(&res,buffer,sizeof(int));
	return res;
}

/*static int snfs_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
	int res = 0;
	int fd;
	char allPath[PATH_MAX];

	fuse_path(path,allPath);

	fd = creat(allPath, mode);
    
    if (fd < 0)
	{
			printf("snfs_create: %s\n", strerror(errno));
			res = -errno;
	}
	fi->fh = fd;

	return res;
}
*/



static int snfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	int res = 0;

	// send fh, size, offset and receive buf back
	//res = pread(fi->fh, buf, size, offset);
	//if (res < 0)
	//{
	//	printf("snfs_read: %s\n", strerror(errno));
	//	res = -errno;
	//	}
	// send fh

	// send the type of command
	send(sock,"6",strlen("6"), 0);
	sleep(1);
	// send the flags
	memcpy(buffer,&fi->flags, sizeof(int));
	send(sock,buffer,sizeof(int), 0);
	sleep(1);
	// send size
	memcpy(buffer, &size, sizeof(size_t));
	send(sock,buffer,sizeof(size_t), 0);
	sleep(1);
	// send offset
	memcpy(buffer, &offset, sizeof(off_t));
	send(sock,buffer,sizeof(off_t), 0);
	sleep(1);

	// receive the buf
	bytes_received=recv(sock,buf,1024,0);
	buf[bytes_received] = '\0'; // dont know whether we need it or not
	
	// receive the response
	bytes_received=recv(sock,buffer,1024,0);
	//buffer[bytes_received] = '\0';
	//res = atoi(buffer);
	memcpy(&res, buffer, sizeof(int));
	return res;	
}

/*
static int snfs_release(const char* path, struct fuse_file_info *fi)
{
	int res = 0;

	res = close(fi->fh);

	return res;
}

static int snfs_truncate(const char* path, off_t newsize)
{
	int res = 0;
	char allPath[PATH_MAX];

	fuse_path(path,allPath);

	res = truncate(allPath,newsize);
	if(res < 0)
	{	
		printf("snfs_truncate: %s\n", strerror(errno));
		res = -errno;
	}

	return res;
}

static int snfs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	int res = 0;
	
	res = pwrite(fi->fh, buf, size, offset);
    if (res < 0)
	{
		printf("snfs_write: %s\n", strerror(errno));
		res = -errno;
	}
    return res;
}

static int snfs_opendir(const char *path, struct fuse_file_info *fi)
{
	DIR *dp;
	int res = 0;

	char allPath[PATH_MAX];

	fuse_path(path,allPath);

	dp = opendir(allPath);
	if(dp == NULL)
	{
		printf("snfs_opendir: %s\n", strerror(errno));
		res = -errno;
	}
	fi->fh = (intptr_t) dp;


	return res;
}

static int snfs_releasedir(const char *path, struct fuse_file_info *fi)
{
	int res = 0;

	closedir((DIR *) (uintptr_t) fi->fh);

	return res;
}

static int snfs_mkdir(const char *path, mode_t mode)
{
	int res = 0;

	char allPath[PATH_MAX];

	fuse_path(path,allPath);

	//printf("%s", allPath);
	res = mkdir(allPath,mode);
	if(res < 0)
	{
		printf("snfs_mkdir: %s\n", strerror(errno));
		res = -errno;
	}
	return res;
}

static int snfs_fgetattr(const char* path, struct stat *statbuf, struct fuse_file_info *fi)
{
	int res = 0;

	res = fstat(fi->fh,statbuf);
	if(res<0)
	{
		printf("snfs_fgetattr: %s\n", strerror(errno));
		res = -errno;
	}
	return res;
}
static int snfs_access (const char * path, int mask)
{

	int res = 0;
    char allPath[PATH_MAX];
   
   fuse_path(path,allPath);
    
    res = access(allPath, mask);
    
    if (res < 0)
	{
		printf("snfs_access: %s\n", strerror(errno));
		res = -errno;
    }
    return res;
	
	
}

static int snfs_mknod(const char *path, mode_t mode, dev_t dev)
{
    int res = 0;
    char allPath[PATH_MAX];
    
    fuse_path(path,allPath);
    
	res = mknod ( allPath, mode, dev);
	
	if ( res < 0)
	{
		printf("snfs_mknod: %s\n", strerror(errno));
		res = -errno;
	}
    return res;
}

static int snfs_chmod(const char *path, mode_t mode)
{
    int res = 0;
    char allPath[PATH_MAX];
    
    fuse_path(path, allPath);
    
    res = chmod(allPath, mode);
    if (res < 0)
	{
		printf("snfs_chmod: %s\n", strerror(errno));
		res = -errno;
    }
    return res;
}

static int snfs_chown(const char *path, uid_t uid, gid_t gid)
  
{
    int res = 0;
    char allPath[PATH_MAX];
   
    fuse_path(path, allPath);
    
    res = chown(allPath, uid, gid);
    if (res < 0)
	{
		printf("snfs_chown: %s\n", strerror(errno));
		res = -errno;
    }
    return res;
}

static int snfs_readlink(const char *path, char *link, size_t size)
{
    int res = 0;
    char allPath[PATH_MAX];
    
    fuse_path(path, allPath);
    
    res = readlink(allPath, link, size - 1);
    if (res < 0)
    {
		printf("snfs_readlink: %s\n", strerror(errno));
		res = -errno;
    }
    else  
    {
		link[res] = '\0';
		res = 0;
    }
    
    return res;
}

static int snfs_flush(const char *path, struct fuse_file_info *fi)
{
    int res = 0;
    
    // chilling here
	
    return res;
}

static int snfs_getxattr(const char *path, const char *name, char *value, size_t size)
{
    int res = 0;
    char allPath[PATH_MAX];
   
    fuse_path(path, allPath);
    
    res = lgetxattr(allPath, name, value, size);
    if (res < 0)
	{
		printf("snfs_getxattr: %s\n", strerror(errno));
		res = -errno;
    }
    
    return res;
}

static int snfs_unlink(const char *path)
{
    int res = 0;
    char allPath[PATH_MAX];
    
    fuse_path(path, allPath);
    
    res = unlink(allPath);
    if (res < 0)
	{
		printf("snfs_unlink: %s\n", strerror(errno));
		res = -errno;
    }
    return res;
}

static int snfs_fsync(const char *path, int datasync, struct fuse_file_info *fi)
{
    int res = 0;
   
    if (datasync)
		res = fdatasync(fi->fh);
    else
		res = fsync(fi->fh);
    
    if (res < 0)
	{
		printf("snfs_fsync: %s\n", strerror(errno));
		res = -errno;
	}
    
    return res;
}
*/
static struct fuse_operations snfs_oper = {
	.getattr		= snfs_getattr,	// works
	//.fgetattr		= snfs_fgetattr,	//  need
	.readdir		= snfs_readdir,	//works
	//.create 		= snfs_create,	//works
	.open			= snfs_open,	//works
	.read			= snfs_read,	//works
	/*.release		= snfs_release,	//works
	.truncate		= snfs_truncate,	//works
	.write			= snfs_write,	//works
	.opendir		= snfs_opendir,	//works
	.releasedir		= snfs_releasedir,	//works
	.mkdir			= snfs_mkdir,	//works
	.access 		= snfs_access,  // need
	.mknod 			= snfs_mknod,	// ????
	.chmod  		= snfs_chmod,	// extra
	.chown			= snfs_chown,	// extra
	.readlink 		= snfs_readlink,	// need it?
	.flush 			= snfs_flush,	// need
	.getxattr 		= snfs_getxattr,  // need
	.unlink 		= snfs_unlink,   // need
	.fsync 			= snfs_fsync,	// need
	*/
};

int main(int argc, char *argv[])
{
	if (argc != 4)
	{
		printf( "Usage: clientSNFS <port> <server_addr> <directory_path>\n");
		return 0;
	}
	
	// Do we have to get rid of the port and the server addr, then move the path up
	argv_1 =  argv[1]; // save the port
	argv_2 = argv[2]; // save the server addr
	
	argv[1] =  argv[3];  // move the path up, so right now we have ./clientSNFS mountdir
	argv[2] = NULL;
	argv[3] = NULL;
	argc = argc -2;
	
	// set up socket
	//  set up all variables
	int port;
 	//int sock;
 	//int bytes_recieved;  
    struct hostent *host;
    struct sockaddr_in server_addr; 
    int fuse_res ; 

	// assign host and port and directory
	port = atoi(argv_1);
    host = gethostbyname(argv_2);

	// create a socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket error");
        return -1;
    }

	// socket struct
    server_addr.sin_family = AF_INET;     
    server_addr.sin_port = htons(port);   
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    bzero(&(server_addr.sin_zero),8); 

	// make connection
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) 
    {
        perror("Connect error");
        return -1;
    }
        
	// Now, pass all the business to fuse
	fuse_res = fuse_main(argc, argv, &snfs_oper, NULL);

	// close the socket after 
	close(sock);
	
	return fuse_res;
}
