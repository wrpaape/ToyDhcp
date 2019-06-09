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
#include <time.h>

unsigned int
random_uint(int max)
{
    static int rand_seeded = 0;
    if (!rand_seeded) {
        srand(time(NULL));
        rand_seeded = 1;
    }

    const unsigned int x = (RAND_MAX + 1u) / max;
    const unsigned int y = x * max;
    unsigned int r;
    do {
          r = rand();
    } while(r >= y);
    return r / x;
}

uint8_t
generate_transaction_id()
{
    return random_uint(UINT8_MAX);
}

int
main(void)
{
    return EXIT_SUCCESS;
}
