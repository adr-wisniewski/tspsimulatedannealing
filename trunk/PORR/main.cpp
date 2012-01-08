// PORR.cpp : Defines the entry point for the console application.
#include "TspProblem.h"
#include "Timer.h"
#include "Algorithm.h"
#include "Random.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <limits>
#include <ctime>
#include <cstring>
#include <cstdlib>

// constants for defaults
const int ITERATIONS = 1;
const unsigned MAX_DISTANCE = 100;
const float INITIAL_TEMPERATURE = 10000.0f;
const float MINIMAL_TEMPERATURE = 0.01f;
const int SCHEDULE_STEPS = 1000000;

// program options
int cities = -1;
int iterantions = -1;
int seed = -1;
std::string ifile;
std::string ofile;
float initialTemperature = -1;
float minimalTemperature = -1;
int scheduleSteps = -1;


// observerd times
double preparationTime;
double algorithmTime;
double resultsTime;

// this is for options checking
#define GUARD(v) guard(v, #v, __FILE__, __LINE__)

void guard(bool value, const char* line, const char* filename, int linenumber) {
	if(!value) {
		std::cerr << "GUARD ERROR " << line << " in " << filename << " at " << linenumber << std::endl;
		abort();
	}
}

// this is written as simply as possible
bool process_options(int argc, char* argv[], TspProblem &problem) {

	// parse optional
	for(int currentArgument = 1; currentArgument < argc; currentArgument += 2) {
		GUARD(currentArgument + 1 < argc);
		GUARD(strlen(argv[currentArgument]) >= 2);
		GUARD(argv[currentArgument][0] == '-');

		switch(argv[currentArgument][1]) {
		case 'c':
			cities = atoi(argv[currentArgument+1]);
			break;

		case 'i':
			iterantions = atoi(argv[currentArgument+1]);
			break;

		case 'r':
			seed = atoi(argv[currentArgument+1]);
			break;

		case 't':
			initialTemperature = static_cast<float>(atof(argv[currentArgument+1]));
			break;

		case 'm':
			minimalTemperature = static_cast<float>(atof(argv[currentArgument+1]));
			break;

		case 's':
			scheduleSteps = static_cast<int>(atof(argv[currentArgument+1]));
			break;

		case 'f':
			ifile = std::string(argv[currentArgument+1]);
			break;

		case 'o':
			ofile = std::string(argv[currentArgument+1]);
			break;
		}
	}

	GUARD(!!(cities != -1) ^ !!(!ifile.empty()));	// -c xor -i
	GUARD(!(!ofile.empty() && !ifile.empty()));		// !(-o && -i)

	// apply defaults
	iterantions = iterantions != -1 ? iterantions : ITERATIONS;
	seed = seed != -1 ? seed : static_cast<int>(time(0));
	initialTemperature = initialTemperature != -1 ? initialTemperature : INITIAL_TEMPERATURE;
	minimalTemperature = minimalTemperature != -1 ? minimalTemperature : MINIMAL_TEMPERATURE;
	scheduleSteps = scheduleSteps != -1 ? scheduleSteps : SCHEDULE_STEPS;

	if( !ifile.empty() ) {
		std::cerr << "Using input file " << ifile << std::endl;
		std::ifstream file(ifile.c_str());
		GUARD(file.good());
		problem.Unserialize(file);
		file.close();
	} else {
		GUARD(cities > 1);
		std::cerr << "Creating random problem of size " << cities << std::endl;
		problem.Randomize(cities, MAX_DISTANCE);
	}

	// save problem for later use
	if( !ofile.empty() ) {
		std::cerr << "Dumping problem to file " << ofile << std::endl;
		std::ofstream file(ofile.c_str());
		GUARD(file.good());
		problem.Serialize(file);
		file.close();
	}

	// show options
	std::cerr << "Provided options: "		<< std::endl;
	std::cerr << "\tProblem size: "			<< problem.GetCitiesCount() << std::endl;
	std::cerr << "\tIterations: "			<< iterantions << std::endl;
	std::cerr << "\tInitial temperature: "	<< initialTemperature << std::endl;
	std::cerr << "\tMinimal temperature: "	<< minimalTemperature << std::endl;
	std::cerr << "\tSchedule steps:\t"		<< scheduleSteps << std::endl;
	return true;
}

void print_results(SimulatedAnnealing::Solution &bestSolution) {
	// print best solution
	std::cout << std::fixed << std::setw(20) << std::setprecision(10) << bestSolution.cost << std::endl;
	for(unsigned i = 0, size = bestSolution.cycle.size(); i < size; ++i) {
		std::cout << bestSolution.cycle[i] << " ";
	}
	std::cout << std::endl;
}

void do_algorithm_iterations(const TspProblem &problem, int iterations, SimulatedAnnealing::Solution &bestSolution, int seed) {
	bestSolution.cost = std::numeric_limits<float>::max();

	#pragma omp parallel 
	{
		SimulatedAnnealing::Solution threadBest;
		threadBest.cost = std::numeric_limits<float>::max();

		#pragma omp for
		for(int i = 0; i < iterations; ++i) {
			// create all needed elements
			SimulatedAnnealing::Solution solution;
			SimulatedAnnealing::InitialGenerator generator;
			SimulatedAnnealing::Schedule schedule(initialTemperature, minimalTemperature, scheduleSteps);
			SimulatedAnnealing::Variation variation;

			// initialize random generator
			Random::Seed(seed + i);

			// generate initial solution
			generator.Generate(problem, solution);

			// solve problem
			SimulatedAnnealing::Solve(problem, schedule, variation, solution);

			// update best solution
			if(solution.cost < threadBest.cost ) {
				threadBest = solution;
			}
		}

		#pragma omp critical 
		{
			if(threadBest.cost < bestSolution.cost ) {
				bestSolution = threadBest;
			}
		}
	}
}

int main(int argc, char* argv[])
{
	Timer::Instance.Start();

	// load options
	TspProblem problem;
	if(!process_options(argc, argv, problem))
		return -1;

	// mark preparation time
	preparationTime = Timer::Instance.ElapsedSeconds();
	std::cerr << "Finished preparation in " << preparationTime << "s. Current time is: " << preparationTime << std::endl;

	// start algorithm
	SimulatedAnnealing::Solution bestSolution;
	do_algorithm_iterations(problem, iterantions, bestSolution, seed);

	// mark algorithm time
	algorithmTime = Timer::Instance.ElapsedSeconds();
	std::cerr << "Finished algorithm in " << algorithmTime - preparationTime << "s. Current time is: " << algorithmTime << std::endl;

	// print results
	print_results(bestSolution);

	// mark results time
	resultsTime = Timer::Instance.ElapsedSeconds();
	std::cerr << "Finished result output in " << resultsTime - algorithmTime << "s. Current time is: " << resultsTime << std::endl;

	std::cerr << preparationTime << "\t" << algorithmTime - preparationTime << "\t" << resultsTime - algorithmTime << "\t" << std::endl;
	return 0;
}

