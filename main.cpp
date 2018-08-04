

#include <iostream>
#include "christofides.hpp"
#include "threads.hpp"

int incrementCities(int, int);

int main(int argc, char** argv) {
	// Check that the user entered a filename on the command line
	if (argc < 2){
		std::cout << "To run this program type: './tsp yourFile.txt'" << std::endl;
		exit(-1);
	}
	// Begin output to CLI
	std::cout << "|--------------------------------"<< std::endl;
	std::string inputFile, outputFile;				// Read file names from input
	// Set argument from CLI to be the input and output files
	inputFile = outputFile = argv[1];
	// Append .tour to the output file as per project specs
	outputFile.append(".tour");
	Christofides tsp(inputFile, outputFile);	// Create a new Christofides object
	int numberOfCities = tsp.numberOfCities;  // Gets the size (aka # of cities)
	tsp.getCities();													// Read cities from file
	std::cout << "| " << inputFile << " OPENED SUCCESSFULLY" << std::endl;
	std::cout << "| Number of Cities To Analyze: " << tsp.numberOfCities << std::endl;
	std::cout << "| Finding Path..." << std::endl;
	tsp.buildMatrix();	// Fill matrix with distances between nodes
	tsp.findMST();			// Find the minimum spanning tree
	tsp.findMatch();
	clock_t start, end;
	start = clock();								// Start clock for timing algo
	Thread threads[1];							// Create array of thread objects
	int best = 2147483647;					// Typical value of max int
	int bestIndex, finish = 1;
	// Note that tsp_example_1 and tsp_example_2 will increment
	// by 1. tsp_example_3 will increment by 4960.
 	int increment = 1;
	if (numberOfCities >= 400 && numberOfCities < 1040)
		increment = 4;
	else if (numberOfCities >= 1040 && numberOfCities < 1800)
		increment = 8;
	else if (numberOfCities >= 1800 && numberOfCities < 3200)
		increment = 32;
	else if (numberOfCities >= 3200 && numberOfCities < 4000)
		increment = 48;
	else if (numberOfCities >= 4000 && numberOfCities < 5000)
		increment = 120;
	else if (numberOfCities >= 5000 && numberOfCities < 6496)
		increment = 248;
	else if (numberOfCities >= 6496 && numberOfCities < 8000)
		increment = 496;
	else if (numberOfCities >= 8000 && numberOfCities < 9000)
		increment = 992;
	else if (numberOfCities >= 9000 && numberOfCities < 10000)
		increment = 1984;
	else if (numberOfCities >= 10000 && numberOfCities < 12496)
		increment = 2976;
	else if (numberOfCities >= 12496 && numberOfCities < 14992)
		increment = 3968;
	else if (numberOfCities >= 14992)
		increment = 4960;
	// END CITY BREAKPOINTS FOR SPEED OPTIMIZATION
	int remaining = numberOfCities;
	int node = 0;
	int count = 0;	 // Start at 0 for vertex and count for thread
	while (remaining >= increment) {
		if (remaining < increment)
			finish = remaining/increment; // Iter moves 1 * increment nodes
		long i = 0;
		while (i < finish) {
			threads[i].startNode = node;
			threads[i].mainID = count;
			threads[i].mainTSP = &tsp;
			threads[i].begin();
			node = node + increment;
			count++;
			i++;
		}
		i = 0;	// Wait for all the threads
		while (i < finish) {
			threads[i].join();
			i++;
		}
		remaining = remaining - (finish * increment);
	}
	// Loop through each index used and find shortest path
	long i = 0;
	while (i < count) {
		if (tsp.pathVals[i][1] < best) {
			bestIndex = tsp.pathVals[i][0];
			best = tsp.pathVals[i][1];
		}
		i++;
	}
	// Store the best path
	tsp.euler(bestIndex, tsp.circuit);
	tsp.makeHamiltonian(tsp.circuit, tsp.pathLength);
	// Can add more of the shrinkPath() functions to lower the path #
	// Keeping at 1 right now for increasing the speed while staying below the
	// 1.25 threshold. Calling it too many times slows everything down.
	// EDIT - Changed the loop to call shrinkPath more times and still stay
	// 				within the three minute timeframe for the competition.
	for (int shrinkCount = 0; shrinkCount < 6; shrinkCount++){
		tsp.shrinkPath();
	}
	std::cout << "| Best Path: " << tsp.pathLength << std::endl;
	end = clock();	// Stop clock for timing algo
	// Store the time and then report it on the CLI
	double reportTime = ((double)(end-start))/CLOCKS_PER_SEC;
	std::cout << "| Time To Complete: " << reportTime << "s\n";
	// End output to CLI
	std::cout << "|--------------------------------" << std::endl;
	// Print to file
	std::ofstream outputStream;
	outputStream.open(outputFile.c_str(), std::ios::out);
	outputStream << tsp.pathLength << std::endl;
	std::vector<int>::iterator addVals = tsp.circuit.begin();
	while (addVals != tsp.circuit.end()) {
		outputStream << *addVals << std::endl;
		++addVals;
	}
	outputStream.close();
	return 0;
}
