

#include "christofides.hpp"
#include "threads.hpp"

struct threadInfo {
	int threadID;
	Christofides *tsp;
};
struct threadInfo *data;

// Constructor
Christofides::Christofides(std::string in, std::string out){
	inputFile = in;
	outputFile = out;
	nodeCount();	// Sets n to num of lines read in
	graph = new int*[numberOfCities]; // Allocate new memory for cities
	for (int i = 0; i < numberOfCities; i++) {
		graph[i] = new int[numberOfCities];
		for (int j = 0; j < numberOfCities; j++)
			graph[i][j] = 0;
	}
	cost = new int*[numberOfCities];
	for (int i = 0; i < numberOfCities; i++) {
		cost[i] = new int[numberOfCities];
	}
	pathVals = new int*[numberOfCities];
	for (int i = 0; i < numberOfCities; i++) {
		pathVals[i] = new int[numberOfCities];
	}
	adjacent = new std::vector<int> [numberOfCities];
};

Christofides::~Christofides(){
	// Destructor
	for (int i = 0; i < numberOfCities; i++) {
		delete [] graph[i];
		delete [] pathVals[i];
		delete [] cost[i];
	}
	delete [] graph;
	delete [] pathVals;
	delete [] cost;
	delete [] adjacent;
}

void Christofides::nodeCount() {
	int count = 0;
	std::ifstream inStream;
	inStream.open(inputFile.c_str(), std::ios::in);
	if (!inStream) {
	  std::cout << "Can't Open File: " << inputFile << std::endl;
	  exit(1);
	}
	std::string notVisited;
	while (std::getline(inStream, notVisited))
	   ++count;
	numberOfCities = count;
	inStream.close();						// Close out the file
};

void Christofides::getCities(){
	std::ifstream inStream;
	inStream.open(inputFile.c_str(), std::ios::in);
	if (!inStream) {
	  std::cout << "Can't Open File: " << inputFile << std::endl;
	  exit(1);
	}
	int c, x, y, i = 0; // c = city, x & y are coordinates
	while (!inStream.eof() ) {
		inStream >> c >> x >> y; 	// Read in city ID and coordinates
		struct City c = {x, y};		// Set the values to the struct
		cities.push_back(c);			// Push back new city
		i++;
	}
	inStream.close();						// Close out the file
};

// Calculates the distance between two cities
int Christofides::getDistance(struct Christofides::City city1, struct Christofides::City city2) {
	int distanceX = (float)((city1.x - city2.x)*(city1.x - city2.x));
	int distanceY = (float)((city1.y - city2.y)*(city1.y - city2.y));
	return (floor((float) (sqrt(distanceX + distanceY)) + 0.5));
};

void *loadTheDistances(void* arguments){
	struct threadInfo *threadData = (struct threadInfo *) arguments;
	int threadID = (*threadData).threadID;
	Christofides *tsp = (*threadData).tsp;
	int **graph = (*tsp).graph;
	int start, end;
	start = (*tsp).startIndex[threadID];
	end = (*tsp).endIndex[threadID];
	int i = start;
	// Load matrix with the distance from
	// every city to each other
	while (i <= end) {
		int j = i;
		while (j < (*tsp).numberOfCities) {
			graph[i][j] = graph[j][i] =  (*tsp).getDistance((*tsp).cities[i], (*tsp).cities[j]);
			j++;
		}
		i++;
	}
	pthread_exit(NULL);
}

void Christofides::buildMatrix(){
	int amount = numberOfCities * 0.2;
	int x = numberOfCities - amount;
	int remaining = numberOfCities - x * 1;
	int half = 1/2 + 1, position = 0, i = 0;
	while (i < half) {
		startIndex[i] = position;
		position = position + (x - 1);
		endIndex[i] = position;
		position++;
		i++;
	}
	int remainingThreads = 1 - half + 1;
	int extraToEach = remaining / remainingThreads;
	// Divide remainer among second half of threads
	i = half;
	while (i < 1) {
		startIndex[i] = position;
		position = position + (x + extraToEach - 1);
		endIndex[i] = position;
		position++;
		i++;
	}
	endIndex[0] = numberOfCities - 1;
	int s;
	void *attribute;
	data = new struct threadInfo[numberOfCities];
	pthread_t *thread = new pthread_t[numberOfCities]; // New memory for IDs
	pthread_attr_t newAttribute;

	// Initialize and set thread detached newAttribute
	pthread_attr_init(&newAttribute);
	pthread_attr_setdetachstate(&newAttribute, PTHREAD_CREATE_JOINABLE);
	long j = 0;
	while (j < 1) {
		data[j].threadID = j;
		data[j].tsp = this;
		s = pthread_create(&thread[j], &newAttribute, loadTheDistances, (void*)&data[j]);
		if (s)
			exit(-1);
		j++;
	}
	// Free newAttribute, then wait for other threads
	pthread_attr_destroy(&newAttribute);
	j = 0;
	while (j < 1) {
		s = pthread_join(thread[j], &attribute);
		if (s)
			exit(-1);
		j++;
	}
	delete [] data;
}

// Choose min edge weight, connect vertex in A to vertex outside
void Christofides::findMST() {
	int key[numberOfCities];   				// Key values used to pick minimum weight edge in cut
	int withinMST[numberOfCities];  	// To represent set of vertices not yet included in MST
	int parent[numberOfCities];
	int v = 0;
	while (v < numberOfCities) {
		key[v] = 2147483647; 						// Max int value
		withinMST[v] = 0;								// Not in MST
		v++;
	}
	key[0] = 0;												// Node 0 is the root
	parent[0] = -1; 									// First node is always root
	for (int i = 0; i < numberOfCities - 1; i++) {
		int v = getMinimum(key, withinMST);
		withinMST[v] = 1;	// Add node v to MST
		// Look at each node that is adjacent to v
		for (int u = 0; u < numberOfCities; u++) {
			if (graph[v][u] && withinMST[u] == 0 && graph[v][u] < key[u]) {
				parent[u] = v;				// Update parent
				key[u] = graph[v][u];	// Update key if distance is smaller than u
			}
		}
	}
	int node1 = 0;
	while (node1 < numberOfCities) {
		int node2 = parent[node1];
		if (node2 != -1) {
			adjacent[node1].push_back(node2);
			adjacent[node2].push_back(node1);
		}
		node1++;
	}
};

// Gets the minimum index for findMST
int Christofides::getMinimum(int key[], int mstIndex[]) {
	int min = 2147483647; // Max int value
	int minimumIndex;
	for (int v = 0; v < numberOfCities; v++)
		if (mstIndex[v] == 0 && key[v] < min) {
			min = key[v];
			minimumIndex = v;
		}
	return minimumIndex;
};

// Find a match in subgraph
void Christofides::findMatch() {
	int closest, length;
	std::vector<int>::iterator tmp, first;
	// Finds nodes with odd degrees to get subgraph
	int oddVerts = 0;
	while (oddVerts < numberOfCities) {
		if ((adjacent[oddVerts].size() % 2) != 0 )
			odds.push_back(oddVerts);
		oddVerts++;
	}
	while (!odds.empty()) {
		first = odds.begin();
		std::vector<int>::iterator i = odds.begin() + 1;
		std::vector<int>::iterator end = odds.end();
		length = 2147483647;	// Max int value
		while (i != end) {
			if (graph[*first][*i] < length) {
				length = graph[*first][*i];
				closest = *i;
				tmp = i;
			}
			++i;
		}
		// Two nodes are matched
		adjacent[*first].push_back(closest);
		adjacent[closest].push_back(*first);
		// At end of list so erase
		odds.erase(tmp);
		odds.erase(first);
	}
}

// Modifies euler path or copy
void Christofides::euler (int position, std::vector<int> &path) {
	std::vector<int> *temp = new std::vector<int> [numberOfCities];
	int i = 0;
	while (i < numberOfCities) {
		temp[i].resize(adjacent[i].size());
		temp[i] = adjacent[i];
		i++;
	}
	path.clear();
	std::stack<int> myStack;
	// Continue this loop until the node has no more neighbors
	while (!myStack.empty() || temp[position].size() > 0 ) {
		if (temp[position].size() == 0) {
			path.push_back(position);	// If no neighbors, add it to our circuit
			int last = myStack.top(); // Pop last node from stack and use as current
			myStack.pop();
			position = last;
		}
		else {
			myStack.push(position);								// Add node to stack
			int neighbor = temp[position].back();	// Take a neighbor
			temp[position].pop_back();	// Remove edge from neighbor and node
	        for (unsigned int i = 0; i < temp[neighbor].size(); i++)
	            if (temp[neighbor][i] == position) { // Find pos of neighbor
	        	    temp[neighbor].erase (temp[neighbor].begin() + i); // Delete it
	                break;
	            }
	        position = neighbor; // Set neighbor as current node
		}
	}
	path.push_back(position);
}

void Christofides::makeHamiltonian(std::vector<int> &path, int &pathDistance) {
	// Remove the nodes we've been at from the tour
	bool visited[numberOfCities]; // Lets us know if we've visited a node
	memset(visited, 0, numberOfCities * sizeof(bool));
	pathDistance = 0;
	int root = path.front();
	std::vector<int>::iterator current = path.begin();
	std::vector<int>::iterator next = (path.begin()+1);
	visited[root] = true;
	while (next != path.end()) {	// Loop through until we hit end of circuit
		if (!visited[*next]) {			// Go to the next city if we need to
			pathDistance += graph[*current][*next];
			current = next;
			visited[*current] = true;
			next = current + 1;
		}
		else
			next = path.erase(next); // Remove next
	}
	pathDistance = pathDistance + graph[*current][*next];
}

void Christofides::swap(std::vector<int> &path, int start, int end, int numberOfCities){
	while (end - start > 0) {
		int temp = path[start % numberOfCities];
		path[start % numberOfCities] = path[end % numberOfCities];
		path[end % numberOfCities] = temp;
		start++;
		end--;
	}
}

int Christofides::findShorter(int **graph, int node1, int node2, int node3, int node4, int &fullDistance){
	if ((graph[node1][node3] + graph[node2][node4]) < (graph[node1][node2] + graph[node3][node4])){
		fullDistance -= (graph[node1][node2] + graph[node3][node4] - graph[node1][node3]
				- graph[node2][node4]);
		return 1;
	}
	return 0;
}

// 2-opt heuristic
int Christofides::twoOpt(int **graph, std::vector<int> &path, int &pathLength, int numberOfCities){
	int counter = 0;
	int optimizePath = 0;
	// Optimizing for competition files
	if (numberOfCities >= 0 && numberOfCities <= 100)
		optimizePath = 5;
	else if (numberOfCities <= 250 && numberOfCities > 100)
		optimizePath = 2;
	else if (numberOfCities <= 500 && numberOfCities > 250)
		optimizePath = 1;
	else if (numberOfCities >= 1000)
		optimizePath = 16;
	int prevDistance = pathLength;
	int node1, node2, edge1, edge2;
	int i = 0; // Go over every edge
	while (i < numberOfCities){
		edge1 = i; // Start edge
		node1 = (i+1) % numberOfCities;
		int j = i + 2;
		while (((j + 1) % numberOfCities) != i){
			edge2 = j % numberOfCities;
			node2 = (j+1) % numberOfCities;
			// If new edges decrease path length, then decrease it
			if (findShorter(graph, path[edge1], path[node1], path[edge2], path[node2], pathLength)) {
				// Swap edge1/node1 and edge2/node2
				swap(path, i+1, j, numberOfCities);
				if (pathLength - prevDistance < 8 && counter == optimizePath)
					break;
				if (pathLength - prevDistance > optimizePath )
					i = 0;
				prevDistance = pathLength;
				counter++;
			}
			j++;
		}
		i++;
	}
	return pathLength;
}

// Finds path length from the node specified
int Christofides::findSmallest(int pos) {
	std::vector<int>path;		// Euler vector
	euler(pos, path);
	int length;
	makeHamiltonian(path, length); // Make it a hamiltonian
	// We can reduce the following two calls to
	// twoOpt in order to lower the time to find
	// optimal path, however the path will get further
	// from the optimal solution
	for (int shrinkIt = 0; shrinkIt < 3; shrinkIt++){
		twoOpt(graph, path, length, numberOfCities); // Optimize the path length
	}
	return length;
}

void Christofides::shrinkPath(){
	// Calling twoOpt less than 3 times creates a larger path
	// Calling twoOpt more than 3 times does not shorten path any further
	for (int shrinkAgain = 0; shrinkAgain < 3; shrinkAgain++){
		twoOpt(graph, circuit, pathLength, numberOfCities);
	}
}
