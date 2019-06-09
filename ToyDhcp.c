#include "ToyDhcp.h"
#include <time.h>

uint16_t SERVER_PORT_NUMBER = 3300 + 718;

void
failure(const char *cause)
{
    perror(cause);
    exit(EXIT_FAILURE);
}

static unsigned int
random_uint(unsigned int max)
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
random_octet()
{
    return random_uint(UINT8_MAX);
}

