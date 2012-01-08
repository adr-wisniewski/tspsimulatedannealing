#include "TspProblem.h"
#include <iostream>
#include "Random.h"

TspProblem::TspProblem() : citiesCount(0), distanceMatrix(0L)  {
	// empty
}

TspProblem::~TspProblem() {
	delete distanceMatrix;
}

void TspProblem::Randomize(unsigned _citiesCount, unsigned maxDistance) {
	assert(distanceMatrix == 0L);

	citiesCount = _citiesCount;	
	distanceMatrix = new float[citiesCount*citiesCount];

	for( unsigned i = 0, count = citiesCount * citiesCount; i < count; ++i ) {
		float multiplier = Random::Next() / float(Random::Max());
		distanceMatrix[i] = multiplier * maxDistance;
	}
}

void TspProblem::Serialize(std::ostream &out) {
	assert(distanceMatrix);

	out << citiesCount << std::endl;

	for( unsigned from = 0; from < citiesCount; ++from ) {
		for( unsigned to = 0; to < citiesCount; ++to ) {
			out << DistanceMatrix(from, to) << " ";
		}

		out << std::endl;
	}
}

void TspProblem::Unserialize(std::istream &in) {
	assert(distanceMatrix == 0L);

	in >> citiesCount;
	distanceMatrix = new float[citiesCount * citiesCount];

	for( unsigned from = 0; from < citiesCount; ++from ) {
		for( unsigned to = 0; to < citiesCount; ++to ) {
			in >> DistanceMatrix(from, to);
		}
	}
}