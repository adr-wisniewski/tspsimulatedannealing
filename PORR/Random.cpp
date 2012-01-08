#include "Random.h"
#include <cstdlib>

namespace Random {

static unsigned seed;
#pragma omp threadprivate ( seed )

void Seed(unsigned _seed) {
#ifdef _WIN32
	::srand(_seed);
#else
	Random::seed = _seed;
#endif
}

unsigned Next() {
#ifdef _WIN32
	return static_cast<unsigned>(::rand());
#else
	return rand_r(&Random::seed);
#endif
}

unsigned Max() {
	return RAND_MAX;
}

}

