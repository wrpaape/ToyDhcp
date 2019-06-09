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
                  &server_address.sin_addr)) {
		failure("inet_pton()");
    }

	if (connect(client,
			    (const struct sockaddr *) &server_address,
			    sizeof(server_address) != 0)) {
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

    if (send(client,
             &discovery_id,
             sizeof(discovery_id),
             0) != sizeof(discovery_id)) {
        failure("send()");
    }

    (void) puts("sent");
}

static void
receive_offer(int                      client,
              struct IpAddressMessage *offer)
{
    (void) fputs("Client: receive_offer() - receiving offer...", stdout);

    if (recv(client,
             offer,
             sizeof(*offer),
             0) != sizeof(*offer)) {
        failure("recv()");
    }

    (void) printf("received offer %u (%s)\n",
                  (unsigned int) offer->id,
                  offer->address);
}

static void
send_request(int client,
             struct IpAddressMessage *request)
{
    request->id = random_octet();

    (void) printf("Client: send_request() - sending request %u (%s)...",
                  (unsigned int) request->id,
                  request->address);

    if (send(client,
             request,
             sizeof(*request),
             0) != sizeof(*request)) {
        failure("send()");
    }

    (void) puts("sent");
}

static void
receive_ack(int client)
{
    (void) fputs("Client: receive_ack() - receiving ACK...", stdout);

    struct IpAddressMessage ack;
    if (recv(client,
             &ack,
             sizeof(ack),
             0) != sizeof(ack)) {
        failure("recv()");
    }

    (void) printf("received ack %u (%s)\n",
                  (unsigned int) ack.id,
                  ack.address);
}

int
main(void)
{
    int client = create_client_socket("127.0.0.1");

    send_discovery(client);

    struct IpAddressMessage message;
    receive_offer(client, &message);

    send_request(client, &message);

    receive_ack(client);

    if (close(client) != 0) {
        failure("close()");
    }

    return EXIT_SUCCESS;
}
