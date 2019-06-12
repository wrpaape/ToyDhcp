#include "ToyDhcp.h"

const int SERVER_BACKLOG = 128;

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
              struct sockaddr_in *client_address)
{
    socklen_t address_length = sizeof(*client_address);
    (void) memset(client_address, 0, address_length);
    int client = accept(server,
                        (struct sockaddr *) client_address,
                        &address_length);
    if (client < 0) {
        failure("accept()");
    }
    return client;
}

static uint8_t
receive_discovery(int client)
{
    (void) fputs("Server: receive_discovery() - receiving discovery...", stdout);
    (void) fflush(stdout);

    uint8_t discovery_id;
    if (recv(client,
             &discovery_id,
             sizeof(discovery_id),
             0) != sizeof(discovery_id)) {
        failure("recv() - ID");
    }

    (void) printf("received discovery %u\n",
                  (unsigned int) discovery_id);
    (void) fflush(stdout);

    return  discovery_id;
}

static void
create_offer(uint8_t discovery_id,
             char    ip_address[INET_ADDRSTRLEN])
{
    (void) snprintf(ip_address,
                    INET_ADDRSTRLEN,
                    "%u.%u.%u.%u",
                    (unsigned int) random_octet(),
                    (unsigned int) random_octet(),
                    (unsigned int) random_octet(),
                    (unsigned int) discovery_id);
}

static void
send_offer(int     client,
           uint8_t discovery_id)
{
    uint8_t offer_id = random_octet();
    char ip_address[INET_ADDRSTRLEN];
    create_offer(discovery_id, ip_address);
    (void) printf("Server: send_offer() - sending offer %u (%s)...",
                  (unsigned int) offer_id,
                  ip_address);
    (void) fflush(stdout);

    if (send(client,
             &offer_id,
             sizeof(offer_id),
             0) != sizeof(offer_id)) {
        failure("send() - ID");
    }

    if (send(client,
             ip_address,
             INET_ADDRSTRLEN,
             0) != INET_ADDRSTRLEN) {
        failure("send() - IP address");
    }

    (void) puts("sent");
    (void) fflush(stdout);
}

static void
receive_request(int  client,
                char ip_address[INET_ADDRSTRLEN])
{
    (void) fputs("Server: receive_request() - receiving request...", stdout);
    (void) fflush(stdout);

    uint8_t request_id;
    if (recv(client,
             &request_id,
             sizeof(request_id),
             0) != sizeof(request_id)) {
        failure("recv() - ID");
    }

    if (recv(client,
             ip_address,
             INET_ADDRSTRLEN,
             0) != INET_ADDRSTRLEN) {
        failure("recv() - IP address");
    }

    (void) printf("received request %u (%s)\n",
                  (unsigned int) request_id,
                  ip_address);
    (void) fflush(stdout);
}

static void
send_ack(int        client,
         const char ip_address[INET_ADDRSTRLEN])
{
    uint8_t ack_id = random_octet();

    (void) printf("Server: send_ack() - sending ACK %u (%s)...",
                  (unsigned int) ack_id,
                  ip_address);
    (void) fflush(stdout);

    if (send(client,
             &ack_id,
             sizeof(ack_id),
             0) != sizeof(ack_id)) {
        failure("send() - ID");
    }

    if (send(client,
             ip_address,
             INET_ADDRSTRLEN,
             0) != INET_ADDRSTRLEN) {
        failure("send() - IP address");
    }

    (void) puts("sent");
    (void) fflush(stdout);
}

static void
parse_ip_address(const struct sockaddr_in *address,
                 char                      readable_ip_address[INET_ADDRSTRLEN])
{
    if (inet_ntop(AF_INET,
                  &address->sin_addr,
                  readable_ip_address,
                  INET_ADDRSTRLEN) == NULL) {
        failure("inet_ntop()");
    }
}

static void
handle_client(int                       client,
              const struct sockaddr_in *client_address)
{
    char ip_address[INET_ADDRSTRLEN];
    parse_ip_address(client_address,
                     ip_address);

    (void) printf(
        "================================================================================\n"
        "Server: handle_client() - handling client %s\n",
        ip_address
    );
    (void) fflush(stdout);

    uint8_t discovery_id = receive_discovery(client);

    send_offer(client, discovery_id);

    receive_request(client, ip_address);

    send_ack(client, ip_address);

    (void) puts("================================================================================");
    (void) fflush(stdout);
}


int
main(void)
{
    int server = create_server_socket();

    while (1) {
        struct sockaddr_in client_address;
        int client = accept_client(server,
                                   &client_address);

        handle_client(client,
                      &client_address);

        if (close(client) != 0) {
            failure("close() - client");
        }
    }

    if (close(server) != 0) {
        failure("close() - server");
    }

    return EXIT_SUCCESS;
}
