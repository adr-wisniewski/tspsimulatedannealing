// PORR.cpp : Defines the entry point for the console application.
#include "TspProblem.h"
#include "Timer.h"
#include "Algorithm.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <ctime>

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

// this is written as simply as possible
bool process_command_line(int argc, char* argv[]) {

	// parse optional
	for(int currentArgument = 1; currentArgument < argc; currentArgument += 2) {
		assert(currentArgument + 1 < argc);
		assert(strlen(argv[currentArgument]) >= 2);
		assert(argv[currentArgument][0] == '-');

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

	assert(!!(cities != -1) ^ !!(!ifile.empty()));		// -c xor -i
	assert(!!(!ofile.empty()) ^ !!(!ifile.empty()));	// -o xor -i
	return true;
}

void do_algorithm_iterations(const TspProblem &problem, int iterations, SimulatedAnnealing::Solution &bestSolution) {
	std::cerr << "Launching " << iterations << " iterations of algorithm on problem size " << problem.GetCitiesCount() << std::endl;

	SimulatedAnnealing::Solution solution;
	SimulatedAnnealing::InitialGenerator generator;

	for( int i = 0; i < iterations; ++i) {
		std::cerr << "\tIteration " << i << std::endl;
		generator.Generate(problem, solution);

		SimulatedAnnealing::Schedule schedule(initialTemperature, minimalTemperature, scheduleSteps);
		SimulatedAnnealing::Variation variation;

		SimulatedAnnealing::Solve(problem, schedule, variation, solution);

		if( i == 0 || solution.cost < bestSolution.cost ) {
			bestSolution = solution;
		}
	}
}

int main(int argc, char* argv[])
{
	Timer::Instance.Start();

	if(!process_command_line(argc, argv))
		return -1;

	// apply defaults
	iterantions = iterantions != -1 ? iterantions : ITERATIONS;
	seed = seed != -1 ? seed : static_cast<int>(time(0));
	initialTemperature = initialTemperature != -1 ? initialTemperature : INITIAL_TEMPERATURE;
	minimalTemperature = minimalTemperature != -1 ? minimalTemperature : MINIMAL_TEMPERATURE;
	scheduleSteps = scheduleSteps != -1 ? scheduleSteps : SCHEDULE_STEPS;

	// initialize random
	srand(seed);

	// initialize problem
	TspProblem problem;
	if( !ifile.empty() ) {
		std::cerr << "Using input file " << ifile << std::endl;
		std::ifstream file(ifile);
		assert(file.good());
		problem.Unserialize(file);
		file.close();
	} else {
		assert(cities > 1);
		std::cerr << "Creating random problem of size " << cities << std::endl;
		problem.Randomize(cities, MAX_DISTANCE);
	}

	// save problem for later use
	if( !ofile.empty() ) {
		std::cerr << "Dumping problem to file " << ofile << std::endl;
		std::ofstream file(ofile);
		assert(file.good());
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

	// mark preparation time
	preparationTime = Timer::Instance.ElapsedSeconds();
	std::cerr << "Finished preparation in " << preparationTime << "s. Current time is: " << preparationTime << std::endl;

	// start algorithm
	SimulatedAnnealing::Solution bestSolution;
	do_algorithm_iterations(problem, iterantions, bestSolution);

	// mark algorithm time
	algorithmTime = Timer::Instance.ElapsedSeconds();
	std::cerr << "Finished algorithm in " << algorithmTime - preparationTime << "s. Current time is: " << algorithmTime << std::endl;

	// print best solution
	std::cout << std::fixed << std::setw(20) << std::setprecision(10) << bestSolution.cost << std::endl;
	for(unsigned i = 0, size = bestSolution.cycle.size(); i < size; ++i) {
		std::cout << bestSolution.cycle[i] << " ";
	}
	std::cout << std::endl;

	// TODO: remove this
	system("PAUSE");
	return 0;
}

