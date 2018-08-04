

#ifndef THREADS_HPP
#define THREADS_HPP
#include "christofides.hpp"

class Thread {
	private:
		pthread_t ID;															// Thread IDs
		static void *execute(void *thr);					// Calls run
		Thread(const Thread& arg);								// Prevents copying
		Thread& operator=(const Thread& newVar);	// Prevents assignment
		virtual void run();
	public:
		Thread() {																// Constructor
			mainTSP = NULL;
			ID = 0;
			mainID = startNode = -1;
		};
	virtual ~Thread() {};												// Destructor
	void begin();
	void join();
	long getThreadID();													// Get thread ID
	int startNode;															// Index of start node
	Christofides *mainTSP;											// Pointer to Christofides
	int mainID;																	// Main tracked ID
};
#endif
