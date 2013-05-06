


#define _OPEN_SYS_ITOA_EXT

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <limits.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <inttypes.h>

int errno;

int debug = 1;	 //if 1 it'll show output msgs surrounded by if(debug), for troubleshooting

char* machine_path; // store the absolute path

void fuse_path(const char* path, char full_path[PATH_MAX]) //gets the path given and concats the rest of the path of the machine.
{
	strcpy(full_path,machine_path);
	strncat(full_path, path, PATH_MAX);
	printf("this is a path:%s\n",full_path);

}

void* handleClient(void* sockfd){
        
       char recvBuf[1024];
       int bytes_received;
       char allPath[PATH_MAX]; // full path
       int res =0;      

       // typecasting to get the sock arg
		int* temp =(int*) sockfd;
        int client_socket = *temp;
        
        if(debug)
                printf("Handling socket: %d\n", client_socket);
        
        while(1){
			// received mess from client      
			bytes_received = recv(client_socket,recvBuf,1024,0);
			recvBuf[bytes_received] = '\0';
			if ( bytes_received )
			{
				// a big switch statement here:
				/*
				 *
				 *  1.getattr		= snfs_getattr,	// works
				    2.fgetattr		= snfs_fgetattr,	//  need
				    3.readdir		= snfs_readdir,	//works
					4.create 		= snfs_create,	//works
					5.open			= snfs_open,	//works
					6.read			= snfs_read,	//works
					7.release		= snfs_release,	//works
					8.truncate		= snfs_truncate,	//works
					9.write			= snfs_write,	//works
					10.opendir		= snfs_opendir,	//works
					11.releasedir		= snfs_releasedir,	//works
					12.mkdir			= snfs_mkdir,	//works
					13.access 		= snfs_access,  // need
					14.mknod 			= snfs_mknod,	// ????
					15.chmod  		= snfs_chmod,	// extra
					16.chown			= snfs_chown,	// extra
					17.readlink 		= snfs_readlink,	// need it?
					18.flush 			= snfs_flush,	// need
					19.getxattr 		= snfs_getxattr,  // need
					20.unlink 		= snfs_unlink,   // need
					21.fsync 			= snfs_fsync,	// need 
					
					will implement rm uf have enough time
				 */
				
				 if (strcmp(recvBuf,"1") == 0)    // getattr func
				 {   
					  
					struct stat stbuf ;  
					
					bytes_received = recv(client_socket,recvBuf,1024,0);
					recvBuf[bytes_received] = '\0';  // this the path from client
					fuse_path(recvBuf,allPath); // fullpath in server side
									
					// call the real getattr function
					res = lstat(allPath, &stbuf);// return the statbuf to client
					
					// send the stbuf back
					memcpy(recvBuf, &stbuf, sizeof(struct stat));
					send(client_socket, recvBuf, sizeof(struct stat), 0);				
					sleep(1);
					
					// send the res
					//sprintf( recvBuf, "%d", res);
					memcpy(recvBuf, &res, sizeof(int));
					send(client_socket, recvBuf, sizeof(int), 0 );
										
				}
				else if (strcmp(recvBuf,"2") == 0)
				{
					
				}
				else if (strcmp(recvBuf,"3") == 0)   // readdir function
				{
					int res =0;
					uint64_t fh;
					DIR *dp;
					struct dirent *de;
				
					
					// receiv fi->fh from client;
					bytes_received = recv(client_socket,recvBuf,1024,0);
					memcpy(&fh, recvBuf, sizeof(uint64_t));
					
					dp = (DIR *) (uintptr_t)fh;
					de = readdir(dp);
					
					// u have to send de back field by filed in of de to client!!! 
					if(de == 0){
						printf("snfs_readdir: %s\n", strerror(errno));
						res = -errno;
						send(client_socket,"res", strlen("res"), 0);
						sleep(1);
						sprintf( recvBuf, "%d", res); 
						send( client_socket, recvBuf, strlen(recvBuf), 0 );
						break; // cause warning: control reaches end of non-void function
					}

					// this function is in client side !!!!!
					// in server side, we just need to read all d_name field in dp
					/*do {
						if (filler(buf, de->d_name, NULL, 0) != 0)
						{	
							res = -ENOMEM;
							//sprintf( recvBuf, "%d", res); // send the respond
							//send( client_socket, recvBuf, strlen(recvBuf), 0 );
							//return res;
						}
						
					} while ((de = readdir(dp)) != NULL);
					*/
					
					// send all d_names back to client 
					do
					{
						send(client_socket, de->d_name, strlen(de->d_name), 0 );
						sleep(1);	
					}while( (de =readdir(dp)) != NULL );

										
				}
				else if (strcmp(recvBuf,"4") == 0)
				{
					
				}
				else if (strcmp(recvBuf,"5") == 0) // open struct
				{
					
					int res = 0;
					int fd;
					int flag;
					
					// receive the path
					bytes_received = recv(client_socket,recvBuf,1024,0);
					recvBuf[bytes_received] = '\0'; 
					fuse_path(recvBuf,allPath); 

					// receive the flag
					bytes_received = recv(client_socket,recvBuf,1024,0);
					//recvBuf[bytes_received] = '\0'; 
					memcpy( &flag, recvBuf, sizeof(int) );
					
					// call the  open function
					fd = open(allPath, flag);
					
					if(fd < 0)
					{
						printf("snfs_open: %s\n", strerror(errno));
						res = -errno;
					}
					
					// send back the fd
					memcpy(recvBuf,&fd, sizeof(int));
					send(client_socket,recvBuf,sizeof(int), 0);
					sleep(1);
					
					// send back the res					
					memcpy(recvBuf,&res, sizeof(int));
					send(client_socket,recvBuf,sizeof(int), 0);
					
				}
				else if (strcmp(recvBuf,"6") == 0) // read struct
				{
					
					int res = 0;
					int fh;
					size_t size;
					off_t offset;
					char * buf;
					
					// receive flags
					bytes_received = recv(client_socket,recvBuf,1024,0);
					memcpy( &fh, recvBuf, sizeof(int));
					
					// receive size 
					bytes_received = recv(client_socket,recvBuf,1024,0);
					memcpy( &size, recvBuf, sizeof(size_t));
					
					// receive offset
					bytes_received = recv(client_socket,recvBuf,1024,0);
					memcpy( &offset, recvBuf, sizeof(off_t));
									
					res = pread(fh, buf, size, offset);
					if (res < 0)
					{
						printf("snfs_read: %s\n", strerror(errno));
						res = -errno;
					}
					
					//send the buf back
					send(client_socket,buf,strlen(buf), 0);
					sleep(1);
					
					// send back the res					
					memcpy(recvBuf,&res, sizeof(int));
					send(client_socket,recvBuf,sizeof(int), 0);
			
				}
				else if (strcmp(recvBuf,"7") == 0)
				{
					
				}
				else if (strcmp(recvBuf,"8") == 0)
				{
					
				}
				else if (strcmp(recvBuf,"9") == 0)
				{
					
				}
				else if (strcmp(recvBuf,"10") == 0)
				{
					
				}
				else if (strcmp(recvBuf,"11") == 0)
				{
				}
				else if (strcmp(recvBuf,"12") == 0)
				{
				}
				else if (strcmp(recvBuf,"13") == 0)
				{
				}
				else if (strcmp(recvBuf,"14") == 0)
				{
					
				}
				else if (strcmp(recvBuf,"15") == 0)
				{
					
				}
				else if (strcmp(recvBuf,"16") == 0)
				{
					
				}
				else if (strcmp(recvBuf,"17") == 0)
				{
					
				}
				else if (strcmp(recvBuf,"18") == 0)
				{
				}
				else if (strcmp(recvBuf,"19") == 0)
				{
				}
				else if (strcmp(recvBuf,"20") == 0)
				{
					
				}
				
				else 
				{	
					//if (strcmp(recvBuf,"21") == 0)
					
				}
			
			} // 
			
			fflush(stdout);
			
		} // end of while, finish all the thing and close the socket
		close(client_socket);
}


int main(int argc, char *argv[])
{
		// check arguments
		if (argc != 3)
		{
			printf( "Usage: serverSNFS <port> <mount directory>\n");
			return 0;
		}
	
	    // get the absolute path from the root to the mounted directory
		machine_path = realpath(argv[2],NULL); 
		
		// declare vars
		int port;
		int sd, sd_current;
        unsigned int addrlen;
        struct sockaddr_in sin;
        struct sockaddr_in client_socket;
        int counter = 0;//keeps track of the threads created
        pthread_t pthreadClient[10];//array of 10 threads
		
		
		// set up socket
		port = atoi(argv[1]);		
		if (port < 1024 || port > 65536)
		{
			printf("Port number must be between 1024 and 65536\n");
			return 0;
		}
		if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
                perror("not able to open a socket");
                //exit(1);
        }
        if(debug)
               printf("Listening for Connections on port: %d\n", port);

       

        //socket structure
        
        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = INADDR_ANY;
        sin.sin_port = htons(port);

        //bind socket to the given port
        if(bind(sd, (struct sockaddr*) &sin, sizeof(sin)) == -1){
                perror("not able to bind");
                //exit(1);
        }

		// listen and ready to connect
        while(1){
                if(listen(sd,5) == -1){ //not able to listen
                        perror("not able to listen");
                        //exit(1);
                }

                //wait for clients to connect
                addrlen = sizeof(client_socket);
                if((sd_current = accept(sd, (struct sockaddr*) &client_socket, &addrlen)) == -1){
                        perror("not able to accept");
                        return 0;
                }
                //print out client name and port
                if(debug)
                        printf("Accepting client: %s, port: %d\n", inet_ntoa(client_socket.sin_addr), ntohs(client_socket.sin_port));

                if(debug){
                        printf("client sock: %d\n", sd_current);
                }
                //create thread for every connection
                 pthread_create(&pthreadClient[counter], NULL, handleClient, (void *)&sd_current);
                 pthread_detach(pthreadClient[counter]);
                 counter++;
        }
	
		return 0;
}
	
	
	
	
	
	

