#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <stdint.h>

uint16_t SERVER_PORT_NUMBER = 3300 + 718;

static void
failure(const char *cause)
{
    perror(cause);
    exit(EXIT_FAILURE);
}

static int
create_server_socket()
{
    int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == -1) {
        failure("socket()");
    }

    int option = SO_REUSEADDR  /* allow other sockets to bind() to this port */
               | SO_REUSEPORT; /* force reuse of this port */
    int option_value = 1; /* true */
    if (setsockopt(server,
                   SOL_SOCKET, /* manipulate options at the sockets API level */
                   option,
                   &option_value,
                   sizeof(option_value)) != 0) {
        failure("setsockopt()");
    }

	struct sockaddr_in address;
    (void) memset(&address, 0, sizeof(address));
	address.sin_family      = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port        = htons(SERVER_PORT_NUMBER);
    if (bind(server,
             (const struct sockaddr *) &address,
             sizeof(address)) != 0) {
        failure("bind()");
    }

    return server;
}

static int
accept_client(int                 server,
              struct sockaddr_in *address)
{
    socklen_t address_length = sizeof(*address);
    (void) memset(address, 0, address_length);
    int client = accept(server,
                        (struct sockaddr *) address,
                        &address_length);
    if (client < 0) {
        failure("accept()");
    }
    return client;
}


static void
handle_client(int server, int client)
{

}


int
main(void)
{
    int server = create_server_socket();

    while (1) {
        struct sockaddr_in client_address;
        int client = accept_client(server,
                                   &client_address);

        handle_client(server, client);

        if (close(client) != 0) {
            failure("close()");
        }
    }

    return EXIT_SUCCESS;
}
