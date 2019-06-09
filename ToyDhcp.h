#ifndef TOY_DHCP_H
#define TOY_DHCP_H

/* common headers */
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


/* common constants */
extern uint16_t SERVER_PORT_NUMBER;


/* common functions */
void
failure(const char *cause);

uint8_t
random_octet();

#endif /* ifndef TOY_DHCP_H */
