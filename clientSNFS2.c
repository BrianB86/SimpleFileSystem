
#define FUSE_USE_VERSION 26

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

#define FUSE_USE_VERSION 26
#define _XOPEN_SOURCE 500


//Let's assume that only two files are there stored in the server, each with size of 19

//static const char *file01_path = "/file01";

//static const int file01_len = 19;

//static const char *file02_path = "/file02";

//static const int file02_len = 19;

int errno;

//#define machine_path ((char*) fuse_get_context()->private_data)

char* machine_path;

static void fuse_path(const char* path, char full_path[PATH_MAX]) //gets the path given and concats the rest of the path of the machine.
{
	strcpy(full_path,machine_path);
	strncat(full_path, path, PATH_MAX); 
	printf("this is a path:%s\n",full_path);

}


static int snfs_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;
	
	/*
	memset(stbuf, 0, sizeof(struct stat));
	
	if (strcmp(path, "/testing") == 0) {
		stbuf->st_mode = S_IFDIR | 0777;
		stbuf->st_nlink = 2;
	} else if (strcmp(path, file01_path) == 0) {
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = file01_len;
	} else if (strcmp(path, file02_path) == 0) {
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = file02_len;
	}
	else
		res = -ENOENT;
	
	*/
	char allPath[PATH_MAX];
	fuse_path(path,allPath);
	res = lstat(allPath,stbuf);
	if(res!=0)
		 printf("snfs_getattr: %s\n", strerror(errno));
	return res;
}

static int snfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi)
{
	/*
	(void) offset;
	(void) fi;

	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, file01_path + 1, NULL, 0);
	filler(buf, file02_path + 1, NULL, 0);
	return 0;
	*/
	
	int res = 0; 
	DIR *dp;
	struct dirent *de;
	char allPath[PATH_MAX];
	fuse_path(path,allPath);
	
	dp = (DIR *) opendir(allPath);
	
	de = readdir(dp);
	
	if(de == 0){
		printf("There are no files in your directory not ever '.' and '..'\n");
	return -1;
	}
	
	
	do {
	if (filler(buf, de->d_name, NULL, 0) != 0) {
	    return -ENOMEM;
	}
    } while ((de = readdir(dp)) != NULL);
    
	return res;
}

static int snfs_open(const char *path, struct fuse_file_info *fi)
{
	int res = 0;
	int fd;
	char allPath[PATH_MAX];
	
	fuse_path(path,allPath);
	
	fd = open(allPath, fi->flags);
	if(fd < 0)
		printf("snfs_open: %s\n", strerror(errno));
	 
	 fi->fh = fd;
	 
	printf("open a file:");

	return res;
}

static int snfs_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
	int res = 0;
	int fd;
	char allPath[PATH_MAX];
	
	fuse_path(path,allPath);
	
	fd = creat(allPath,mode);
	if(fd < 0)
		printf("snfs_create: %s\n", strerror(errno));
	
	fi->fh = fd;

	printf("Created a file:");

	return res;
}

static int snfs_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
	/*
	size_t len;
	(void) fi;

	if( ( strcmp(path, file01_path) != 0 ) && ( strcmp(path, file02_path) != 0 ) )
		return -ENOENT;

    char recvBuf[1024] = {0};

	//sendRequestToServer(path,recvBuf,1024);

	len = strlen(recvBuf);
	if (offset < len) {
		if (offset + size > len)
			size = len - offset;
		memcpy(buf, recvBuf + offset, size);
	} else
		size = 0;

	return size;
	*/
	
	int res = 0;
	
	res = pread(fi->fh, buf, size, offset);
    if (res < 0)
	printf("snfs_read: %s\n", strerror(errno));
    
    return res;
}

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
	printf("snfs_truncate: %s\n", strerror(errno));
	
	return res;
}

static int snfs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	int res = 0;
	
	res = pwrite(fi->fh, buf, size, offset);
	
	if(res < 0)
		printf("snfs_write: %s\n", strerror(errno));
		
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
		printf("snfs_opendir: %s\n", strerror(errno));
		
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
	
	res = mkdir(allPath,mode);
	if(res < 0)
		printf("snfs_mkdir: %s\n", strerror(errno));
	
	return res;
}

static struct fuse_operations snfs_oper = {
	.getattr	= snfs_getattr,
	.readdir	= snfs_readdir,
	.create     = snfs_create,
	.open		= snfs_open,
	.read		= snfs_read,
	.release	= snfs_release,
	.truncate	= snfs_truncate,
	.write		= snfs_write,
	.opendir	= snfs_opendir,
	.releasedir	= snfs_releasedir,
	.mkdir		= snfs_mkdir,
};

int main(int argc, char *argv[])
{
	mkdir("hello",S_IRWXU);
	machine_path = realpath("hello",NULL);
	printf("path:%s\n",machine_path);
		
	return fuse_main(argc, argv, &snfs_oper, NULL);
}
