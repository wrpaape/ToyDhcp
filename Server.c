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
receive_discovery(int client)
{
    (void) fputs("Server: receive_discovery() - receiving discovery...", stdout);

    uint8_t discovery_id;
    if (recv(client,
             &discovery_id,
             sizeof(discovery_id),
             0) != sizeof(discovery_id)) {
        failure("recv() - ID");
    }

    (void) printf("received discovery %u\n",
                  (unsigned int) discovery_id);
}

static uint8_t
create_offer(char ip_address[IP_ADDRESS_SIZE])
{
    uint8_t offer_id = random_octet();
    (void) snprintf(ip_address,
                    IP_ADDRESS_SIZE,
                    "%u.%u.%u.%u",
                    (unsigned int) random_octet(),
                    (unsigned int) random_octet(),
                    (unsigned int) random_octet(),
                    (unsigned int) offer_id);

    return offer_id;
}

static void
send_offer(int client)
{
    char ip_address[IP_ADDRESS_SIZE];
    uint8_t offer_id = create_offer(ip_address);
    (void) printf("Server: send_offer() - sending offer %u (%s)...",
                  (unsigned int) offer_id,
                  ip_address);

    if (send(client,
             &offer_id,
             sizeof(offer_id),
             0) != sizeof(offer_id)) {
        failure("send() - ID");
    }

    if (send(client,
             ip_address,
             IP_ADDRESS_SIZE,
             0) != IP_ADDRESS_SIZE) {
        failure("send() - IP address");
    }

    (void) puts("sent");
}

static void
receive_request(int  client,
                char ip_address[IP_ADDRESS_SIZE])
{
    (void) fputs("Server: receive_request() - receiving request...", stdout);

    uint8_t request_id;
    if (recv(client,
             &request_id,
             sizeof(request_id),
             0) != sizeof(request_id)) {
        failure("recv() - ID");
    }

    if (recv(client,
             ip_address,
             IP_ADDRESS_SIZE,
             0) != IP_ADDRESS_SIZE) {
        failure("recv() - IP address");
    }

    (void) printf("received request %u (%s)\n",
                  (unsigned int) request_id,
                  ip_address);
}

static void
send_ack(int        client,
         const char ip_address[IP_ADDRESS_SIZE])
{
    uint8_t ack_id = random_octet();

    (void) printf("Server: send_ack() - sending ACK %u (%s)...",
                  (unsigned int) ack_id,
                  ip_address);

    if (send(client,
             &ack_id,
             sizeof(ack_id),
             0) != sizeof(ack_id)) {
        failure("send() - ID");
    }

    if (send(client,
             ip_address,
             IP_ADDRESS_SIZE,
             0) != IP_ADDRESS_SIZE) {
        failure("send() - IP address");
    }

    (void) puts("sent");
}


static void
handle_client(int client)
{
    receive_discovery(client);

    send_offer(client);

    char ip_address[IP_ADDRESS_SIZE];
    receive_request(client, ip_address);

    send_ack(client, ip_address);
}


int
main(void)
{
    int server = create_server_socket();

    while (1) {
        struct sockaddr_in client_address;
        int client = accept_client(server,
                                   &client_address);

        handle_client(client);

        if (close(client) != 0) {
            failure("close()");
        }
    }

    if (close(server) != 0) {
        failure("close()");
    }

    return EXIT_SUCCESS;
}
