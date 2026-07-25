/*
 * Lightweight fuzz driver for x12_proto_decode_surface (ADR-0016).
 * Usage: fuzz_harness [iterations]
 * Feeds pseudo-random buffers; must not crash (exit 0).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "x12_proto.h"

static uint32_t
xorshift(uint32_t *s)
{
    uint32_t x = *s;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *s = x;
    return x;
}

int
main(int argc, char **argv)
{
    uint32_t seed = 0xC0FFEEu;
    int iters = 20000;
    uint8_t buf[256];
    x12_proto_surface_decoded_t out;
    int i, n;

    if (argc > 1)
        iters = atoi(argv[1]);
    if (iters < 1)
        iters = 1;

    for (i = 0; i < iters; i++) {
        n = (int)(xorshift(&seed) % sizeof(buf));
        for (int j = 0; j < n; j++)
            buf[j] = (uint8_t)xorshift(&seed);
        if (n >= 4) {
            buf[0] = 128; /* fake major */
            buf[1] = (uint8_t)(xorshift(&seed) % 16);
            uint16_t len = (uint16_t)((n + 3) / 4);
            buf[2] = (uint8_t)(len & 0xff);
            buf[3] = (uint8_t)(len >> 8);
        }
        (void)x12_proto_decode_surface(buf[0], buf, (size_t)n,
                                       xorshift(&seed) % 5,
                                       (uint8_t)(xorshift(&seed) % 3), &out);
    }
    printf("fuzz_harness: %d iterations OK\n", iters);
    return 0;
}
