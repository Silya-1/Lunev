CFLAGS := -Wall -O2 -std=c11
server: server.c 
	$(CC) $(CFLAGS) -o $@ $^ -lm
client: client.c
	$(CC) $(CFLAGS) -o $@ $^ -lm -pthread
clean: 
	rm -f server client


