#ifndef CHRISTOFIDES_HPP
#define CHRISTOFIDES_HPP
#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stack>
#include <vector>
#include <cstring>
#include <cmath>
#include <fstream>
#include <pthread.h>

class Christofides {
	private:
		// Coordinates of node from file
		struct City {
		int x, y;
		};
		std::string inputFile;	// inputFile
		std::string outputFile;	// outputFile with .tour appended
		std::vector<int>odds;		// List of nodes that are odd
		int **cost;							// Used to find matching odd nodes
		void nodeCount();				// Used to find the count of nodes
		int getMinimum(int key[], int mstSet[]);
	public:
		int numberOfCities;					// Amount of nodes
		std::vector<int>circuit;		// Euler Circuit
		std::vector<City>cities;		// Store the cities and their coordinates
		int **graph;								// Distances between each city
		int pathLength;							// Shortest path length
		std::vector<int> *adjacent;	// Array of dyn arrs, holds a list of nodes
		int startIndex[1], endIndex[1];
		int **pathVals;							// Store length of TSP path from each node
		Christofides(std::string in, std::string out);						// Constructor
		~Christofides();																					// Destructor
		int getDistance(struct City city1, struct City city2);		// Calc distance
		void getCities();						// Get all Cities
		void buildMatrix();					// Build out the matrix
		void findMST();							// Get the Minimum Spanning Tree
		void findMatch();						// Get the match
		int findSmallest(int);			// Get the best city to start at
		void euler(int pos, std::vector<int> &);
		void makeHamiltonian(std::vector<int> &, int&); // Make a hamiltonian path
		void shrinkPath();											// Call twoOpt and shrink the path size
		// 2-Opt (Optimization Heuristic) - Just using Christofides wasn't efficient enough
		// Currently looking into Lin-Kernighan Heuristic which utilizes 2-opt and 3-opt
		// Not sure we'll have enough time to implement that here
		int twoOpt(int **graph, std::vector<int> &path, int &pathLength, int numberOfCities);
		int findShorter(int **graph, int node1, int node2, int node3, int node4, int &fullDistanceance);
		void swap(std::vector<int> &path, int start, int end, int numberOfCities);
};
#endif
