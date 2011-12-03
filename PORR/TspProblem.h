#ifndef PORR__TSPPROBLEM_H__INCLUDED
#define PORR__TSPPROBLEM_H__INCLUDED

#include <assert.h>
#include <ios>

class TspProblem {
public:
	TspProblem();
	~TspProblem();

	// initialization
	void Randomize(unsigned _citiesCount, unsigned maxDistance);

	// serialization
	void Serialize(std::ostream &out);
	void Unserialize(std::istream &in);

	// access
	inline float GetDistance(unsigned from, unsigned to) const {
		assert(distanceMatrix);
		assert(from < citiesCount);
		assert(to < citiesCount);
		assert(from != to);
		return distanceMatrix[from * citiesCount + to];
	}

	inline unsigned GetCitiesCount() const {
		return citiesCount;
	}

protected:

	float &DistanceMatrix(unsigned from, unsigned to) {
		assert(from < citiesCount);
		assert(to < citiesCount);
		return distanceMatrix[from * citiesCount + to];
	}

	unsigned citiesCount;
	float *distanceMatrix;
};

#endif