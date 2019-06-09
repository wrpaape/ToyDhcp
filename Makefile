CC = gcc
CFLAGS = -O0 -g
LDFLAGS = -lnsl -lresolv

.PHONY: all zip clean clean-all

all: Server Client

zip: Client.c Server.c Makefile
	zip William_Paape_lab2.zip $^

# report.pdf: README.md
# 	pandoc $< -o $@

Client: Client.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

Server: Server.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

clean:
	$(RM) Client Server

clean-all: clean
	$(RM) William_Paape_lab2.zip

