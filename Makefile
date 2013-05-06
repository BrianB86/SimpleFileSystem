CC = gcc

CFLAG = -Wall `pkg-config fuse --cflags --libs` -D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE

target: serverSNFS clientSNFS 

clientSNFS: clientSNFS.c 
	$(CC) $(CFLAG) -o $@ $<

serverSNFS: serverSNFS.c
	$(CC) $(CFLAG) -o $@ $< -pthread

clean:
	rm serverSNFS clientSNFS
