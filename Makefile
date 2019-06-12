CC = gcc
CFLAGS = -O0 -g -c
LDFLAGS = -lnsl -lresolv

.PHONY: all zip clean clean-all

all: Server Client

zip: Server.c Client.c ToyDhcp.h ToyDhcp.c Makefile readme.txt README.pdf
	zip William_Paape_lab2.zip $^

README.pdf: readme.txt
	pandoc $< -o $@

Server: Server.o ToyDhcp.o
	$(CC) $(LDFLAGS) $^ -o $@

Server.o: Server.c ToyDhcp.h
	$(CC) $(CFLAGS) $< -o $@

Client: Client.o ToyDhcp.o
	$(CC) $(LDFLAGS) $^ -o $@

Client.o: Client.c ToyDhcp.h
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) *.o Server Client

clean-all: clean
	$(RM) README.pdf William_Paape_lab2.zip

