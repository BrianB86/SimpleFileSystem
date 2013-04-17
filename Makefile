CC = gcc

CFLAG = -Wall `pkg-config fuse --cflags --libs`

target: serverSNFS clientSNFS 

clientSNFS: clientSNFS.c 
	$(CC) $(CFLAG) -o $@ $<

serverSNFS: serverSNFS.c
	$(CC) $(CFLAG) -o $@ $<

clean:
	rm serverSNFS clientSNFS
