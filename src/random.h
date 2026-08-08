#pragma once

void srand_msvc(unsigned int seed);
int rand_msvc();

void srand_custom(int seed);
int rand_custom();

unsigned int get_rand_msvc_state();
