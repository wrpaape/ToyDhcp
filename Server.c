#include "ToyDhcp.h"

int SERVER_BACKLOG = 128;

static int
create_server_socket()
{
    int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == -1) {
        failure("socket()");
    }

    static const int option = SO_REUSEADDR  /* allow other sockets to bind() to this port */
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

    if (listen(server, SERVER_BACKLOG) != 0) {
        failure("listen()");
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
receive_discovery(int server)
{

}

static void
send_offer(int server)
{

}

static void
receive_request(int server)
{

}

static void
send_ack(int server)
{

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

        receive_discovery(client);

        send_offer(client);

        receive_request(client);

        send_ack(client);

        if (close(client) != 0) {
            failure("close()");
        }
    }

    return EXIT_SUCCESS;
}
