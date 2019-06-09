#include "ToyDhcp.h"

int create_client_socket(const char *server_ip)
{
    int client = socket(AF_INET, SOCK_STREAM, 0);
    if (client == -1) {
        failure("socket()");
    }

	struct sockaddr_in server_address;
    (void) memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port   = htons(SERVER_PORT_NUMBER);
	if (inet_pton(AF_INET,
                  server_ip,
                  &server_address.sin_addr) != 1) {
		failure("inet_pton()");
    }

	if (connect(client,
			    (const struct sockaddr *) &server_address,
			    sizeof(server_address)) != 0) {
        failure("connect()");
    }

    return client;
}

static void
send_discovery(int client)
{
    uint8_t discovery_id = random_octet();

    (void) printf("Client: send_discovery() - sending discovery %u...",
                  (unsigned int) discovery_id);
    (void) fflush(stdout);

    if (send(client,
             &discovery_id,
             sizeof(discovery_id),
             0) != sizeof(discovery_id)) {
        failure("send() - ID");
    }

    (void) puts("sent");
    (void) fflush(stdout);
}

static void
receive_offer(int  client,
              char ip_address[INET_ADDRSTRLEN])
{
    (void) fputs("Client: receive_offer() - receiving offer...", stdout);
    (void) fflush(stdout);

    uint8_t offer_id;
    if (recv(client,
             &offer_id,
             sizeof(offer_id),
             0) != sizeof(offer_id)) {
        failure("recv() - ID");
    }

    if (recv(client,
             ip_address,
             INET_ADDRSTRLEN,
             0) != INET_ADDRSTRLEN) {
        failure("recv() - IP address");
    }

    (void) printf("received offer %u (%s)\n",
                  (unsigned int) offer_id,
                  ip_address);
    (void) fflush(stdout);
}

static void
send_request(int        client,
             const char ip_address[INET_ADDRSTRLEN])
{
    uint8_t request_id = random_octet();

    (void) printf("Client: send_request() - sending request %u (%s)...",
                  (unsigned int) request_id,
                  ip_address);
    (void) fflush(stdout);

    if (send(client,
             &request_id,
             sizeof(request_id),
             0) != sizeof(request_id)) {
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
receive_ack(int client)
{
    (void) fputs("Client: receive_ack() - receiving ACK...", stdout);
    (void) fflush(stdout);

    uint8_t ack_id;
    if (recv(client,
             &ack_id,
             sizeof(ack_id),
             0) != sizeof(ack_id)) {
        failure("recv() - ID");
    }

    char ip_address[INET_ADDRSTRLEN];
    if (recv(client,
             &ip_address,
             sizeof(ip_address),
             0) != sizeof(ip_address)) {
        failure("recv() - IP address");
    }

    (void) printf("received ack %u (%s)\n",
                  (unsigned int) ack_id,
                  ip_address);
    (void) fflush(stdout);
}

int
main(void)
{
    int client = create_client_socket("127.0.0.1");

    send_discovery(client);

    char ip_address[INET_ADDRSTRLEN];
    receive_offer(client, ip_address);

    send_request(client, ip_address);

    receive_ack(client);

    if (close(client) != 0) {
        failure("close()");
    }

    return EXIT_SUCCESS;
}
