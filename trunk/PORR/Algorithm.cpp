#include "Algorithm.h"
#include <iostream>
#include <cmath>
#include "Random.h"

namespace SimulatedAnnealing {

Solution& Solution::operator=(const Solution &other) {
	cost = other.cost;
	cycle = other.cycle;
	return *this;
}

float Solution::CalculateCost(const TspProblem &problem) {
	float result  = 0;
	const unsigned cities = problem.GetCitiesCount();

	for(unsigned i = 1; i < cities; ++i) {
		result += problem.GetDistance(cycle[i - 1], cycle[i]);
	}

	result += problem.GetDistance(cycle[cities - 1], cycle[0]);
	return result;
}


std::pair<bool, bool> Solution::AreNeighbours(unsigned index1, unsigned index2) {
	const unsigned cities = cycle.size();

	if( (index2 == 0 && index1 == cities -1) || (index2 == index1 + 1) )
		return std::make_pair(true, false);

	if( (index1 == 0 && index2 == cities -1) || (index1 == index2 + 1) )
		return std::make_pair(true, true);

	return std::make_pair(false, false);
}

void InitialGenerator::Generate(const TspProblem &problem, Solution &solution) {
	unsigned cities = problem.GetCitiesCount();
	solution.cycle.resize(cities);

	// generate random starting position // this could use another independent random generator
	std::vector<unsigned> allCities;
	allCities.resize(cities);

	for(unsigned i = 0; i < cities; ++i) {
		allCities[i] = i;
	}

	for(unsigned i = 0; i < cities; ++i) {
		unsigned randomCity = Random::Next() % allCities.size();
		solution.cycle[i] = allCities[randomCity];
		allCities.erase(allCities.begin() + randomCity);
	}

	solution.cost = solution.CalculateCost(problem);
}

Schedule::Schedule(float _maxTemperature, float _minTemperature, int _steps) 
	: currentTemperature(_maxTemperature), minimalTemperature(_minTemperature), step((_maxTemperature - _minTemperature)/_steps) {
	assert(currentTemperature > minimalTemperature);
	assert(_steps > 0);
	assert(step > 0);
}

float Schedule::GetNextTemperature() {
	return currentTemperature -= step;
}

bool Schedule::IsFinished() {
	return currentTemperature <= minimalTemperature;
}


void Variation::Do(const TspProblem &problem, Solution &solution) {
	unsigned cities = problem.GetCitiesCount();
	assert(cities == solution.cycle.size());
	assert(cities <= Random::Max());
	firstIndex = static_cast<unsigned>(Random::Next()) % cities;

	// this is uniform probability implementation
	/*do {
		secondIndex = static_cast<unsigned>(Random::Next()) % cities;
	} while( secondIndex == firstIndex );*/

	// but this has O(1) complexity, pessimistically two iterations
	secondIndex = static_cast<unsigned>(Random::Next()) % cities;
	while(secondIndex == firstIndex) {
		if(++secondIndex >= cities) {
			secondIndex = 0;
		}
	}

	previousCost = solution.cost;

	std::pair<bool, bool> areNeighbours = solution.AreNeighbours(firstIndex, secondIndex);
	if(areNeighbours.first && areNeighbours.second) {
		std::swap(firstIndex, secondIndex);
	}

	unsigned firstCity = solution.cycle[firstIndex];
	unsigned secondCity = solution.cycle[secondIndex];

	unsigned beforeFirstCity = solution.cycle[firstIndex == 0 ? cities - 1 : firstIndex - 1];
	unsigned afterFirstCity = solution.cycle[firstIndex ==  cities - 1 ? 0 : firstIndex + 1];

	unsigned beforeSecondCity = solution.cycle[secondIndex == 0 ? cities - 1 : secondIndex - 1];
	unsigned afterSecondCity = solution.cycle[secondIndex ==  cities - 1 ? 0 : secondIndex + 1];

	solution.cost -= problem.GetDistance(beforeFirstCity, firstCity);
	solution.cost -= problem.GetDistance(secondCity, afterSecondCity);

	if(!areNeighbours.first) {
		solution.cost -= problem.GetDistance(firstCity, afterFirstCity);
		solution.cost -= problem.GetDistance(beforeSecondCity, secondCity);
	} else {
		solution.cost -= problem.GetDistance(firstCity, secondCity);
	}
	
	SwapCities(solution);

	solution.cost += problem.GetDistance(beforeFirstCity, secondCity);
	solution.cost += problem.GetDistance(firstCity, afterSecondCity);

	if(!areNeighbours.first) {
		solution.cost += problem.GetDistance(secondCity, afterFirstCity);
		solution.cost += problem.GetDistance(beforeSecondCity, firstCity);
	} else {
		solution.cost += problem.GetDistance(secondCity, firstCity);
	}
	
	/*// TODO: remove debug only
	float validCost = solution.CalculateCost(problem);
	float delta = std::abs(solution.cost - validCost);
	if ( delta > 0.2) {
		std::cerr << "Accumulated error is " << delta << std::endl;
	}*/
}

void Variation::UndoLast(Solution &solution) {
	SwapCities(solution);
	solution.cost = previousCost;
}

void Variation::SwapCities(Solution &solution) {
	unsigned temp = solution.cycle[firstIndex];
	solution.cycle[firstIndex] = solution.cycle[secondIndex];
	solution.cycle[secondIndex] = temp;
}

void Solve(const TspProblem &problem, Schedule &schedule, Variation &variation, Solution &solution) {
	Solution currentSolution = solution;
	//unsigned steps = 0; // TODO: remove this
	float temperature;
	float oldCost;

	do {
		//++steps; // TODO: remove this

		temperature = schedule.GetNextTemperature();
		oldCost = currentSolution.cost;

		variation.Do(problem, currentSolution);

		// accept new best solution immediately
		if( currentSolution.cost < solution.cost ) {
			solution = currentSolution;
		// if solution is worse it might not get accepted
		} else if(currentSolution.cost >= oldCost) {
			float costDelta = currentSolution.cost - oldCost;
			float probabilityThreshold = exp(-costDelta/temperature);
			float probabilitySample = Random::Next() / static_cast<float>(Random::Max());

			// step back to previous solution
			if( probabilitySample > probabilityThreshold ) {
				variation.UndoLast(currentSolution);
			}
		}

		/*if( steps % 100000 == 0 ) { // TODO: remove this
			std::cerr << "\t\tAlgorithm passed " << steps << " steps. The temperature is " << temperature << std::endl;
			 currentSolution.cost = currentSolution.CalculateCost(problem); // recalculate cost due to floating point arithmetic error accumulation
		}*/

	} while(!schedule.IsFinished());

	// std::cerr << "\t\tAlgorithm took " << steps << " steps" << std::endl; // TODO: remove this
}

} // namespace SimulatedAnnealing