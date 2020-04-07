server:
	 gcc -Wall -std=c99 server.c -o server
	 gcc -Wall -std=c99 client.c -o client
clean:
	@rm -rf server client
