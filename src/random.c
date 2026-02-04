/*
 * Pseudorandom generation routines
 */

static unsigned int seed_msvc = 1;
static double seed_custom = 0;

void srand_msvc(unsigned int seed) {
    seed_msvc = seed;
}

int rand_msvc() {
    seed_msvc = seed_msvc * 214013 + 2531011;
    return (seed_msvc >> 16) & 0x7fff;
}

void srand_custom(int seed) {
    seed_custom = (double)seed;
}

int rand_custom() {
    double seed = 1.4294484665 * seed_custom;
    while (seed > 65535.0f)
        seed -= 65535.0f;
    seed_custom = seed;
    return 65535.0f * ((long double)seed - (long double)(int)seed);
}
