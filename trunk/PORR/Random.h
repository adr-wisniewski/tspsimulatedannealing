#ifndef PORR__RANDOM_H__INCLUDED
#define PORR__RANDOM_H__INCLUDED

namespace Random {
	void Seed(unsigned seed);
	unsigned Next();
	unsigned Max();
}

#endif