#ifndef PORR__SIMULATEDANNEALING_H__INCLUDED
#define PORR__SIMULATEDANNEALING_H__INCLUDED

#include <vector>
#include <utility>
#include "TspProblem.h"

namespace SimulatedAnnealing {

	class Solution {
	public:
		Solution& operator=(const Solution &other);
		float CalculateCost(const TspProblem &problem);
		std::pair<bool, bool> AreNeighbours(unsigned index1, unsigned index2);

	public:
		std::vector<unsigned> cycle;
		float cost;
	};

	class InitialGenerator {
	public:
		void Generate(const TspProblem &problem,  Solution &solution);
	};

	// annealing schedule (could be pure virtual)
	class Schedule {
	public:
		Schedule(float maxTemperature, float minTemperature, int steps);
		float GetNextTemperature();
		bool IsFinished();

	protected:
		float currentTemperature;
		float minimalTemperature;
		float step;
	};

	// solution variation method (could be pure virtual)
	class Variation {
	public:
		void Do(const TspProblem &problem, Solution &solution);
		void UndoLast(Solution &solution);

	protected:
		void SwapCities(Solution &solution);

		unsigned firstIndex, secondIndex;
		float previousCost;
	};

	void Solve(const TspProblem &problem, Schedule &schedule, Variation &variation, Solution &solution);
}

#endif