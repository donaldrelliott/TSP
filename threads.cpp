

#include "threads.hpp"

// Starts thread
void Thread::begin() {
	int k;
	if ((k = pthread_create(&ID, NULL, &Thread::execute, this)) != 0)
		throw "Error";
}

// Waits for the thread to end
void Thread::join() {
	void *status;
	int k = pthread_join(ID, &status);
	if (k)
		exit(-1);
}

// Return the thread ID
long Thread::getThreadID(){
	return (long)ID;
}

void *Thread::execute(void *thr) {
	static_cast<Thread *>(thr)->run();
	return NULL;
}

void Thread::run() {
	int result = (*mainTSP).findSmallest(startNode);
	(*mainTSP).pathVals[mainID][0] = startNode;
	(*mainTSP).pathVals[mainID][1] = result;
	pthread_exit(NULL);
}

// Remove edge 1/2, then reconnect with new path
void reverse(std::vector<int> &path, int start, int end, int n){
	while (end-start > 0){
		int temp = path[start % n];
		path[start % n] = path[end % n];
		path[end % n] = temp;
		start++;
		end--;
	}
}
